/*
 * GunHandler.cpp
 *
 *  Created on: 18 Jan 2019
 *      Author: xasin
 */

#include "GunHandler.h"

#include "setup.h"
#include "IR.h"

#include "../weapons/wyre.h"
#include "../weapons/mylin.h"
#include "../weapons/zinger.h"

#include "empty_click.h"
#include "reload_full.h"

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"

namespace Lasertag {

const AudioCassette emptyClick(empty_click, sizeof(empty_click));
const AudioCassette reloadFull(reload_full, sizeof(reload_full));

GunHandler::GunHandler(gpio_num_t trgPin, AudioHandler &audio)
	:	mqttAmmo(0), lastMQTTPush(0),
		fireState(NO_GUN),
		currentGunID(0),
		shotTick(0), salveCounter(0), lastShotTick(0),
		emptyClickPlayed(false),
		lastTick(0),
		gunHeat(0),
		triggerPin(trgPin),
		shot_performed(false),
		audio(audio) {

	gpio_set_direction(triggerPin, GPIO_MODE_INPUT);
	gpio_set_pull_mode(triggerPin, GPIO_PULLUP_ONLY);
	gpio_pullup_en(triggerPin);
}

GunSpecs &GunHandler::cGun() {
	if(0 >= currentGunID || currentGunID > 3)
		return LZR::Weapons::wyre;

	GunSpecs * gunSets[] = {
			&LZR::Weapons::wyre,
			&LZR::Weapons::mylin,
			&LZR::Weapons::zinger,
	};

	return *gunSets[currentGunID-1];
}

bool GunHandler::triggerPressed() {
	return gpio_get_level(triggerPin) == 0;
}

void GunHandler::handle_shot() {
	cGun().currentClipAmmo--;
	gunHeat += cGun().perShotHeatup;

	fireState = POST_SHOT_DELAY;
	shotTick  = xTaskGetTickCount() + (cGun().perShotDelay*(95 + esp_random()%10))/100;
	lastShotTick = xTaskGetTickCount();

	shot_performed = true;

	audio.insert_cassette(cGun().shotSounds);

	LZR::IR::send_signal();
}

void GunHandler::shot_tick() {
	shot_performed = false;

	auto playerGunID = LZR::player.get_gun_num();
	if(playerGunID != currentGunID) {
		currentGunID = playerGunID;

		if(playerGunID == 0)
			fireState = NO_GUN;
		else {
			fireState = WEAPON_SWITCH_DELAY;
			shotTick = xTaskGetTickCount() + cGun().weaponSwitchDelay;
		}
	}

	while(true) {
		switch(fireState) {
		case NO_GUN:
			break;

		case WEAPON_SWITCH_DELAY:
			if(xTaskGetTickCount() > shotTick) {
				fireState = WAIT_ON_VALID;
				continue;
			}
		break;

		case RELOAD_DELAY:
			if(xTaskGetTickCount() > shotTick) {
				auto refillAmount = cGun().perReloadRecharge;
				if(cGun().currentReserveAmmo >= 0 || refillAmount > cGun().currentReserveAmmo) {
					refillAmount = cGun().currentReserveAmmo;
				}

				auto newAmmo = cGun().currentClipAmmo + refillAmount;
				if(newAmmo > cGun().clipSize)
					newAmmo = cGun().clipSize;

				if(cGun().currentReserveAmmo >= 0) {
					cGun().currentReserveAmmo -= newAmmo - cGun().currentClipAmmo;
				}
				cGun().currentClipAmmo = newAmmo;

				if((cGun().currentClipAmmo < cGun().clipSize) && !triggerPressed())
					shotTick = xTaskGetTickCount() + cGun().perReloadDelay;
				else
					fireState = WAIT_ON_VALID;
				continue;
			}
		break;

		case WAIT_ON_VALID:
			if(!LZR::player.can_shoot())
				break;
			if(cGun().currentClipAmmo < cGun().shotsPerSalve && cGun().currentReserveAmmo != 0) {
				shotTick = xTaskGetTickCount() + cGun().perReloadDelay;
				fireState = RELOAD_DELAY;
			}
			if(!triggerPressed())
				break;

			if(cGun().currentClipAmmo == 0) {
				if(!emptyClickPlayed) {
					audio.insert_cassette(emptyClick);
					emptyClickPlayed = true;
				}

				break;
			}

			if(cGun().postTriggerTicks != 0)
				audio.insert_cassette(cGun().chargeSounds);

			shotTick  = xTaskGetTickCount() + cGun().postTriggerTicks;
			fireState = POST_TRIGGER_RELEASE;
			continue;

		case POST_TRIGGER_RELEASE:
			if(triggerPressed() && cGun().postTriggerRelease)
				break;

			fireState = POST_TRIGGER_DELAY;
			continue;

		case POST_TRIGGER_DELAY:
			if(xTaskGetTickCount() >= shotTick) {
				salveCounter = cGun().shotsPerSalve;
				handle_shot();
			}
		break;

		case POST_SHOT_DELAY:
			if(xTaskGetTickCount() < shotTick)
				break;

			if(--salveCounter > 0) {
				handle_shot();
				break;
			}

			fireState = POST_SALVE_RELEASE;
			shotTick = xTaskGetTickCount() + cGun().postSalveDelay;
			continue;

		case POST_SALVE_RELEASE:
			if(triggerPressed() && cGun().postSalveRelease)
				break;
			fireState = POST_SALVE_DELAY;
			continue;

		case POST_SALVE_DELAY:
			if(xTaskGetTickCount() >= shotTick) {
				fireState = WAIT_ON_VALID;
				continue;

				if(!triggerPressed() && !cGun().postSalveRelease && !cGun().postTriggerRelease) {
					if(gunHeat > 0.4)
						audio.insert_cassette(cGun().cooldownSounds);
				}
			}
		}

		break;
	}

	if(!triggerPressed())
		emptyClickPlayed = false;
}

void GunHandler::fx_tick() {
	gunHeat *= cGun().perTickCooldown;

//	TickType_t cooldownSoundTick = lastShotTick + cGun().postShotCooldownTicks;
//	if(xTaskGetTickCount() >= cooldownSoundTick && lastTick < cooldownSoundTick)
//		audio.insert_cassette(cGun().cooldownSounds);
}

bool GunHandler::was_shot_tick() {
	return shot_performed;
}
TickType_t GunHandler::timeSinceLastShot() {
	return xTaskGetTickCount() - lastShotTick;
}
uint8_t GunHandler::getGunHeat() {
	if(gunHeat > 1)
		return 255;
	if(gunHeat < 0)
		return 0;

	return gunHeat * (255-3) + 3;
}

void GunHandler::tick() {
	reload_tick();
	shot_tick();

	fx_tick();

	if((cGun().currentAmmo != mqttAmmo) && (xTaskGetTickCount() > (lastMQTTPush+300))) {
		struct {
			int32_t currentAmmo;
			int32_t maxAmmo;
		} ammoData = {cGun().currentAmmo, cGun().maxAmmo};
		LZR::mqtt.publish_to(LZR::player.get_topic_base() +"/Stats/Ammo", &ammoData, sizeof(ammoData), 0, true);

		mqttAmmo = cGun().currentAmmo;
		lastMQTTPush = xTaskGetTickCount();
	}

	lastTick = xTaskGetTickCount();
}

} /* namespace Lasertag */
