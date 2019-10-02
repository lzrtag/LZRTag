
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

				_start_game_thread();
			end

			def _start_game_thread()
				@gameTickThread = Thread.new() do
					loop do
						Thread.stop() until(@nextGame.is_a? LZRTag::Game::Base);

						@currentGame = @nextGame;
						set_phase(:starting);

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

			def consume_event(event, data)
				super(event, data)

				return unless @currentGame
				@currentGame.consume_event(event, data);
			end

			def start_game(game = @lastGame)
				@lastGame = game;

				game = game.new(self) if game.is_a? Class and game <= LZRTag::Game::Base;
				unless(game.is_a? LZRTag::Game::Base)
					raise ArgumentError, "Game class needs to be specified!"
				end
				@nextGame = game;

				@gameTickThread.run();
			end

			def stop_game()
				@nextGame = nil;
			end

			def set_phase(nextPhase)
				allowedPhases = [:idle]
				if(@currentGame)
					allowedPhases = [allowedPhases, @currentGame.phases].flatten
				end

				puts "Allowed phases are: #{allowedPhases}"
				raise ArgumentError, "Phase must be valid!" unless allowedPhases.include? nextPhase

				oldPhase = @gamePhase
				@gamePhase = nextPhase;
				send_event(:gamePhaseEnds, oldPhase, nextPhase)
				send_event(:gamePhaseStarts, nextPhase, oldPhase);
				@mqtt.publish_to "Lasertag/Game/Phase", @gamePhase.to_s, retain: true
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

				@mqtt.publish_to "Lasertag/Game/Participating", plNameArray.to_json(), retain: true
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
