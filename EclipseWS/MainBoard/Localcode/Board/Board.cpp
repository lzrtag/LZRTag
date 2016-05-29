/*
 * Board.cpp
 *
 *  Created on: 11.05.2016
 *      Author: xasin
 */

#include "Board.h"
#include "../Game/Game.h"


namespace Board {

	uint16_t nozzleDuration = 0;

	IRLed outputLED = IRLed();

	namespace Nozzle {
		void set(uint8_t color) {
			NOZZLE_PORTx &= ~(COLOR_WHITE);
			NOZZLE_PORTx |= color;
		}

		void off() {
			NOZZLE_PORTx &= ~(COLOR_WHITE);
		}

		void flash(uint8_t color) {
			flash(color, DEFAULT_FLASH_LEN);
		}
		void flash(uint8_t color, uint8_t duration) {
			set(color);
			nozzleDuration = duration * 4;
		}
	}

	void ISR1a() {
		IR::update();

		if(nozzleDuration == 1)
			Nozzle::off();

		if(nozzleDuration != 0) nozzleDuration--;
	}

	void init() {
		TRIGGER_PORTx 	|= (1<< TRIGGER_PIN);
		TRANSMIT_DDRx 	|= (1<< TRANSMIT_PIN);
		NOZZLE_DDRx 	|= (COLOR_WHITE);

		// Initialize the TIMER1 for 2kHz ISR
		Timer1::set_prescaler(TIMER1_PRESC_1);
		Timer1::set_OCR1A(1999);
		Timer1::set_mode(TIMER1_MODE_CTC);

		IR::init(&RECEIVER_PORTx, RECEIVER_PIN, &outputLED, 0);

		sei();
	}
}
