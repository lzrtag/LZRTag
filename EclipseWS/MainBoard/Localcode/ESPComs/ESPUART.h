/*
 * ESPUART.h
 *
 *  Created on: 21.07.2017
 *      Author: xasin
 */

#ifndef LOCALCODE_ESPCOMS_ESPUART_H_
#define LOCALCODE_ESPCOMS_ESPUART_H_

#include <avr/io.h>
#include <avr/interrupt.h>

#include "Endpoint.h"
#include "Source.h"

namespace ESPComs {

#define ESP_BAUDRATE 31250
#define START_CHAR '!'

enum RXStates {
	WAIT_FOR_START_RX,
	RX_COMMAND,
	RX_DATA
};

enum TXStates {
	WAIT_FOR_START_TX,
	TX_IDLE,
	TX_SENDING,
};

void onReset(void (* const resetCallback)());

bool tryToStart(Source * txSource);
void init();

} /* namespace ESPComs */

#endif /* LOCALCODE_ESPCOMS_ESPUART_H_ */
