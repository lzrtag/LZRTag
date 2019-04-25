/*
 * IR_RX.cpp
 *
 *  Created on: 22.07.2017
 *      Author: xasin
 */

#include "IR_RX.h"

#include "../Board/VestBlink.h"

namespace IR {
namespace RX {

void (* RXCallback)(ShotPacket rxData) = 0;

IRStates 	RXState 		= IDLE;
uint8_t 	segmentPosition = 0;
uint16_t 	data			= 0;
uint8_t 	checksum		= 0;

bool getPinRX() {
	return (PINB & 1) == 0;
}

ISR(TIMER1_CAPT_vect) {
	adjustTiming();
}

ISR(TIMER1_COMPB_vect) {
	update();
}

void startRX() {
	RXState = START;
	TIFR1  |= (1<< OCF1B);
	TIMSK1 |= (1<< OCIE2B);
}
void stopRX() {
	TIMSK1 &= ~(1<< OCIE2B);

	RXState = IDLE;
	segmentPosition = 0;
	data			= 0;
	checksum		= 0;
}

void adjustTiming() {
	// Adjust the frame timing a little forwards to re-synch with the signal
	uint16_t tempOCR1B = ICR1 + FRAME_TICKS/4;
	if(tempOCR1B >= FRAME_TICKS)
		tempOCR1B -= FRAME_TICKS;
	OCR1B = tempOCR1B;

	if(RXState == IDLE)
		startRX();
}

void update() {
	bool RXInput = getPinRX();

	switch(RXState) {
	case IDLE: stopRX(); break;

	case START:
		if(RXInput != ((START_BITS >> (segmentPosition)) & 1) ) {
			stopRX();
			return;
		}
		segmentPosition++;

		if(segmentPosition == START_FRAMES) {
			segmentPosition = 0;
			RXState = DATA;
		}
	break;

	case DATA:
		if(RXInput)
			data |= (1<< segmentPosition);
		segmentPosition++;

		if(segmentPosition == DATA_BITS) {
			segmentPosition = 0;
			RXState = CHECKSUM;

			for(uint8_t i=0; i<3; i++)
				checksum += 0xF & (data >> (i*4));
		}
	break;

	case CHECKSUM:
		if(RXInput != ((checksum >> (segmentPosition)) & 1) ) {
			stopRX();
			return;
		}
		segmentPosition++;

		if(segmentPosition == CHECKSUM_FRAMES) {
			if(RXCallback != 0)
				RXCallback(*(ShotPacket *)&data);
			stopRX();
		}
	break;
	}
}

void setCallback(void (*callback)(ShotPacket data)) {
	RXCallback = callback;
}

}
}
