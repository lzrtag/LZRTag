/*
 * Board.h
 *
 *  Created on: 28.05.2016
 *      Author: xasin
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef LOCALCODE_BOARD_H_
#define LOCALCODE_BOARD_H_

#define COLOR_RED 	0b001
#define COLOR_GREEN 0b010
#define COLOR_BLUE	0b100

#define COLOR_MAGENTA	(COLOR_RED | COLOR_BLUE)
#define COLOR_CYAN		(COLOR_GREEN | COLOR_BLUE)
#define COLOR_YELLOW	(COLOR_RED | COLOR_GREEN)
#define COLOR_WHITE		(COLOR_RED | COLOR_GREEN | COLOR_BLUE)

namespace Board {
	void init();

	void set_led(uint8_t color);
}



#endif /* LOCALCODE_BOARD_H_ */
