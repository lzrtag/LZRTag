/*
 * IR_RX.cpp
 *
 *  Created on: 22.07.2017
 *      Author: xasin
 */

#include "IR_RX.h"

namespace IR {
namespace RX {

void (* RXCallback)(ShotPacket rxData) = 0;

IRStates 	RXState 		= IDLE;
uint8_t 	segmentPosition = 0;
uint16_t 	data			= 0;
uint8_t 	checksum		= 0;

bool getPinRX() {
	return !(PINB & 1);
}

ISR(TIMER1_CAPT_vect) {
	adjustTiming();
}

ISR(TIMER1_COMPB_vect) {
	update();
}

void startRX() {
	RXState = START;
	TIMSK1 |= (1<< OCIE2B);
}
void stopRX() {
	TIMSK1 &= ~(1<< OCIE2B);

	RXState = IDLE;
	segmentPosition = 0;
	data			= 0;
	checksum		= CHECKSUM_START_VAL;
}

void adjustTiming() {
	if(RXState == IDLE) {
		startRX();
	}
	// Adjust the frame timing a little forwards to re-synch with the signal
	OCR1B = ICR1 + (ICR1 < FRAME_TICKS/2 ? FRAME_TICKS/2 : -FRAME_TICKS/2);
}

void update() {
	switch(RXState) {
	case IDLE: stopRX(); break;

	case START:
		if(getPinRX() != ((START_BITS >> (segmentPosition++)) & 1) ) {
			stopRX();
			return;
		}

		if(segmentPosition == START_FRAMES) {
			segmentPosition = 0;
			RXState = DATA;
		}
	break;

	case DATA:
		if(getPinRX()) {
			data |= (1<< segmentPosition);
			checksum++;
		}
		segmentPosition++;

		if(segmentPosition == DATA_BITS) {
			segmentPosition = 0;
			RXState = CHECKSUM;
		}
	break;

	case CHECKSUM:
		if(getPinRX() != ((checksum >> (segmentPosition++)) & 1) ) {
			stopRX();
			return;
		}

		if(segmentPosition == CHECKSUM_FRAMES) {
			if(RXCallback != 0)
				RXCallback(*(ShotPacket *)&data);
			stopRX();
		}
	break;
	}
}

void init() {
	// Pull-Up on ICP1
	PORTB |= (1);

	OCR1A = FRAME_TICKS -1;

	// Initialize the Input Capture event - Noise Canceler on, Falling Edge. CTC with OCR1A as TOP, no prescaler.
	TCCR1B 	|= (1<< ICNC1 | 1<< WGM12 | 1<<CS10);
	TIMSK1 	|= (1<< ICIE1);
}

}
}
