/*
 * mylin.cpp
 *
 *  Created on: 7 May 2019
 *      Author: xasin
 */


#include "mylin.h"

namespace LZR {
namespace Weapons {

#include "mylin_fire.h"
#include "wyre_cooldown.h"

#define AC(name) AudioCassette(name, sizeof(name), 60000)

// Effective shot rate:
// 8*108 + 8*100 + 8*30 + 1800 + 8*600 = 8504 Ticks
// 1063 Ticks per shot
// Rate: 0.5644Hz
// DPS Multiplicator: 1.7716

Lasertag::GunSpecs mylin {
	.weaponSwitchDelay = 3*600,

	.currentReserveAmmo = -1,
	.clipSize = 12,
	.currentClipAmmo = 12,

	.postTriggerTicks = 108,
	.postTriggerRelease = false,

	.shotsPerSalve = 1,

	.perShotDelay  = 100,

	.postShotCooldownTicks = 200,
	.postShotVibrationTicks = 100,

	.postSalveDelay = 0,
	.postSalveRelease = true,

	.perReloadDelay = 40*60,
	.perReloadRecharge = 12,

	.perShotHeatup = 0.4,
	.perTickCooldown = 0.98,

	.chargeSounds = {
			AC(mylin_fire),
	},
	.shotSounds = {
	},
	.cooldownSounds = {
			AC(wyre_cooldown)
	}
};

}
}
