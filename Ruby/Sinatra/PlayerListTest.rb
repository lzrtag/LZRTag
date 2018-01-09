#!/usr/bin/ruby2.4

require 'json'
require_relative '../GameInterface/LTagGame.rb'

$mqtt = MQTT::SubHandler.new('xasin.hopto.org');
$game = Lasertag::Game.new($mqtt);

$game.on_connect do |pName, player|
	player.ammo = 1000;

	player.team = rand(1..6);
	player.brightness = 7;

	player.hitConfig = {
		hitFlashBrightness: 10,
		hitFlashDuration: 180,
		hitVibration: 100,

		deathDuration: 5000,
	}

	player.fireConfig = {
		shotLocked: false,
	}

	player.data[:hitpoints] = 10;
end

require_relative 'BaseClasses/JSONBase'

$mqtt.subscribe_to "Lasertag/Game/Events" do |topic, data|
	begin
		data = JSON.parse(data);
		if(data["type"] == "hit") then
			player = $game[data["target"]];
			if(player.data[:hitpoints] -= 1) <= 0 then
				player.data[:hitpoints] = 0;
				player.dead = true;
			else
				player.hit
			end
			$game[data["shooterID"]].noise(duration: 0.02, startF: 2000);
		end
	rescue
	end
end

Thread.new do loop do
	sleep 0.1;
	$game.each_connected do |pName, player|
		if(player.data[:hitpoints] < 10 and not player.dead?) then
			player.data[:hitpoints] += 0.02;
		end

		player.heartbeat = ((player.data[:hitpoints] <= 3) and (not player.dead?));
		player.brightness = (player.data[:hitpoints] <= 3) ? 6 : 7;
	end
end end

Lasertag::HTTP::Basic.set :game, $game
Lasertag::HTTP::Basic.set :bind, "0.0.0.0"
Lasertag::HTTP::Basic.run!
