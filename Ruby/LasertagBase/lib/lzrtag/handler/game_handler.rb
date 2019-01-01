
require_relative 'count_handler.rb'

module LZRTag
	module Handler
		class Game < Count
			attr_reader :currentGame
			attr_reader :gameRunning

			def initialize(*data, **argHash)
				super(*data, **argHash)

				@lastTick = Time.now();

				@lastGame    = nil;
				@currentGame = nil;
				@nextGame 	 = nil;

				@gameRunning = false;

				_start_game_thread();
			end

			def _start_game_thread()
				@gameTickThread = Thread.new() do
					loop do
						Thread.stop() until(@nextGame.is_a? LZRTag::Game::Base);

						@currentGame = @nextGame;
						send_event(:gameStarting, @currentGame)
						@currentGame._on_start_raw();

						@gameRunning = true;
						send_event(:gameStarted, @currentGame);

						while(@currentGame == @nextGame)
							sleep @currentGame.tickTime
							dT = Time.now() - @lastTick;
							@lastTick = Time.now();

							send_event(:gameTick, dT);
						end

						@gameRunning = false;
						send_event(:gameStopping, @currentGame);
						@currentGame._on_end_raw();
						send_event(:gameStopped, @currentGame);
						@currentGame = @nextGame;
					end
				end
				@gameTickThread.abort_on_exception = true;
			end

			def start_game(game = @lastGame)
				unless(game.is_a? LZRTag::Game::Base)
					raise ArgumentError, "Game class needs to be specified!"
				end
				@lastGame = game;

				@nextGame = game;
				@gameTickThread.run();
			end
		end
	end
end
