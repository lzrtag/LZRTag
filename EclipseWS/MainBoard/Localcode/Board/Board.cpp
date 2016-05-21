/*
 * Board.cpp
 *
 *  Created on: 11.05.2016
 *      Author: xasin
 */

#include "Board.h"

namespace Board {

	uint16_t nozzleDuration = 0;

	IRLed outputLED = IRLed();

	namespace Nozzle {
		void set(uint8_t color) {
			PORTD &= ~(0b111);
			PORTD |= color & 0b111;
		}

		void off() {
			PORTD &= ~(0b111);
		}

		void flash(uint8_t color) {
			flash(color, DEFAULT_FLASH_LEN);
		}
		void flash(uint8_t color, uint8_t duration) {
			set(color);
			nozzleDuration = duration * 4;
		}
	}

	namespace IRHandler {
		void on_receive() {
			if(IR::message == 0b11001100)
				PORTD |= (2);
		}
	}

	void ISR1a() {
		IR::update();

		if(nozzleDuration == 0) {}
		else if(--nozzleDuration == 0) {
			Nozzle::off();
		}
	}

	void init() {
		DDRD 	|= (0b01101111);	// Initialise inputs
		PORTD 	|= (0b10010000);	// Init pullups

		// Initialize the TIMER1 for 4kHz ISR
		Timer1::set_prescaler(TIMER1_PRESC_1);
		Timer1::set_OCR1A(499);
		Timer1::set_mode(TIMER1_MODE_CTC);

		IR::init(&PORTD, 7, &outputLED, &IRHandler::on_receive);

		sei();
	}
}
