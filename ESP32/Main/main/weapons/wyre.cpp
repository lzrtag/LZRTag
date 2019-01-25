/*
 * wyre.cpp
 *
 *  Created on: 25 Jan 2019
 *      Author: xasin
 */


#include "wyre.h"

namespace LZR {
namespace Weapons {

const Lasertag::GunSpecs wyre {
	.maxAmmo = 8,
	.postTriggerTicks = 0,
	.postTriggerRelease = false,

	.shotsPerSalve = 1,

	.perShotDelay = 140,

	.postShotCooldownTicks = 200,
	.postShotVibrationTicks = 50,

	.postSalveDelay = 0,
	.postSalveRelease = false,

	.postShotReloadBlock = 6000,
	.postReloadReloadBlock = 600,
	.perReloadRecharge = 2,

	.perShotHeatup = 0.1,
	.perTickCooldown = 0.99,

	.chargeSounds = {},
	.shotSounds = {
			AudioCassette(wyre_fire_1, sizeof(wyre_fire_1))
	},
	.cooldownSounds = {
			AudioCassette(wyre_cooldown, sizeof(wyre_cooldown))
	}
};

}
}
