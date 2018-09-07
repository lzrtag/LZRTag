#!/usr/bin/ruby2.4

require 'json'
require_relative '../GameInterface/LTagGame.rb'

$mqtt = MQTT::SubHandler.new('localhost');
$game = Lasertag::Game.new($mqtt);

class RandomTeam < Lasertag::EventHook
	def onPlayerConnect(player)
		player.ammo = 5;

		player.team = rand(1..6);
		player.brightness = 7;

		player.hitConfig = {
			hitFlashBrightness: 10,
			hitFlashDuration: 180,
			hitVibration: 100,

			deathDuration: 7000,
		}

		player.fireConfig = {
			shotLocked: false,
			perShotDelay: 400,
		}

		player.data[:hitpoints] = 1.5;
	end

	def onHit(player, shooter)
		if(player.data[:hitpoints] -= 1) <= 0 then
			player.data[:hitpoints] = 0;
			player.dead = true;
		else
			player.hit
		end
		shooter.noise(duration: 0.02, startF: 2000);
	end

	def onGameTick(dt)
		$game.each_connected do |player|
			if(player.data[:hitpoints] < 1.5 and not player.dead?) then
				player.data[:hitpoints] += 0.15*dt;
			end

			player.heartbeat = ((player.data[:hitpoints] <= 1) and (not player.dead?));
			player.brightness = (player.data[:hitpoints] <= 1) ? 6 : 7;
		end
	end
end

$game.add_hook(RandomTeam);

require_relative 'BaseClasses/JSONBase'

Lasertag::HTTP::Basic.set :game, $game
Lasertag::HTTP::Basic.set :bind, "0.0.0.0"
Lasertag::HTTP::Basic.run!
