/*
 * Board.cpp
 *
 *  Created on: 11.05.2016
 *      Author: xasin
 */

#include "Board.h"
#include "../Game/Game.h"


namespace Board {
	IRLed outputLED = IRLed();

	namespace Nozzle {
		uint16_t nozzleDuration = 0;

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
			nozzleDuration = duration;
		}

		void update() {
			if(nozzleDuration == 1)
				Nozzle::off();
			if(nozzleDuration != 0) nozzleDuration--;
		}
	}

	namespace Vibrator {
		uint16_t vibratorDuration = 0;

		void off() {
			VIBRATOR_PORTx &= ~(1<< VIBRATOR_PIN);
		}

		void on() {
			VIBRATOR_PORTx |= (1<< VIBRATOR_PIN);
		}

		void vibrate(uint16_t duration) {
			on();
			vibratorDuration += duration;
		}

		void update() {
			if(vibratorDuration == 1)
				off();
			if(vibratorDuration != 0)
				vibratorDuration--;
		}
	}

	namespace Buzzer {
		uint16_t 	endFreq = 0;
		uint16_t 	durationDiv = 0;
		int16_t		percentShift = 0;
		uint16_t 	buzzerDuration = 0;

		void off() {
			Timer2::set_OC2B_mode(TIMER2_OC2B_OFF);
		}
		void on() {
			Timer2::set_OC2B_mode(TIMER2_OC2B_NPWM_TOGGLE);
		}

		void set_frequency(uint16_t freq) {
			Timer2::set_OCR2A((uint8_t)(F_CPU / 2 / 32 / freq) -1);
		}

		void sweep(uint16_t start, uint16_t end, uint16_t duration) {
			endFreq = end;
			durationDiv = duration / 100;
			percentShift = (start - end) / 100;

			buzzerDuration = duration;
			set_frequency(start);
			on();
		}

		void update() {
			if(buzzerDuration == 1)
				off();
			if(buzzerDuration > 1)
				set_frequency(endFreq + percentShift * (buzzerDuration/durationDiv));

			if(buzzerDuration != 0)
				buzzerDuration--;
		}
	}



	void ISR1a() {
		IR::update();

		Nozzle::update();
		Vibrator::update();
		Buzzer::update();
	}

	void init() {
		TRIGGER_PORTx 	|= (1<< TRIGGER_PIN);
		TRANSMIT_DDRx 	|= (1<< TRANSMIT_PIN);
		NOZZLE_DDRx 	|= (COLOR_WHITE);
		VIBRATOR_DDRx 	|= (1<< VIBRATOR_PIN);
		BUZZER_DDRx 	|= (1<< BUZZER_PIN);

		// Initialize the TIMER1 for 1kHz ISR
		Timer1::set_prescaler(TIMER1_PRESC_1);
		Timer1::set_OCR1A(3999);
		Timer1::set_mode(TIMER1_MODE_CTC);

		// Initialize the TIMER2 for Buzzer PWM
		Timer2::set_prescaler(TIMER2_PRESC_32);
		Timer2::set_mode(TIMER2_MODE_TOP_OCR2A);
		Timer2::set_OCR2A(50);

		IR::init(&RECEIVER_PORTx, RECEIVER_PIN, &outputLED, 0);

		sei();
	}
}
