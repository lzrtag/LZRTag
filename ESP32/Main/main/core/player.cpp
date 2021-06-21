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
#include "../../LZROptions.h"

#include <cJSON.h>

namespace LZR {

Player::Player(const std::string devID, Xasin::MQTT::Handler &mqtt) :
	ID(0),
	team(0), brightness(0),
	isMarked(false),
	markerColor(0),
	heartbeat(false),
	name(""),
	deadUntil(0), hitUntil(0), vibrateUntil(0),
	currentGun(0), shotLocked(0),
	mqtt(mqtt), should_reload(false) {

	mqtt.subscribe_to("event/#",
		[this](Xasin::MQTT::MQTT_Packet data) {
			if(data.topic == "hit")
				hitUntil = xTaskGetTickCount() + atof(data.data.data())*600;
			else if(data.topic == "vibrate")
				vibrateUntil = xTaskGetTickCount() + atof(data.data.data())*600;
			else if(data.topic == "reload")
				should_reload = true;
		}
	, 0);

	mqtt.subscribe_to("get/#",
			[this](Xasin::MQTT::MQTT_Packet data) {

		cJSON * json;
		if(data.data.size() > 0)
			json = cJSON_Parse(data.data.data());
		else
			json = cJSON_CreateNull();

		if(json == nullptr)
			return;

		if(data.topic == "id" && (cJSON_IsNumber(json) || cJSON_IsNull(json)))
			ID = json->valueint;
		else if(data.topic == "team" && (cJSON_IsNumber(json) || cJSON_IsNull(json)))
			team = json->valueint;
		else if(data.topic == "brightness" && (cJSON_IsNumber(json) || cJSON_IsNull(json)))
			brightness = json->valueint;
		else if(data.topic == "gun_config") {
			if(cJSON_IsNull(json)) {
				currentGun = 0;
			}
			else if(cJSON_IsNumber(json))
				currentGun = json->valueint;
			
			shotLocked = currentGun <= 0;
		}
		else if(data.topic == "mark_config") {
			isMarked = !cJSON_IsFalse(json);
			uint32_t markerCode = json->valueint;

			if(markerCode <= 0)
				isMarked = false;
			else if(markerCode < 8)
				markerColor = LZR::teamColors[markerCode].vestShotEnergy;
			else
				markerColor = markerCode;
		}
		else if(data.topic == "heartbeat")
			heartbeat = cJSON_IsTrue(json);
		else if(data.topic == "name")
			name = json->valuestring;
		else if(data.topic == "dead") {
			if(cJSON_IsTrue(json)) {
				if(deadUntil == 0) {
					deadUntil = portMAX_DELAY;
				}
			}
			else {
				deadUntil = 0;
			}
		}

		cJSON_Delete(json);
	}, 1);
}

void Player::init() {
	Xasin::MQTT::Handler::start_wifi(WIFI_STATION_SSID, WIFI_STATION_PASSWD);

	mqtt.start(MQTT_SERVER_ADDR);
	mqtt.set_status("OK");
}

void Player::tick() {
	if((deadUntil != 0) && (xTaskGetTickCount() > deadUntil)) {
		deadUntil = 0;
		mqtt.publish_to("get/dead", "false", 0, 1, true);
	}
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
Xasin::NeoController::Color Player::get_marked_color() {
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
void Player::set_gun_ammo(int32_t current, int32_t clipsize, int32_t total) {
	if(gun_ammo_info == nullptr) {
		gun_ammo_info = cJSON_CreateObject();
		cJSON_AddNumberToObject(gun_ammo_info, "current", 0);
		cJSON_AddNumberToObject(gun_ammo_info, "clipsize", 0);
		cJSON_AddNumberToObject(gun_ammo_info, "total", 0);
	}

	bool changed = false;

	cJSON * json = cJSON_GetObjectItem(gun_ammo_info, "current");
	if(json->valueint != current) {
		changed = true;
		cJSON_SetNumberValue(json, current);
	}

	json = cJSON_GetObjectItem(gun_ammo_info, "clipsize");
	if(json->valueint != clipsize) {
		changed = true;
		cJSON_SetNumberValue(json, clipsize);
	}

	json = cJSON_GetObjectItem(gun_ammo_info, "total");
	if(json->valueint != total) {
		changed = true;
		cJSON_SetNumberValue(json, total);
	}

	if(changed) {
		char * printed = cJSON_PrintUnformatted(gun_ammo_info);
		mqtt.publish_to("get/ammo", printed, strlen(printed), true, 0);
		cJSON_free(printed);
	}
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
