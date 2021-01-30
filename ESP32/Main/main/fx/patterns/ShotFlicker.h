/*
 * ShotFlicker.h
 *
 *  Created on: 5 Sep 2019
 *      Author: xasin
 */

#ifndef MAIN_FX_PATTERNS_SHOTFLICKER_H_
#define MAIN_FX_PATTERNS_SHOTFLICKER_H_

#include "BasePattern.h"
#include "../ManeAnimator.h"

namespace LZR {
namespace FX {

class ShotFlicker: public BasePattern {
private:
	ManeAnimator anim;

public:
	const float maxLen;
	const int   pointCount;

	ShotFlicker(float length, int points);

	void tick();

	void apply_color_at(Xasin::NeoController::Color &tgt, float pos);
};

} /* namespace FX */
} /* namespace LZR */

#endif /* MAIN_FX_PATTERNS_SHOTFLICKER_H_ */
