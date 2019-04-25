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

#include "cJSON.h"
#include <cstring>

namespace LZR {
namespace IR {

const char *lTag = "LZR::IR";

rmt_item32_t start_rmt_item = {};
rmt_item32_t rmt_bit_true = {};
rmt_item32_t rmt_bit_false = {};

#define TPM int(80000.0/20)
#define TPB int(TPM*0.5)

std::vector<rmt_item32_t> tx_timecodes;
uint8_t lastShotArbitration = 1;

void init_rmt_bits() {
	start_rmt_item.duration0 	= TPB*3;
	start_rmt_item.level0		= true;
	start_rmt_item.duration1	= TPB;
	start_rmt_item.level1		= false;

	rmt_bit_true.duration0 		= TPB * 7/8;
	rmt_bit_true.level0			= true;
	rmt_bit_true.duration1		= TPB * 1/8;
	rmt_bit_true.level1			= false;

	rmt_bit_false.duration0		= 1;
	rmt_bit_false.level0		= false;
	rmt_bit_false.duration1		= TPB -1;
	rmt_bit_false.level1		= false;
}

void send_hit_event(uint8_t pID, uint8_t arbCode) {
	auto output = cJSON_CreateObject();

	cJSON_AddStringToObject(output, "type", "hit");
	cJSON_AddNumberToObject(output, "shooterID", pID);
	cJSON_AddStringToObject(output, "target", player.deviceID.data());
	cJSON_AddNumberToObject(output, "arbCode", arbCode);

	char *outStr = cJSON_PrintUnformatted(output);

	mqtt.publish_to("Lasertag/Game/Events", outStr, strlen(outStr));

	cJSON_Delete(output);
}

void ir_rx_try_parse(rmt_item32_t *data, size_t num) {
	if(data == 0)
		return;
	if(num < 2)
		return;

#pragma pack(1)
	union {
		struct {
			uint8_t start:8;
			uint8_t id:8;
			uint8_t shotCode:4;
			uint8_t checksum:4;
		} bits;
		uint32_t reg;
	} dataBoi = {};
#pragma pack(0)

	uint8_t bitPos = 0;
	for(uint8_t i=0; i<num; i++) {
		int sLen = data[i].duration0 - 0.5*TPB;

		while((sLen > 0) && (bitPos < 24)) {
			dataBoi.reg |= (1^data[i].level0) << bitPos++;
			sLen -= TPB;
		}

		sLen = data[i].duration1 - 0.5*TPB;

		while((sLen > 0) && (bitPos < 24)) {
			dataBoi.reg |= (1^data[i].level1) << bitPos++;
			sLen -= TPB;
		}

		if(bitPos >= 24)
			break;
		if(data[i].duration0 == 0 || data[i].duration1 == 0)
			break;
	}

	ESP_LOGV(lTag, "Raw shot data is: %d", dataBoi.reg);

	if(dataBoi.bits.start != 0b1110111)
		return;

	for(uint8_t block = 2; block<5; block++)
		dataBoi.bits.checksum -= 0b1111 & (dataBoi.reg >> (4*block));

	if(dataBoi.bits.checksum != 0)
		return;


	send_hit_event(dataBoi.bits.id, dataBoi.bits.shotCode);
	ESP_LOGD(lTag, "Shot recorded. ID: %3d Code: %2d", dataBoi.bits.id, dataBoi.bits.shotCode);
}

void ir_rx_task(void *args) {
	RingbufHandle_t rx_buffer = nullptr;
	rmt_get_ringbuf_handle(RMT_CHANNEL_2, &rx_buffer);

	rmt_rx_start(RMT_CHANNEL_2, 1);

	ESP_LOGI(lTag, "RX Task started");

	while(true) {
		size_t dataNum = 0;
		rmt_item32_t *headItem = (rmt_item32_t *)xRingbufferReceive(rx_buffer, &dataNum, 5*600);
		if(headItem == nullptr)
			continue;

		ESP_LOGD(lTag, "Got a bit of data!");
		ir_rx_try_parse(headItem, dataNum);

		vRingbufferReturnItem(rx_buffer, headItem);
	}
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
	tx_cfg.carrier_duty_percent = 40;
	tx_cfg.carrier_freq_hz = 40000;

	cfg.tx_config = tx_cfg;

	cfg.channel  = RMT_CHANNEL_1;
	cfg.rmt_mode = RMT_MODE_TX;
	cfg.clk_div  = 20;
	cfg.gpio_num = PIN_IR_OUT;
	cfg.mem_block_num = 1;

	ESP_ERROR_CHECK(rmt_config(&cfg));
	ESP_ERROR_CHECK(rmt_driver_install(RMT_CHANNEL_1, 0, 0));

	cfg = {};

	rmt_rx_config_t rx_config = {};
	rx_config.filter_en = true;
	rx_config.filter_ticks_thresh = 5;
	rx_config.idle_threshold = TPB * 10;

	cfg.rx_config = rx_config;

	cfg.clk_div = 20;
	cfg.mem_block_num = 1;
	cfg.channel  = RMT_CHANNEL_2;
	cfg.rmt_mode = RMT_MODE_RX;
	cfg.gpio_num = PIN_IR_IN;

	ESP_ERROR_CHECK(rmt_config(&cfg));
	ESP_ERROR_CHECK(rmt_driver_install(RMT_CHANNEL_2, 1000, 0));

	xTaskCreate(ir_rx_task, "LZR:IR:RX", 4096, nullptr, 5, nullptr);

	esp_log_level_set(lTag, ESP_LOG_INFO);

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
	} dataBoi = {};
#pragma pack(0)

	if(cCode == -1) {
		lastShotArbitration++;
		if(lastShotArbitration == 16)
			lastShotArbitration = 8;

		cCode = lastShotArbitration;
	}

	dataBoi.bits.pID = LZR::player.get_id();
	dataBoi.bits.sID = cCode;

	for(uint8_t i=0; i<3; i++)
		dataBoi.bits.check += 0b1111 & (dataBoi.reg >> (i*4));

	for(uint8_t i=0; i<16; i++)
		tx_timecodes.push_back((dataBoi.reg & 1<<i) != 0 ? rmt_bit_true : rmt_bit_false);

	auto ret = rmt_write_items(RMT_CHANNEL_1, tx_timecodes.data(), tx_timecodes.size(), 1);
	ESP_ERROR_CHECK(ret);
	ESP_LOGV(lTag, "Sent beam");
}

}
}
