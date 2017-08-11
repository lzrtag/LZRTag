/*
 * VestBlink.h
 *
 *  Created on: Aug 1, 2017
 *      Author: xasin
 */

#ifndef LOCALCODE_BOARD_VESTBLINK_H_
#define LOCALCODE_BOARD_VESTBLINK_H_

#include <avr/io.h>

#define SOFTPWM_LEN 10

namespace Board {
namespace Vest {

extern uint8_t LEDPWM[3];

extern uint8_t mode;
extern uint8_t team;

struct BlinkOverrides {
	uint16_t duration;
	uint16_t mode;
};

extern BlinkOverrides overrides;

void update();
void slowUpdate();

}
}


#endif /* LOCALCODE_BOARD_VESTBLINK_H_ */
