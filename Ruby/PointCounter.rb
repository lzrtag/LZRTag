#!/usr/bin/ruby

require 'mqtt'
require 'json'

require_relative 'MQTTSubscriber.rb'

$mHost = "iot.eclipse.org"

$ltTopic = "Lasertag/Game";
$eventT  = $ltTopic + "/Events";
$statusT = $ltTopic + "/Status";
$statusC = $statusT + "Control";

$playerTopic = "Lasertag/Players/+"
$idTopic	 = $playerTopic + "/ID"

$recGameTime = 0.2;
$gameTimer = 0;
$scoreboard = Hash.new() do |h, k|
	h[k] = {
		deaths: 0,
		kills:  0,
		id:		0,
		};
end

def freshScoreboard
	$scoreboard.each_key { |player|
		$scoreboard[player][:kills] 	= 0;
		$scoreboard[player][:deaths] 	= 0;
	}
end

print "Connecting ... "
$mqtt = MQTTSubs.new(MQTT::Client.connect($mHost), autoListen: false);
print "Connected!\n"

$mqtt.publishTo $statusT, "stop", retain: true

def setAllBrightnesses(l)
	$scoreboard.each_key do |player|
		$mqtt.publishTo "Lasertag/Players/#{player}/Brightness", l, retain: true
	end
end

def endGame()
	$mqtt.publishTo $statusT, "stop", retain: true

	puts "\nThe game has ended! Scores are as follows:\n"
	$scoreboard.each do |key, value|
		printf "%8s | %3i kills | %3i deaths\n", key, value[:kills], value[:deaths];
	end
	puts "\n\n";

	$gameTimer = 0;

	setAllBrightnesses(1);
end

def startGame()
	puts "\n\n"
	nString = "### A NEW GAME BEGINS ###"
	nString.length.times { print "#"; }; print "\n";
	puts nString;
	nString.length.times { print "#"; }; print "\n\n";

	3.times do
		$mqtt.publishTo $statusT, "startwarn"
		sleep 1;
	end
	$mqtt.publishTo $statusT, "start", retain: true
	freshScoreboard();

	$gameTimer = $recGameTime;
	setAllBrightnesses(3);
	$scoreboard.each_key do |player|
		$mqtt.publishTo "Lasertag/Players/#{player}/Kills", 0
	end
end

Thread.new do
	while true
		sleep 3;
		if($gameTimer > 0.05)
			$gameTimer -= 0.05;
		elsif($gameTimer > 0)
			endGame();
		end

		if($gameTimer > 0)
			$mqtt.publishTo $ltTopic + "/Time", $gameTimer
		else
			$mqtt.publishTo $ltTopic + "/Time", $recGameTime
		end
	end
end

$mqtt.subscribeTo $eventT do |tList, message|
	if($gameTimer > 0)
		data = JSON.parse(message);

		if(data["type"] == "hit")
			print "#{data["shooter"]} hit #{data["target"]}!\n";
			$scoreboard[data["shooter"]][:kills]+=1;
			$scoreboard[data["target"]][:deaths]+=1;
			$mqtt.publishTo "Lasertag/Players/#{data["shooter"]}/Kills", $scoreboard[data["shooter"]][:kills]
		end
	end
end

$mqtt.subscribeTo $statusC do |tList, message|
	if(message == "stop" and $gameTimer > 0)
		endGame();
	elsif($gameTimer == 0 and message == "start")
		startGame();
	end
end

$mqtt.subscribeTo $ltTopic + "/SetTime" do |tList, message|
	if($gameTimer > 0)
		$gameTimer = message.to_f;
	else
		puts "New game time was set: #{message}";
		$recGameTime = message.to_f;
	end
end

$mqtt.subscribeTo $idTopic do |tList, message|
	nPlayer = tList[0];
	if(message == "" and $scoreboard.has_key? nPlayer)
		print "Player unregistered - #{nPlayer}\n";
		$scoreboard.delete(nPlayer);
	elsif( not $scoreboard.has_key? nPlayer)
		print "New Player registered - #{nPlayer}\n";
		$scoreboard[nPlayer][:id] = message.to_i;
	end
end

$mqtt.lockAndListen
