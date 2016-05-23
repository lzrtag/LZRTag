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

	uint8_t ISR1aPRESC = 0;

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

	namespace IRHandler {
		void on_receive() {
		}
	}

	void ISR1a() {
		IR::update();

		if(nozzleDuration == 0) {}
		else if(--nozzleDuration == 0) {
			Nozzle::off();
		}

		if(++ISR1aPRESC == 4) {
			ISR1aPRESC = 0;

			Game::update();
		}
	}

	void init() {
		TRIGGER_PORTx 	|= (1<< TRIGGER_PIN);
		TRANSMIT_DDRx 	|= (1<< TRANSMIT_PIN);
		NOZZLE_DDRx 	|= (COLOR_WHITE);

		DDRD 	|= (0b01101111);	// Initialise inputs
		PORTD 	|= (0b10010000);	// Init pullups

		// Initialize the TIMER1 for 4kHz ISR
		Timer1::set_prescaler(TIMER1_PRESC_1);
		Timer1::set_OCR1A(499);
		Timer1::set_mode(TIMER1_MODE_CTC);

		IR::init(&RECEIVER_PORTx, RECEIVER_PIN, &outputLED, &IRHandler::on_receive);

		sei();
	}
}
