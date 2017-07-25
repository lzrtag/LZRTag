/*
 * ESPUART.cpp
 *
 *  Created on: 21.07.2017
 *      Author: xasin
 */

#include "ESPUART.h"

namespace ESPComs {

RXStates RXStatus = WAIT_FOR_START_RX;
TXStates TXStatus = WAIT_FOR_START_TX;

uint8_t * 	RXData;
uint8_t 	RXToReceive = 0;

Endpoint * RXEndpoint = 0;

ISR(USART_RX_vect) {
	uint8_t rData = UDR0;

	switch(RXStatus) {
	case WAIT_FOR_START_RX:
		if(rData == START_CHAR) {
			RXStatus = RX_COMMAND;
			UDR0 = START_CHAR;
		}
	break;

	case RX_COMMAND:
		RXEndpoint = Endpoint::getHeadEndpoint();
		while(1) {
			if(RXEndpoint->targetCommand == rData) {
				RXData = RXEndpoint->targetData;
				RXToReceive = RXEndpoint->dataLength;
				break;
			}
			RXEndpoint = RXEndpoint->nextEndpoint;
			if(RXEndpoint == 0)
				return;
		}

		if(RXToReceive != 0)
			RXStatus = RX_DATA;
		else
			RXEndpoint->callCallback();
	break;

	case RX_DATA:
		*(RXData++) = rData;
		if(--RXToReceive == 0) {
			RXStatus = RX_COMMAND;
			RXEndpoint->callCallback();
		}
	break;
	}
}

uint8_t * 	TXData;
uint8_t 	TXToTransmit = 0;
ISR(USART_TX_vect) {
	switch(TXStatus) {
	case WAIT_FOR_START_TX:
		TXStatus = TX_COMMAND;
	break;
	}
}

ISR(USART_UDRE_vect) {
	if(TXToTransmit == 0)
		UCSR0B &= ~(1<< UDRIE0);
}

void init() {
	// Init of the baudrate register (Asynchronous normal)
	UBRR0 = F_CPU/16/ESP_BAUDRATE -1;

	// UART Initialisation
	UCSR0B = (1<< RXEN0 | 1<< TXEN0 | 1<<RXCIE0 | 1<<TXCIE0);
	UCSR0C = (1<< UCSZ01 | 1<< UCSZ00);
}

}
