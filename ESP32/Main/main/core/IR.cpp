/*
 * IR.cpp
 *
 *  Created on: 18 Feb 2019
 *      Author: xasin
 */


#include "IR.h"
#include "../IODefs.h"

namespace LZR {
namespace IR {

void init() {
	rmt_config_t cfg = {};
	rmt_tx_config_t tx_cfg = {};
	tx_cfg.idle_level = RMT_IDLE_LEVEL_LOW;
	tx_cfg.idle_output_en = true;
	tx_cfg.loop_en = false;
	cfg.tx_config = tx_cfg;

	cfg.channel  = RMT_CHANNEL_1;
	cfg.rmt_mode = RMT_MODE_TX;
	cfg.clk_div  = 200;
	cfg.gpio_num = PIN_IR_OUT;
	cfg.mem_block_num = 1;

	rmt_config(&cfg);
	rmt_driver_install(RMT_CHANNEL_1, 0, 0);

	rmt_rx_config_t rx_config = {};
	rx_config.filter_en = false;
	rx_config.idle_threshold = 1200;
	cfg.rx_config = rx_config;
	cfg.tx_config = {};

	cfg.channel  = RMT_CHANNEL_2;
	cfg.rmt_mode = RMT_MODE_RX;
	cfg.gpio_num = PIN_IR_IN;

	rmt_config(&cfg);
	rmt_driver_install(RMT_CHANNEL_2, 1, 0);

}

void send_signal(int8_t cCode) {

}

}
}
