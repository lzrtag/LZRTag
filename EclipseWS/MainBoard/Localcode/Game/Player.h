/*
 * Player.h
 *
 *  Created on: 22.05.2016
 *      Author: xasin
 */

#ifndef LOCALCODE_GAME_PLAYER_H_
#define LOCALCODE_GAME_PLAYER_H_

#include <avr/io.h>

namespace Game {
namespace Player {
	extern uint8_t ID;

	extern uint16_t life, lifeRegenTimer;
	extern uint16_t shield, shieldRegenTimer;

	extern void (*on_death)();
	extern void (*on_hit)();

	uint8_t get_team();
	uint8_t get_team_color();

	uint8_t get_life_percent();
	uint8_t get_shield_percent();

	bool has_full_life();
	bool has_full_shield();

	void set_team(uint8_t team);
	void set_number(uint8_t number);

	bool is_alive();
	void take_hit(uint8_t hitSignature);

	void update();
	}
}



#endif /* LOCALCODE_GAME_PLAYER_H_ */
