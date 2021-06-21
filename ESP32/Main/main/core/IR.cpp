/*
 * IR.cpp
 *
 *  Created on: 18 Feb 2019
 *      Author: xasin
 */

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"

#include "IR.h"
#include "../IODefs.h"

#include "xasin/xirr/Transmitter.h"
#include "xasin/xirr/Receiver.h"

#include "cJSON.h"
#include <cstring>

#include <xnm/net_helpers.h>

namespace LZR {
namespace IR {

const char *lTag = "LZR::IR";

auto ir_tx = Xasin::XIRR::Transmitter(PIN_IR_OUT, RMT_CHANNEL_1);
auto ir_rx = Xasin::XIRR::Receiver(PIN_IR_IN, RMT_CHANNEL_2);

rmt_item32_t start_rmt_item = {};
rmt_item32_t rmt_bit_true = {};
rmt_item32_t rmt_bit_false = {};

#define TPM int(80000.0/20)
#define TPB int(TPM*0.5)

std::vector<rmt_item32_t> tx_timecodes;
uint8_t lastShotArbitration = 1;

void send_hit_event(uint8_t pID, uint8_t arbCode) {
	if(mqtt.is_disconnected())
		return;

	auto output = cJSON_CreateObject();

	cJSON_AddNumberToObject(output, "shooterID", pID);
	cJSON_AddStringToObject(output, "target", XNM::NetHelpers::get_device_id().data());
	cJSON_AddNumberToObject(output, "arbCode", arbCode);

	char outStr[100] = {};
	cJSON_PrintPreallocated(output, outStr, 100, false);

	mqtt.publish_to("event/ir_hit", outStr, strlen(outStr));

	cJSON_Delete(output);
}


void init() {
	ir_tx.init();
	ir_rx.init();

	ir_rx.on_rx = [](const void *data, uint8_t len, uint8_t channel) {
		if(channel == 129) {
			uint8_t beaconID = *reinterpret_cast<const uint8_t*>(data);
			ESP_LOGD(lTag, "Got beacon code: %d", beaconID);

			if(!mqtt.is_disconnected()) {
				char oBuff[10] = {};
				sprintf(oBuff, "%d", beaconID);
				mqtt.publish_to("event/ir_beacon", oBuff, strlen(oBuff));
			}
		}
		else if(channel >= 130 && channel < 134) {
			const uint8_t *dPtr = reinterpret_cast<const uint8_t *>(data);
			if(dPtr[0] != player.get_id())
				send_hit_event(dPtr[0], channel - 130);
		}
	};

	esp_log_level_set(lTag, ESP_LOG_INFO);

	ESP_LOGI(lTag, "Init finished");
}

void add_start() {
	tx_timecodes.push_back(start_rmt_item);
	tx_timecodes.push_back(start_rmt_item);
}

void send_signal(int8_t cCode) {
	if(cCode == -1) {
		lastShotArbitration++;
		if(lastShotArbitration == 4)
			lastShotArbitration = 0;

		cCode = lastShotArbitration;
	}

	uint8_t shotID = player.get_id();

	ir_tx.send(shotID, 130 + cCode);
}

}
}
