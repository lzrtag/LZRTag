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

namespace LZR {

class Player {
private:
	void process_data(Xasin::MQTT::MQTT_Packet data);

	uint8_t ID;

	int team;
	int brightness;

	bool heartbeat;

	std::string name;

	TickType_t deadUntil;
	TickType_t hitUntil;

	uint8_t currentGun;
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
	int get_brightness();

	bool get_heartbeat();

	std::string get_name();

	bool can_shoot();

	bool is_dead();
	bool is_hit();
};

}


#endif /* MAIN_CORE_PLAYER_H_ */
