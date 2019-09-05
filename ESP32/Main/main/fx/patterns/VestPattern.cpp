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
}

uint16_t VestPattern::get_normized_trapez(int32_t pos, uint16_t trap_percent) {
	trap_percent >>= 2;

	if(pos < 0)
		return 0;
	if(pos >= (2<<16))
		return 0;

	if(pos < trap_percent)
		return ((2<<16 -1) * pos)/trap_percent;
	else if(pos > ((2<<16 -1) - trap_percent))
		return ((2<<16 -1) * (2<<16 - pos -1))/trap_percent;

	return 2<<16 - 1;
}

uint16_t VestPattern::get_timefunc_shifted(int32_t ticks) {
	int32_t tickCnt = xTaskGetTickCount() + ticks;

	if(timefunc_period != 0)
		tickCnt %= timefunc_period;

	switch(time_func) {
	default: return 0;

	case time_func_t::LINEAR:
		if(tickCnt > timefunc_p1_period)
			return 2<<16 -1;
		if(tickCnt < 0)
			return 0;
		return ((2<<16-1)*tickCnt)/timefunc_p1_period;

	case time_func_t::TRAPEZ:
		return get_normized_trapez(((2<<16)*tickCnt)/timefunc_p1_period, timefunc_trap_percent);

	case time_func_t::HALF_SINE:
		if(tickCnt > timefunc_p1_period)
			return 0;
		return (2<<16-1)*sin((M_PI*tickCnt) / timefunc_p1_period);

	case time_func_t::EQUAL_SINE:
		return (2<<15) + (2<<15) * sin((M_PI*tickCnt) / timefunc_p1_period);
	}
}

uint16_t VestPattern::get_patternfunc_at(float pos) {

}

} /* namespace FX */
} /* namespace LZR */
