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

namespace LZR {

class Player {
private:
	void process_data(Xasin::MQTT::MQTT_Packet data);

	uint8_t ID;

	int team;
	int brightness;

	bool isMarked;
	Peripheral::Color markerColor;

	bool heartbeat;

	std::string name;

	TickType_t deadUntil;
	TickType_t hitUntil;

	int 	currentGun;
	bool	shotLocked;

public:
	Xasin::MQTT::Handler &mqtt;
	const std::string deviceID;

	Player(const std::string devID, Xasin::MQTT::Handler &mqtt);

	void init();
	void tick();

	std::string get_topic_base();

	int get_id();

	int get_team();
	pattern_mode_t get_brightness();

	bool 		is_marked();
	Peripheral::Color get_marked_color();

	bool get_heartbeat();

	std::string get_name();

	bool can_shoot();
	int  get_gun_num();

	bool is_dead();
	bool is_hit();
};

}


#endif /* MAIN_CORE_PLAYER_H_ */
