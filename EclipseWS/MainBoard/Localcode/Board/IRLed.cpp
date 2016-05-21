/*
 * IRLed.cpp
 *
 *  Created on: 12.05.2016
 *      Author: xasin
 */

#include "IRLed.h"

IRLed::IRLed() {
	DDRD |= (1<< 6);

	// Initialize the TIMER0 for IR-PWM mode
	Timer0::set_prescaler(TIMER0_PRESC_1);
	Timer0::set_mode(TIMER0_MODE_FREQ);
	Timer0::set_OCR0A(49);

}

void IRLed::on() {
	Timer0::set_OCA0_mode(TIMER0_OCA0_TOGGLE);
	this->status = 0x00;
}
void IRLed::off() {
	Timer0::set_OCA0_mode(TIMER0_OCA0_OFF);
	this->status = 0xff;
}
