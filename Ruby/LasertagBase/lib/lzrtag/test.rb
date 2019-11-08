

require_relative '../lzrtag.rb'

class DebugHook < LZRTag::Hook::Debug
	def initialize(handler)
		super(handler);

		@eventBlacklist = [:gameTick, :playerRegenerated, :gameStarted, :gameStarting];
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

$mqtt = MQTT::SubHandler.new("mqtt://192.168.6.29");
$handler = LZRTag.Handler.new($mqtt);

$handler.add_hook(DebugHook);

class TestGame < LZRTag::Game::Base
	def initialize(handler)
		super(handler);
	end

	hook :teamSelect, LZRTag::Hook::TeamSelector
	hook :regenerator, LZRTag::Hook::Regenerator, {
		regRate: 5,
		regDelay: 2,
		autoReviveThreshold: 10
	}
	hook :damager, LZRTag::Hook::Damager, {
		dmgPerShot: 10
	}

	hook :guns, LZRTag::Hook::GunSelector

	phase_prep :starting do
		@handler.gamePhase = :teamSelect
	end

	phase :teamSelect do |dT|
		if((!@handler.brightnessMap[:active].empty?) && (@handler.brightnessMap[:teamSelect].empty?))
			@handler.set_phase(:countdown)
		end
	end

	phase_prep :countdown do
		@phaseTime = -10;
		@nextBeep = -10;

		@handler.each_participating do |pl|
			pl.brightness = :idle;

			pl.heartbeat = true;
		end
	end

	phase :countdown do |dT|
		if(@phaseTime >= 0)
			@handler.set_phase(:running)
		elsif(@phaseTime > @nextBeep)
			#@handler.each_participating do |pl| pl.noise(); end
			if(@nextBeep == -4)
				@handler.each_participating do |pl| pl.sound("GAME START"); end
			end
			@nextBeep += 1;
		end
	end

	phase_end :countdown do
		@handler.each_participating do |pl| pl.heartbeat = false; end
	end


	phase_prep :running do
		@phaseTime = -1*60;

		@handler.each_participating do |pl|
			pl.brightness = :active
		end
	end

	phase :running do
		@handler.stop_game() if @phaseTime > 0
	end

	phase_end :running do
		puts "\n\nGAME COMPLETED\nSTATS:\n%8s | %8s | %8s | %8s" % ["Name", "Kills", "Deaths", "K/D"];

		@handler.each_participating do |pl|
			puts "%8s | %8d | %8d | %8d" % [pl.name, pl.stats["Kills"], pl.stats["Deaths"], pl.ratio_kd];

			pl.heartbeat = false;
			pl.brightness = :idle;
			pl.gunNo = 0;
			pl.dead = false;
		end
	end
end

$handler.register_game("Test", TestGame);

sleep 3

$handler.start_game(TestGame);

while(true) do
	sleep 0.5;
end
