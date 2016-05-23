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
	namespace Config {
		uint8_t gun_cfg();
		uint8_t game_duration_cfg();
		uint8_t player_cfg();
		bool friendlyfire();
		bool uses_teams();
	}

	bool is_running();
	void update();
}

#endif /* LOCALCODE_GAME_GAME_H_ */
