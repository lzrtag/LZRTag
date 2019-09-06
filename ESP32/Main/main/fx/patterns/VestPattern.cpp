/*
 * VestPattern.cpp
 *
 *  Created on: 5 Sep 2019
 *      Author: xasin
 */

#include "VestPattern.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <cmath>

namespace LZR {
namespace FX {

VestPattern::VestPattern() : BasePattern() {

	time_func = time_func_t::LINEAR;
	timefunc_period = 600;
	timefunc_p1_period = 300;
	timefunc_trap_percent = 0.5 * (1<<16);

	pattern_func = pattern_func_t::SINE;

	sine_center = ((1<<15)-1);
	sine_amplitude = ((1<<15)-1);

	pattern_period = 5*255;
	pattern_p1_length = 3*255;
	pattern_trap_percent = 0.3 * (1<<16);
}

uint16_t VestPattern::get_normized_trapez(int32_t pos, uint16_t trap_percent) {
	trap_percent >>= 2;

	if(pos < 0)
		return 0;
	if(pos >= (1<<16))
		return 0;

	if(pos < trap_percent)
		return (((1<<16) -1) * pos)/trap_percent;
	else if(pos > (((1<<16) -1) - trap_percent))
		return (((1<<16) -1) * ((1<<16) - pos -1))/trap_percent;

	return (1<<16) - 1;
}

uint16_t VestPattern::get_timefunc_shifted(int32_t ticks) {
	int32_t tickCnt = xTaskGetTickCount() + ticks;

	if(timefunc_period != 0)
		tickCnt %= timefunc_period;

	switch(time_func) {
	default: return 0;

	case time_func_t::LINEAR:
		if(tickCnt > timefunc_p1_period)
			return (1<<16) -1;
		if(tickCnt < 0)
			return 0;
		return (((1<<16)-1)*tickCnt)/timefunc_p1_period;

	case time_func_t::TRAPEZ:
		return get_normized_trapez(((2<<16)*tickCnt)/timefunc_p1_period, timefunc_trap_percent);

	case time_func_t::HALF_SINE:
		if(tickCnt > timefunc_p1_period)
			return 0;
		return ((1<<16)-1)*sin((M_PI*tickCnt) / timefunc_p1_period);

	case time_func_t::EQUAL_SINE:
		return (1<<15) -1 + (1<<15) * sin((M_PI*tickCnt) / timefunc_p1_period);
	}
}

uint16_t VestPattern::get_patternfunc_at(float pos) {
	int32_t bitPos = ((1<<8)-1) * pos;

	switch(pattern_func) {
	default: return 0;

	case pattern_func_t::SINE: {
		int32_t sVal = sine_center + sine_amplitude * sin(2*M_PI*(bitPos / float(pattern_period) + get_timefunc_shifted(0)/float(1<<16)));
		if(sVal < 0)
			return 0;
		if(sVal > ((1<<16) -1))
			return ((1<<16) -1);
		return sVal;
	}

	case pattern_func_t::TRAPEZ: {
		bitPos += pattern_p1_length/2 - pattern_period*get_timefunc_shifted(0);
		bitPos %= pattern_period;
		bitPos  = (bitPos<<16) / pattern_p1_length;

		return	get_normized_trapez(bitPos, pattern_trap_percent);
	}

	case pattern_func_t::CHASE_TRAPEZ: {
		return 0;
	}

	}
}

void VestPattern::apply_color_to(Peripheral::Color &tgt, float pos) {
	tgt.merge_overlay(overlayColor, get_patternfunc_at(pos) >> 8);
}

} /* namespace FX */
} /* namespace LZR */
