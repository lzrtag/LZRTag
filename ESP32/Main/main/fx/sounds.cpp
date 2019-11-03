/*
 * sounds.cpp
 *
 *  Created on: 30 Mar 2019
 *      Author: xasin
 */


#include "sounds.h"

#include "sounds/game_start.h"

#include "sounds/kill_score.h"
#include "sounds/minor_score.h"

#include "sounds/own_death.h"
#include "sounds/own_hit.h"

#include "sounds/empty_click.h"
#include "sounds/reload_full.h"

#include "sounds/denybeep.h"

#include "../core/setup.h"

using namespace Xasin::Peripheral;

namespace LZR {
namespace Sounds {

auto cassette_game_start	= AudioCassette(sound_game_start, sizeof(sound_game_start));

auto cassette_kill_scored 	= AudioCassette(sound_kill_score, sizeof(sound_kill_score));
auto cassette_minor_score	= AudioCassette(sound_minor_score, sizeof(sound_minor_score));

auto cassette_death 		= AudioCassette(sound_own_death, sizeof(sound_own_death));
auto cassette_hit			= AudioCassette(sound_own_hit, 	 sizeof(sound_own_hit));

auto cassette_click 	   = AudioCassette(empty_click, sizeof(empty_click));
auto cassette_reload_full = AudioCassette(reload_full, sizeof(reload_full));

auto cassette_deny = AudioCassette(raw_denybeep, sizeof(raw_denybeep));

void play_audio(std::string aName) {
	if(aName == "GAME START")
		audioManager.insert_cassette(cassette_game_start);
	else if(aName == "KILL SCORE")
		audioManager.insert_cassette(cassette_kill_scored);
	else if(aName == "MINOR SCORE")
		audioManager.insert_cassette(cassette_minor_score);
	else if(aName == "DEATH")
		audioManager.insert_cassette(cassette_death);
	else if(aName == "HIT")
		audioManager.insert_cassette(cassette_hit);
	else if(aName == "CLICK")
		audioManager.insert_cassette(cassette_click);
	else if(aName == "RELOAD FULL")
		audioManager.insert_cassette(cassette_reload_full);
	else if(aName == "DENY")
		audioManager.insert_cassette(cassette_deny);
}

void init() {
	mqtt.subscribe_to(player.get_topic_base() + "/Sound/#", [](Xasin::MQTT::MQTT_Packet data) {
		if(data.topic == "File")
			play_audio(data.data);
		else if(data.topic == "Note") {
#pragma pack(1)
			struct note_data_t {
				uint32_t frequency;
				uint32_t volume;
				uint32_t duration;
			} note = *reinterpret_cast<const note_data_t*>(data.data.data());
#pragma pack(0)

			if(note.duration < 5000)
				LZR::audioManager.insert_sample(new TriangleWave(note.frequency, note.volume, note.duration));
		}
	});
}

}
}
