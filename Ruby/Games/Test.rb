require_relative '../GameInterface/LTagGame.rb'

$mqtt = MQTT::SubHandler.new('localhost');
$game = Lasertag::Game.new($mqtt, clean_on_exit: true);

class RandomTeam < Lasertag::EventHook
	def onPlayerConnect(player)
		player.team = rand(1..6);
		player.brightness = 7;
	end
end

class KillOnHit_DM < Lasertag::EventHook
	def onPlayerConnect(player)
		player.ammo = 100;

		player.hitConfig = {
			hitFlashBrightness: 10,
			hitFlashDuration: 180,
			hitVibration: 100,

			deathDuration: 5000,
		}

		player.fireConfig = {shotLocked: false};
	end
end

def sendData()
	$r.ammo = 1000;

	$r.team = rand(1..6);
	$r.brightness = 7;

	$r.hitConfig = {
		hitFlashBrightness: 10,
		hitFlashDuration: 180,
		hitVibration: 100,

		deathDuration: 5000,
	}

	$r.fireConfig = {
		shotLocked: false,
	}

	$r.data[:hitpoints] = 10;
end

$game.on_connect do |pName, player|
	#puts "Sending player data to #{pName}!"
	$r = player;
	sendData();
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

$mqtt.subscribe_to "Lasertag/Game/Events" do |topic, data|
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
end

if(__FILE__ == $0) then
	loop do
		print "#{$game["Red"].inspect}      \r" if $game["Red"];
		sleep 1;
	end
end
