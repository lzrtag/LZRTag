/*
 * ShotFlicker.cpp
 *
 *  Created on: 5 Sep 2019
 *      Author: xasin
 */

#include "../../core/setup.h"
#include "../animatorThread.h"
#include "ShotFlicker.h"

#include "../sounds.h"

namespace LZR {
namespace FX {

ShotFlicker::ShotFlicker(float length, int points) :
		BasePattern(), anim(points),
		maxLen(length), pointCount(points) {

	anim.baseTug   = 0.0013;
	anim.basePoint = 0.0;
	anim.dampening = 0.94;
	anim.ptpTug    = 0.015;
	anim.wrap 	   = true;
}

void ShotFlicker::tick() {
	if(gunHandler.was_shot_tick())
		anim.points[0].pos = 1;
	if(Sounds::note_until > xTaskGetTickCount())
		anim.points[0].pos = Sounds::volume;

	anim.tick();
}
void ShotFlicker::apply_color_at(Peripheral::Color &tgt, float index) {
	Peripheral::Color shotColor = Color::HSV(Sounds::suggested_hue);

	if(index < 0)
		index = 0;
	if(index > maxLen)
		index = maxLen;

	shotColor.bMod(anim.scalarPoints[(index/maxLen) * pointCount]);
	tgt.merge_add(shotColor);
}

} /* namespace FX */
} /* namespace LZR */
