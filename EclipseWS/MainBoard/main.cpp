/*
 * main.cpp
 *
 *  Created on: May 9, 2016
 *      Author: xasin
 */


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "Libcode/TIMER/Timer0.h"

#include "Localcode/Board/Board.h"

void IR_on() {
	Timer0::set_OCA0_mode(TIMER0_OCA0_TOGGLE);
}
void IR_off() {
	Timer0::set_OCA0_mode(TIMER0_OCA0_OFF);
}

ISR(TIMER1_COMPA_vect) {
	Board::ISR1a();
}

int main() {
	Board::init();

	uint8_t cs[2] = {COLOR_CYAN, COLOR_YELLOW};

	while(true) {
		for(uint8_t j=0; j < 2; j++) {
			for(uint8_t i = 0; i < 3; i++) {				Board::flash_nozzle_LED(cs[j]);

				_delay_ms(100);
			}

			_delay_ms(300);
		}
	}

	return 0;
}
