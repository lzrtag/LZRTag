

require_relative '../lzrtag.rb'

class DebugHook < LZRTag::Hook::Base
	def initialize()
		super();
	end

	def consume_event(evtName, data)
		super(evtName, data);

		return if [:gameTick, :playerRegenerated, :gameStarted, :gameStarting].include? evtName
		puts "Caught event: #{evtName} with data: #{data}";
	end
end

DebugHook.on :playerHurt do |player, fromPlayer|
	player.sound("HIT");
	fromPlayer.sound("MINOR SCORE");
end
DebugHook.on :playerKilled do |targetPlayer, sourcePlayer|
	targetPlayer.sound("DEATH");
	sourcePlayer.sound("KILL SCORE") if(sourcePlayer)
end
DebugHook.on :playerFullyRegenerated do |player|
	player.dead = false;
end

DebugHook.on [:playerRegenerated, :playerHurt] do |player|
	player.heartbeat = (player.life < 30);
end

$mqtt = MQTT::SubHandler.new("192.168.178.50");

$handler = LZRTag.Handler.new($mqtt);

$handler.add_hook(DebugHook);
$handler.add_hook(LZRTag::Hook::RandomTeam);
$handler.add_hook(LZRTag::Hook::Damager.new(dmgPerShot: 25));
$handler.add_hook(LZRTag::Hook::Regenerator.new(regRate: 7, regDelay: 3, autoReviveThreshold: 21));

$handler.add_hook(LZRTag::Hook::TeamSelector);

class TestGame < LZRTag::Game::Base
	def initialize(handler)
		super(handler);
	end

	phase :starting do |dT|
		if(@handler.brightnessCount[:active] >= 1)
			@handler.set_phase(:countdown)
		end
	end

	phase_prep :countdown do
		@phaseTime = -3.9;
		@nextBeep = -4;
	end

	phase :countdown do |dT|
		if(@phaseTime >= 0)
			@handler.each do |pl| pl.noise(frequency: 1000); end
			@handler.set_phase(:running)
		elsif(@phaseTime > @nextBeep)
			@handler.each do |pl| pl.noise(); end
			@nextBeep += 1;
		end
	end

	phase_prep :running do
		@phaseTime = -3*60;
	end

	phase :running do
	end
end

$handler.start_game(TestGame);

$handler.each do |pl|
	pl.ammo = 100;
	pl.gunNo = 2;
end

while(true) do
	sleep 0.5;
end
