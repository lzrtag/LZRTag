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

#include "BF-RINO/BF-RINO_shot_1.h"
#include "BF-RINO/BF-RINO_shot_2.h"
#include "BF-RINO/BF-RINO_shot_3.h"
#include "BF-RINO/BF-RINO_shot_4.h"
#include "BF-RINO/BF-RINO_shot_5.h"
#include "BF-RINO/BF-RINO_shot_6.h"
#include "BF-RINO/BF-RINO_shot_7.h"
#include "BF-RINO/BF-RINO_shot_8.h"
#include "BF-RINO/BF-RINO_shot_9.h"
#include "BF-RINO/BF-RINO_shot_10.h"

#define AC(name) Xasin::Audio::opus_audio_bundle_t({0, sizeof(name), 120, name})

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
		(encoded_audio_BF_RINO_shot_1),
		(encoded_audio_BF_RINO_shot_2),
		(encoded_audio_BF_RINO_shot_3),
		(encoded_audio_BF_RINO_shot_4),
		(encoded_audio_BF_RINO_shot_5),
		(encoded_audio_BF_RINO_shot_6),
		(encoded_audio_BF_RINO_shot_7),
		(encoded_audio_BF_RINO_shot_8),
		(encoded_audio_BF_RINO_shot_9),
		(encoded_audio_BF_RINO_shot_10),
	},
	.shotSounds = {
	},
	.cooldownSounds = {
			AC(wyre_cooldown)
	}
};

}
}
