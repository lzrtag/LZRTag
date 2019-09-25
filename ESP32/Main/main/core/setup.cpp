/*
 * setup.cpp
 *
 *  Created on: 25 Jan 2019
 *      Author: xasin
 */

#include "setup.h"

#include "../IODefs.h"
#include "IR.h"
#include "gyro.h"
#include "../fx/animatorThread.h"

#include "driver/ledc.h"

#include <array>
#include <cmath>

#include "esp_log.h"

namespace LZR {

CORE_WEAPON_STATUS main_weapon_status = INITIALIZING;

Housekeeping::BatteryManager battery = Housekeeping::BatteryManager();

Xasin::Peripheral::AudioHandler  audioManager = Xasin::Peripheral::AudioHandler();
Peripheral::NeoController 	RGBController = Peripheral::NeoController(PIN_WS2812_OUT, RMT_CHANNEL_0, WS2812_NUMBER);
Xasin::I2C::LSM6DS3			gyro = Xasin::I2C::LSM6DS3();

Xasin::MQTT::Handler mqtt = Xasin::MQTT::Handler();

LZR::Player player = LZR::Player("", mqtt);
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

	inCFG.pin_bit_mask = (
			1<< PIN_IR_IN |
			1<< PIN_TRIGR |
			1<< PIN_BAT_CHGING | 1<< PIN_CTRL_FWD | 1<< PIN_CTRL_BACK);
	inCFG.mode			= GPIO_MODE_INPUT;
	inCFG.pull_up_en	= GPIO_PULLUP_ENABLE;
	inCFG.pull_down_en	= GPIO_PULLDOWN_DISABLE;
	inCFG.intr_type		= GPIO_INTR_DISABLE;

	gpio_config(&inCFG);

	inCFG.pin_bit_mask = 1<< PIN_CTRL_DOWN;
	inCFG.mode = GPIO_MODE_INPUT_OUTPUT_OD;
	gpio_config(&inCFG);

	XaI2C::MasterAction::init(PIN_I2C_SDA, PIN_I2C_SCL, I2C_NUM_0, 50000);
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
TickType_t lastBatteryUpdate = 0;

void take_battery_measurement() {
	uint32_t rawBattery = 0;
	for(int i=0; i<6; i++) {
		rawBattery += adc1_get_raw(ADC_BAT_MES);
		vTaskDelay(4);
	}
	rawBattery = rawBattery/6;
	rawBattery = ((3300 * rawBattery)/ 4096) * 3/2 * (3853/3691.0); // TODO Measure proper ADC correction factors

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

	// Small debounce for battery percentage
	auto bMin = 5;
	if(main_weapon_status == NOMINAL)
		bMin = 1;
	if(battery.current_capacity() < bMin && !battery.is_charging) {
		if(main_weapon_status == NOMINAL) {
			ESP_LOGE("LZR::Core", "Battery critically low, shutting down!");
			vTaskDelay(3);
			esp_restart();
		}

		main_weapon_status = DISCHARGED;
	}

	if(xTaskGetTickCount() > (30*600 + lastBatteryUpdate)) {
		ESP_LOGI("LZR::Core", "%sBattery level: %s%d",
				battery.current_capacity() < 20 ? LOG_COLOR("33") : "",
				battery.is_charging ? "^" : "",
				battery.current_mv());

		lastBatteryUpdate = xTaskGetTickCount();
	}
}

void setup_ping_req() {
	mqtt.subscribe_to(player.get_topic_base() + "/System/Ping",
			[](Xasin::MQTT::MQTT_Packet data) {

		struct {
			uint32_t batLevel;
			uint32_t batCapacity;
			uint32_t ping;
		} sysData = {
			.batLevel = battery.current_mv(),
			.batCapacity = battery.current_capacity(),
		.ping = uint32_t((xTaskGetTickCount() - *reinterpret_cast<const uint32_t*>(data.data.data()))/0.6)
		};

		mqtt.publish_to(player.get_topic_base() + "/System", &sysData, sizeof(sysData), 1);
	});
}

void send_ping_req() {
	uint32_t outData = xTaskGetTickCount();
	mqtt.publish_to(player.get_topic_base() + "/System/Ping", &outData, 4, 0);
}

int old_switch_position = 255;
int nav_switch_debounce = 0;
void navswitch_tick() {
	uint8_t nSwitch = LZR::read_nav_switch();

	if(old_switch_position != nSwitch)
		nav_switch_debounce++;
	else
		nav_switch_debounce = 0;

	if(nav_switch_debounce >= 2) {
		old_switch_position = nSwitch;
		nav_switch_debounce = 0;

		if((!LZR::mqtt.is_disconnected()) && (old_switch_position != 0)) {
			uint8_t dataBuffer = '0' + old_switch_position;

			mqtt.publish_to(LZR::player.get_topic_base() + "/NSwitch", &dataBuffer, 1, 0, 2);
		}
	}
}

void housekeeping_thread(void *args) {
	TickType_t nextHWTick = xTaskGetTickCount();

	if(!gpio_get_level(PIN_BAT_CHGING))
		main_weapon_status = CHARGING;

	while(true) {
		if(xTaskGetTickCount() > nextHWTick) {
			take_battery_measurement();

			if(!mqtt.is_disconnected())
				send_ping_req();

			nextHWTick += 1800;
		}

		navswitch_tick();
		GYR::tick();

		vTaskDelay(30);
	}
}

void shutdown_system() {
	LZR::FX::target_mode = LZR::OFF;
	vTaskDelay(100);

	esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
	esp_deep_sleep_start();
}

void setup() {
	power_config();

	vTaskDelay(10);

	setup_io_pins();
	setup_adc();
	set_ledc();

	IR::init();
	GYR::init();

	xTaskCreate(housekeeping_thread, "Housekeeping", 2*1024, nullptr, 10, nullptr);

	setup_audio();

	start_animation_thread();

    LZR::FX::target_mode = LZR::BATTERY_LEVEL;
    vTaskDelay(200);

    if(main_weapon_status == DISCHARGED) {
    	ESP_LOGE("LZR::Core", "Battery low, sleeping!");

    	vTaskDelay(3*600);
    	shutdown_system();
    }
    else if(main_weapon_status == CHARGING) {
    	ESP_LOGI("LZR::Core", "Charging detected, entering CHG mode");

    	LZR::FX::target_mode = LZR::CHARGE;
    }
    else {
    	player.init();

        vTaskDelay(3*600);
        LZR::FX::target_mode = LZR::PLAYER_DECIDED;

    	setup_ping_req();

    	main_weapon_status = NOMINAL;
    }

	ESP_LOGI("LZR::Core", "Init finished");
}

uint8_t read_nav_switch() {
	uint8_t outVal = 0;

	if(!gpio_get_level(PIN_CTRL_DOWN))
		outVal = 1;
	else {
		gpio_set_level(PIN_CTRL_DOWN, false);

		if(!gpio_get_level(PIN_CTRL_FWD))
			outVal = 2;
		if(!gpio_get_level(PIN_CTRL_BACK))
			outVal = 3;
	}


	gpio_set_level(PIN_CTRL_DOWN, true);
	return outVal;
}

}
