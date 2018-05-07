/*
 * ESPUART.cpp
 *
 *  Created on: 21.07.2017
 *      Author: xasin
 */

#include "ESPUART.h"

namespace ESPComs {

#define UDREI_ON 	UCSR0B |= (1<< UDRIE0)
#define UDREI_OFF	UCSR0B &= ~(1<< UDRIE0)

void (* resetCallback)() = 0;

RXStates RXStatus = WAIT_FOR_START_RX;
TXStates TXStatus = WAIT_FOR_START_TX;

uint8_t * 	RXData;
uint8_t 	RXToReceive = 0;

Endpoint * 	RXEndpoint = 0;

uint8_t * 	TXData;
uint8_t 	TXToTransmit = 0;

Source 	*	TXSource = 0;

ISR(USART_RX_vect) {
	uint8_t rData = UDR0;

	switch(RXStatus) {
	case WAIT_FOR_START_RX:
		if(rData == START_CHAR) {
			RXStatus = RX_COMMAND;
			UDR0 = START_CHAR;
			UDREI_ON;
		}
	break;

	case RX_COMMAND:
		if(rData == START_CHAR) {
			UDR0 = START_CHAR;
			UDREI_ON;
			if(resetCallback != 0)
				resetCallback();
			return;
		}

		RXEndpoint = Endpoint::getHeadEndpoint();
		while(1) {
			if(RXEndpoint == 0)
				return;
			if(RXEndpoint->targetCommand == rData) {
				RXData = (uint8_t*)RXEndpoint->targetData;
				RXToReceive = RXEndpoint->dataLength;
				break;
			}
			RXEndpoint = RXEndpoint->nextEndpoint;
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

void startTX() {
	TXStatus 	 = TX_SENDING;
	TXData 		 = (uint8_t*)TXSource->targetData;
	TXToTransmit = TXSource->dataLength;

	UDR0 = TXSource->targetCommand;
	UDREI_ON;
}

void nextTXSource() {
	TXSource = Source::getHeadSource();
	while(TXSource != 0) {
		if(TXSource->ignited) {
			startTX();
			TXSource->ignited = false;
			return;
		}
		TXSource = TXSource->nextSource;
	}

	UDREI_OFF;
	TXStatus = TX_IDLE;
}

void onReset(void (* const newResetCallback)()) {
	resetCallback = newResetCallback;
}

bool tryToStart(Source * txSource) {
	if(TXStatus != TX_IDLE)
		return false;

	TXSource = txSource;
	startTX();

	return true;
}

ISR(USART_UDRE_vect) {
	switch(TXStatus) {
	case TX_SENDING:
		if(TXToTransmit != 0) {
			UDR0 = *(TXData++);
			TXToTransmit--;
			return;
		}

	case TX_IDLE:
	case WAIT_FOR_START_TX:
		nextTXSource();
	break;
	}
}

void init() {
	// Init of the baudrate register (Asynchronous normal)
	UBRR0 = F_CPU/16/ESP_BAUDRATE -1;

	// UART Initialisation
	UCSR0B = (1<< RXEN0 | 1<< TXEN0 | 1<<RXCIE0);
	UCSR0C = (1<< UCSZ01 | 1<< UCSZ00);

	DDRD 	&= ~(1);
	PORTD 	|= (1);
}

}
