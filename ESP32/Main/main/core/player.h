/*
 * player.h
 *
 *  Created on: 27 Feb 2019
 *      Author: xasin
 */

#ifndef MAIN_CORE_PLAYER_H_
#define MAIN_CORE_PLAYER_H_

#include "xasin/mqtt/Handler.h"

namespace LZR {

class Player {
private:

	void process_data(Xasin::MQTT::MQTT_Packet data);

	int team;
	int brightness;

public:
	Xasin::MQTT::Handler &mqtt;
	const std::string name;

	Player(const std::string name, Xasin::MQTT::Handler &mqtt);

	void init();

	int get_team();
	int get_brightness();
};

}


#endif /* MAIN_CORE_PLAYER_H_ */
