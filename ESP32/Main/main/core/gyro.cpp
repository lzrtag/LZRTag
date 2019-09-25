/*
 * gyro.cpp
 *
 *  Created on: 25 Sep 2019
 *      Author: xasin
 */


#include "gyro.h"
#include "setup.h"

#include <cmath>
#include <cstring>

namespace LZR {
namespace GYR {

void init() {
	gyro.init();
}
void tick() {
	gyro.update();
	}

}

}
}
