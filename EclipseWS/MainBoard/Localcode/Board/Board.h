/*
 * Board.h
 *
 *  Created on: 11.05.2016
 *      Author: xasin
 */

#ifndef LOCALCODE_BOARD_BOARD_H_
#define LOCALCODE_BOARD_BOARD_H_

#include <avr/io.h>
#include <avr/interrupt.h>

#include "../../Libcode/TIMER/Timer0.h"
#include "../../Libcode/TIMER/Timer1.h"
#include "../../Libcode/TIMER/Timer2.h"

#include "../IRComs/IR.h"

#include "../Connector.h"

#include "Pins.h"

#define COLOR_RED 		(1<< NOZZLE_R_PIN)
#define COLOR_GREEN		(1<< NOZZLE_G_PIN)
#define COLOR_BLUE		(1<< NOZZLE_B_PIN)

#define COLOR_MAGENTA	(COLOR_RED | COLOR_BLUE)
#define COLOR_CYAN		(COLOR_GREEN | COLOR_BLUE)
#define COLOR_YELLOW	(COLOR_RED | COLOR_GREEN)
#define COLOR_WHITE		(COLOR_RED | COLOR_GREEN | COLOR_BLUE)

#define DEFAULT_FLASH_LEN 20

namespace Board {

	void init();

	namespace Nozzle {
		void set(uint8_t color);

		void off();

		void flash(uint8_t color);
		void flash(uint8_t color, uint8_t duration);
	}

	namespace Vibrator {
		void vibrate(uint16_t duration);
	}

	namespace Buzzer {
		void on();
		void off();

		void set_frequency(uint16_t);

		void sweep(uint16_t start, uint16_t end, uint16_t duration);
	}

	void ISR1a();
}

#endif /* LOCALCODE_BOARD_BOARD_H_ */
