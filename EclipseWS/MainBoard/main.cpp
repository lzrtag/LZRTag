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

#include "Localcode/IRComs/IR_RX.h"
#include "Localcode/IRComs/IR_TX.h"

#include "Localcode/ESPComs/ESPUART.h"


ESPComs::Endpoint ColorEP(101, &Board::Vest::team, 1, 0);
ESPComs::Endpoint VestBrightnessEP(200, &Board::Vest::mode, 1, 0);

struct {
	uint8_t length;
	uint8_t startFreq;
	uint8_t endFreq;
} buzzCommand;
void playPing() {
	Board::Buzzer::sweep(buzzCommand.startFreq*60, buzzCommand.endFreq*60, buzzCommand.length*10);
}
ESPComs::Endpoint PingEndpoint(11, &buzzCommand, 3, playPing);

uint8_t vibrDuration;
void playVibration() {
	Board::Vibrator::vibrate(vibrDuration*10);
}
ESPComs::Endpoint VibrationEP(10, &vibrDuration, 1, playVibration);

void IRRXCB(IR::ShotPacket data) {
}

int main() {
	_delay_ms(1500);
	ESPComs::init();

	Board::Vest::mode = 3;

	Connector::init();

	IR::RX::setCallback(IRRXCB);

	while(true) {
	}
	return 0;
}
