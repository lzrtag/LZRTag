/*
 * gyro.cpp
 *
 *  Created on: 25 Sep 2019
 *      Author: xasin
 */


#include "gyro.h"
#include "setup.h"

#include "SSD1306.h"
#include "DrawBox.h"
using namespace Peripheral;
using namespace XaI2C;

Peripheral::OLED::SSD1306 screen = Peripheral::OLED::SSD1306();
OLED::DrawBox drawBox = OLED::DrawBox(128, 64, &screen);
OLED::DrawBox box1 = OLED::DrawBox(128, 64, &screen);

std::string topString = std::string("TOP OF DISPLAY");

std::string bottomString = std::string("BOTTOM OF DISPLAY");


// OLED::DrawBox testBattery = OLED::DrawBox(8, 6, &screen);
#include <cmath>
#include <cstring>

namespace LZR {
namespace GYR {

const char * const name_list[] = {
		"unknown",
		"laidDown",
		"pointsUp",
		"pointsDown",
		"active",
};

void render_top() {
	box1.write_string(0, 0, topString, 2 , 0, nullptr);
}
void render_bottom() {
	drawBox.write_string(10, 24, std::string("- MatPat"),21 , 1, nullptr);
}

void init() {
	drawBox = OLED::DrawBox(128, 64, &screen);
	screen.initialize();
	drawBox.onRedraw = render_bottom;
	box1.onRedraw = render_top;
}

uint32_t   position_debounce = 0;
position_t sent_position = UNKNOWN;



#define BELOW_DELTA(value, offset, max_offset) (fabs(value-offset) < max_offset)
void tick() {
	render_top();
	render_bottom();
	if(topString == std::string("TOP OF DISPLAY")) {
		topString = std::string("top of display");
	} else if(topString == std::string("top of display")) {
		topString = std::string("TOP OF DISPLAY");
	}

	// gyro.update();

	// position_t current_gyro = ACTIVE;
	// if((fabs(gyro[0]) < 0.05) && (fabs(gyro[1]) < 0.05) && (fabs(fabs(gyro[2])-1) < 0.05))
	// 	current_gyro = LAID_DOWN;

	// if(BELOW_DELTA(gyro[1], 0, 0.15) && BELOW_DELTA(fabs(gyro[0]), 1, 0.1) && BELOW_DELTA(gyro[2], 0, 0.15)) {
	// 	if(gyro[0] > 0)
	// 		current_gyro = POINT_UP;
	// 	else
	// 		current_gyro = POINT_DOWN;
	// }

	// if(current_gyro != sent_position)
	// 	position_debounce++;
	// else
	// 	position_debounce = 0;

	// uint32_t debounce_time = 5;
	// if(sent_position == LAID_DOWN || current_gyro == LAID_DOWN)
	// 	debounce_time = 1*20;

	// if((position_debounce > debounce_time) && !LZR::mqtt.is_disconnected()) {
	// 	const char *posName = name_list[current_gyro];

	// 	LZR::mqtt.publish_to("get/gyro", posName, strlen(posName), true);

	// 	printf("GYRO switch to mode: %s\n", posName);

	// 	sent_position = current_gyro;
	// 	position_debounce = 0;
	// }

}

}
}
