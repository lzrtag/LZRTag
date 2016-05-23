/*
 * Connector.cpp
 *
 *  Created on: 23.05.2016
 *      Author: xasin
 */

#include "Connector.h"

namespace Connector {

	namespace Effects {
	void on_shot() {
		Board::Nozzle::flash(Game::Player::get_team_color());
	}
	}

	uint8_t ISR1aPRESC = 0;

	void update() {
		Board::ISR1a();

		if(++ISR1aPRESC == 4) {
			ISR1aPRESC = 0;

			Game::update();
		}

		if((TRIGGER_PINx & (1<< TRIGGER_PIN)) != 0)
			Game::Weapon::shoot();
	}

	void init() {
		Board::init();

		Game::Weapon::on_shot = &Effects::on_shot;

		Game::start();
	}
}
