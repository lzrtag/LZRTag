/*
 * Board.cpp
 *
 *  Created on: 11.05.2016
 *      Author: xasin
 */

#include "Board.h"

namespace Board {

	uint16_t nozzleDuration = 0;

	void init() {
		DDRD 	|= (0b01101111);	// Initialise inputs
		PORTD 	|= (0b10010000);	// Init pullups


		// Initialize the TIMER0 for IR-PWM mode
		Timer0::set_prescaler(TIMER0_PRESC_1);
		Timer0::set_mode(TIMER0_MODE_FREQ);
		Timer0::set_OCR0A(49);

		// Initialize the TIMER1 for 4kHz ISR
		Timer1::set_prescaler(TIMER1_PRESC_1);
		Timer1::set_OCR1A(499);
		Timer1::set_mode(TIMER1_MODE_CTC);

		sei();

	}

	void set_nozzle_LED(uint8_t color) {
		PORTD &= ~(0b111);
		PORTD |= color & 0b111;
	}

	void flash_nozzle_LED(uint8_t color) {
		flash_nozzle_LED(color, DEFAULT_FLASH_LEN);
	}
	void flash_nozzle_LED(uint8_t color, uint8_t duration) {
		set_nozzle_LED(color);
		nozzleDuration = duration * 4;
	}

	void ISR1a() {
		if(nozzleDuration == 0) {}
		else if(--nozzleDuration == 0) {
			PORTD &= ~(0b111);
		}
	}
}
