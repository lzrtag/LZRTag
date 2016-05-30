/*
 * Player.cpp
 *
 *  Created on: 22.05.2016
 *      Author: xasin
 */

#include "../Board/Board.h"
#include "Weapon.h"
#include "Player.h"
#include "Game.h"

namespace Game {
namespace Player {

	const uint16_t playerLRegenTimerTable[1] = 	{5000};
	const uint16_t playerSRegenTimerTable[1] = 	{2000};
	const uint8_t playerLRegenRateTable[1] = 	{10};
	const uint8_t playerSRegenRateTable[1] = 	{30};

	const uint16_t playerSMaxTable[1] =			{0xffff};

	uint8_t ID = 0;

	uint16_t life = 0xffff, lifeRegenTimer = 0;
	uint16_t shield = 0x000f, shieldRegenTimer = 0;

	void (*on_death)() = 0;
	void (*on_hit)() = 0;

	void set_team(uint8_t team) {
		ID &= ~(0b11);
		ID |= team;
	}
	void set_number(uint8_t number) {
		ID &= ~(0b1100);
		ID |= (number << 2);
	}

	uint8_t get_team() {
		return (ID & 0b11);
	}
	uint8_t get_team_color() {
		if(!Game::Config::uses_teams())
			return COLOR_WHITE;
		else
			switch(get_team()) {
			case 0:
				return COLOR_WHITE;
			break;

			case 1:
				return COLOR_RED;
			break;

			case 2:
				return COLOR_GREEN;
			break;

			case 3:
				return COLOR_BLUE;
			break;
			}
			return COLOR_WHITE;
	}

	uint8_t get_life_percent() {
		return (life / 656);
	}
	uint8_t get_shield_percent() {
		uint16_t divisor = playerSMaxTable[Config::player_cfg()] / 100 + 1;
		return shield / divisor;
	}

	bool has_full_life() {
		return life == 0xffff;
	}
	bool has_full_shield() {
		return shield >= playerSMaxTable[Config::player_cfg()];
	}

	bool is_alive() {
		return life != 0;
	}

	void take_hit(uint8_t hitSignature) {
		if((hitSignature & 0b1111) == ID)
			return;
		else if(!Config::friendlyfire() && (hitSignature & 0b11) == get_team())
			return;
		else {

			if(on_hit != 0) on_hit();

			lifeRegenTimer = playerLRegenTimerTable[Config::player_cfg()];
			shieldRegenTimer = playerSRegenTimerTable[Config::player_cfg()];

			uint8_t damage = Weapon::damage_from_signature(hitSignature);
			if(shield >= damage) {
				shield -= damage;
				damage = 0;
			}
			else {
				damage -= shield;
				shield = 0;
				life = (damage > life ? 0 : life - damage);
				if(life == 0)
					if(on_death != 0) on_death();
			}
		}
	}

	void update() {
		if((lifeRegenTimer == 0) && !has_full_life())
			life = (life > (0xffff - playerLRegenRateTable[Config::player_cfg()])) ?
					(life + playerLRegenRateTable[Config::player_cfg()]) :
					(0xffff);

		if((shieldRegenTimer == 0) && !has_full_shield())
			shield = (shield > (playerSMaxTable[Config::player_cfg()] - playerSRegenRateTable[Config::player_cfg()])) ?
					(shield + playerSRegenRateTable[Config::player_cfg()]) :
					(playerSMaxTable[Config::player_cfg()]);

		if(lifeRegenTimer != 0)	lifeRegenTimer--;
		if(shieldRegenTimer != 0) shieldRegenTimer--;
	}
}
}


