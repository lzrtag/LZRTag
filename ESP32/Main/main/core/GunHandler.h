/*
 * GunHandler.h
 *
 *  Created on: 18 Jan 2019
 *      Author: xasin
 */

#ifndef MAIN_GUNHANDLER_H_
#define MAIN_GUNHANDLER_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"

#include <vector>

#include "AudioHandler.h"

#include "player.h"

using namespace Xasin::Peripheral;

namespace Lasertag {

enum FIRE_STATE {
	NO_GUN,
	WEAPON_SWITCH_DELAY,
	RELOAD_DELAY,
	WAIT_ON_VALID,
	POST_TRIGGER_DELAY,
	POST_TRIGGER_RELEASE,
	POST_SHOT_DELAY,
	POST_SALVE_DELAY,
	POST_SALVE_RELEASE
};

#define GUN_TAG "LZR::Gun"

class GunSpecs {
public:
	int weaponSwitchDelay;

	int currentReserveAmmo;
	int clipSize;
	int currentClipAmmo;

	int  postTriggerTicks;
	bool postTriggerRelease;

	int	 shotsPerSalve;

	int	 perShotDelay;

	int  postShotCooldownTicks;
	int	 postShotVibrationTicks;

	int	 postSalveDelay;
	bool postSalveRelease;

	int  perReloadDelay;
	int  perReloadRecharge;

	double perShotHeatup;
	double perTickCooldown;

	CassetteCollection	chargeSounds;
	CassetteCollection	shotSounds;
	CassetteCollection	cooldownSounds;
};

class GunHandler {
private:
	int mqttAmmo;
	TickType_t lastMQTTPush;

	FIRE_STATE 		fireState;

	int currentGunID;

	TickType_t shotTick;
	int salveCounter;
	TickType_t lastShotTick;

	TickType_t lastTick;

	float gunHeat;

	const gpio_num_t triggerPin;
	bool pressAlreadyTriggered;

	bool shot_performed;
	void handle_shot();

	void shot_tick();
	void fx_tick();

	void deny_beep();

	void set_fire_state(FIRE_STATE nextState);

	void handle_reload_delay();
	void handle_wait_valid();

public:
	AudioHandler &audio;

	GunSpecs &cGun();

	GunHandler(gpio_num_t trgtPin, AudioHandler &audio);

	bool triggerPressed();

	bool was_shot_tick();
	TickType_t timeSinceLastShot();
	uint8_t getGunHeat();

	void tick();
};

} /* namespace Lasertag */

#endif /* MAIN_GUNHANDLER_H_ */
