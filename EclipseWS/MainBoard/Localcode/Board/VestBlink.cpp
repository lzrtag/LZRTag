/*
 * VestBlink.cpp
 *
 *  Created on: Aug 1, 2017
 *      Author: xasin
 */

#include "Pins.h"
#include "VestBlink.h"

namespace Board {
namespace Vest {

uint8_t PWMPresc = SOFTPWM_LEN-1;
uint8_t LEDPWM[3];

uint8_t team = 0;
uint8_t mode = 1;
uint8_t phase = 0;

uint8_t slowPresc = 100;
void update() {
	if(PWMPresc == 0)
		PWMPresc = SOFTPWM_LEN;
	PWMPresc--;

	for(uint8_t i=0; i<3; i++) {
		if(LEDPWM[i] <= PWMPresc)
			VEST_PORTx &= ~((1<< VEST_R_PIN)<< i);
		else
			VEST_PORTx |=  ((1<< VEST_R_PIN) << i);
	}

	if(--slowPresc == 0) {
		slowPresc = 90;
		slowUpdate();
	}
}

void lightsOff() {
	for(uint8_t i=0; i<3; i++)
		LEDPWM[i] = 0;
}
void lightsOn() {
	for(uint8_t i=0; i<3; i++)
		LEDPWM[i] = SOFTPWM_LEN;
}
void teamColorOn(uint8_t brightness) {
	for(uint8_t i=0; i<3; i++)
		if(((1<<i) & team) != 0)
			LEDPWM[i] = brightness;
}

BlinkOverrides overrides = {0, 0};

void slowUpdate() {
	phase--;
	if(phase == 255)
		phase = 11;

	lightsOff();

	uint8_t lMode = mode;
	if(overrides.duration != 0) {
		overrides.duration--;
		lMode = overrides.mode;
	}

	switch(lMode) {
	case 0:
		if(phase == 0)
			teamColorOn(1);
	break;

	case 1:
		if((phase%6) == 0)
			teamColorOn(1);
	break;

	default:
	case 3:
		teamColorOn(2);
	case 2:
		if((phase%6) == 0)
			teamColorOn(SOFTPWM_LEN);
	break;

	case 4:
		teamColorOn(4);
		if((phase%6) == 0)
			teamColorOn(10);
	break;

	case 5:
		if(((phase & 1) == 0) && (phase < 6)) {
			lightsOn();
		}
		else
			teamColorOn(4);
	break;

	case 6:
		if(phase < 6)
			teamColorOn(phase/2);
		else
			teamColorOn(6 - phase/2);
	break;
	case 7:
		if(phase < 6)
			teamColorOn(phase + 1);
		else
			teamColorOn(12 - phase);
	break;

	case 10:
		if((phase & 0b1))
			lightsOn();
		else
			LEDPWM[team] = 10;
	break;
	}
}

}
}
