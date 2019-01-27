/*
 * setup.cpp
 *
 *  Created on: 25 Jan 2019
 *      Author: xasin
 */

#include "setup.h"

#include "../IODefs.h"

#include "../fx/animatorThread.h"

#include "driver/ledc.h"

namespace LZR {

Housekeeping::BatteryManager battery = Housekeeping::BatteryManager();

Xasin::Peripheral::AudioHandler  audioManager = Xasin::Peripheral::AudioHandler();
Peripheral::NeoController RGBController = Peripheral::NeoController(PIN_WS2812_OUT, RMT_CHANNEL_0, 5);

Lasertag::GunHandler gunHandler = Lasertag::GunHandler(PIN_TRIGR, audioManager);

void setup_io_pins() {
	gpio_config_t outCFG = {};

	outCFG.pin_bit_mask = (1<<PIN_IR_OUT |
			1<< PIN_VBRT |
			1<< PIN_BAT_GREEN | 1<< PIN_BAT_RED |
			1<< PIN_CONN_IND);
	outCFG.mode 		= GPIO_MODE_OUTPUT;
	outCFG.pull_down_en = GPIO_PULLDOWN_DISABLE;
	outCFG.pull_up_en 	= GPIO_PULLUP_DISABLE;
	outCFG.intr_type	= GPIO_INTR_DISABLE;

	gpio_config(&outCFG);

	gpio_config_t inCFG = {};

	inCFG.pin_bit_mask = (1<< PIN_I2C_SDA | 1<<PIN_I2C_SCL |
			1<< PIN_IR_IN |
			1<< PIN_TRIGR |
			1<< PIN_BAT_CHGING);
	inCFG.mode			= GPIO_MODE_INPUT;
	inCFG.pull_up_en	= GPIO_PULLUP_ENABLE;
	inCFG.pull_down_en	= GPIO_PULLDOWN_DISABLE;
	inCFG.intr_type		= GPIO_INTR_DISABLE;

	gpio_config(&inCFG);
}

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

void power_config() {
	esp_pm_config_esp32_t pCFG;
	pCFG.max_freq_mhz = 80;
	pCFG.min_freq_mhz = 80;
	pCFG.light_sleep_enable = false;
	esp_pm_configure(&pCFG);
}

void set_audio() {
    i2s_pin_config_t i2sPins = {
    		PIN_I2S_BLCK,
			PIN_I2S_LRCK,
			PIN_I2S_DATA,
			-1
    };
    audioManager.start_thread(i2sPins);
}

void setup() {
	power_config();

	vTaskDelay(10);

	setup_io_pins();

	set_led();

	vTaskDelay(10);

	set_audio();

	vTaskDelay(100);

	start_animation_thread();

	puts("Initialisation finished!");
}
}
