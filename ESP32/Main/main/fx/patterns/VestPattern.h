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
//	CHASE_TRAPEZ,
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
	int32_t timefunc_period;		// Total period after which the time func repeats
	int32_t timefunc_p1_period;	// First-segment period, either the length of the trapez, linear section,
											// or sine-wave
	int32_t timefunc_trap_percent;
	int32_t timefunc_shift;

	pattern_func_t pattern_func;
	int32_t sine_center;				// Center value of the "sine" pattern
	int32_t sine_amplitude;			// Amplitude of the "sine" pattern

	int32_t 	pattern_period;		// Period after which the pattern repeats
	int32_t 	pattern_p1_length;		// P1 length, i.e. length of the trapez or sine wave
	int32_t		pattern_p2_length;	// Movement length, i.e. the x-coordinate amplitude of the time function
	uint16_t 	pattern_trap_percent;
	int32_t		pattern_shift;

	Xasin::NeoController::Color overlayColor;
	bool overlay;

	VestPattern();

	void tick();
	void apply_color_at(Xasin::NeoController::Color &tgt, float pos);

	void set_5050_trapez(int32_t ticks, float fillPercent);
};

} /* namespace FX */
} /* namespace LZR */

#endif /* MAIN_FX_PATTERNS_VESTPATTERN_H_ */
