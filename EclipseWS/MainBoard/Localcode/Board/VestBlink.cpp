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
		slowPresc = 100;
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

uint8_t team = 0;
uint8_t mode = 1;
uint8_t phase = 0;
void slowUpdate() {
	phase--;
	if(phase == 255)
		phase = 11;

	lightsOff();

	switch(mode) {
	case 0:
		if(phase == 0)
			LEDPWM[team] = 1;
	break;

	case 1:
		if((phase%6) == 0)
			LEDPWM[team] = 1;
	break;

	case 3:
		LEDPWM[team] = 2;
	case 2:
		if((phase%6) == 0)
			LEDPWM[team] = 10;
	break;

	case 4:
		LEDPWM[team] = 4;
		if((phase%6) == 0)
			LEDPWM[team] = 10;
	break;

	case 5:
		if((phase%2 == 0) && (phase < 6)) {
			lightsOn();
		}
		else
			LEDPWM[team] = 10;
	break;
	}
}

}
}

