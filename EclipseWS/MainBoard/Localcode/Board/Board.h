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

#include "IRLed.h"

#define COLOR_RED 		0b100
#define COLOR_GREEN		0b010
#define COLOR_BLUE		0b001

#define COLOR_MAGENTA	0b101
#define COLOR_CYAN		0b011
#define COLOR_YELLOW	0b110
#define COLOR_WHITE		0b111

#define DEFAULT_FLASH_LEN 20

namespace Board {

	void init();

	namespace Nozzle {
		void set(uint8_t color);

		void off();

		void flash(uint8_t color);
		void flash(uint8_t color, uint8_t duration);
	}

	void ISR1a();
}

#endif /* LOCALCODE_BOARD_BOARD_H_ */
