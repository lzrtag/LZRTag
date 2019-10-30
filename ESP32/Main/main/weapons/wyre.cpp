/*
 * wyre.cpp
 *
 *  Created on: 25 Jan 2019
 *      Author: xasin
 */


#include "wyre.h"

namespace LZR {
namespace Weapons {

#include "wyre_charge.h"

#include "wyre_cooldown.h"

#include "wyre_fire_1.h"
#include "wyre_fire_2.h"
#include "wyre_fire_3.h"
#include "wyre_fire_4.h"
#include "wyre_fire_5.h"

#define AC(name) AudioCassette(name, sizeof(name))

// Effective shot rate:
// 18*90 + 9*50 + 3000 + 4*1200 = 9870 Ticks
// 548 Ticks per shot
// Rate: 1.09422Hz
// DPS Val: 0.9138

Lasertag::GunSpecs wyre {
	.weaponSwitchDelay = 3*600,

	.currentReserveAmmo = -1,

	.clipSize= 18,
	.currentClipAmmo = 18,

	.postTriggerTicks = 0,
	.postTriggerRelease = false,

	.shotsPerSalve = 2,

	.perShotDelay = 90,

	.postShotCooldownTicks = 200,
	.postShotVibrationTicks = 50,

	.postSalveDelay = 50,
	.postSalveRelease = false,

	.perReloadDelay = 1000,
	.perReloadRecharge = 6,

	.perShotHeatup = 0.05,
	.perTickCooldown = 0.9983,

	.chargeSounds = {
			AC(wyre_charge)
	},
	.shotSounds = {
			AC(wyre_fire_1),
			AC(wyre_fire_2),
			AC(wyre_fire_3),
			AC(wyre_fire_4),
			AC(wyre_fire_5),
	},
	.cooldownSounds = {
			AudioCassette(wyre_cooldown, sizeof(wyre_cooldown))
	}
};
}
}
