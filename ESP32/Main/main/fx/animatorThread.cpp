/*
 * animatorThread.cpp
 *
 *  Created on: 25 Jan 2019
 *      Author: xasin
 */

#include "animatorThread.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "../core/setup.h"

#include "ManeAnimator.h"

#include "driver/ledc.h"
#include <cmath>

#include "colorSets.h"

#include "sounds.h"

#include "patterns/ShotFlicker.h"
#include "patterns/VestPattern.h"

namespace LZR {

using namespace Peripheral;

ColorSet currentColors = teamColors[0];
ColorSet bufferedColors = currentColors;

FXSet 	currentFX = brightnessLevels[0];
FXSet 	bufferedFX = currentFX;

pattern_mode_t target_pattern_mode = IDLE;
pattern_mode_t current_pattern_mode = OFF;

auto vestShotPattern = FX::ShotFlicker(VEST_LEDS);

std::vector<FX::VestPattern> modePatterns = {};

bool flashEnable = false;
bool flashInvert = false;

void set_bat_pwr(uint8_t level, uint8_t brightness = 255) {
	uint8_t gLevel = 255 - pow(255 - 2.55*level, 2)/255;

	ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 255 - (255-gLevel)*pow(brightness, 2)/65025);
	ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 255 - gLevel*pow(brightness, 2)/65025);

	ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
	ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);

}

void status_led_tick() {
	float conIndB = 0;
	float batBrightness = 1;

	switch(LZR::main_weapon_status) {
	case CHARGING: {
		int cycleTime = xTaskGetTickCount() % 1200;

		if(battery.is_charging)
			batBrightness = 0.4 + 0.6*cycleTime/1200;
		else if(cycleTime%600 < 300)
			batBrightness = 0.6;
	break;
	}
	case DISCHARGED:
		if(xTaskGetTickCount()%300 < 150) {
			batBrightness = 0.1;
			conIndB = 0.5;
		}
	break;
	default:
		if(mqtt.is_disconnected() == 0)
			conIndB = (0.3 + 0.3*sin(xTaskGetTickCount()/2300.0 * M_PI));
		else if(mqtt.is_disconnected() == 1)
			conIndB = xTaskGetTickCount()%800 < 600 ? 1 : 0.5;
		else
			conIndB = xTaskGetTickCount()%800 < 400 ? 1 : 0;
	}
	ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, 255 - pow(conIndB ,2)*255);
	ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);


	set_bat_pwr(battery.current_capacity(), batBrightness*255);
}

void vibr_motor_tick() {
	if(gunHandler.timeSinceLastShot() <= gunHandler.cGun().postShotVibrationTicks)
		gpio_set_level(PIN_VBRT, 1);
	else if(flashEnable && flashInvert)
		gpio_set_level(PIN_VBRT, 1);
	else if(player.get_heartbeat() && ((0b101 & (xTaskGetTickCount()/75)) == 0))
		gpio_set_level(PIN_VBRT, 1);
	else
		gpio_set_level(PIN_VBRT, 0);
}

void switch_to_mode(pattern_mode_t pMode) {
	modePatterns.clear();

	switch(pMode) {
	default: break;

	case IDLE: {
		modePatterns.emplace_back();
		auto &ip = modePatterns[0];

		ip.overlayColor = Color(0x666666);
		ip.pattern_func = FX::pattern_func_t::TRAPEZ;
		ip.pattern_period = 255*(VEST_LEDS);
		ip.pattern_p2_length = ip.pattern_period;

		ip.pattern_p1_length = 2*255;
		ip.pattern_trap_percent = 0.5 * (1<<16);

		ip.sine_amplitude = 0.25*(1<<16);

		ip.time_func = FX::time_func_t::LINEAR;
		ip.timefunc_p1_period = 600 * 8;
		ip.timefunc_period = ip.timefunc_p1_period;
	break;
	}

	case TEAM_SELECT:
		for(int i=0; i<3; i++) {
			modePatterns.emplace_back();
			auto &iP = modePatterns[i];

			iP.overlayColor = Color::HSV(120 * i);
			iP.overlayColor.bMod(140);
			iP.overlayColor.alpha = 160;

			iP.pattern_func = FX::pattern_func_t::TRAPEZ;
			iP.pattern_p1_length = 2.5*255;
			iP.pattern_period = 255*(VEST_LEDS + 4);
			iP.pattern_trap_percent = (1<<16) * 0.6;

			iP.pattern_shift = 2*255;
			iP.pattern_p2_length = 255*(VEST_LEDS + 4);
			iP.pattern_trap_percent = 0.3*(1<<16);

			iP.set_5050_trapez(5*600, 0.26);
			iP.timefunc_shift = i * iP.timefunc_period / 10;
		}
	break;
	}

	current_pattern_mode = pMode;
}

