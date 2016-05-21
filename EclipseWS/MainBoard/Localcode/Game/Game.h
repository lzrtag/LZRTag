/*
 * Game.h
 *
 *  Created on: 21.05.2016
 *      Author: xasin
 */

#ifndef LOCALCODE_GAME_GAME_H_
#define LOCALCODE_GAME_GAME_H_

#include <avr/io.h>

namespace Game {

	extern uint16_t gamemode;

	namespace Weapon {
	extern uint16_t ammo, reloadTimer, shotTimer;
	}

	namespace Player {
	extern uint8_t ID;

	extern uint16_t life, lifeRegenTimer;
	extern uint16_t shield, shieldRegenTimer;
	}
}

#endif /* LOCALCODE_GAME_GAME_H_ */
