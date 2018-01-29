require_relative '../GameInterface/LTagGame.rb'

$mqtt = MQTT::SubHandler.new('xasin.hopto.org');
$game = Lasertag::Game.new($mqtt, clean_on_exit: true);

class RandomTeam < Lasertag::EventHook
	def onPlayerConnect(player)
		player.team = rand(1..6);
		player.brightness = 7;
	end
end

class Zombies < Lasertag::EventHook
	def onKill(hitPlayer, sourcePlayer)
		hitPlayer.team = sourcePlayer.team;
	end
end

class LifeBased_DM < Lasertag::EventHook
	def initialize(life = 3, regRate = 0.2, teams: false)
		@life = life;
		@regenerationRate = regRate;

		@disableTeamfire = teams;
	end

	def onPlayerConnect(player)
		player.ammo = 100;

		player.hitConfig = {
			hitFlashBrightness: 10,
			hitFlashDuration: 180,
			hitVibration: 100,

			deathDuration: 5000,
		}

		player.fireConfig = {shotLocked: false};

		player.data[:hitpoints] = @life;
	end

	def processHit(hitPlayer, sourcePlayer, code)
		return false if hitPlayer.dead?
		return false if @disableTeamfire and (hitPlayer.team == sourcePlayer.team)

		return true;
	end

	def onHit(hitPlayer, sourcePlayer)
		if (hitPlayer.data[:hitpoints] -= 1) <= 0 then
			hitPlayer.data[:hitpoints] = 0;
			hitPlayer.kill_by(sourcePlayer);

			sourcePlayer.noise(duration: 0.1, startF: 2000, endF: 2500);
		else
			hitPlayer.hit();
			sourcePlayer.noise(duration: 0.02, startF: 2000);
		end
	end

	def onGameTick(dT)
		@game.each do |player|
			player.heartbeat = ((player.data[:hitpoints] <= 1) and (not player.dead?));
			player.brightness = (player.data[:hitpoints] <= 1) ? 6 : 7;

			next if player.dead?

			if (player.data[:hitpoints] += dT*@regenerationRate) > @life
				player.data[:hitpoints] = @life;
			end
		end
	end
end

$game.add_hook(Lasertag::VerbooseDebugHook)
$game.add_hook(RandomTeam);

deathMatch = LifeBased_DM.new(4, 0.5, teams: true);
$game.add_hook(deathMatch);
$game.add_hook(Zombies);

if(__FILE__ == $0) then
	loop do
		print "#{$game["Red"].inspect}      \r" if $game["Red"];
		sleep 1;
	end
end
