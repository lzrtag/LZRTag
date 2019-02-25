/*
 * IR.cpp
 *
 *  Created on: 18 Feb 2019
 *      Author: xasin
 */

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"

#include "IR.h"
#include "../IODefs.h"

namespace LZR {
namespace IR {

const char *lTag = "LZR::IR";

rmt_item32_t start_rmt_item = {};
rmt_item32_t rmt_bit_true = {};
rmt_item32_t rmt_bit_false = {};

#define TPM (80000.0/20)

std::vector<rmt_item32_t> tx_timecodes;
uint8_t lastShotArbitration = 1;

void init_rmt_bits() {
	start_rmt_item.duration0 	= TPM*1.5;
	start_rmt_item.level0		= true;
	start_rmt_item.duration1	= TPM*0.5;
	start_rmt_item.level1		= false;

	rmt_bit_true.duration0 		= TPM/2 * 7/8;
	rmt_bit_true.level0			= true;
	rmt_bit_true.duration1		= TPM/2 * 1/8;
	rmt_bit_true.level1			= false;

	rmt_bit_false.duration0		= 1;
	rmt_bit_false.level0		= false;
	rmt_bit_false.duration1		= TPM/2 -1;
	rmt_bit_false.level1		= false;
}

void init() {
	init_rmt_bits();

	rmt_config_t cfg = {};
	rmt_tx_config_t tx_cfg = {};

	tx_cfg.idle_level = RMT_IDLE_LEVEL_LOW;
	tx_cfg.idle_output_en = true;
	tx_cfg.loop_en = false;

	tx_cfg.carrier_level = RMT_CARRIER_LEVEL_HIGH;
	tx_cfg.carrier_en = true;
	tx_cfg.carrier_duty_percent = 60;
	tx_cfg.carrier_freq_hz = 40000;

	cfg.tx_config = tx_cfg;

	cfg.channel  = RMT_CHANNEL_1;
	cfg.rmt_mode = RMT_MODE_TX;
	cfg.clk_div  = 20;
	cfg.gpio_num = PIN_IR_OUT;
	cfg.mem_block_num = 1;

	ESP_ERROR_CHECK(rmt_config(&cfg));
	ESP_ERROR_CHECK(rmt_driver_install(RMT_CHANNEL_1, 0, 0));

	rmt_rx_config_t rx_config = {};
	rx_config.filter_en = false;
	rx_config.idle_threshold = 1200;
	cfg.rx_config = rx_config;

	cfg.channel  = RMT_CHANNEL_2;
	cfg.rmt_mode = RMT_MODE_RX;
	cfg.gpio_num = PIN_IR_IN;

	ESP_ERROR_CHECK(rmt_config(&cfg));
	ESP_ERROR_CHECK(rmt_driver_install(RMT_CHANNEL_2, 1, 0));

	esp_log_level_set(lTag, ESP_LOG_VERBOSE);

	ESP_LOGI(lTag, "Init finished");
}

void add_start() {
	tx_timecodes.push_back(start_rmt_item);
	tx_timecodes.push_back(start_rmt_item);
}

void send_signal(int8_t cCode) {
	tx_timecodes.clear();
	add_start();

#pragma pack(1)
	union {
		struct{
			uint8_t pID;
			uint8_t sID:4;
			uint8_t check:4;
		} bits;
		uint16_t reg;
	} dataBoi;
#pragma pack(0)

	if(cCode == -1) {
		lastShotArbitration++;
		if(lastShotArbitration == 15)
			lastShotArbitration = 1;

		cCode = lastShotArbitration;
	}

	dataBoi.bits.pID = 123; // FIXME and replace with MQTT-Assigned value
	dataBoi.bits.sID = cCode;

	for(uint8_t i=0; i<3; i++)
		dataBoi.bits.check ^= 0b1111 & (dataBoi.reg >> i*4);

	for(uint8_t i=0; i<16; i++)
		tx_timecodes.push_back((dataBoi.reg & 1<<i) != 0 ? rmt_bit_true : rmt_bit_false);

	auto ret = rmt_write_items(RMT_CHANNEL_1, tx_timecodes.data(), tx_timecodes.size(), 1);
	ESP_ERROR_CHECK(ret);
	ESP_LOGV(lTag, "Sent beam");
}

}
}
