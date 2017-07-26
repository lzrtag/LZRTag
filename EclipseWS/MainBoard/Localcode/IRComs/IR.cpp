/*
 * IR.cpp
 *
 *  Created on: 26.07.2017
 *      Author: xasin
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "IR.h"
#include "IR_TX.h"

namespace IR {

void (*millisecCallback)() = 0;

uint8_t msPresc = F_FRAME/1000;
ISR(TIMER1_COMPA_vect) {
	IR::TX::update();

	if(--msPresc == 0) {
		msPresc = F_FRAME/1000;
		millisecCallback();
	}
}

void init(void (*msCallback)()) {
	OCR0A = IR_TICKS/2 - 1;

	// Fast-PWM with OCR0A as TOP, OC0A toggle at match.
	TCCR0A |= (1<< COM0A0 | 1<< WGM01 | 1<< WGM00);
	// Clock-Prescaler to 1
	TCCR0B |= (1<< WGM02 | 1<< CS00);


	// Pull-Up on ICP1
	PORTB |= (1);

	OCR1A = FRAME_TICKS -1;

	// Initialize the Input Capture event - Noise Canceler on, Falling Edge. CTC with OCR1A as TOP, no prescaler.
	TCCR1B 	|= (1<< ICNC1 | 1<< WGM12 | 1<<CS10);
	TIMSK1 	|= (1<< ICIE1 | 1<< OCIE1A);

	millisecCallback = msCallback;
}

}
