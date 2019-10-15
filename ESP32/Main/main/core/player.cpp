/*
 * player.cpp
 *
 *  Created on: 27 Feb 2019
 *      Author: xasin
 */

#include "player.h"
#include <stdlib.h>

#include "esp_log.h"

#include "../fx/colorSets.h"

namespace LZR {

Player::Player(const std::string devID, Xasin::MQTT::Handler &mqtt) :
	ID(0),
	team(0), brightness(0),
	isMarked(false),
	markerColor(0),
	heartbeat(false),
	name(""),
	deadUntil(0), hitUntil(0), vibrateUntil(0),
	currentGun(2), shotLocked(0),
	deviceID(devID),
	mqtt(mqtt) {

	if(deviceID == "") {
			uint8_t smacc[6] = {};

			char macStr[14] = {};

			esp_read_mac(smacc, ESP_MAC_WIFI_STA);

			sprintf(macStr, "%02X.%02X.%02X.%02X.%02X.%02X",
				smacc[0], smacc[1], smacc[2],
			 	smacc[3], smacc[4], smacc[5]);

			deviceID = macStr;
		}

	mqtt.subscribe_to("Lasertag/Players/" + deviceID + "/CFG/#",
			[this](Xasin::MQTT::MQTT_Packet data) {

		ESP_LOGD("LZR::Player", "Received %s data!", data.topic.data());

		if(data.topic == "ID")
			ID = atoi(data.data.data());
		else if(data.topic == "Team")
			team = atoi(data.data.data());
		else if(data.topic == "Brightness")
			brightness = atoi(data.data.data());
		else if(data.topic == "GunNo") {
			currentGun = atoi(data.data.data());
			shotLocked = currentGun <= 0;
		}
		else if(data.topic == "Marked") {
			isMarked = (data.data.length() >= 1);
			uint32_t markerCode = atoi(data.data.data());

			if(markerCode <= 0)
				isMarked = false;
			else if(markerCode < 8)
				markerColor = LZR::teamColors[markerCode].vestShotEnergy;
			else
				markerColor = markerCode;
		}
		else if(data.topic == "Heartbeat")
			heartbeat = (data.data == "1");
		else if(data.topic == "Name")
			name = data.data;
		else if(data.topic == "Dead") {
			if((data.data.size() != 0) && (data.data == "1")) {
				if(deadUntil == 0) {
					deadUntil = portMAX_DELAY;
				}
			}
			else {
				deadUntil = 0;
			}
		}
		else if(data.topic == "Dead/Timed") {
			deadUntil = xTaskGetTickCount() + atof(data.data.data())*600;
			this->mqtt.publish_to(get_topic_base() + "/CFG/Dead", "true", 4, 1, true);
		}
		else if(data.topic == "Hit")
			hitUntil = xTaskGetTickCount() + atof(data.data.data())*600;
		else if(data.topic == "Vibrate")
			vibrateUntil = xTaskGetTickCount() + atof(data.data.data())*600;
	}, 1);
}

void Player::init() {
    Xasin::MQTT::Handler::start_wifi("Lasertag\0", "\0");

	 mqtt.start("mqtt://192.168.250.1", get_topic_base() + "/Connection");
	mqtt.set_status("OK");
}

void Player::tick() {
	if((deadUntil != 0) && (xTaskGetTickCount() > deadUntil)) {
		deadUntil = 0;
		mqtt.publish_to(get_topic_base()+"/CFG/Dead", "0", 0, 1, true);
	}
}

std::string Player::get_topic_base() {
	return "Lasertag/Players/" + deviceID;
}

std::string Player::get_device_id() {
	return deviceID;
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

pattern_mode_t Player::get_brightness() {
	if(mqtt.is_disconnected())
		return pattern_mode_t::CONNECTING;

	if(is_dead())
		return pattern_mode_t::DEAD;

	if(brightness < 0)
		return pattern_mode_t::IDLE;
	if(brightness > (pattern_mode_t::PATTERN_MODE_MAX - pattern_mode_t::IDLE))
		return pattern_mode_t::ACTIVE;

	return static_cast<pattern_mode_t>(brightness + pattern_mode_t::IDLE);

}

bool Player::is_marked() {
	return isMarked;
}
Peripheral::Color Player::get_marked_color() {
	return markerColor;
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
bool Player::should_vibrate() {
	if(is_hit())
		return false;

	return xTaskGetTickCount() < vibrateUntil;
}

}
