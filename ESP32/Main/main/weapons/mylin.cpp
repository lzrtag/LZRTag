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

#define AC(name) AudioCassette(name, sizeof(name))

Lasertag::GunSpecs mylin {
	.maxAmmo = 8,
	.currentAmmo = 8,

	.postTriggerTicks = 108,
	.postTriggerRelease = false,

	.shotsPerSalve = 1,

	.perShotDelay  = 100,

	.postShotCooldownTicks = 200,
	.postShotVibrationTicks = 100,

	.postSalveDelay = 0,
	.postSalveRelease = true,

	.postShotReloadBlock = 3*600,
	.postReloadReloadBlock = 600,
	.perReloadRecharge = 1,

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
