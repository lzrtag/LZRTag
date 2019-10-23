

require_relative '../lzrtag.rb'

class DebugHook < LZRTag::Hook::Debug
	def initialize(handler)
		super(handler);

		@eventBlacklist [:gameTick, :playerRegenerated, :gameStarted, :gameStarting];
		@eventBlacklist.flatten!
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

$mqtt = MQTT::SubHandler.new("mqtt://192.168.250.1");
$handler = LZRTag.Handler.new($mqtt);

$handler.add_hook(DebugHook);

class TestGame < LZRTag::Game::Base
	def initialize(handler)
		super(handler);
	end

	hook :teamSelect, LZRTag::Hook::TeamSelector
	hook :regenerator, LZRTag::Hook::Regenerator, {
		regRate: 7,
		regDelay: 3,
		autoReviveThreshold: 21
	}
	hook :damager, LZRTag::Hook::Damager, {
		dmgPerShot: 25
	}

	phase_prep :starting do
		@handler.gamePhase = :teamSelect
	end

	phase :teamSelect do |dT|
		if((@handler.brightnessCount[:active] >= 1) && (@handler.brightnessCount[:teamSelect] == 0))
			@handler.set_phase(:countdown)
		end
	end

	phase_prep :countdown do
		@phaseTime = -10;
		@nextBeep = -10;

		@handler.each_participating do |pl|
			pl.sound("GAME START");
			pl.brightness = :idle;

			pl.heartbeat = true;
		end
	end

	phase :countdown do |dT|
		if(@phaseTime >= 0)
			@handler.each_participating do |pl| pl.noise(frequency: 1000); end
			@handler.set_phase(:running)
		elsif(@phaseTime > @nextBeep)
			@handler.each_participating do |pl| pl.noise(); end
			@nextBeep += 1;
		end
	end

	phase_end :countdown do
		@handler.each_participating do |pl| pl.heartbeat = false; end
	end


	phase_prep :running do
		@phaseTime = -3*60;

		@handler.each_participating do |pl|
			pl.brightness = :active
		end
	end

	phase :running do
		@handler.stop_game() if @phaseTime > 0
	end
end

$handler.register_game("Test", TestGame);

sleep 3

$handler.start_game(TestGame);

while(true) do
	sleep 0.5;
end
