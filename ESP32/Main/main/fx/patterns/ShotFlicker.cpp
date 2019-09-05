/*
 * ShotFlicker.cpp
 *
 *  Created on: 5 Sep 2019
 *      Author: xasin
 */

#include "../../core/setup.h"
#include "../animatorThread.h"
#include "ShotFlicker.h"

namespace LZR {
namespace FX {

ShotFlicker::ShotFlicker(int length) :
		BasePattern(), anim(length) {

	anim.baseTug   = 0.0013;
	anim.basePoint = 0.0;
	anim.dampening = 0.94;
	anim.ptpTug    = 0.015;
	anim.wrap 	   = true;
}

void ShotFlicker::tick() {
	if(gunHandler.was_shot_tick())
		anim.points[0].pos = 1;
	anim.tick();
}
void ShotFlicker::apply_color_at(Peripheral::Color &tgt, float index) {
	Peripheral::Color shotColor = bufferedColors.vestShotEnergy;
	shotColor.bMod(anim.scalarPoints[index]);
	tgt.merge_add(shotColor);
}

} /* namespace FX */
} /* namespace LZR */
