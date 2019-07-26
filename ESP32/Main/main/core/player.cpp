/*
 * player.cpp
 *
 *  Created on: 27 Feb 2019
 *      Author: xasin
 */

#include "player.h"
#include <stdlib.h>

#include "esp_log.h"

#include "../../LZROptions.h"

namespace LZR {

Player::Player(const std::string devID, Xasin::MQTT::Handler &mqtt) :
	ID(0),
	team(0), brightness(0), isMarked(false), heartbeat(false),
	name(""),
	deadUntil(0), hitUntil(0),
	currentGun(0), shotLocked(0),
	mqtt(mqtt), deviceID(devID) {

	if(devID == "") {
		uint8_t smacc[6] = {};

		char macStr[14] = {};

		esp_read_mac(smacc, ESP_MAC_WIFI_STA);

		sprintf(macStr, "%2X.%2X.%2X.%2X.%2X.%2X",
			smacc[0], smacc[1], smacc[2],
		 	smacc[3], smacc[4], smacc[5]);

		deviceID = macStr;
	}

	mqtt.subscribe_to("Lasertag/Players/" + deviceID + "/#",
			[this](Xasin::MQTT::MQTT_Packet data) {

		ESP_LOGD("LZR::Player", "Received %s data!", data.topic.data());

		if(data.topic == "ID")
			ID = atoi(data.data.data());
		else if(data.topic == "Team")
			team = atoi(data.data.data());
		else if(data.topic == "FX/Brightness")
			brightness = atoi(data.data.data());
		else if(data.topic == "GunNo") {
			currentGun = atoi(data.data.data());
			shotLocked = currentGun <= 0;
		}
		else if(data.topic == "FX/Marked")
			isMarked = (data.data == "1");
		else if(data.topic == "FX/Heartbeat")
			heartbeat = (data.data == "1");
		else if(data.topic == "Name")
			name = data.data;
		else if(data.topic == "Dead") {
			if(data.data == "true") {
				if(deadUntil == 0)
					deadUntil = portMAX_DELAY;
			}
			else
				deadUntil = 0;
		}
		else if(data.topic == "Dead/Timed") {
			deadUntil = xTaskGetTickCount() + atof(data.data.data())*600;
			this->mqtt.publish_to(get_topic_base() + "/Dead", "true", 4, 1, true);
		}
		else if(data.topic == "FX/Hit")
			hitUntil = xTaskGetTickCount() + atof(data.data.data())*600;
	});
}

void Player::init() {
	mqtt.start(MQTT_SERVER_ADDR, get_topic_base() + "/Connection");
	mqtt.set_status("OK");
}

void Player::tick() {
	if(deadUntil != 0 && xTaskGetTickCount() > deadUntil) {
		deadUntil = 0;
		mqtt.publish_to(get_topic_base()+"/Dead", "", 0, 1, true);
	}
}

std::string Player::get_topic_base() {
	return "Lasertag/Players/" + deviceID;
}

int Player::get_id() {
	return ID;
}

int Player::get_team() {
	if(team < 0)
		return 0;
	if(team > 7)
		return 0;
	return this->team;
}
int Player::get_brightness() {
	if(is_dead())
		return 1;

	if(brightness < 0)
		return 0;
	if(brightness > 3)
		return 3;
	return brightness;
}

bool Player::is_marked() {
	return isMarked;
}
bool Player::get_heartbeat() {
	return heartbeat;
}

std::string Player::get_name() {
	return name;
}

bool Player::can_shoot() {
	if(ID == 0)
		return false;

	if(is_dead())
		return false;
	if(shotLocked)
		return false;

	return true;
}
int Player::get_gun_num() {
	return currentGun;
}

bool Player::is_dead() {
	return xTaskGetTickCount() < deadUntil;
}
bool Player::is_hit() {
	return xTaskGetTickCount() < hitUntil;
}

}
