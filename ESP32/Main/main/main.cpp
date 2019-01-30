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
