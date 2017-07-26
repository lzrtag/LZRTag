/*
 * IR_TX.cpp
 *
 *  Created on: 26.07.2017
 *      Author: xasin
 */

#include "IR_TX.h"

namespace IR {
namespace TX {

IRStates 	TXState = IDLE;
uint8_t 	segmentPosition = 0;
uint16_t 	data			= 0;
uint8_t 	checksum		= CHECKSUM_START_VAL;

void setIRLED(bool state) {
	if(state)
		DDRD |= (1<< PD6);
	else
		DDRD &= ~(1<< PD6);
}

void startTX(ShotPacket TXData) {
	if(TXState != IDLE)
		return;

	data = *(uint16_t *)&TXData;
	TXState = START;
}
void stopTX() {
	setIRLED(false);

	TXState = IDLE;

	segmentPosition = 0;
	data			= 0;
	checksum		= CHECKSUM_START_VAL;
}


void update() {
	switch(TXState) {
	case IDLE: break;

	case START:
		setIRLED((START_BITS >> (segmentPosition++)) & 1);

		if(segmentPosition == START_FRAMES) {
			segmentPosition = 0;
			TXState = DATA;
		}
	break;

	case DATA:
		if((data >> (segmentPosition++)) & 1) {
			setIRLED(true);
			checksum += 1;
		}
		else
			setIRLED(false);

		if(segmentPosition == DATA_BITS) {
			segmentPosition = 0;

			TXState = CHECKSUM;
		}
	break;

	case CHECKSUM:
		if(segmentPosition == CHECKSUM_FRAMES)
			stopTX();

		setIRLED((checksum >> (segmentPosition++)) & 1);
	break;
	}
}

void init() {
	OCR0A = IR_TICKS - 1;

	// Fast-PWM with OCR0A as TOP, OC0A toggle at match.
	TCCR0A |= (1<< COM0A0 | 1<< WGM01 | 1<< WGM00);
	// Clock-Prescaler to 1
	TCCR0B |= (1<< WGM02 | 1<< CS00);
}

}
}
