/*
 * VestPattern.h
 *
 *  Created on: 5 Sep 2019
 *      Author: xasin
 */

#ifndef MAIN_FX_PATTERNS_VESTPATTERN_H_
#define MAIN_FX_PATTERNS_VESTPATTERN_H_

#include "BasePattern.h"

namespace LZR {
namespace FX {

enum class pattern_func_t {
	SINE,
	TRAPEZ,
	CHASE_TRAPEZ,
};
enum class time_func_t {
	LINEAR,
	TRAPEZ,
	HALF_SINE,
	EQUAL_SINE,
};

class VestPattern: public BasePattern {
private:
	// This returns a time-normized trapez function, with the top being (1-trap_percent) long
	// trap_percent = 65535 equals a triangle wave, 65535 ticks long
	uint16_t get_normized_trapez(int32_t pos, uint16_t trap_percent);

	// This returns the timefunction value, shifted by cntr ticks
	uint16_t get_timefunc_shifted(int32_t cntr);
	uint16_t get_patternfunc_at(float pos);

public:
	time_func_t time_func;
	int32_t timefunc_period;
	int32_t timefunc_p1_period;
	int32_t timefunc_trap_percent;

	pattern_func_t pattern_func;
	int32_t sine_center;
	int32_t sine_amplitude;

	int32_t 	pattern_period;
	int32_t 	pattern_p1_length;
	uint16_t 	pattern_trap_percent;

	Peripheral::Color overlayColor;

	VestPattern();

	void apply_color_to(Peripheral::Color &tgt, float pos);
};

} /* namespace FX */
} /* namespace LZR */

#endif /* MAIN_FX_PATTERNS_VESTPATTERN_H_ */
