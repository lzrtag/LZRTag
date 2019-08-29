/*
 * zinger.cpp
 *
 *  Created on: 7 May 2019
 *      Author: xasin
 */


#include "zinger.h"


namespace LZR {
namespace Weapons {

#include "zinger_fire.h"
#include "wyre_cooldown.h"

#define AC(name) AudioCassette(name, sizeof(name))

// Effective shot rate:
// 30*70 + 3000 + 30*200  = 11100 Ticks
// 370 Ticks per shot, avg.
// Rate: 1.6216Hz
// DPS Multiplicator: 0.61666

Lasertag::GunSpecs zinger {
	.maxAmmo = 30,
	.currentAmmo = 30,

	.postTriggerTicks = 0,
	.postTriggerRelease = false,

	.shotsPerSalve = 1,

	.perShotDelay  = 70,

	.postShotCooldownTicks = 200,
	.postShotVibrationTicks = 30,

	.postSalveDelay = 0,
	.postSalveRelease = false,

	.postShotReloadBlock = 5*600,
	.postReloadReloadBlock = 200,
	.perReloadRecharge = 1,

	.perShotHeatup = 0.02,
	.perTickCooldown = 0.9975,

	.chargeSounds = {
	},
	.shotSounds = {
			AC(zinger_fire),
	},
	.cooldownSounds = {
			AC(wyre_cooldown)
	}
};

}
}
