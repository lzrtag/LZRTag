/*
 * animatorThread.h
 *
 *  Created on: 25 Jan 2019
 *      Author: xasin
 */

#ifndef MAIN_FX_ANIMATORTHREAD_H_
#define MAIN_FX_ANIMATORTHREAD_H_

#include "Color.h"

namespace LZR {

using namespace Peripheral;

enum MarkMode {
	NONE,
	FLASH,
	DFLASH,
	SCAN
};

struct ColorSet {
	Color muzzleFlash;
	Color muzzleHeat;

	Color vestBase;
	Color vestShotEnergy;
};

struct FXSet {
	uint8_t minBaseGlow;
	uint8_t maxBaseGlow;

	float waverAmplitude;
	int   waverPeriod;
	float waverPositionShift;
};

extern ColorSet currentColors;
extern FXSet 	currentFX;

void start_animation_thread();
}


#endif /* MAIN_FX_ANIMATORTHREAD_H_ */