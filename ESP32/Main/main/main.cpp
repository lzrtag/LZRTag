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
#include "esp_event_loop.h"

#include "esp_sleep.h"
#include "esp_pm.h"
#include "esp32/pm.h"

#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "driver/ledc.h"

#include "esp_log.h"

#include "xasin/mqtt/Handler.h"

#include "core/setup.h"

#include "IODefs.h"

#include "BatteryManager.h"
#include "BLESlaveChannel.h"

#include "AudioHandler.h"

#include "NeoController.h"
#include "fx/ManeAnimator.h"

#include "core/IR.h"

using namespace Peripheral;

auto dataRegisters = Xasin::Communication::RegisterBlock();
auto testPipe = Xasin::Communication::BLE_SlaveChannel("TestPipe", dataRegisters);

esp_err_t event_handler(void *context, system_event_t *event) {
	Xasin::MQTT::Handler::try_wifi_reconnect(event);

	LZR::mqtt.wifi_handler(event);

	return ESP_OK;
}

extern "C"
void app_main()
{
    nvs_flash_init();
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_ON);

    tcpip_adapter_init();

    esp_event_loop_init(event_handler, 0);

    LZR::setup();

    //auto testRegister = Xasin::Communication::ComRegister(0xA, dataRegisters, &batLvl, 1, true);

    while(true) {
    	vTaskDelay(1000);

    	LZR::IR::send_signal(-1);
    	//puts("Sent IR Code!");
    }

    fflush(stdout);
    esp_restart();
}
