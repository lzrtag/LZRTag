/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>

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

auto dataRegisters = Xasin::Communication::RegisterBlock();
auto testBMan = Housekeeping::BatteryManager();

auto testPipe = Xasin::Communication::BLE_SlaveChannel("TestPipe", dataRegisters);

#define TEST_PIN_R GPIO_NUM_0
#define TEST_PIN_G GPIO_NUM_2

void set_RG_Level(int8_t percentage, uint8_t bNess = 254) {
	int gLevel = percentage *254 /100;
	int rLevel = 254 - gLevel;

	ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, rLevel*bNess / 254);
	ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, gLevel*bNess / 254);

	ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
	ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
}

extern "C"
void app_main()
{
    nvs_flash_init();
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_ON);

    printf("Hello world!\n");

    testPipe.start();

    esp_pm_config_esp32_t pCFG;
    pCFG.max_freq_mhz = 160;
    pCFG.min_freq_mhz = 80;
    pCFG.light_sleep_enable = true;
    esp_pm_configure(&pCFG);

    rtc_gpio_init(GPIO_NUM_4);
    rtc_gpio_set_direction(GPIO_NUM_4, RTC_GPIO_MODE_OUTPUT_ONLY);

    rtc_gpio_deinit(GPIO_NUM_0);
    rtc_gpio_deinit(GPIO_NUM_2);

    ledc_timer_config_t ledTCFG = {};
    ledTCFG.speed_mode = LEDC_LOW_SPEED_MODE;
    ledTCFG.duty_resolution = LEDC_TIMER_8_BIT;
    ledTCFG.freq_hz = 350;
    ledTCFG.timer_num = LEDC_TIMER_0;

    ledc_timer_config(&ledTCFG);

    ledc_timer_set(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, 350, 8, LEDC_REF_TICK);
    ledc_timer_rst(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0);
    ledc_timer_resume(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0);

    ledc_channel_config_t redLEDCFG = {};
    redLEDCFG.gpio_num = TEST_PIN_R;
    redLEDCFG.speed_mode = LEDC_LOW_SPEED_MODE;
    redLEDCFG.timer_sel = LEDC_TIMER_0;
    redLEDCFG.channel = LEDC_CHANNEL_0;
    redLEDCFG.intr_type = LEDC_INTR_DISABLE;
    redLEDCFG.duty = 250;

    ledc_channel_config(&redLEDCFG);

    ledc_bind_channel_timer(LEDC_LOW_SPEED_MODE, 0, 0);

    redLEDCFG.gpio_num = TEST_PIN_G;
    redLEDCFG.channel = LEDC_CHANNEL_1;
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

    while(true) {

		for (int i = 0; i < 300; i++) {
			//printf("Setting timer to %d...\n", i);
			vTaskDelay(10);
			if(i%300 < 100)
				set_RG_Level(batLvl, (i)*2.5);
			else if(i < 150)
				set_RG_Level(batLvl, (150 - i)*5);
			else
				set_RG_Level(0, 0);
		}
		batLvl++;
		testRegister.update();
    }
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}
