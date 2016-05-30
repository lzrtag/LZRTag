/*
 * Pins.h
 *
 *  Created on: 23.05.2016
 *      Author: xasin
 */

#ifndef LOCALCODE_BOARD_PINS_H_
#define LOCALCODE_BOARD_PINS_H_

#define PROTOTYPE_1_BOARD


#ifdef PROTOTYPE_1_BOARD

#define TRIGGER_PIN 	4
#define TRIGGER_PINx 	PIND
#define TRIGGER_PORTx	PORTD

#define TRANSMIT_PIN	6
#define TRANSMIT_PORTx	PORTD
#define TRANSMIT_DDRx	DDRD

#define RECEIVER_PIN 	7
#define RECEIVER_PORTx 	PORTD

#define NOZZLE_R_PIN	2
#define NOZZLE_G_PIN	1
#define NOZZLE_B_PIN	0
#define NOZZLE_PORTx	PORTD
#define NOZZLE_DDRx		DDRD

#endif

#endif /* LOCALCODE_BOARD_PINS_H_ */
