/*
 * main.cpp
 *
 *  Created on: May 9, 2016
 *      Author: xasin
 */


#include <avr/io.h>
#include <util/delay.h>


int main() {
	DDRD |= (1 << 6);

	while(true) {
		for(uint8_t i = 0; i < 50; i++) {
			PORTD |= (1<< 6);
			_delay_us(12);
			PORTD &= ~(1<< 6);
			_delay_us(13);
		}

		_delay_ms(100);
	}

	return 0;
}
