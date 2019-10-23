
require_relative 'base_hook.rb'

module LZRTag
	module Hook
		class Debug < Base
			attr_accessor :eventWhitelist
			attr_accessor :eventBlacklist

			def initialize()
				super();

				@eventWhitelist = Array.new();
				@eventBlacklist = [:slowTick, :gameTick, :playerInBeacon];
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

		class TeamSelector < Base
			describe_option :possibleTeams, "List of teams that can be selected", {
				type: Array
			}

			def initialize(handler, possibleTeams: [1, 2, 3, 4])
				super(handler);

				@possibleTeams = possibleTeams;
			end

			def in_phase
				return @handler.gamePhase == :teamSelect;
			end
			def is_selecting(pl)
				return ([:idle, :teamSelect].include? pl.brightness)
			end

			on :gamePhaseEnds do |oldPhase, nextPhase|
				if((oldPhase == :teamSelect) && (nextPhase != :idle))
					puts "Selecting active players!"

					nextPlayers = Array.new();
					@handler.each do |pl|
						if([:active, :teamSelect].include? pl.brightness)
							nextPlayers << pl;
						else
							pl.team = 0;
							pl.brightness = :idle;
						end
					end

					@handler.gamePlayers = nextPlayers;

					puts "Game players are: #{@handler.gamePlayers}"
				end
			end

			on :gamePhaseStarts do |nextPhase, oldPhase|
				case(nextPhase)
				when :teamSelect
					@handler.each do |pl|
						pl.brightness = :idle;

						unless(@possibleTeams.include?(pl.team))
							pl.team = @possibleTeams.sample();
						end
					end
				end
			end

			on :poseChanged do |pl, nPose|
				next unless in_phase
				next unless is_selecting(pl)

				pl.brightness = (pl.gyroPose == :laidDown) ? :idle : :teamSelect;
			end

			on :navSwitchPressed do |player, dir|
				next unless in_phase

				newTeam = @possibleTeams.find_index(player.team) || 0;

				newTeam += 1 if(dir == 2)
				newTeam -= 1 if(dir == 3)

				player.team = @possibleTeams[newTeam % @possibleTeams.length]
				if(dir == 1)
					player.brightness = :active
				else
					player.brightness = :teamSelect
				end
			end

			on :playerEnteredBeacon do |pl, beacon|
				next unless in_phase

				next unless is_selecting(pl)
				next unless(@possibleTeams.include? beacon)

				pl.team = beacon;
				pl.brightness = :teamSelect;
			end

			on :playerLeftBeacon do |pl, beacon|
				next unless in_phase

				next unless(pl.team == beacon)
				next unless is_selecting(pl)

				pl.team = 0;
				pl.brightness = :idle
			end
		end

		class Regenerator < Base

			describe_option :regRate, "Regeneraton rate, HP per second"
			describe_option :regDelay, "Healing delay, in s, after a player was hit"
			describe_option :healDead, "Whether or not to heal dead players"

			describe_option :autoReviveThreshold, "The HP a player needs before he is revived"

			describe_option :teamFilter, "Which teams this regenerator belongs to"
			describe_option :phaseFilter, "During which phases this hook should be active"

			def initialize(handler, **options)
				super(handler);

				@regRate = options[:regRate] || 1;
				@regDelay = options[:regDelay] || 10;

				@healDead = options[:healDead] || false;
				@autoReviveThreshold = options[:autoReviveThreshold] || 30;

				@teamFilter = options[:teamFilter] || (0..7).to_a
				@phaseFilter = options[:phaseFilter] || [:running]
			end

			on :gameTick do |dT|
				next unless @phaseFilter.include? @handler.gamePhase

				@handler.each_participating do |pl|
					next unless @teamFilter.include? pl.team

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
			describe_option :dmgPerShot, "Base damage per shot"
			describe_option :useDamageMultiplier, "Shall shots be adjusted per-gun?"
			describe_option :friendlyFire, "Shall friendly-fire be enabled"
			describe_option :hitThreshold, "Limit below dead players will not be hit"

			def initialize(handler, **options)
				super(handler);

				@dmgPerShot = options[:dmgPerShot] || 40;
				@useDamageMultiplier = options[:useDamageMultiplier] || true;
				@friendlyFire = options[:friendlyFire] || false;
				@hitThreshold = options[:hitThreshold] || 10;
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
