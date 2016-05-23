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

uint16_t playerLRegenTimerTable[1] = 	{5000};
uint16_t playerSRegenTimerTable[1] = 	{2000};
uint8_t playerLRegenRateTable[1] = 		{10};
uint8_t playerSRegenRateTable[1] = 		{30};

uint8_t ID = 0;

uint16_t life, lifeRegenTimer;
uint16_t shield, shieldRegenTimer;

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

bool is_alive() {
	return life != 0;
}

void take_hit(uint8_t hitSignature) {
	if((hitSignature & 0b1111) == ID)
		return;
	else if(!Config::friendlyfire() && (hitSignature & 0b11) == get_team())
		return;
	else {

		on_hit();

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
				on_death();
		}
	}
}

void update() {
	if(lifeRegenTimer == 0 && life < 0xfff0) life += playerLRegenRateTable[Config::player_cfg()];
	if(shieldRegenTimer == 0 && shield < 0xfff0) shield += playerSRegenRateTable[Config::player_cfg()];
}
}
}


