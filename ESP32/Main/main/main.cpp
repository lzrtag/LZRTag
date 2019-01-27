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

#include "core/setup.h"

#include "IODefs.h"

#include "BatteryManager.h"
#include "BLESlaveChannel.h"

#include "AudioHandler.h"

#include "NeoController.h"
#include "fx/ManeAnimator.h"


using namespace Peripheral;

auto dataRegisters = Xasin::Communication::RegisterBlock();
auto testPipe = Xasin::Communication::BLE_SlaveChannel("TestPipe", dataRegisters);

void enable_led_pwm(gpio_num_t pin, ledc_channel_t led_channel) {
	ledc_channel_config_t redLEDCFG = {};
	redLEDCFG.gpio_num = pin;
	redLEDCFG.speed_mode = LEDC_LOW_SPEED_MODE;
	redLEDCFG.timer_sel = LEDC_TIMER_0;
	redLEDCFG.channel = led_channel;
	redLEDCFG.intr_type = LEDC_INTR_DISABLE;
	redLEDCFG.duty = 0;

	ledc_channel_config(&redLEDCFG);
}

void set_led() {
	esp_pm_config_esp32_t pCFG;
	pCFG.max_freq_mhz = 80;
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

	enable_led_pwm(PIN_BAT_GREEN, LEDC_CHANNEL_0);
	enable_led_pwm(PIN_BAT_RED,   LEDC_CHANNEL_1);
	enable_led_pwm(PIN_CONN_IND,  LEDC_CHANNEL_2);
}

extern "C"
void app_main()
{
    nvs_flash_init();
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_ON);

    LZR::setup();

    //auto testRegister = Xasin::Communication::ComRegister(0xA, dataRegisters, &batLvl, 1, true);

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
