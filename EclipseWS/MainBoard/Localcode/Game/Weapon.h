/*
 * GunTable.h
 *
 *  Created on: 22.05.2016
 *      Author: xasin
 */

#ifndef LOCALCODE_GAME_WEAPON_H_
#define LOCALCODE_GAME_WEAPON_H_

#include <avr/io.h>

namespace Game {
	namespace Weapon {
	extern uint16_t ammo, reloadTimer, shotTimer;

	uint8_t damage_from_signature(uint8_t hitSignature);

	bool can_shoot();

	bool shoot();

	void update();
	}
}


#endif /* LOCALCODE_GAME_WEAPON_H_ */
