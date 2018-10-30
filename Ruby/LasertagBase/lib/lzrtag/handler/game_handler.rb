
require_relative 'count_handler.rb'

module LZRTag
	module Handler
		class Game
			attr_reader :currentGame

			def initialize(*data)
				super(*data)

				@lastTick = Time.now();

				@lastGame    = nil;
				@currentGame = nil;

				@gameTickThread = Thread.new() do
					loop do
						Thread.stop() until(@currentGame);

						sleep @currentGame.tickTime
						dT = Time.now() - @lastTick;
						@lastTick = Time.now();

						send_event(:gameTick, dT);
					end
				end
			end

			def _start_game_synch(game)
				@currentGame = game;
				@lastGame = game;

				send_event(:gameStarting)
				@currentGame.on_start();

				send_event(:gameStarted);
				@gameTickThread.run();
			end

			def _stop_game_synch()
				return unless @currentGame
			end

			def start_game(game = @lastGame)
				Thread.new() do
					if(@currentGame)
						_stop_game_synch();
					end
					_start_game_synch(game || @currentGame);
				end
			end
		end
	end
end
