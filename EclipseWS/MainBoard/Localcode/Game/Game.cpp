/*
 * Game.cpp
 *
 *  Created on: 21.05.2016
 *      Author: xasin
 */

#include "Game.h"

namespace Game {

	uint16_t gamemode;

	uint8_t get_gun_cfg() {
		return (gamemode & 0b0111100000000000) >> 11;
	}
	uint8_t get_game_duration() {
		return (gamemode & 0b0000011110000000) >> 7;
	}
	uint8_t get_player_cfg() {
		return (gamemode & 0b0000000001111100) >> 2;
	}
	uint8_t get_friendlyfire() {
		return (gamemode & 0b10) >> 1;
	}
	uint8_t get_uses_teams() {
		return gamemode & 0b1;
	}

	namespace Weapon {
	uint16_t ammo, reloadTimer, shotTimer;
	}

	namespace Player {
	uint8_t ID;

	uint16_t life, lifeRegenTimer;
	uint16_t shield, shieldRegenTimer;
	}
}

