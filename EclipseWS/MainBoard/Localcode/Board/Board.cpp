/*
 * Board.cpp
 *
 *  Created on: 11.05.2016
 *      Author: xasin
 */

#include "Board.h"
#include "../Game/Game.h"


namespace Board {
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
		uint8_t vibratorMode = 1;

		void off() {
			VIBRATOR_PORTx &= ~(1<< VIBRATOR_PIN);
		}

		void on() {
			VIBRATOR_PORTx |= (1<< VIBRATOR_PIN);
		}

		void vibrate(uint16_t duration) {
			if(duration == 0) {
				vibratorDuration = 0;
				off();
			}
			else {
				on();
				if(duration > vibratorDuration) {
					vibratorDuration = duration;
				}
			}
		}

		void update() {
			if(vibratorDuration != 0) {
				vibratorDuration--;
				switch(vibratorMode) {
				default: break;
				case 1:
					if(vibratorDuration & 0b100000)
						off();
					else
						on();
				}
			}
			else
				off();
		}
	}

	namespace Buzzer {
		uint16_t 	endFreq 	 = 0;
		uint16_t 	fullDuration = 0;
		int16_t		freqShift = 0;
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
			fullDuration = duration;
			freqShift = (start - end);

			buzzerDuration = duration;
			set_frequency(start);
			on();
		}

		void update() {
			if(buzzerDuration == 1)
				off();
			if(buzzerDuration > 1)
				set_frequency(endFreq + ((freqShift * (((uint32_t)buzzerDuration<<12)/fullDuration))>>12));

			if(buzzerDuration != 0)
				buzzerDuration--;
		}
	}

	namespace Trigger {
		uint8_t triggerDebounce = 0;
		bool triggerStatus = 0;

		ESPComs::Source TriggerPressS(0, &triggerStatus, 1);

		bool getTrigger() {
			return (TRIGGER_PINx & (1<< TRIGGER_PIN)) != 0;
		}

		void update() {
			if(triggerDebounce != 0)
				triggerDebounce--;
			else {
				if(triggerStatus != getTrigger()) {
					triggerDebounce = 30;
					triggerStatus = getTrigger();
					TriggerPressS.fire();
				}
			}
		}
	}

	void reset() {
		Vibrator::vibrate(0);

		Buzzer::buzzerDuration = 0;
		Buzzer::off();
	}

	void ISR1a() {
		Nozzle::update();
		Vibrator::update();
		Buzzer::update();
		Vest::update();
		Trigger::update();
	}

	void init() {
		TRIGGER_PORTx 	|= (1<< TRIGGER_PIN);
		NOZZLE_DDRx 	|= (COLOR_WHITE);
		VIBRATOR_DDRx 	|= (1<< VIBRATOR_PIN);
		BUZZER_DDRx 	|= (1<< BUZZER_PIN);

		VEST_DDRx		|= (0b111 << VEST_R_PIN);

		IR::init(Connector::update);

		// Initialize the TIMER2 for Buzzer PWM
		Timer2::set_prescaler(TIMER2_PRESC_32);
		Timer2::set_mode(TIMER2_MODE_TOP_OCR2A);
		Timer2::set_OCR2A(50);

		sei();
	}
}
