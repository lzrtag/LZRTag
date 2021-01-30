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

#define AC(name) Xasin::Audio::opus_audio_bundle_t({0, sizeof(name), 120, name})

// Effective shot rate:
// 30*70 + 3000 + 30*200  = 11100 Ticks
// 370 Ticks per shot, avg.
// Rate: 1.6216Hz
// DPS Multiplicator: 0.61666

Lasertag::GunSpecs zinger {
	.weaponSwitchDelay = 1*600,

	.currentReserveAmmo = -1,
	.clipSize = 18,
	.currentClipAmmo = 18,

	.postTriggerTicks = 0,
	.postTriggerRelease = false,

	.shotsPerSalve = 1,

	.perShotDelay  = 70,

	.postShotCooldownTicks = 200,
	.postShotVibrationTicks = 30,

	.postSalveDelay = 0,
	.postSalveRelease = false,

	.perReloadDelay = 15*60,
	.perReloadRecharge = 6,

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
