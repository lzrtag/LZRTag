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

#include "../fx/sounds.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include "esp_log.h"

#include <cmath>

namespace Lasertag {

GunHandler::GunHandler(gpio_num_t trgPin, AudioHandler &audio)
	:	mqttAmmo(0), lastMQTTPush(0),
		fireState(NO_GUN),
		currentGunID(0),
		shotTick(0), salveCounter(0), lastShotTick(0),
		lastTick(0),
		gunHeat(0),
		triggerPin(trgPin), pressAlreadyTriggered(false),
		shot_performed(false),
		audio(audio) {

	gpio_set_direction(triggerPin, GPIO_MODE_INPUT);
	gpio_set_pull_mode(triggerPin, GPIO_PULLUP_ONLY);
	gpio_pullup_en(triggerPin);

	esp_log_level_set(GUN_TAG, ESP_LOG_DEBUG);

	LZR::player.mqtt.subscribe_to(LZR::player.get_topic_base() + "/Stats/Ammo/#",
		[this](Xasin::MQTT::MQTT_Packet data) {
			if(0 >= currentGunID || currentGunID > 3)
				return;

			if(data.topic == "SetReserve")
				cGun().currentReserveAmmo = atoi(data.data.data());
			else if(data.topic == "SetClip")
				cGun().currentClipAmmo = atoi(data.data.data());
		});
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
	if(cGun().currentClipAmmo > 0)
		cGun().currentClipAmmo--;
	// We have to abort the salve if we have no ammo left
	// This also plays the empty click
	else if(cGun().currentClipAmmo == 0) {
		LZR::Sounds::play_audio("CLICK");
		pressAlreadyTriggered = true;

		fireState = POST_SALVE_RELEASE;
		shotTick = xTaskGetTickCount() + cGun().postSalveDelay;

		return;
	}

	gunHeat += cGun().perShotHeatup;

	fireState = POST_SHOT_DELAY;
	shotTick  = xTaskGetTickCount() + (cGun().perShotDelay*(95 + esp_random()%10))/100;
	lastShotTick = xTaskGetTickCount();

	shot_performed = true;

	audio.insert_cassette(cGun().shotSounds);

	LZR::IR::send_signal();
}

void GunHandler::deny_beep() {
	if(triggerPressed() && !pressAlreadyTriggered) {
		pressAlreadyTriggered = true;
		LZR::Sounds::play_audio("DENY");
	}
}

void GunHandler::set_fire_state(FIRE_STATE newState) {
	if(newState == fireState)
		return;

	fireState = newState;
	shotTick = xTaskGetTickCount();
	switch(fireState) {
		default: shotTick = 0; break;

		case WEAPON_SWITCH_DELAY:
			shotTick += cGun().weaponSwitchDelay;
		break;
		case RELOAD_DELAY:
			shotTick += cGun().perReloadDelay;
		break;

		case POST_TRIGGER_RELEASE:
		case POST_TRIGGER_DELAY:
			shotTick += cGun().postTriggerTicks;
		break;
		case POST_SHOT_DELAY:
			shotTick += (cGun().perShotDelay*(95 + esp_random()%10))/100;
		break;
		case POST_SALVE_RELEASE:
		case POST_SALVE_DELAY:
			shotTick += cGun().postSalveDelay;
		break;
	}
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
			LZR::Sounds::play_audio("SWITCH WEAPON");
		}
	}

	while(true) {
		switch(fireState) {
		case NO_GUN:
			break;

		case WEAPON_SWITCH_DELAY:
			if(triggerPressed() && !pressAlreadyTriggered) {
				pressAlreadyTriggered = true;
				LZR::Sounds::play_audio("DENY");
			}
			if(xTaskGetTickCount() < shotTick)
				break;
			fireState = WAIT_ON_VALID;
			continue;
		break;

		case RELOAD_DELAY: {
			// Detect infinite clip ammo and skip reload
			if(cGun().currentClipAmmo < 0) {
				fireState = WAIT_ON_VALID;
				continue;
			}
			// We don't need to reload if our clip ammo is at or above maximum
			if(cGun().currentClipAmmo >= cGun().clipSize) {
				fireState = WAIT_ON_VALID;
				continue;
			}
			// Equally, if we have nothing more to reload with, skip.
			if(cGun().currentReserveAmmo == 0) {
				fireState = WAIT_ON_VALID;
				continue;
			}
			// Skip reloading if we aren't being forced to reload and the player
			// pressed the trigger.
			// Useful for shotgun-type weapons with piecewise reloading
			if(!LZR::player.should_reload && triggerPressed()) {
				fireState = WAIT_ON_VALID;
				continue;
			}
			// Tell the player we're reloading by beeping a bit :P
			if(triggerPressed() && !pressAlreadyTriggered) {
				pressAlreadyTriggered = true;
				LZR::Sounds::play_audio("DENY");
			}

			if(xTaskGetTickCount() < shotTick)
				break;

			// Cap refill amount to how much we have left
			// This also handles infinite refill ammo (reserveAmmo < 0)
			auto refillAmount = cGun().perReloadRecharge;
			if((cGun().currentReserveAmmo >= 0) && (refillAmount > cGun().currentReserveAmmo)) {
				refillAmount = cGun().currentReserveAmmo;
			}

			// Cap the refilled ammo amount to our clip size
			auto newAmmo = cGun().currentClipAmmo + refillAmount;
			if(newAmmo > cGun().clipSize)
				newAmmo = cGun().clipSize;

			// Subtract the amount of ammo we are refilling from the reserve ammo
			// Also tracking infinite reserve ammo
			if(cGun().currentReserveAmmo >= 0) {
				cGun().currentReserveAmmo -= newAmmo - cGun().currentClipAmmo;
			}
			cGun().currentClipAmmo = newAmmo;

			// Clear the reloading flag from the player
			LZR::player.should_reload = false;

			// If our clip isn't full yet, continue reloading if we can.
			// Think of a shotgun that is loaded clip by clip
			if((cGun().currentClipAmmo < cGun().clipSize) && cGun().currentReserveAmmo != 0) {
				shotTick = xTaskGetTickCount() + cGun().perReloadDelay;
				ESP_LOGD(GUN_TAG, "Reloaded a little, continuing");
			}
			else {
				fireState = WAIT_ON_VALID;
				ESP_LOGD(GUN_TAG, "Reload complete!");
				LZR::Sounds::play_audio("RELOAD FULL");
				continue;
			}

			break;
		}

		case WAIT_ON_VALID:
			// Players may not shoot nor reload if they are
			// disabled!
			if(!LZR::player.can_shoot())
				break;

			// First, check if a reload needs to be forced. This happens only
			// when we have no more ammo at all, but have some in reserve, or
			// if the player actively wants to reload
			if((cGun().currentClipAmmo == 0 && cGun().currentReserveAmmo != 0)
					||
				(LZR::player.should_reload)) {

				LZR::player.should_reload = true;
				shotTick = xTaskGetTickCount() + cGun().perReloadDelay;
				fireState = RELOAD_DELAY;
				continue;
			}

			// Wait for the user to press the trigger
			if(!triggerPressed())
				break;

			// Play an empty clip sound if we don't have any more ammo and
			// reloading didn't work either.
			if(cGun().currentClipAmmo == 0) {
				if(!pressAlreadyTriggered)
					LZR::Sounds::play_audio("CLICK");
				pressAlreadyTriggered = true;

				break;
			}

			// Mark that this press was already handled - used later!
			pressAlreadyTriggered = true;

			// Otherwise, continue with our magic~
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
				break;
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
			if(pressAlreadyTriggered && cGun().postSalveRelease)
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

	// Clear the trigger handle flag on release
	// Is used to require re-pressing the trigger for some actions
	if(!triggerPressed()) {
		pressAlreadyTriggered = false;
	}
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
	shot_tick();
	fx_tick();

	if((cGun().currentClipAmmo != mqttAmmo) && (xTaskGetTickCount() > (lastMQTTPush+300))) {
		ESP_LOGD(GUN_TAG, "Weapon is: %d/%d (%d)", cGun().currentClipAmmo, cGun().clipSize, cGun().currentReserveAmmo);
		struct {
			int32_t currentAmmo;
			int32_t clipSize;
			int32_t reserveAmmo;
		} ammoData = {cGun().currentClipAmmo, cGun().clipSize, cGun().currentReserveAmmo};
		LZR::mqtt.publish_to(LZR::player.get_topic_base() +"/Stats/Ammo", &ammoData, sizeof(ammoData), 0, true);

		mqttAmmo = cGun().currentClipAmmo;
		lastMQTTPush = xTaskGetTickCount();
	}

	lastTick = xTaskGetTickCount();
}

} /* namespace Lasertag */
