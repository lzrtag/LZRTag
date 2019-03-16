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

namespace LZR {

using namespace Peripheral;

ColorSet currentColors = {
		.muzzleFlash = Material::PURPLE,
		.muzzleHeat	 = Material::ORANGE,
		.vestBase	 = Material::GREEN,
		.vestShotEnergy	 = Color(0x22FF22),
		.vestMark	 = 0xFFFFFF,
};
FXSet 	 currentFX = {
		.minBaseGlow = 200,
		.maxBaseGlow = 200,

		.waverAmplitude = 0.8,
		.waverPeriod 	= 1000,
		.waverPositionShift = -0.2,
};

auto vestBaseColor	 = Color();
auto vestBufferLayer = Layer(4);

auto vestShotAnimator = ManeAnimator(vestBufferLayer.length());
auto vestShotOverlay = Layer(vestBufferLayer.length());

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
	gpio_set_level(PIN_VBRT,
			gunHandler.timeSinceLastShot() <= gunHandler.cGun().postShotVibrationTicks
			? 1 : 0);
}

void vest_tick() {
	// Muzzle flash and heatup
	Color &newMuzzleColor = RGBController.colors[0];
	newMuzzleColor = 0;
	currentColors.muzzleHeat.alpha = gunHandler.getGunHeat();
	newMuzzleColor.merge_overlay(currentColors.muzzleHeat);

	if(gunHandler.was_shot_tick())
		newMuzzleColor.merge_overlay(currentColors.muzzleFlash);

	// Vest colorset smooth fading
	vestBaseColor.merge_overlay(currentColors.vestBase, 2);
	Color currentVestColor = vestBaseColor;
	currentVestColor.bMod(currentFX.minBaseGlow +
			(currentFX.maxBaseGlow - currentFX.minBaseGlow)*gunHandler.getGunHeat()/255);


	bool flashEnable  = ((xTaskGetTickCount()/30) & 0b1) == 0;
	bool flashCounter = ((xTaskGetTickCount()/20) & 0b10) == 0;

	if((xTaskGetTickCount() % (5*600)) > 600) {
		flashEnable = false;
	}

	// Basic vest wavering & heatup
	vestBufferLayer.fill(currentVestColor);
	for(int i=0; i<vestBufferLayer.length(); i++) {
		float currentPhase = (xTaskGetTickCount()/float(currentFX.waverPeriod) + i*currentFX.waverPositionShift)*M_PI*2;
		float currentFactor = 1-currentFX.waverAmplitude/2 + currentFX.waverAmplitude/2*sin(currentPhase);

		if(flashEnable) {
			if(flashCounter ^ (i&1))
				currentFactor *= 0.3;
			else {
				currentFactor += 0.4;
				vestBufferLayer[i].merge_overlay(0xFFFFFF, 160);
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
	vestShotOverlay.fill(currentColors.vestShotEnergy);
	vestShotOverlay.alpha_set(vestShotAnimator.scalarPoints);
	RGBController.colors.merge_add(vestShotOverlay, 1);
}

void animation_thread(void *args) {
	TickType_t lastTick;

	vestShotAnimator.baseTug   = 0.0013;
	vestShotAnimator.basePoint = 0.0;
	vestShotAnimator.dampening = 0.94;
	vestShotAnimator.ptpTug    = 0.015;
	vestShotAnimator.wrap 	   = true;

	vestShotOverlay.fill(Material::ORANGE);

	while(true) {
		status_led_tick();

		if(main_weapon_status == NOMINAL) {
			gunHandler.tick();

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
    TaskHandle_t animatorTaskHandle;
    xTaskCreatePinnedToCore(animation_thread, "Animator", 4*1024, nullptr, 10, &animatorTaskHandle, 1);
}
}
