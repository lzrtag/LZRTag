/*
 * main.cpp
 *
 *  Created on: 28.05.2016
 *      Author: xasin
 */

#include <avr/io.h>
#include <util/delay.h>

#include <avr/interrupt.h>

#include "IR/IR_Handler.h"

#include "Localcode/Board.h"

void on_received() {
	switch(IR::message & 0b11) {
	case 0:
		Board::set_led(COLOR_WHITE);
	break;

	case 1:
		Board::set_led(COLOR_RED);
	break;

	case 2:
		Board::set_led(COLOR_GREEN);
	break;

	case 3:
		Board::set_led(COLOR_BLUE);
	break;
	}

	_delay_ms(1);
	Board::set_led(0);
}

uint8_t sPresc = 1;
ISR(TIM0_COMPA_vect) {
	if(--sPresc == 0) {
		sPresc = 4;
		IR::receive();
	}
}

int main() {

	IR::init(&PORTB, PB3, 0, &on_received);

	Board::init();

	while(true) {
	}

	return 0;
}
