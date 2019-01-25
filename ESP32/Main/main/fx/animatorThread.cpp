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

void animation_thread(void *args) {
	TickType_t lastTick;

	Color muzzleBaseColor  = 0;

	Color muzzleHeatColor  = Color(Material::DEEP_ORANGE);
	Color muzzleFlashColor = Color(Material::PURPLE).merge_overlay(0xFFFFFF, 100);

	Layer vestBaseLayer = Layer(4);
	vestBaseLayer.fill(Color(Material::PURPLE, 100));
	Layer vestBufferLayer = vestBaseLayer;
	vestBaseLayer.alpha = 30;

	auto vestShotAnimator = ManeAnimator(vestBaseLayer.length());
	vestShotAnimator.baseTug   = 0.0013;
	vestShotAnimator.basePoint = 0.1;
	vestShotAnimator.dampening = 0.94;
	vestShotAnimator.ptpTug    = 0.015;
	vestShotAnimator.wrap 	   = true;

	auto vestShotOverlay = Layer(vestBaseLayer.length());
	vestShotOverlay.fill(Material::DEEP_PURPLE);
	vestShotOverlay.alpha = 130;

	while(true) {

		gunHandler.tick();

		if(gunHandler.timeSinceLastShot() < 3)
			vestShotAnimator.points[0].pos = 1;

		for(int i=3; i!=0; i--)
			vestShotAnimator.tick();


		for(int i=0; i<vestBaseLayer.length(); i++)
			vestBaseLayer[i] = Color(Material::PURPLE, (50 + (170.0*gunHandler.getGunHeat())/255)*(0.8 + 0.4*sin((xTaskGetTickCount() - i*300)/1200.0 * M_PI)));
		vestBufferLayer.merge_overlay(vestBaseLayer);

		RGBController.colors.merge_overlay(vestBufferLayer, 1);

		vestShotOverlay.alpha_set(vestShotAnimator.scalarPoints);
		RGBController.colors.merge_overlay(vestShotOverlay, 1);

		Color newMuzzleColor = muzzleBaseColor;
		muzzleHeatColor.alpha = gunHandler.getGunHeat();
		newMuzzleColor.merge_overlay(muzzleHeatColor);

		if(gunHandler.was_shot_tick())
			newMuzzleColor.merge_overlay(muzzleFlashColor);
		gpio_set_level(PIN_VBRT,
				gunHandler.timeSinceLastShot() <= gunHandler.cGun().postShotVibrationTicks
				? 1 : 0);

		ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, pow((0.5 + 0.5*sin(xTaskGetTickCount()/1200.0 * M_PI)),2)*255);
		ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);

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
