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
#include "../IODefs.h"

#include "ManeAnimator.h"

#include "driver/ledc.h"
#include <cmath>

#include "colorSets.h"

#include "sounds.h"

namespace LZR {

using namespace Peripheral;

ColorSet currentColors = teamColors[0];
ColorSet bufferedColors = currentColors;

FXSet 	currentFX = brightnessLevels[0];
FXSet 	bufferedFX = currentFX;
float 	lastFXPhase = 0;

auto vestBufferLayer = Layer(WS2812_NUMBER - 1);

auto vestShotAnimator = ManeAnimator(vestBufferLayer.length());
auto vestShotOverlay = Layer(vestBufferLayer.length());

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

#define COLOR_FADE(cName, alpha) bufferedColors.cName.merge_overlay(currentColors.cName, alpha)
#define FX_FADE(fxName, alpha)	 bufferedFX.fxName = (bufferedFX.fxName * (1-alpha) + currentFX.fxName * alpha)

void vest_tick() {
	// Vest color fading
	COLOR_FADE(muzzleFlash, 13);
	COLOR_FADE(muzzleHeat,  6);
	COLOR_FADE(vestBase, 	4);
	COLOR_FADE(vestShotEnergy, 10);

	FX_FADE(minBaseGlow, 0.02);
	FX_FADE(maxBaseGlow, 0.02);
	FX_FADE(waverAmplitude, 0.02);
	FX_FADE(waverPeriod, 0.03);
	FX_FADE(waverPositionShift, 0.01);

	// Muzzle heatup
	Color newMuzzleColor = 	bufferedColors.muzzleHeat;
	newMuzzleColor.bMod(gunHandler.getGunHeat()*0.6);

	// Muzzle flash for shots
	if(gunHandler.was_shot_tick())
		newMuzzleColor.merge_overlay(bufferedColors.muzzleFlash);
	RGBController.colors[0] = newMuzzleColor;

	// Generation of vest base color + heatup
	Color currentVestColor = bufferedColors.vestBase;
	currentVestColor.bMod(bufferedFX.minBaseGlow +
			(bufferedFX.maxBaseGlow - bufferedFX.minBaseGlow)*gunHandler.getGunHeat()/255.0);

	vestBufferLayer.fill(currentVestColor);

	// Basic vest wavering
	lastFXPhase += 10 / bufferedFX.waverPeriod;

	for(int i=0; i<vestBufferLayer.length(); i++) {
		float currentPhase = (lastFXPhase + i*bufferedFX.waverPositionShift)*M_PI*2;
		float currentFactor = 1-bufferedFX.waverAmplitude/2 + bufferedFX.waverAmplitude/2*sin(currentPhase);

		if(flashEnable) {
			if(flashInvert ^ (i&1))
				currentFactor *= 0.3;
			else {
				currentFactor = 1;
				vestBufferLayer[i].merge_overlay(0xFFFFFF, 130);
			}
		}

		if(currentFactor < 0)
			currentFactor = 0;
		if(currentFactor > 1)
			currentFactor = 1;

		vestBufferLayer[i].bMod(255 * currentFactor);
	}

	RGBController.colors.merge_overlay(vestBufferLayer, 1);

	// Vest shot flaring & wave animation
	if(gunHandler.was_shot_tick())
		vestShotAnimator.points[0].pos = 1;
	for(int i=3; i!=0; i--)
		vestShotAnimator.tick();

	vestShotOverlay.fill(bufferedColors.vestShotEnergy);
	vestShotOverlay.alpha_set(vestShotAnimator.scalarPoints);
	RGBController.colors.merge_add(vestShotOverlay, 1);
}

void animation_thread(void *args) {
	vestShotAnimator.baseTug   = 0.0013;
	vestShotAnimator.basePoint = 0.0;
	vestShotAnimator.dampening = 0.94;
	vestShotAnimator.ptpTug    = 0.015;
	vestShotAnimator.wrap 	   = true;

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
			RGBController.fill(0);
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
		//vTaskDelayUntil(&lastTick, 10);
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
