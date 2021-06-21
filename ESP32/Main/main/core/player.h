/*
 * player.h
 *
 *  Created on: 27 Feb 2019
 *      Author: xasin
 */

#ifndef MAIN_CORE_PLAYER_H_
#define MAIN_CORE_PLAYER_H_

#include "freertos/FreeRTOS.h"
#include "xasin/mqtt/Handler.h"

#include "../fx/animatorThread.h"

#include <cJSON.h>

namespace LZR {

class Player {
private:
	void process_data(Xasin::MQTT::MQTT_Packet data);

	uint8_t ID;

	int team;
	int brightness;

	bool isMarked;
	Xasin::NeoController::Color markerColor;

	bool heartbeat;

	std::string name;

	TickType_t deadUntil;
	TickType_t hitUntil;
	TickType_t vibrateUntil;

	int 	currentGun;

	cJSON * gun_ammo_info;

	bool	shotLocked;

public:
	Xasin::MQTT::Handler &mqtt;

	bool should_reload;

	Player(const std::string devID, Xasin::MQTT::Handler &mqtt);

	void init();
	void tick();

	int get_id();

	int get_team();
	pattern_mode_t get_brightness();

	bool 		is_marked();
	Xasin::NeoController::Color get_marked_color();

	bool get_heartbeat();

	std::string get_name();

	bool can_shoot();
	int  get_gun_num();
	void set_gun_ammo(int32_t current, int32_t clipsize, int32_t total);

	bool is_dead();
	bool is_hit();
	bool should_vibrate();
};

}


#endif /* MAIN_CORE_PLAYER_H_ */
