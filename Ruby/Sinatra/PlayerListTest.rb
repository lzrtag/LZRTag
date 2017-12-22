#!/usr/bin/ruby2.4

require 'json'
require_relative '../GameInterface/LTagGame.rb'

$mqtt = MQTT::SubHandler.new('xasin.hopto.org');
$game = Lasertag::Game.new($mqtt);

require_relative 'BaseClasses/JSONBase'

$game.on_connect do |pName, player|
	player.ammo = 1000;

	player.hitConfig = {
		dieOnHit: true,
		deathDuration: 5000,
	}

	player.fireConfig = {
		shotLocked: false,
	}

	player.team = rand(1..7);
end

$mqtt.subscribe_to "Lasertag/Game/Events" do |tList, data|
	begin
		data = JSON.parse(data);

		if(data["type"] == "hit") then
			target = $game[data["target"]];
			unless(target.dead?) then
				shooter = $game[data["shooterID"].to_i];
				shooter.noise();

				target.team 		= rand(1..7);
				target.brightness = rand(0..7);
			end
		end
	rescue
	end
end

Lasertag::HTTP::Basic.set :game, $game
Lasertag::HTTP::Basic.run!
