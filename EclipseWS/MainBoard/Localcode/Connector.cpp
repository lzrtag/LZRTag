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

			if(Game::Weapon::shot_delay() > 100)
				Board::Vibrator::vibrate(Game::Weapon::shot_delay()/2);

			Board::Buzzer::sweep(500, 3000, Game::Weapon::shot_delay() / 2);
		}

		void on_reload() {
			Board::Buzzer::sweep(500, 500, 10);
		}
	}

	void update() {
		Board::ISR1a();

		Game::update();

		if((TRIGGER_PINx & (1<< TRIGGER_PIN)) != 0)
			Game::Weapon::shoot();
	}

	void init() {
		Board::init();

		Game::Weapon::on_shot = &Effects::on_shot;
		Game::Weapon::on_reload = &Effects::on_reload;

		Game::start();
	}
}
