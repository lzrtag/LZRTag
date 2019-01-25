/*
 * ManeAnimator.h
 *
 *  Created on: 13 Nov 2018
 *      Author: xasin
 */

#ifndef MAIN_MANEANIMATOR_H_
#define MAIN_MANEANIMATOR_H_

#include <vector>

class ManeAnimator {
public:
	struct PointData {
		float pos;
		float vel;
	};

	std::vector<PointData> points;
	std::vector<uint8_t>   scalarPoints;

	float basePoint;
	float baseTug;

	float ptpTug;

	float dampening;

	bool wrap;

	ManeAnimator(const int length);

	void tick();
};

#endif /* MAIN_MANEANIMATOR_H_ */
