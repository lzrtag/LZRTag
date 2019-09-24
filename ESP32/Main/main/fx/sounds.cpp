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

#include "../core/setup.h"

#include <cmath>

using namespace Xasin::Peripheral;

namespace LZR {
namespace Sounds {

volatile uint32_t note_until = 0;
volatile float volume = 0;
volatile uint32_t suggested_hue = 0;

auto cassette_game_start	= AudioCassette(sound_game_start, sizeof(sound_game_start));

auto cassette_kill_scored 	= AudioCassette(sound_kill_score, sizeof(sound_kill_score));
auto cassette_minor_score	= AudioCassette(sound_minor_score, sizeof(sound_minor_score));

auto cassette_death 		= AudioCassette(sound_own_death, sizeof(sound_own_death));
auto cassette_hit			= AudioCassette(sound_own_hit, 	 sizeof(sound_own_hit));


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
}

void init() {
	mqtt.subscribe_to(player.get_topic_base() + "/FX/Sound", [](Xasin::MQTT::MQTT_Packet data) {
		play_audio(data.data);
	});

	mqtt.subscribe_to(player.get_topic_base() + "/MIDI", [](Xasin::MQTT::MQTT_Packet data) {
#pragma pack(1)
		struct midi_data_t {
			uint32_t pitch;
			uint32_t volume;
			uint32_t duration;
			uint32_t hue;
		};
#pragma pack(0)

		midi_data_t note = *reinterpret_cast<const midi_data_t *>(data.data.data());
		
		float freq = 440.0 * pow(2.0, (float(note.pitch)-69)/12.0);
		LZR::audioManager.insert_sample(new Xasin::Peripheral::SquareWave(freq, note.volume, note.duration));

		note_until = xTaskGetTickCount() + note.duration/1000.0 * 600;
		volume = note.volume / 12000.0;
		suggested_hue = note.hue;
	});
}

}
}
