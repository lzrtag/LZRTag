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

enum pattern_mode_t : uint16_t {
	OFF,
	BATTERY_LEVEL,
	CHARGE,
	
	CONNECTING,

	PLAYER_DECIDED,

	IDLE,
	TEAM_SELECT,
	DEAD,
	ACTIVE,

	PATTERN_MODE_MAX, // Not to be used, just a placeholder
};


namespace FX {
	extern volatile pattern_mode_t target_mode;

	void mode_tick();
	void init();
}

extern ColorSet currentColors;
extern FXSet 	currentFX;

extern ColorSet bufferedColors;

void start_animation_thread();
}


#endif /* MAIN_FX_ANIMATORTHREAD_H_ */
