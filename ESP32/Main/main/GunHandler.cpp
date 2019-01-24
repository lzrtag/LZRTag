/*
 * GunHandler.cpp
 *
 *  Created on: 18 Jan 2019
 *      Author: xasin
 */

#include "GunHandler.h"

namespace Lasertag {

GunSpecs::GunSpecs() {
	maxAmmo = 50;

	postTriggerTicks   = 0;
	postTriggerRelease = false;

	shotsPerSalve = 3;

	perShotDelay   = 70;
	postSalveDelay = 200;
	postSalveRelease = false;

	postShotReloadBlock =   600;
	postReloadReloadBlock = 60;
	perReloadRecharge = maxAmmo;

	perShotHeatup 	= 0.007;
	perTickCooldown = 0.998;
}

GunHandler::GunHandler(gpio_num_t trgPin)
	:	fireState(WAIT_ON_VALID),
		reloadState(FULL),
		shotTick(0), salveCounter(0), lastShotTick(0),
		reloadTick(0),
		gunHeat(0),
		triggerPin(trgPin),
		currentGun() {

	currentAmmo = currentGun.maxAmmo;

	gpio_set_direction(triggerPin, GPIO_MODE_INPUT);
	gpio_set_pull_mode(triggerPin, GPIO_PULLUP_ONLY);
	gpio_pullup_en(triggerPin);
}

bool GunHandler::triggerPressed() {
	return gpio_get_level(triggerPin) == 0;
}

void GunHandler::handle_shot() {
	puts("PEW!");

	currentAmmo--;
	gunHeat += currentGun.perShotHeatup;

	fireState = POST_SHOT_DELAY;
	shotTick  = xTaskGetTickCount() + currentGun.perShotDelay;
	lastShotTick = xTaskGetTickCount();

	reloadState = POST_SHOT_WAIT;
	reloadTick  = xTaskGetTickCount() + currentGun.postShotReloadBlock;
}

void GunHandler::shot_tick() {
	bool fireStateChanged = true;

	while(fireStateChanged) {
		fireStateChanged = false;

		switch(fireState) {
		case WAIT_ON_VALID:
			if(!triggerPressed())
				break;
			if(currentAmmo < currentGun.shotsPerSalve)
				break;

			if(currentGun.postTriggerTicks != 0)
				puts("Weapon charging!");

			shotTick  = xTaskGetTickCount() + currentGun.postTriggerTicks;
			fireState = POST_TRIGGER_DELAY;

		case POST_TRIGGER_DELAY:
			if(xTaskGetTickCount() < shotTick)
				break;
			fireState = POST_TRIGGER_RELEASE;

		case POST_TRIGGER_RELEASE:
			if(!triggerPressed() || !currentGun.postTriggerRelease) {
				salveCounter = currentGun.shotsPerSalve;
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

			fireState = POST_SALVE_DELAY;
			shotTick = xTaskGetTickCount() + currentGun.postSalveDelay;

		case POST_SALVE_DELAY:
			if(xTaskGetTickCount() < shotTick)
				break;

			fireState = POST_SALVE_RELEASE;

		case POST_SALVE_RELEASE:
			if(!triggerPressed() || !currentGun.postSalveRelease) {
				fireState = WAIT_ON_VALID;
				fireStateChanged = true;

				if(!triggerPressed() && !currentGun.postSalveRelease && !currentGun.postTriggerRelease) {
					puts("Shot loop stopped!");
				}
			}
		break;
		}
	}
}

void GunHandler::reload_tick() {
	if(currentAmmo >= currentGun.maxAmmo)
		return;

	if(reloadTick > xTaskGetTickCount())
		return;

	currentAmmo += currentGun.perReloadRecharge;
	if(currentAmmo > currentGun.maxAmmo)
		currentAmmo = currentGun.maxAmmo;

	reloadTick = xTaskGetTickCount() + currentGun.postReloadReloadBlock;

	puts("Reloaded a 'lil");
}

void GunHandler::graphics_tick() {
	gunHeat *= currentGun.perTickCooldown;
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

	graphics_tick();
}

} /* namespace Lasertag */
