#!/usr/bin/ruby2.4

require 'json'
require_relative '../GameInterface/LTagGame.rb'

$mqtt = MQTT::SubHandler.new('iot.eclipse.org');
$game = Lasertag::Game.new($mqtt);

require_relative 'BaseClasses/JSONBase'

$game.on_connect do |pName, player|
	player.ammo = 1000;
	player.hitConfig = {
		dieOnHit: true,
		deathTime: 3000,
	}
end

$mqtt.subscribe_to "Lasertag/Game/Events" do |tList, data|
	begin
		data = JSON.parse(data);

		puts data

		if(data["type"] == "hit") then
			target = $game[data["target"]];
			target.team = (target.team)%7 + 1;

			shooter = $game[data["shooterID"].to_i];
			shooter.noise();
		end
	rescue
	end
end

Lasertag::HTTP::Basic.set :game, $game
Lasertag::HTTP::Basic.run!
