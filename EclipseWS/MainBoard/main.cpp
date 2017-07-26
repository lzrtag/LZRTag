/*
 * main.cpp
 *
 *  Created on: May 9, 2016
 *      Author: xasin
 */


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "Libcode/TIMER/Timer0.h"
#include "Localcode/Board/Board.h"

#include "Localcode/Connector.h"

#include "Localcode/Game/Player.h"

#include "Localcode/IRComs/IR.h"
#include "Localcode/ESPComs/ESPUART.h"

uint8_t dbgColor = 0;
void setColor() {
	PORTB &= ~(0b111 << PB3);
	PORTB |= (dbgColor & 0b111) << PB3;
}
ESPComs::Endpoint DebugEndpoint(100, &dbgColor, 1, setColor);

uint8_t pingFreq = 0;
void playPing() {
	Board::Buzzer::sweep(pingFreq*50, pingFreq*50, 20);
}
ESPComs::Endpoint PingEndpoint(99, &pingFreq, 1, playPing);

int main() {
	_delay_ms(2000);
	ESPComs::init();

	Connector::init();

	DDRB |= (0b111 << PB3);

	IR::init(Connector::update);
	
  while(true) {
	}
	return 0;
}
