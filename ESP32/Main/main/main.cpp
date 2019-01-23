/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <cmath>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "nvs_flash.h"

#include "esp_sleep.h"
#include "esp_pm.h"
#include "esp32/pm.h"

#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "driver/ledc.h"

#include "IODefs.h"

#include "BatteryManager.h"
#include "BLESlaveChannel.h"

#include "AudioHandler.h"

#include "NeoController.h"
#include "ManeAnimator.h"

#include "GunHandler.h"

#include "audio_example_file.h"

using namespace Peripheral;

auto audio_example_cassette = Xasin::Peripheral::AudioCassette(audio_table, sizeof(audio_table));

auto dataRegisters = Xasin::Communication::RegisterBlock();
auto testBMan = Housekeeping::BatteryManager();

auto testPipe = Xasin::Communication::BLE_SlaveChannel("TestPipe", dataRegisters);

auto audioMan = Xasin::Peripheral::AudioHandler();

auto RGBController = Peripheral::NeoController(PIN_WS2812_OUT, RMT_CHANNEL_0, 5);

auto gunHandler = Lasertag::GunHandler(PIN_TRIGR);

void animator_task(void *data) {
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

		if(gunHandler.timeSinceLastShot() < 3) {
			newMuzzleColor.merge_overlay(muzzleFlashColor);
		    //audioMan.insert_cassette(audio_example_cassette);
		}
		gpio_set_level(PIN_VBRT, gunHandler.timeSinceLastShot() <= 30 ? 1 : 0);


		Color actualMuzzle = Color();
		actualMuzzle.r = newMuzzleColor.g;
		actualMuzzle.g = newMuzzleColor.r;
		actualMuzzle.b = newMuzzleColor.b;
		RGBController.colors[0] = actualMuzzle;

		RGBController.update();

		vTaskDelayUntil(&lastTick, 10);
	}
}

void set_pins() {
	gpio_set_direction(PIN_IR_OUT, GPIO_MODE_OUTPUT);
	gpio_set_level(PIN_IR_OUT, false);

	gpio_set_direction(PIN_IR_IN, GPIO_MODE_INPUT);

	gpio_set_direction(PIN_BAT_CHGING, GPIO_MODE_INPUT);

	gpio_set_direction(PIN_VBRT, GPIO_MODE_OUTPUT);
	gpio_set_level(PIN_VBRT, false);
}

extern "C"
void app_main()
{
    nvs_flash_init();
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_ON);

    set_pins();

    esp_pm_config_esp32_t pCFG;
    pCFG.max_freq_mhz = 160;
    pCFG.min_freq_mhz = 80;
    pCFG.light_sleep_enable = false;
    esp_pm_configure(&pCFG);

    ledc_timer_config_t ledTCFG = {};
    ledTCFG.speed_mode = LEDC_LOW_SPEED_MODE;
    ledTCFG.duty_resolution = LEDC_TIMER_8_BIT;
    ledTCFG.freq_hz = 350;
    ledTCFG.timer_num = LEDC_TIMER_0;

    ledc_timer_config(&ledTCFG);

    ledc_timer_set(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, 350, 8, LEDC_REF_TICK);

    ledc_channel_config_t redLEDCFG = {};
    redLEDCFG.gpio_num = PIN_BAT_GREEN;
    redLEDCFG.speed_mode = LEDC_LOW_SPEED_MODE;
    redLEDCFG.timer_sel = LEDC_TIMER_0;
    redLEDCFG.channel = LEDC_CHANNEL_0;
    redLEDCFG.intr_type = LEDC_INTR_DISABLE;
    redLEDCFG.duty = 250;

    ledc_channel_config(&redLEDCFG);

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    uint8_t batLvl = 0;

    auto testRegister = Xasin::Communication::ComRegister(0xA, dataRegisters, &batLvl, 1, true);

    TaskHandle_t animatorTaskHandle;
    xTaskCreatePinnedToCore(animator_task, "Animator", 4*1024, nullptr, 10, &animatorTaskHandle, 1);

    audioMan.start_thread();

    while(true) {
    	vTaskDelay(10000);
    }

//    while(true) {
//
//		for (int i = 0; i < 300; i++) {
//			//printf("Setting timer to %d...\n", i);
//			vTaskDelay(10);
//			if(i%300 < 100)
//				set_RG_Level(batLvl, (i)*2.5);
//			else if(i < 150)
//				set_RG_Level(batLvl, (150 - i)*5);
//			else
//				set_RG_Level(0, 0);
//		}
//		batLvl++;
//		testRegister.update();
//    }
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}
