/*
 * patternModeHandling.cpp
 *
 *  Created on: 11 Sep 2019
 *      Author: xasin
 */

#include "animatorThread.h"
#include "../core/setup.h"

#include "patterns/VestPattern.h"

#include <xnm/net_helpers.h>

#include <vector>

namespace LZR {
namespace FX {

volatile pattern_mode_t target_mode = IDLE;
pattern_mode_t current_mode = IDLE;

std::vector<FX::VestPattern> modePatterns = {};

void switch_to_mode(pattern_mode_t pMode) {
	modePatterns.clear();

	switch(pMode) {
	case OFF:
	case PATTERN_MODE_MAX:
	case PLAYER_DECIDED:
	case BATTERY_LEVEL:
		break;

	case CONNECTING: {
		modePatterns.emplace_back();
		auto &ip = modePatterns[0];

		ip.pattern_func = FX::pattern_func_t::TRAPEZ;
		ip.pattern_period = 255*(VEST_LEDS);
		ip.pattern_p2_length = ip.pattern_period - 255;

		ip.pattern_p1_length = 2*255;
		ip.pattern_trap_percent = 0.5 * (1<<16);

		ip.time_func = FX::time_func_t::TRAPEZ;
		ip.timefunc_p1_period = 600 * 1.6;
		ip.timefunc_period = ip.timefunc_p1_period;
		ip.timefunc_trap_percent = 0.9*(1<<16);
	break;
	}

	case OTA:
	case CHARGE: {
		switch_to_mode(IDLE);

		auto &ip = modePatterns[0];
		ip.pattern_p1_length = 1.5*255;

		ip.timefunc_p1_period = 10*600;
		ip.timefunc_period = ip.timefunc_p1_period;
	break;
	}

	case IDLE: {
		modePatterns.emplace_back();
		auto &ip = modePatterns[0];

		ip.overlayColor = Color(0x333333);
		ip.pattern_func = FX::pattern_func_t::TRAPEZ;
		ip.pattern_period = 255*(VEST_LEDS);
		ip.pattern_p2_length = ip.pattern_period;

		ip.pattern_p1_length = 2*255;
		ip.pattern_trap_percent = 0.5 * (1<<16);

		ip.sine_amplitude = 0.2*(1<<16);
		ip.sine_center 	  = 0;

		ip.time_func = FX::time_func_t::LINEAR;
		ip.timefunc_p1_period = 600 * 8;
		ip.timefunc_period = ip.timefunc_p1_period;
	break;
	}

	case TEAM_SELECT:
		for(int i=0; i<3; i++) {
			modePatterns.emplace_back();
			auto &iP = modePatterns[i];

			iP.overlayColor = Color::HSV(120 * i, 255, 95);
			iP.overlayColor.alpha = 255;

			iP.pattern_func = FX::pattern_func_t::TRAPEZ;
			iP.pattern_p1_length = 2.5*255;
			iP.pattern_period = 255*(VEST_LEDS + 4);
			iP.pattern_trap_percent = (1<<16) * 0.5;

			iP.pattern_shift = 2*255;
			iP.pattern_p2_length = 255*(VEST_LEDS + 4);
			iP.pattern_trap_percent = 0.6*(1<<16);

			iP.time_func = FX::time_func_t::LINEAR;
			iP.timefunc_p1_period = 0.75*600;
			iP.timefunc_period = 5*600;

			iP.timefunc_shift = (2-i)*600*0.2;
		}
	break;

	case DEAD: {
		modePatterns.emplace_back();
		auto &ip = modePatterns[0];

		ip.overlayColor = Color(0x333333);
		ip.pattern_func = FX::pattern_func_t::TRAPEZ;
		ip.pattern_period = 255*(VEST_LEDS);
		ip.pattern_p2_length = ip.pattern_period;

		ip.pattern_p1_length = 2*255;
		ip.pattern_trap_percent = 0.5 * (1<<16);

		ip.time_func = FX::time_func_t::LINEAR;
		ip.timefunc_p1_period = 600 * 3;
		ip.timefunc_period = ip.timefunc_p1_period;
	break;
	}

	break;

	case ACTIVE: {
		modePatterns.emplace_back();
		auto &ip = modePatterns[0];

		ip.pattern_func = FX::pattern_func_t::SINE;
		ip.pattern_period = 255*(VEST_LEDS)*5;
		ip.pattern_p1_length = 5*255;

		ip.sine_amplitude = 0.1*(1<<16);
		ip.sine_center 	  = 0.55*(1<<16);

		ip.time_func = FX::time_func_t::EQUAL_SINE;
		ip.timefunc_p1_period = 600 * 8;
		ip.timefunc_period = ip.timefunc_p1_period;

		modePatterns.emplace_back();
		auto &ij = modePatterns[1];

		ij.pattern_func = FX::pattern_func_t::SINE;
		ij.pattern_period = -255*(VEST_LEDS);

		ij.pattern_p1_length = 3*255;

		ij.sine_amplitude = 0.05*(1<<16);
		ij.sine_center 	  = 0;

		ij.time_func = FX::time_func_t::LINEAR;
		ij.timefunc_p1_period = 2*600;
		ij.timefunc_period = ij.timefunc_p1_period;

		ij.overlay = false;
	break;
	}
	}

	current_mode = pMode;
}

void mode_tick() {
	pattern_mode_t targetPattern = target_mode;
	if(target_mode == pattern_mode_t::PLAYER_DECIDED)
		targetPattern = LZR::player.get_brightness();

	if(XNM::NetHelpers::OTA::get_state() == XNM::NetHelpers::OTA::DOWNLOADING)
		targetPattern = OTA;

	if(targetPattern != current_mode)
		switch_to_mode(targetPattern);

	switch(current_mode) {
	case OFF:
	case PATTERN_MODE_MAX:
	case PLAYER_DECIDED:
		break;

	case CHARGE:
		modePatterns[0].overlayColor = Color::HSV(120*LZR::battery.current_capacity() / 100.0, 200, 35);
		if(!LZR::battery.is_charging)
			RGBController.colors.fill(Color(Material::GREEN, 20), 1, -1);
	break;

	case IDLE:
		modePatterns[0].overlayColor = Color(0x333333);
		modePatterns[0].overlayColor.merge_overlay(bufferedColors.vestBase, 45);
	break;

	case BATTERY_LEVEL:
		RGBController.colors.fill(Color::HSV(120*LZR::battery.current_capacity() / 100.0, 200, 50), 1, -1);
	break;

	case CONNECTING:
		modePatterns[0].overlayColor = (LZR::mqtt.is_disconnected() == 1) ? Color(Material::AMBER, 70) : Color(Material::PINK, 70);
	break;

	case DEAD:
	case TEAM_SELECT: {
		Color tColor = bufferedColors.vestBase;
		tColor.bMod(90);
		RGBController.colors.fill(tColor, 1, -1);
	break;
	}

	case ACTIVE:
		modePatterns[0].overlayColor = bufferedColors.vestBase;
		modePatterns[1].overlayColor = bufferedColors.vestShotEnergy;
	break;

	case OTA:
		modePatterns[0].overlayColor = Color(Material::BLUE, 200, 35);
	break;
	}

	for(auto pattern : modePatterns) {
		pattern.tick();

		for(int i=0; i<RGBController.length-1; i++)
			pattern.apply_color_at(RGBController.colors[i+1], i);
	}
}

void init() {
	target_mode = OFF;
	current_mode = OFF;
}

}
}
