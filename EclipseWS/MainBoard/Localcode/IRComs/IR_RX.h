/*
 * IR_RX.h
 *
 *  Created on: 22.07.2017
 *      Author: xasin
 */

#ifndef LOCALCODE_IRCOMS_IR_RX_H_
#define LOCALCODE_IRCOMS_IR_RX_H_

#include <avr/io.h>
#include <avr/interrupt.h>

#include "IR.h"

namespace IR {
namespace RX {

extern void (*RXCallback)();

enum RXStates {
	RX_IDLE,
	RX_START,
	RX_DATA,
	RX_CHECKSUM
};

void adjustTiming();
void update();

void init();

}
}


#endif /* LOCALCODE_IRCOMS_IR_RX_H_ */
