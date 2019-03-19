/*
 * colorSets.cpp
 *
 *  Created on: 15 Mar 2019
 *      Author: xasin
 */


#include "colorSets.h"

namespace LZR {

const ColorSet teamColors[] = {
		{ // Team 0 - Inactive/Grey
			.muzzleFlash = 0x555555,
			.muzzleHeat  = 0,
			.vestBase    = 0x444444,
			.vestShotEnergy = 0,
		},
		{ // Team 1 - Red
			.muzzleFlash = Material::CYAN,
			.muzzleHeat  = 0x44FFFF,
			.vestBase = Material::RED,
			.vestShotEnergy = Color(Material::RED).merge_overlay(Material::DEEP_ORANGE, 50)
		},
		{ // Team 2 - Green
			.muzzleFlash = Material::PINK,
			.muzzleHeat  = 0xFF44FF,
			.vestBase	 = Material::GREEN,
			.vestShotEnergy = Color(Material::GREEN).merge_overlay(Material::LIME, 70)
		},
		{ // Team 3 - Orange
			.muzzleFlash = Material::BLUE,
			.muzzleHeat  = 0x5555FF,
			.vestBase 	 = Material::ORANGE,
			.vestShotEnergy = Material::YELLOW,
		},
		{ // Team 4 - Blue
			.muzzleFlash = Material::YELLOW,
			.muzzleHeat  = 0xFFFF44,
			.vestBase 	 = Material::BLUE,
			.vestShotEnergy = Material::INDIGO,
		},
		{ // Team 5 - Purple
			.muzzleFlash = Material::GREEN,
			.muzzleHeat  = 0x55FF55,
			.vestBase	 = Material::PURPLE,
			.vestShotEnergy = Material::PINK,
		},
		{ // Team 6 - Cyan
			.muzzleFlash = Material::RED,
			.muzzleHeat  = 0xFF5555,
			.vestBase 	 = Material::CYAN,
			.vestShotEnergy = Color(Material::CYAN).merge_overlay(Material::DEEP_ORANGE, 20)
		},
		{ // Team 7 - White
			.muzzleFlash = 0xFFFFFF,
			.muzzleHeat  = 0x999999,
			.vestBase 	 = 0xEEEEFF,
			.vestShotEnergy = 0xAAAAFF
		}
};

}
