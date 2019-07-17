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

#include "Libcode/Communication/ESPUart/ESPUART.h"

volatile bool hasBeenShot = false;

uint8_t playerID = 255;

ESPComs::Endpoint PlayerIDEP(100, &playerID, 1, 0);

ESPComs::Endpoint ColorEP(101, &Board::Vest::team, 1, 0);
ESPComs::Endpoint VestBrightnessEP(200, &Board::Vest::mode, 1, 0);
ESPComs::Endpoint BrightnessOverrideEp(12, &Board::Vest::overrides, 3, 0);

struct BuzzCommand {
	uint16_t length;
	uint16_t startFreq;
	uint16_t endFreq;
};
void playPing() {
	BuzzCommand buzzCommand = *reinterpret_cast<BuzzCommand*>(&ESPComs::Endpoint::pubBuffer);

	Board::Buzzer::sweep(buzzCommand.startFreq, buzzCommand.endFreq, buzzCommand.length);
}
ESPComs::Endpoint PingEndpoint(11, &ESPComs::Endpoint::pubBuffer, 6, playPing);

void playVibration() {
	Board::Vibrator::vibrate(*(uint16_t *)&ESPComs::Endpoint::pubBuffer);
}
ESPComs::Endpoint VibrationEP(10, &ESPComs::Endpoint::pubBuffer, 2, playVibration);
void setVibrationMode() {
	Board::Vibrator::patternTiming = 0;
}
ESPComs::Endpoint VibrationPatternEP(110, &Board::Vibrator::patternMode, 1, setVibrationMode);


uint8_t currentShotID = 1;
void handleShots() {
	if(ESPComs::Endpoint::pubBuffer[0] == 0) {
		Board::Vibrator::vibrate(200);
		Board::Nozzle::flash(Board::Vest::team << 1);
		Board::Buzzer::sweep(3000, 1000, 150);
		IR::TX::startTX({playerID, currentShotID++});
		if(currentShotID == 16)
			currentShotID = 8;
	}
}
ESPComs::Endpoint ShootCommandEP(0, &ESPComs::Endpoint::pubBuffer, 1, handleShots);

IR::ShotPacket recShotData = {15, 15};
ESPComs::Source HitDetectSRC(1, &recShotData, 2);
void IRRXCB(IR::ShotPacket data) {
	if(data.playerID == playerID)
		return;
	recShotData = data;
	HitDetectSRC.fire();
}

int main() {
	DDRD 	|= 0b11;
	PORTD 	|= 0b11;

	Connector::init();

	_delay_ms(1500);
	ESPComs::init();
	ESPComs::onReset(Board::reset);

	Board::Vest::mode = 2;
	Board::Vest::team = 5;

	IR::RX::setCallback(IRRXCB);

	while(true) {
	}
	return 0;
}
