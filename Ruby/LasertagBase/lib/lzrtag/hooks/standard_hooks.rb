
require_relative 'base_hook.rb'

module LZRTag
	module Hook
		class Debug < Base
			attr_accessor :eventWhitelist
			attr_accessor :eventBlacklist

			def initialize()
				super();

				@eventWhitelist = Array.new();
				@eventBlacklist = Array.new();
			end

			def consume_event(evtName, data)
				super(evtName, data);

				return if @eventBlacklist.include? evtName
				unless(@eventWhitelist.empty?)
					return unless @eventWhitelist.include? evtName
				end

				puts "Caught event: #{evtName} with data: #{data}";
			end
		end

		class RandomTeam < Base
			attr_accessor :teamWhitelist

			def initialize()
				super();

				@teamWhitelist = (1..6).to_a;
			end

			def on_hookin(game)
				super(game);

				game.each do |pl|
					reassignTeam(pl);
				end
			end

			def reassignTeam(player)
				minCount = @handler.teamCount.values.min[1];
				(@teamWhitelist.shuffle()).each do |t|
					if(@handler.teamCount[t] == minCount)
						player.team = t;
						break;
					end
				end
			end

			on :playerRegistered do |player|
				puts "Reassigning player: #{player}"
				reassignTeam(player);
			end
		end

		class TeamSelector < Base
			def initialize(possibleTeams: [1, 2, 3, 4])
				super();

				@possibleTeams = possibleTeams;
			end

			on :playerRegistered do |pl|
				pl.brightness = (pl.gyroPose == :laidDown) ? :idle : :teamSelect;
			end

			on :poseChanged do |pl, nPose|
				next if(pl.brightness == :active)

				pl.brightness = (pl.gyroPose == :laidDown) ? :idle : :teamSelect;
			end

			on :navSwitchPressed do |player, dir|
				next if player.brightness == :active

				newTeam = @possibleTeams.find_index(player.team) || 0;

				newTeam += 1 if(dir == 2)
				newTeam -= 1 if(dir == 3)

				player.team = @possibleTeams[newTeam % @possibleTeams.length]

				player.brightness = :active if(dir == 1)
			end
		end

		class Regenerator < Base
			def initialize(regRate: 1, regDelay: 10, healDead: false, autoReviveThreshold: 30)
				super();

				@regRate = regRate;
				@regDelay = regDelay;

				@healDead = healDead;
				@autoReviveThreshold = autoReviveThreshold;
			end

			on :gameTick do |dT|
				@handler.each do |pl|
					if((Time.now() - pl.lastDamageTime) >= @regDelay)
						pl.regenerate(dT * @regRate);
					end

					if(pl.dead and pl.life >= @autoReviveThreshold)
						pl.dead = false;
					end
				end
			end
		end

		class Damager < Base
			def initialize(dmgPerShot: 40, useDamageMultiplier: true, friendlyFire: false, hitThreshold: 10)
				super();

				@dmgPerShot = dmgPerShot;
				@useDamageMultiplier = useDamageMultiplier;
				@friendlyFire = friendlyFire;
				@hitThreshold = hitThreshold
			end

			def process_raw_hit(hitPlayer, sourcePlayer)
				unless(@friendlyFire)
					return false if hitPlayer.team == sourcePlayer.team
				end
				return false if(hitPlayer.dead && (hitPlayer.life < @hitThreshold));

				return true;
			end

			on :playerHit do |hitPlayer, sourcePlayer|
				shotMultiplier = 1;

				if((@useDamageMultiplier) && (!sourcePlayer.nil?))
					shotMultiplier = sourcePlayer.gunDamage();
				end

				hitPlayer.damage_by(@dmgPerShot * shotMultiplier, sourcePlayer);
				hitPlayer.hit();
			end
		end
	end
end
