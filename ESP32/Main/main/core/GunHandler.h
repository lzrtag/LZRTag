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
	WAIT_ON_VALID,
	POST_TRIGGER_DELAY,
	POST_TRIGGER_RELEASE,
	POST_SHOT_DELAY,
	POST_SALVE_DELAY,
	POST_SALVE_RELEASE
};

enum RELOAD_STATE {
	FULL,
	POST_SHOT_WAIT,
	POST_RELOAD_WAIT
};

#define GUN_TAG "LZR::Gun"

class GunSpecs {
public:
	int  maxAmmo;

	int  postTriggerTicks;
	bool postTriggerRelease;

	int	 shotsPerSalve;

	int	 perShotDelay;

	int  postShotCooldownTicks;
	int	 postShotVibrationTicks;

	int	 postSalveDelay;
	bool postSalveRelease;

	int	 postShotReloadBlock;
	int	 postReloadReloadBlock;
	int  perReloadRecharge;

	double perShotHeatup;
	double perTickCooldown;

	CassetteCollection	chargeSounds;
	CassetteCollection	shotSounds;
	CassetteCollection	cooldownSounds;
};

extern const GunSpecs defaultGun;

class GunHandler {
private:
	int currentAmmo;
	int mqttAmmo;
	TickType_t lastMQTTPush;

	FIRE_STATE 		fireState;
	RELOAD_STATE	reloadState;

	TickType_t shotTick;
	int salveCounter;
	TickType_t lastShotTick;

	bool emptyClickPlayed;

	TickType_t reloadTick;

	TickType_t lastTick;

	float gunHeat;

	const gpio_num_t triggerPin;

	bool shot_performed;
	void handle_shot();

	void shot_tick();
	void reload_tick();
	void fx_tick();

	GunSpecs const *currentGun;

public:
	AudioHandler &audio;

	GunSpecs const &cGun();

	GunHandler(gpio_num_t trgtPin, AudioHandler &audio);

	bool triggerPressed();

	bool was_shot_tick();
	TickType_t timeSinceLastShot();
	uint8_t getGunHeat();

	void tick();
};

} /* namespace Lasertag */

#endif /* MAIN_GUNHANDLER_H_ */
