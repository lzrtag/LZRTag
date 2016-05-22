/*
 * Game.cpp
 *
 *  Created on: 21.05.2016
 *      Author: xasin
 */

#include "Game.h"
#include "Player.h"
#include "Weapon.h"


namespace Game {

	uint16_t gamemode;
	uint32_t gameTimer;

	namespace Config {
		uint8_t gun_cfg() {
			return (gamemode & 0b0111100000000000) >> 11;
		}
		uint8_t game_duration() {
			return (gamemode & 0b0000011110000000) >> 7;
		}
		uint8_t player_cfg() {
			return (gamemode & 0b0000000001111100) >> 2;
		}
		bool friendlyfire() {
			return (gamemode & 0b10) >> 1;
		}
		bool uses_teams() {
			return gamemode & 0b1;
		}
	}

	void update() {
		gameTimer++;

		Weapon::update();
		Player::update();
	}
}

