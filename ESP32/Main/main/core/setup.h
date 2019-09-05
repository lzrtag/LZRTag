/*
 * setup.h
 *
 *  Created on: 7 Jan 2019
 *      Author: xasin
 */

#ifndef MAIN_SETUP_H_
#define MAIN_SETUP_H_

#include "xasin/BatteryManager.h"

#include "AudioHandler.h"
#include "NeoController.h"

#include "xasin/mqtt/Handler.h"

#include "player.h"
#include "GunHandler.h"

namespace LZR {

enum CORE_WEAPON_STATUS {
	INITIALIZING,
	DISCHARGED,
	CHARGING,
	NOMINAL,
};

extern CORE_WEAPON_STATUS main_weapon_status;

extern Housekeeping::BatteryManager battery;

extern Xasin::Peripheral::AudioHandler audioManager;
extern Peripheral::NeoController	RGBController;

extern Xasin::MQTT::Handler mqtt;

extern LZR::Player	player;
extern Lasertag::GunHandler	gunHandler;

void setup();

}

#endif /* MAIN_SETUP_H_ */
