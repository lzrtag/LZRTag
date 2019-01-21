/*
 * ManeAnimator.cpp
 *
 *  Created on: 13 Nov 2018
 *      Author: xasin
 */

#include "ManeAnimator.h"

ManeAnimator::ManeAnimator(const int length) :
	points(length), scalarPoints(length) {

	for(PointData &p : points) {
		p.pos = 0.0;
		p.vel = 0;
	}

	basePoint = 0.3;
	baseTug = 0.0003;

	dampening = 0.995;

	ptpTug = 0.013;

	wrap = false;
}

void ManeAnimator::tick() {

	for(int i=0; i<(points.size()-1); i++) {
		float vTrans = (points[i].pos - points[i+1].pos)*ptpTug;

		points[i].vel -= vTrans;
		points[i+1].vel += vTrans;
	}
	if(wrap) {
		float vTrans = (points[points.size()-1].pos - points[0].pos)*ptpTug;

		points[points.size()-1].vel -= vTrans;
		points[0].vel += vTrans;
	}

	for(PointData &p: points) {
		p.vel += (basePoint - p.pos)*baseTug;
		p.vel *= dampening;

		p.pos += p.vel;
	}

	for(int i=0; i<points.size(); i++) {
		auto p = points[i].pos;
		if(p < 0)
			scalarPoints[i] = 0;
		else if(p > 1)
			scalarPoints[i] = 255;
		else
			scalarPoints[i] = 255*p;
	}
}
