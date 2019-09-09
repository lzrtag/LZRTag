/*
 * animatorThread.h
 *
 *  Created on: 25 Jan 2019
 *      Author: xasin
 */

#ifndef MAIN_FX_ANIMATORTHREAD_H_
#define MAIN_FX_ANIMATORTHREAD_H_

#include "Color.h"
#include "../IODefs.h"

#define VEST_LEDS WS2812_NUMBER-1

namespace LZR {

using namespace Peripheral;

struct ColorSet {
	Color muzzleFlash;
	Color muzzleHeat;

	Color vestBase;
	Color vestShotEnergy;
};

struct FXSet {
	float minBaseGlow;
	float maxBaseGlow;

	float waverAmplitude;
	float waverPeriod;
	float waverPositionShift;
};

enum pattern_mode_t {
	OFF,
	IDLE,
	TEAM_SELECT,
	ACTIVE,
};

extern pattern_mode_t target_pattern_mode;

extern ColorSet currentColors;
extern FXSet 	currentFX;

extern ColorSet bufferedColors;

void start_animation_thread();
}


#endif /* MAIN_FX_ANIMATORTHREAD_H_ */