void mode_tick() {
	if(target_pattern_mode != current_pattern_mode)
		switch_to_mode(target_pattern_mode);

	switch(current_pattern_mode) {
	case OFF: break;
	case IDLE: break;

	case TEAM_SELECT: {
		Color tColor = bufferedColors.vestBase;
		tColor.bMod(60);
		RGBController.colors.fill(tColor, 1, -1);
	break;
	}

	case ACTIVE: break; // TODO
	}

	for(auto pattern : modePatterns) {
		pattern.tick();

		for(int i=0; i<RGBController.length-1; i++)
			pattern.apply_color_at(RGBController.colors[i+1], i);
	}
}

#define COLOR_FADE(cName, alpha) bufferedColors.cName.merge_overlay(currentColors.cName, alpha)
#define FX_FADE(fxName, alpha)	 bufferedFX.fxName = (bufferedFX.fxName * (1-alpha) + currentFX.fxName * alpha)

void vest_tick() {

	////////////////////
	// Vest color fading
	////////////////////
	COLOR_FADE(muzzleFlash, 13);
	COLOR_FADE(muzzleHeat,  6);
	COLOR_FADE(vestBase, 	4);
	COLOR_FADE(vestShotEnergy, 10);

	FX_FADE(minBaseGlow, 0.02);
	FX_FADE(maxBaseGlow, 0.02);
	FX_FADE(waverAmplitude, 0.02);
	FX_FADE(waverPeriod, 0.03);
	FX_FADE(waverPositionShift, 0.01);

	////////////////////
	// Muzzle heatup
	////////////////////
	Color newMuzzleColor = 	bufferedColors.muzzleHeat;
	newMuzzleColor.bMod(gunHandler.getGunHeat()*0.6);

	////////////////////
	// Muzzle flash for shots
	////////////////////
	if(gunHandler.was_shot_tick())
		newMuzzleColor.merge_overlay(bufferedColors.muzzleFlash);
	RGBController.colors[0] = newMuzzleColor;

	////////////////////
	// Generation of vest base color + heatup
	////////////////////
	Color currentVestColor = bufferedColors.vestBase;
	currentVestColor.bMod(bufferedFX.minBaseGlow +
			(bufferedFX.maxBaseGlow - bufferedFX.minBaseGlow)*gunHandler.getGunHeat()/255.0);

	// Reset of all colors
	RGBController.colors.fill(0, 1, -1);

	////////////////////
	// Basic vest wavering
	///////////////////
	mode_tick();

	/////////////////////////////////////
	// Vest shot flaring & wave animation
	/////////////////////////////////////
	vestShotPattern.tick();
	for(int i=1; i<RGBController.length; i++) {
		vestShotPattern.apply_color_at(RGBController.colors[i], i-1);
	}
}

void animation_thread(void *args) {
	while(true) {
		status_led_tick();

		if(main_weapon_status == NOMINAL) {
			player.tick();
			gunHandler.tick();

			currentColors = teamColors[player.get_team()];
			currentFX = brightnessLevels[player.get_brightness()];

			if(player.is_hit()) {
				if(player.is_dead())
					flashEnable = true;
				else
					flashEnable  = ((xTaskGetTickCount()/30) & 0b1) == 0;
				flashInvert = ((xTaskGetTickCount()/20) & 0b10) == 0;
			}
			else
				flashEnable = false;

			vest_tick();
			vibr_motor_tick();
		}
		else {
			RGBController.colors.fill(0);
			gpio_set_level(PIN_VBRT, false);
		}

		Color newMuzzleColor = RGBController.colors[0];
		Color actualMuzzle = Color();
		actualMuzzle.r = newMuzzleColor.g;
		actualMuzzle.g = newMuzzleColor.r;
		actualMuzzle.b = newMuzzleColor.b;
		RGBController.colors[0] = actualMuzzle;

		RGBController.update();

		vTaskDelay(10);
	}
}

void start_animation_thread() {
	currentColors = teamColors[0];
	bufferedColors = currentColors;

	currentFX = brightnessLevels[0];

	Sounds::init();

    TaskHandle_t animatorTaskHandle;
    xTaskCreatePinnedToCore(animation_thread, "Animator", 4*1024, nullptr, 10, &animatorTaskHandle, 1);
}
}
