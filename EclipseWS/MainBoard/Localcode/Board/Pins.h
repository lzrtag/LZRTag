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

#define NOZZLE_R_PIN	1
#define NOZZLE_G_PIN	2
#define NOZZLE_B_PIN	3
#define NOZZLE_PORTx	PORTC
#define NOZZLE_DDRx		DDRC

#define VIBRATOR_PIN 	5
#define VIBRATOR_DDRx	DDRD
#define VIBRATOR_PORTx	PORTD

#define BUZZER_PIN 		3
#define BUZZER_DDRx		DDRD
#define BUZZER_PORTx 	PORTD

#endif

#endif /* LOCALCODE_BOARD_PINS_H_ */
