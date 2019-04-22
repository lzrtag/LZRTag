/*
 * player.cpp
 *
 *  Created on: 27 Feb 2019
 *      Author: xasin
 */

#include "player.h"
#include <stdlib.h>

namespace LZR {

Player::Player(const std::string name, Xasin::MQTT::Handler &mqtt) :
	team(0), brightness(0),
	mqtt(mqtt), name(name) {

	mqtt.subscribe_to("Lasertag/Players/" + name + "/#",
			[this](Xasin::MQTT::MQTT_Packet data) {

		if(data.topic == "Team")
			team = atoi(data.data.data());
	});
}

void Player::init() {
	mqtt.start("mqtt://192.168.250.1", "Lasertag/Players/" + name + "/Status");
	mqtt.set_status("OK");
}

int Player::get_team() {
	return this->team;
}

}