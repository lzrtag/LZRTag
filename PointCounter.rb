#!/usr/bin/ruby

require 'mqtt'
require 'json'

$mHost = "iot.eclipse.org"

$ltTopic = "Lasertag/Game";
$eventT  = $ltTopic + "/Events";
$statusT = $ltTopic + "/Status";
$statusC = $statusT + "Control";

$playerTopic = "Lasertag/Players/+"
$idTopic	 = $playerTopic + "/ID"

def getTList(topic)
	tList = Array.new();
	return topic.scan(/[^\/]+/);
end
def matchesTopic(recTopic, tPattern)
	rTList = getTList recTopic;
	tTList = getTList tPattern;

	return false unless rTList.length >= tTList.length;

	tTList.each_index { |i|
		next if tTList[i] == "+"
		return true if tTList[i] == "#"

		return false if tTList[i] != rTList[i];
	}

	return tTList.length == rTList.length;
end

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
MQTT::Client.connect($mHost) do |c|
	print "Connected!\n"

	$mqtt = c;

	c.publish($statusT, "stop", true);

	c.subscribe($statusC);
	c.subscribe($eventT);
	c.subscribe($idTopic);
	c.subscribe($ltTopic + "/SetTime");

	def setAllBrightnesses(l)
		$scoreboard.each_key do |player|
			$mqtt.publish("Lasertag/Players/#{player}/Brightness", l, true);
		end
	end

	def endGame()
		$mqtt.publish($statusT, "stop", true);

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
			$mqtt.publish($statusT, "startwarn");
			sleep 1;
		end
		$mqtt.publish($statusT, "start");
		freshScoreboard();

		$gameTimer = $recGameTime;
		setAllBrightnesses(3);
		$scoreboard.each_key do |player|
			$mqtt.publish("Lasertag/Players/#{player}/Kills", 0, false);
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
				$mqtt.publish($ltTopic + "/Time", $gameTimer);
			else
				$mqtt.publish($ltTopic + "/Time", $recGameTime);
			end
		end
	end

	c.get() do |topic, message|
		if(topic == $eventT and $gameTimer > 0)
			data = JSON.parse(message);

			if(data["type"] == "hit")
				print "#{data["shooter"]} hit #{data["target"]}!\n";
				$scoreboard[data["shooter"]][:kills]+=1;
				$scoreboard[data["target"]][:deaths]+=1;
				$mqtt.publish("Lasertag/Players/#{data["shooter"]}/Kills", $scoreboard[data["shooter"]][:kills], false);
			end
		end

		if(topic == $statusC)
			if(message == "stop" and $gameTimer > 0)
				endGame();
			elsif($gameTimer == 0 and message == "start")
				startGame();
			end
		end

		if(topic == $ltTopic + "/SetTime")
			if($gameTimer > 0)
				$gameTimer = message.to_f;
			else
				puts "New game time was set: #{message}";
				$recGameTime = message.to_f;
			end
		end

		if(matchesTopic(topic, $idTopic))
			nPlayer = getTList(topic)[2];
			if(message == "" and $scoreboard.has_key? nPlayer)
				print "Player unregistered - #{nPlayer}\n";
				$scoreboard.delete(nPlayer);
			elsif( not $scoreboard.has_key? nPlayer)
				print "New Player registered - #{nPlayer}\n";
				$scoreboard[nPlayer][:id] = message.to_i;
			end
		end
	end
end
