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

#include <xnm/net_helpers.h>

#include "test.h"
// #include "SSD1306.h"
// #include "DrawBox.h"



using namespace Xasin::NeoController;

esp_err_t event_handler(void *context, system_event_t *event) {
	Xasin::MQTT::Handler::try_wifi_reconnect(event);

	LZR::mqtt.wifi_handler(event);

	return ESP_OK;
}

// using namespace Peripheral;
// font = Peripheral::OLED::console_font_6x8;
// using namespace XaI2C;

// Peripheral::OLED::SSD1306 screen = Peripheral::OLED::SSD1306();
// Peripheral::OLED::DrawBox *drawBox = nullptr;
// Peripheral::OLED::FontType fontface = nullptr;

// Peripheral::OLED::DrawBox drawBox = Peripheral::OLED::DrawBox(128, 32 &screen);
// extern FontType console_font_6x8;


// #include "SSD1306.h"
// #include "DrawBox.h"
// using namespace Peripheral;
// using namespace XaI2C;

// Peripheral::OLED::SSD1306 screen = Peripheral::OLED::SSD1306();
// OLED::DrawBox drawBox = OLED::DrawBox(128, 64, &screen);

extern "C"
void app_main()
{
    nvs_flash_init();
    //esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    //esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_ON);

    tcpip_adapter_init();

    esp_event_loop_init(event_handler, 0);

    LZR::setup();

    XNM::NetHelpers::init_global_r3_ca();
    XNM::NetHelpers::set_mqtt(LZR::mqtt);

    XNM::NetHelpers::init();
    // screen.initialize();
    // drawBox.visible = true;




    // drawBox = new OLED::DrawBox(100, 32, &screen);
    // drawBox.visible = true;
    // drawBox.write_string(32, 23, std::string("HELLO"), 1 , 1, nullptr);


    while(true) {
		 vTaskDelay(10*600);
        //  drawBox.write_string(0, 0, std::string("HP:100"),21 , 1, nullptr);
        //  drawBox.write_string(0, 36, std::string("GUN:3"),21 , 1, nullptr);
    }

    fflush(stdout);
    esp_restart();
}
