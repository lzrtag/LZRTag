/*
 * Board.cpp
 *
 *  Created on: 28.05.2016
 *      Author: xasin
 */

#include "Board.h"

namespace Board {
	void init() {
		DDRB 	|= (0b111);
		PORTB 	|= (0b1000);

		TCCR0A |= (1<< WGM01);
		TCCR0B |= (1<< CS01);

		TIMSK0 |= (1<< OCIE0A);

		OCR0A = 139 -1;

		sei();
	}

	void set_led(uint8_t color) {
		PORTB &= ~(0b111);
		PORTB |= (color);
	}
}
