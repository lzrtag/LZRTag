
#include "../core/setup.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace LZR {
	uint32_t vibr_motor_count = 0;

	TickType_t last_button_tick = 0;

	void button_bump() {
		last_button_tick = xTaskGetTickCount();
	}

	float heartbeat_pattern() {
		static float beat_amplitude = 0; 

		beat_amplitude *= 0.93F;

		if(!player.get_heartbeat())
			return beat_amplitude;

		if((0b101 & (xTaskGetTickCount()/75)) == 0)
			beat_amplitude = 0.4F;

		return beat_amplitude;
	}

	void apply_hit_pattern(float &prev) {
		static float intensity = 0;
		static float current_hit_level = 0;

		intensity *= 0.9F;

		if(player.is_hit())
			intensity = 1;

		if(intensity < 0.01F)
			return;

		if((vibr_motor_count & 0b11) == 0)
			current_hit_level = 0.3F + 0.7F * (esp_random() >> 24) / 255.0F;
		
		prev = (current_hit_level*intensity) + (1-intensity) * prev;
	}

	void apply_button_pattern(float &prev) {
		if((xTaskGetTickCount() - last_button_tick) < 20/portTICK_PERIOD_MS)
			prev = 0.3F;
	}

	void apply_shot_pattern(float &prev) {
		gunHandler.apply_vibration(prev);
	}

	void vibrator_tick() {
		last_button_tick++;

		float out_level = heartbeat_pattern();
		apply_button_pattern(out_level);

		apply_hit_pattern(out_level);

		apply_shot_pattern(out_level);

		drv.send_rtp(std::max(0, std::min<int>(255, out_level*255)));
		gpio_set_level(PIN_VBRT, out_level > 0.3F);
	}
}