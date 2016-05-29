/*
 * Gun.cpp
 *
 *  Created on: 22.05.2016
 *      Author: xasin
 */

#include "../Board/Board.h"

#include "Weapon.h"
#include "Player.h"
#include "Game.h"

namespace Game {
namespace Weapon {

	const uint8_t gunDmgTable[1] = 			{1};
	const uint16_t gunShotDelayTable[1] = 	{200};
	const uint16_t gunReloadDelayTable[1] = {500};
	const uint8_t gunMagSizeTable[1] = 		{10};

	uint16_t ammo = 1, reloadTimer = 1, shotTimer = 0;

	void (*on_shot)() = 0;
	void (*on_reload)() = 0;

	uint8_t damage_from_signature(uint8_t hitSignature) {
		return gunDmgTable[(hitSignature & 0b11110000) >> 4];
	}

	bool can_shoot() {
		if(shotTimer != 0)
			return false;

		return Player::is_alive() && (ammo != 0) && Game::is_running();
	}

	bool shoot() {
		if(!can_shoot()) return false;

		if(on_shot != 0) on_shot();

		ammo--;

		reloadTimer = gunReloadDelayTable[Config::gun_cfg()];

		if(ammo != 0)
			shotTimer = gunShotDelayTable[Config::gun_cfg()];
		else {
			if(on_reload != 0) on_reload();
		}

		IR::send_8(Player::ID | (Config::gun_cfg() << 4));

		return true;
	}

	void update() {
		if(reloadTimer == 1)
			ammo = gunMagSizeTable[Config::gun_cfg()];

		if(reloadTimer != 0) 	reloadTimer--;
		if(shotTimer != 0) 		shotTimer--;
	}
}
}
