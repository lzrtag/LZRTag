
require_relative 'GameInterface/LTagGame.rb'

$mqtt = MQTT::SubHandler.new('iot.eclipse.org');
$game = Lasertag::Game.new($mqtt);

$timerRunning 	= false;
$gameTimer 		= 0;
$gameDuration 	= 3;

$gameRunning = false;

def gamePrep
	$game.remove_disconnected
	$game.each do |name, player|
		player.data = {
			deaths: 	0,
			kills:	0
		}
	end
end

def set_game_status(status)
	$mqtt.publishTo "Lasertag/Game/Status", status, retain: status != "startwarn";
end

def set_all_brightnesses(l)
	$game.each do |name, player|
		player.brightness = l;
	end
end

def end_game()
	return unless $gameRunning;
	$gameRunning = false;

	set_game_status "stop";

	puts "\nThe game has ended! Scores are as follows:\n"
	$game.each do |name, player|
		printf "%8s | %3i kills | %3i deaths\n", name, player.data[:kills] || 0, player.data[:deaths] || 0;
	end
	puts "\n\n";

	$timerRunning = false;
	$gameTimer = 0;
	set_all_brightnesses 1;
end

def start_game()
	return if $gameRunning;

	$gameTimer = $gameDuration;
	$gameRunning = true;

	if $game.num_connected < 2 then
		puts "A new game can't be started with less than two players!\n Sorry :P";
		return;
	end

	gamePrep

	puts "\n\n"
	nString = "### A NEW GAME BEGINS ###"
	nString.length.times { print "#"; }; print "\n";
	puts nString;
	nString.length.times { print "#"; }; print "\n\n";

	3.times do
		set_game_status "startwarn"
		sleep 1;
	end

	$timerRunning = true;
	set_game_status "start"

	$game.each do |name, player|
		player.brightness = 3;
		$mqtt.publishTo "Lasertag/Players/#{name}/Kills", 0
	end
end

$mqtt.subscribeTo "Lasertag/Game/Events" do |tList, message|
	data = JSON.parse(message);

	if(data["type"] == "hit") then
		shooter = $game[data["shooterID"].to_i];
		target 	= $game[data["target"]];
		print "#{shooter.name} hit #{target.name}!\n"
		shooter.data[:kills] += 1;
		target.data[:deaths] += 1;
		$mqtt.publishTo "Lasertag/Players/#{shooter.name}/Kills", shooter.data[:kills];
	end
end

$mqtt.subscribeTo "Lasertag/Game/StatusControl" do |tList, message|
	if(message == "stop" and $gameTimer > 0)
		end_game
	elsif($gameTimer == 0 and message == "start")
		start_game
	end
end

$mqtt.subscribeTo "Lasertag/Game/SetTime" do |tList, message|
	if($gameTimer > 0)
		$gameTimer = (message.to_f * 60).to_i;
		puts "Game timer was set to #{message} minutes!";
	else
		$gameDuration = (message.to_f * 60).to_i;
		puts "New game length: #{message} minutes.";
	end
end

$game.on_register do |name, player|
	player.data = {
		deaths: 	0,
		kills:	0
	}
end
$game.on_connect do |name, player|
	puts "Player #{name} connected!\n";
end
$game.on_disconnect do |name, player|
	puts "Player #{name} has disconnected :c\n";
end

set_game_status "stop";

at_exit {
	print "Disconnecting Lasertag Clients ... "
	$game.remove_disconnected();
	puts "Done!"
}

$gameShouldRun = true;
Signal.trap("SIGINT") do
	$gameShouldRun = false;
	$stdout.seek(-2, :CUR);
	Thread.new do
		Thread::main.run();
	end
end

while $gameShouldRun do
	sleep 1
	next unless $timerRunning;

	if $gameTimer > 1 then
		$gameTimer -= 1;
	else
		end_game
	end

	pubTime = $gameTimer;
	pubTime = $gameDuration if $gameTimer == 0;

	$mqtt.publishTo "Lasertag/Game/Time", pubTime;
end

end_game
