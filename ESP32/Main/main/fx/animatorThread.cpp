/*
 * animatorThread.cpp
 *
 *  Created on: 25 Jan 2019
 *      Author: xasin
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "../core/setup.h"
#include "../IODefs.h"

#include "ManeAnimator.h"

#include "driver/ledc.h"
#include <cmath>

namespace LZR {

using namespace Peripheral;

Color muzzleBaseColor  = 0;

Color muzzleHeatColor  = Color(Material::DEEP_ORANGE);
Color muzzleFlashColor = Color(Material::PURPLE).merge_overlay(0xFFFFFF, 100);

Layer vestBaseLayer = Layer(4);
Layer vestBufferLayer = vestBaseLayer;

ManeAnimator vestShotAnimator = ManeAnimator(vestBaseLayer.length());

auto vestShotOverlay = Layer(vestBaseLayer.length());

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
		conIndB = (0.3 + 0.3*sin(xTaskGetTickCount()/3000.0 * M_PI));
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
	newMuzzleColor = muzzleBaseColor;
	muzzleHeatColor.alpha = gunHandler.getGunHeat();
	newMuzzleColor.merge_overlay(muzzleHeatColor);
	if(gunHandler.was_shot_tick())
		newMuzzleColor.merge_overlay(muzzleFlashColor);

	// Basic vest wavering & heatup
	for(int i=0; i<vestBaseLayer.length(); i++)
		vestBaseLayer[i] = Color(Material::GREEN,
				(125 + (130.0*gunHandler.getGunHeat())/255)*(0.75 + 0.25*sin((xTaskGetTickCount() - i*300)/1200.0 * M_PI)));
	vestBufferLayer.merge_overlay(vestBaseLayer);

	RGBController.colors.merge_overlay(vestBufferLayer, 1);

	// Vest shot flaring & wave animation
	if(gunHandler.was_shot_tick())
		vestShotAnimator.points[0].pos = 1;
	for(int i=3; i!=0; i--)
		vestShotAnimator.tick();
	vestShotOverlay.alpha_set(vestShotAnimator.scalarPoints);
	RGBController.colors.merge_add(vestShotOverlay, 1);
}

void animation_thread(void *args) {
	TickType_t lastTick;

	vestBaseLayer.fill(Color(Material::PURPLE, 100));
	vestBaseLayer.alpha = 30;

	vestShotAnimator.baseTug   = 0.0013;
	vestShotAnimator.basePoint = 0.1;
	vestShotAnimator.dampening = 0.94;
	vestShotAnimator.ptpTug    = 0.015;
	vestShotAnimator.wrap 	   = true;

	vestShotOverlay.fill(Material::DEEP_PURPLE);

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

		vTaskDelayUntil(&lastTick, 10);
	}
}

void start_animation_thread() {
    TaskHandle_t animatorTaskHandle;
    xTaskCreatePinnedToCore(animation_thread, "Animator", 4*1024, nullptr, 10, &animatorTaskHandle, 1);
}
}
