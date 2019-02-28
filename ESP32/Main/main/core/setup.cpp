/*
 * setup.cpp
 *
 *  Created on: 25 Jan 2019
 *      Author: xasin
 */

#include "setup.h"

#include "../IODefs.h"
#include "IR.h"
#include "../fx/animatorThread.h"

#include "driver/ledc.h"

#include <array>

#include "esp_log.h"

namespace LZR {

CORE_WEAPON_STATUS main_weapon_status = INITIALIZING;

Housekeeping::BatteryManager battery = Housekeeping::BatteryManager();

Xasin::Peripheral::AudioHandler  audioManager = Xasin::Peripheral::AudioHandler();
Peripheral::NeoController RGBController = Peripheral::NeoController(PIN_WS2812_OUT, RMT_CHANNEL_0, 5);

Xasin::MQTT::Handler mqtt = Xasin::MQTT::Handler();

Lasertag::GunHandler gunHandler = Lasertag::GunHandler(PIN_TRIGR, audioManager);

void setup_io_pins() {
	gpio_config_t outCFG = {};

	outCFG.pin_bit_mask = (1<< PIN_VBRT |
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

void setup_adc() {
	adc1_config_width(ADC_WIDTH_BIT_12);
	adc1_config_channel_atten(ADC_BAT_MES, ADC_ATTEN_DB_11);

	adc_power_on();
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

void set_ledc() {
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

void setup_audio() {
    i2s_pin_config_t i2sPins = {
    		PIN_I2S_BLCK,
			PIN_I2S_LRCK,
			PIN_I2S_DATA,
			-1
    };
    audioManager.start_thread(i2sPins);
}

std::array<uint16_t, 20> battery_samples;
int8_t battery_sample_pos = -1;
void take_battery_measurement() {
	uint32_t rawBattery = 0;
	for(int i=0; i<6; i++) {
		rawBattery += adc1_get_raw(ADC_BAT_MES);
		vTaskDelay(4);
	}
	rawBattery = rawBattery/6;
	rawBattery = ((3300 * rawBattery)/ 4096) * 3 / 2 * (4195/4130); // TODO Measure proper ADC correction factors

	if(battery_sample_pos == -1)
		battery_samples.fill(rawBattery);
	else
		battery_samples[battery_sample_pos] = rawBattery;

	if(++battery_sample_pos >= battery_samples.size())
		battery_sample_pos = 0;

	uint32_t battery_avg = 0;
	for(auto s : battery_samples)
		battery_avg += s;

	battery.set_voltage(battery_avg / battery_samples.size());
	battery.is_charging = !gpio_get_level(PIN_BAT_CHGING);

	if(battery.current_capacity() < 5 && !battery.is_charging)
		main_weapon_status = DISCHARGED;

	ESP_LOGI("LZR::Core", "%sBattery level: %s%d",
			battery.current_capacity() < 20 ? LOG_COLOR("33") : "",
			battery.is_charging ? "^" : "",
			battery.current_capacity());
}

void housekeeping_thread(void *args) {
	TickType_t nextTick;

	// FIXME DEBUG
//	if(!gpio_get_level(PIN_BAT_CHGING))
//		main_weapon_status = CHARGING;

	while(true) {
		take_battery_measurement();

		vTaskDelayUntil(&nextTick, 1800);
	}
}

void setup() {
	power_config();

	vTaskDelay(10);

	setup_io_pins();
	setup_adc();
	set_ledc();

	xTaskCreate(housekeeping_thread, "Housekeeping", 2*1024, nullptr, 10, nullptr);

	vTaskDelay(10);

	IR::init();
	setup_audio();
	mqtt.start("mqtt://iot.eclipse.org");

	vTaskDelay(10);

	start_animation_thread();

	if(main_weapon_status == INITIALIZING)
		main_weapon_status = NOMINAL;

	puts("Initialisation finished!");
}
}
