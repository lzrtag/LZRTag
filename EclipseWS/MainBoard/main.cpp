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

ISR(TIMER1_COMPA_vect) {
	Board::ISR1a();
}

int main() {
	Board::init();

	while(true) {
		_delay_ms(1000);
		IR::send_8(0b11001100);
	}

	return 0;
}
