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
	time_func = time_func_t::TRAPEZ;
	timefunc_period = 4000;
	timefunc_p1_period = 4000 - 2000*(1-0.7);
	timefunc_trap_percent = 0.7 * (1<<16);
	timefunc_shift = 0;

	pattern_func = pattern_func_t::TRAPEZ;

	sine_center = ((1<<15)-1);
	sine_amplitude = ((1<<15)-1);

	pattern_period = 3*255;
	pattern_p1_length = 1.8*255;
	pattern_p2_length = 2*255;
	pattern_trap_percent = 0.5 * (1<<16);
	pattern_shift = 0;

	overlay = true;
}

uint16_t VestPattern::get_normized_trapez(int32_t pos, uint16_t trap_percent) {
	trap_percent >>= 1;

	if(pos < 0)
		return 0;
	if(pos >= (1<<16))
		return 0;

	if(pos < trap_percent)
		return (((1<<16) -1) * pos)/trap_percent;

	pos = ((1<<16) - pos -1);
	if(pos < trap_percent)
		return (((1<<16) -1) * pos)/trap_percent;

	return (1<<16) - 1;
}

uint16_t VestPattern::get_timefunc_shifted(int32_t ticks) {
	int32_t tickCnt = xTaskGetTickCount() + ticks + timefunc_shift;

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
		return get_normized_trapez((((1<<16)-1)*tickCnt)/timefunc_p1_period, timefunc_trap_percent);

	case time_func_t::HALF_SINE:
		if(tickCnt > timefunc_p1_period)
			return 0;
		return ((1<<16)-1)*sin((M_PI*tickCnt) / timefunc_p1_period);

	case time_func_t::EQUAL_SINE:
		return int32_t(1<<15) + int32_t((1<<15) - 10) * sin((2*M_PI*tickCnt) / timefunc_p1_period);
	}
}

uint16_t VestPattern::get_patternfunc_at(float pos) {
	int32_t bitPos = 255.0 * pos + pattern_shift;

	switch(pattern_func) {
	default: return 0;

	case pattern_func_t::SINE: {
		float patternPos = bitPos + pattern_period * get_timefunc_shifted(0)/float(1<<16);
		patternPos /= pattern_p1_length;
		int32_t sVal = sine_center + sine_amplitude * sin(2*M_PI*patternPos);

		if(sVal < 0)
			return 0;
		if(sVal > ((1<<16) -1))
			return ((1<<16) -1);
		return sVal;
	}

	case pattern_func_t::TRAPEZ:
		bitPos += pattern_p1_length/2 - ((pattern_p2_length*get_timefunc_shifted(0))>>16);

		if(pattern_period != 0)
			bitPos %= pattern_period;

		if(bitPos < 0)
			bitPos += pattern_period;
		bitPos  = (bitPos<<16) / pattern_p1_length;

		return get_normized_trapez(bitPos, pattern_trap_percent);
	}

}

void VestPattern::tick() {
}

void VestPattern::apply_color_at(Xasin::NeoController::Color &tgt, float pos) {
	if(!enabled)
		return;

	if(overlay)
		tgt.merge_overlay(overlayColor, get_patternfunc_at(pos) >> 8);
	else
		tgt.merge_add(overlayColor, get_patternfunc_at(pos)>>8);
}

void VestPattern::set_5050_trapez(int ticks, float fillPercent) {
	time_func = time_func_t::TRAPEZ;

	timefunc_period = ticks;
	timefunc_p1_period = ticks - ticks*0.5*(1-fillPercent);
	timefunc_trap_percent = fillPercent * ((1<<16)-1);
}

} /* namespace FX */
} /* namespace LZR */
