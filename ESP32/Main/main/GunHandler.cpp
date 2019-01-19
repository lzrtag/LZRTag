/*
 * GunHandler.cpp
 *
 *  Created on: 18 Jan 2019
 *      Author: xasin
 */

#include "GunHandler.h"

namespace Lasertag {

GunHandler::GunHandler(gpio_num_t trgPin)
	:	fireState(WAIT_ON_VALID),
		reloadState(FULL),
		shotTick(0), salveCounter(0),
		reloadTick(0),
		triggerPin(trgPin),
		currentGun() {

	currentAmmo = currentGun.maxAmmo;

	gpio_set_pull_mode(triggerPin, GPIO_PULLUP_ONLY);
	gpio_pullup_en(triggerPin);
}

bool GunHandler::triggerPressed() {
	return gpio_get_level(triggerPin) == 0;
}

void GunHandler::handle_shot() {
	puts("PEW!");

	fireState = POST_SHOT_DELAY;
	shotTick  = xTaskGetTickCount() + currentGun.perShotDelay;
}

void GunHandler::tick() {
	bool fireStateChanged = true;

	while(fireStateChanged) {
		fireStateChanged = false;

		switch(fireState) {
		case WAIT_ON_VALID:
			if(!triggerPressed())
				break;

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
					// TODO Add shot-loop-stop here.
				}
			}
		break;
		}
	}
}

} /* namespace Lasertag */
