
require_relative 'count_handler.rb'

module LZRTag
	module Handler
		class Game < Count
			attr_reader :currentGame
			attr_reader :gamePhase

			attr_reader :gamePlayers

			def initialize(*data, **argHash)
				super(*data, **argHash)

				@lastTick = Time.now();

				@lastGame    = nil;
				@currentGame = nil;
				@nextGame 	 = nil;

				@gamePhase = :idle;

				@gamePlayers = Array.new();

				@knownGames = Hash.new();

				_start_game_thread();

				@mqtt.subscribe_to "Lasertag/Game/Controls/#" do |data, topics|
					case topics[0]
					when "SetPhase"
						phase = data.to_sym;
						if(get_allowed_phases().include? phase)
							set_phase(phase);
						end
					when "SetGame"
						if(@knownGames[data])
							start_game(@knownGames[data])
						elsif(data == "STOP")
							stop_game();
						end
					end
				end

				clean_game_topics();
				at_exit {
					clean_game_topics();
				}
			end

			def clean_game_topics()
				@mqtt.publish_to "Lasertag/Game/ParticipatingPlayers", [].to_json(), retain: true
				@mqtt.publish_to "Lasertag/Game/KnownGames", @knownGames.keys.to_json, retain: true;
				@mqtt.publish_to "Lasertag/Game/CurrentGame", "", retain: true
			end

			def _start_game_thread()
				@gameTickThread = Thread.new() do
					loop do
						Thread.stop() until(@nextGame.is_a? LZRTag::Game::Base);

						@currentGame = @nextGame;
						set_phase(:starting);

						@lastTick = Time.now();
						while(@currentGame == @nextGame)
							sleep @currentGame.tickTime
							dT = Time.now() - @lastTick;
							@lastTick = Time.now();

							send_event(:gameTick, dT);
						end

						set_phase(:idle);
						sleep 1;
						@currentGame = nil;
					end
				end
				@gameTickThread.abort_on_exception = true;
			end

			def register_game(gameTag, game)
				raise ArgumentError, "Game Tag must be a string!" unless gameTag.is_a? String
				raise ArgumentError, "Game must be a LZRTag::Game class" unless game <= LZRTag::Game::Base

				@knownGames[gameTag] = game;

				@mqtt.publish_to "Lasertag/Game/KnownGames", @knownGames.keys.to_json, retain: true;
			end

			def consume_event(event, data)
				super(event, data)

				return unless @currentGame
				@currentGame.consume_event(event, data);
			end

			def start_game(game = @lastGame)
				@lastGame = game;

				if(gKey = @knownGames.key(game))
					@mqtt.publish_to "Lasertag/Game/CurrentGame", gKey, retain: true
				end

				game = game.new(self) if game.is_a? Class and game <= LZRTag::Game::Base;
				unless(game.is_a? LZRTag::Game::Base)
					raise ArgumentError, "Game class needs to be specified!"
				end
				@nextGame = game;

				@gameTickThread.run();
				@mqtt.publish_to "Lasertag/Game/Phase/Valid",
					get_allowed_phases.to_json(), retain: true
			end

			def stop_game()
				@nextGame = nil;
				@mqtt.publish_to "Lasertag/Game/CurrentGame", "", retain: true
			end

			def get_allowed_phases()
				allowedPhases = [:idle]
				if(@currentGame)
					allowedPhases = [allowedPhases, @currentGame.phases].flatten
				end

				return allowedPhases;
			end

			def set_phase(nextPhase)
				allowedPhases = get_allowed_phases();

				puts "Allowed phases are: #{allowedPhases}"
				raise ArgumentError, "Phase must be valid!" unless allowedPhases.include? nextPhase

				oldPhase = @gamePhase
				@gamePhase = nextPhase;
				send_event(:gamePhaseEnds, oldPhase, nextPhase)
				send_event(:gamePhaseStarts, nextPhase, oldPhase);
				@mqtt.publish_to "Lasertag/Game/Phase/Current", @gamePhase.to_s, retain: true

			end
			def gamePhase=(nextPhase)
				set_phase(nextPhase)
			end

			def gamePlayers=(newPlayers)
				raise ArgumentError, "Game player list shall be an array!" unless newPlayers.is_a? Array
				@gamePlayers = newPlayers;

				plNameArray = Array.new();
				@gamePlayers.each do |pl|
					plNameArray << pl.deviceID();
				end

				@mqtt.publish_to "Lasertag/Game/ParticipatingPlayers", plNameArray.to_json(), retain: true
			end
			def each_participating()
				@gamePlayers.each do |pl|
					yield(pl)
				end
			end

			def in_game?(player)
				return @gamePlayers.include? player
			end
		end
	end
end
