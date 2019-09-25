/*
 * gyro.h
 *
 *  Created on: 25 Sep 2019
 *      Author: xasin
 */

#ifndef MAIN_CORE_GYRO_H_
#define MAIN_CORE_GYRO_H_

namespace LZR {
namespace GYR {

enum position_t {
	UNKNOWN,
	LAID_DOWN,
	POINT_UP,
	POINT_DOWN,
	ACTIVE,
};

void init();
void tick();

}
}


#endif /* MAIN_CORE_GYRO_H_ */
