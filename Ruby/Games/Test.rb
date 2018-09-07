require_relative '../GameInterface/LTagGame.rb'

$mqtt = MQTT::SubHandler.new('localhost');
$game = Lasertag::Game.new($mqtt, clean_on_exit: true);

class RandomTeam < Lasertag::EventHook
	def onPlayerConnect(player)
		player.team = rand(1..6);
		player.brightness = 7;
	end
end

class LifeBased_DM < Lasertag::EventHook
	def initialize(regRate = 50/6.0, teams: false)
		@regenerationRate = regRate;

		@disableTeamfire = teams;
	end

	def onPlayerConnect(player)
		player.ammo = 6;

		player.hitConfig = {
			hitFlashBrightness: 10,
			hitFlashDuration: 180,
			hitVibration: 100,

			deathDuration: 5000,
		}

		player.fireConfig = {
			shotLocked: false,

			ammoCap: 6,
			perReloadAmmo: 6,

			perShotDelay: 200,
		};
	end
	def onPlayerDisconnect(player)
		player.fireConfig = nil;
		player.heartbeat = false;
	end

	def processHit(hitPlayer, sourcePlayer, code)
		return false if hitPlayer.dead?
		return false if @disableTeamfire and (hitPlayer.team == sourcePlayer.team)

		return true;
	end

	def onHit(hitPlayer, sourcePlayer)
		unless(hitPlayer.damage_by(100/6.0, sourcePlayer));
			hitPlayer.hit();
			sourcePlayer.noise(duration: 0.02, startF: 2000);
		end
	end

	def onKill(hitPlayer, sourcePlayer)
		sourcePlayer.noise(duration: 0.1, startF: 2000, endF: 2500);
	end

	def onGameTick(dT)
		@game.each_connected do |player|
			player.heartbeat = ((player.hitpoints <= 20) and (not player.dead?));
			player.brightness = (player.hitpoints <= 20) ? 6 : 7;

			next if player.dead?

			player.regenerate(dT*@regenerationRate)
		end
	end
end

#$game.add_hook(Lasertag::VerbooseDebugHook)

class Zombies < Lasertag::EventHook
	def initialize()
		@configHooks = [RandomTeam.new, LifeBased_DM.new(teams: true)]
	end

	def onHookin(game)
		super(game);

		Thread.new do start(); end
	end

	def start()
		@game.add_hook(@configHooks[0]);

		sleep 1;
		@game.each_connected do |pl| pl.brightness = 1; end

		4.times do |i|
			@game.each_connected do |pl|
				pl.noise(duration: (i==3) ? 1 : 0.25, startF: (i==3) ? 600 : 440);
			end
			sleep 1;
		end

		@configHooks.each do |h| @game.add_hook(h); end
	end


	def _crown_winners(winnerTeam)
		puts "Crowning winner team: #{winnerTeam}"
		Thread.new do
			@configHooks.each do |h| @game.remove_hook(h); end

			@game.each_connected do |pl|
				pl.brightness = (pl.team == winnerTeam) ? 5 : 2;
			end

			sleep 10;

			start();
		end.abort_on_exception = true;
	end

	def onKill(hitPlayer, sourcePlayer)
		hitPlayer.team = sourcePlayer.team;

		return if @game.num_connected <= 1;

		teamNumbers = Hash.new(0);
		@game.each_connected do |pl|
			teamNumbers[pl.team] += 1;
		end
		puts "Teams after kill: #{teamNumbers}";

		winnerTeam = nil;
		teamNumbers.each do |key, val| winnerTeam = key if val == @game.num_connected; end

		_crown_winners(winnerTeam) if winnerTeam;
	end
end

$zombies = Zombies.new();
$game.add_hook($zombies);

loop do
	break;

	loop do
		sleep 0.1
		nAlive = 0;
		$game.each_connected do |pl|
			nAlive += 1 unless pl.dead?
		end

		break if nAlive == 1;
	end

	$game.each_connected do |pl|
		pl.dead = false;
		pl.brightness = 1;
		pl.team = 7;

		pl.hitConfig = {
			hitFlashBrightness: 10,
			hitFlashDuration: 180,
			hitVibration: 100,

			dieOnHit: true,
		}
		pl.fireConfig = nil;
	end

	rand(4..13).times do |i|
		$game.each_connected do |pl|
			pl.noise()
		end
		sleep 1;
	end

	$game.each_connected do |pl|
		pl.noise(startF: 1000);
		pl.vibrate(0.2);
		pl.brightness = 7;
		pl.fireConfig = {
			shotLocked: false,
			perShotDelay: 1000,
		}
		pl.ammo = 100;
	end
end

if(__FILE__ == $0) then
	print "Going into server mode ..."

	require_relative "../Sinatra/BaseClasses/JSONBase.rb"

	Lasertag::HTTP::Basic.set :game, $game
	Lasertag::HTTP::Basic.set :bind, "0.0.0.0"
	Lasertag::HTTP::Basic.run!
end
