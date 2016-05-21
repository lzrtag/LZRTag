/*
 * IRLed.h
 *
 *  Created on: 12.05.2016
 *      Author: xasin
 */

#ifndef LOCALCODE_BOARD_IRLED_H_
#define LOCALCODE_BOARD_IRLED_H_

#include "../../Libcode/Communication/IR/IR_LED.h"
#include "../../Libcode/TIMER/Timer0.h"

#include <avr/io.h>

class IRLed: public IR_LED {
public:
	uint8_t status = 0xff;

	IRLed();

	void on();
	void off();
};

#endif /* LOCALCODE_BOARD_IRLED_H_ */
