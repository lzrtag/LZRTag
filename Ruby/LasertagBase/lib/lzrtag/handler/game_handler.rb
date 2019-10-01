
require_relative 'count_handler.rb'

module LZRTag
	module Handler
		class Game < Count
			attr_reader :currentGame
			attr_reader :gameRunning

			attr_reader :gamePhase

			def initialize(*data, **argHash)
				super(*data, **argHash)

				@lastTick = Time.now();

				@lastGame    = nil;
				@currentGame = nil;
				@nextGame 	 = nil;

				@gamePhase = :idle;

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
				send_event(:gamePhaseChanged, nextPhase, oldPhase);
				@mqtt.publish_to "Lasertag/Game/Phase", @gamePhase.to_s, retain: true
			end
		end
	end
end
