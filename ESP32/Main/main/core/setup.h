/*
 * setup.h
 *
 *  Created on: 7 Jan 2019
 *      Author: xasin
 */

#ifndef MAIN_SETUP_H_
#define MAIN_SETUP_H_

#include "BatteryManager.h"

#include "AudioHandler.h"
#include "NeoController.h"

#include "GunHandler.h"

namespace LZR {

extern Housekeeping::BatteryManager battery;

extern Xasin::Peripheral::AudioHandler audioManager;
extern Peripheral::NeoController	RGBController;

extern Lasertag::GunHandler	gunHandler;

void animation_task();

void setup();

}

#endif /* MAIN_SETUP_H_ */
