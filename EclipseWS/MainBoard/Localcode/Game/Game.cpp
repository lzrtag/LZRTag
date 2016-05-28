/*
 * Game.cpp
 *
 *  Created on: 21.05.2016
 *      Author: xasin
 */

#include "../Board/Board.h"

#include "Game.h"
#include "Player.h"
#include "Weapon.h"


namespace Game {

	const uint32_t gameTimesTable[1] = {0};

	uint16_t gamemode = 1;
	uint32_t gameTimer = 0;

	namespace Config {
		uint8_t gun_cfg() {
			return (gamemode & 0b0111100000000000) >> 11;
		}
		uint8_t game_duration_cfg() {
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

	void start() {
		gameTimer = 1;
	}

	bool is_running() {
		if(gameTimer == 0)
			return false;
		if(gameTimesTable[Config::game_duration_cfg()] == 0)
			return true;
		if(gameTimer > gameTimesTable[Config::game_duration_cfg()])
			return false;


		return true;
	}

	void update() {
		if(gameTimer != 0) gameTimer++;

		Weapon::update();
		Player::update();
	}
}

