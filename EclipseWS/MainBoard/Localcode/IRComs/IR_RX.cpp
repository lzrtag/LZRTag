/*
 * IR_RX.cpp
 *
 *  Created on: 22.07.2017
 *      Author: xasin
 */

#include "IR_RX.h"

namespace IR {
namespace RX {

ISR(TIMER1_CAPT_vect) {
	adjustTiming();
}

ISR(TIMER1_COMPB_vect) {
	update();
}

void adjustTiming() {

}

void update() {
}

void init() {

	// Initialize the Input Capture event!
	TCCR1B 	|= (1<< ICNC1);
	TIMSK1 	|= (1<< ICIE1);
}

}
}
