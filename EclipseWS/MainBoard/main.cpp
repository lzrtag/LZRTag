/*
 * main.cpp
 *
 *  Created on: May 9, 2016
 *      Author: xasin
 */


#include <avr/io.h>
#include <util/delay.h>

#include "Libcode/TIMER/Timer0.h"

void IR_on() {
	Timer0::set_OCA0_mode(TIMER0_OCA0_TOGGLE);
}
void IR_off() {
	Timer0::set_OCA0_mode(TIMER0_OCA0_OFF);
}

int main() {
	DDRD |= (1);

	Timer0::set_prescaler(TIMER0_PRESC_1);
	Timer0::set_mode(TIMER0_MODE_FREQ);
	Timer0::set_OCR0A(49);

	while(true) {
		PORTD ^= (1);
		_delay_ms(1000);
	}

	return 0;
}
