/*
 * main.cpp
 *
 *  Refined on: 10.05.2019
 *      Author: xasin
 *
 *  Enjoy the nerdness!
 */

#include <stdio.h>
#include <cmath>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"

#include "esp_log.h"

#include "core/setup.h"

#include "fx/animatorThread.h"

using namespace Peripheral;

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
    LZR::audioManager.volumeMod = 200;

    LZR::setup();
    Xasin::MQTT::Handler::start_wifi("Lasertag\0", "\0");

    while(true) {
    }

    fflush(stdout);
    esp_restart();
}
