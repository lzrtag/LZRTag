
require_relative 'count_handler.rb'

module LZRTag
	module Handler
=begin
	Game handler class, managing game registration and game ticks.
	This class manages the lifecycle of any lasertag game started in it.
	This includes sending a gameTick, managing phase and game state transitions,
	sending out but also receiving information on the game state via MQTT, etc.

@example
	handler = LZRTag::Handler::Game.new(mqtt);

	handler.register_game("My Game", SomeGame);
	handler.register_game("Other game!", SomeOtherGame);

	handler.start_game(SomeGame); # This will fetch the registered name and publish to MQTT
											# Alternatively, sending to Lasertag/Game/Controls/SetGame with
											# the string name will switch to the game.

	# Game and phase switching is possible at any time, and will be handled asynchronously
	sleep 5;
	handler.set_phase(:somePhase);
=end
		class Game < Count
			# Returns the instance of the currently active game, or nil if none is present
			attr_reader :currentGame

			# currently active game phase
			# When set it will start a phase change, sending :gamePhaseEnds and
			# :gamePhaseStarts events
			# @see set_phase
			# @return Symbol
			attr_reader :gamePhase

			# List of in-game players.
			# This list is mainly to keep track of which players the game is acting
			# upon, but updating it will also send a list of player ID Strings to
			# MQTT (Lasertag/Game/ParticipatingPlayers), and will send :playerEnteredGame
			# and :playerLeftGame events
			# @return Array<LZRTag::Player::Base>
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

				@mqtt.subscribe_to "Lasertag/Game/Controls/+" do |data, topics|
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
				@mqtt.publish_to "Lasertag/Game/KnownGames", [].to_json, retain: true;
				@mqtt.publish_to "Lasertag/Game/CurrentGame", "", retain: true
			end
			private :clean_game_topics;

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

						puts "Stopping current game.".green
						set_phase(:idle);
						sleep 1;
						@currentGame = nil;
					end
				end
				@gameTickThread.abort_on_exception = true;
			end
			private :_start_game_thread;

			# Register a game by a tag.
			# This function will register a given LZRTag::Game::Base class under a given
			# string tag. This tag can then be used to, via MQTT, start the game, and is
			# also used to give players a cleartext game name.
			# A list of games is published to Lasertag/Game/KnownGames
			# @param gameTag [String] Cleartext name of the game
			# @param game [LZRTag::Game::Base] The game class to register
			# @see start_game
			def register_game(gameTag, game)
				raise ArgumentError, "Game Tag must be a string!" unless gameTag.is_a? String
				raise ArgumentError, "Game must be a LZRTag::Game class" unless game <= LZRTag::Game::Base

				@knownGames[gameTag] = game;

				@mqtt.publish_to "Lasertag/Game/KnownGames", @knownGames.keys.to_json, retain: true;
			end

			# @private
			def consume_event(event, data)
				super(event, data)

				return unless @currentGame
				@currentGame.consume_event(event, data);
			end

			# Starts a given new game (or the last one).
			# This function will take either a String (as registered with register_game),
			# or a LZRTag::Game::Base class, instantiate it, and start it.
			# If no fitting game was found, the game is instead stopped.
			#
			# @note The first phase that is started by default is :starting, the Game
			#    class must define at least a phase_start hook to change the phase and
			#    configure the game!
			# @param game [String,LZRTag::Game::Base] The game, or game name, to start
			def start_game(game = @lastGame)
				@lastGame = game;

				if(game.is_a? String and gClass = @knownGames[game])
					game = gClass;
				elsif(game.is_a? String)
					stop_game();
					return;
				end

				if(gKey = @knownGames.key(game))
					@mqtt.publish_to "Lasertag/Game/CurrentGame", gKey, retain: true
					puts "Starting game #{gKey}!".green
				else
					@mqtt.publish_to "Lasertag/Game/CurrentGame", "", retain: true
				end

				game = game.new(self) if game.is_a? Class and game <= LZRTag::Game::Base;
				unless(game.is_a? LZRTag::Game::Base)
					raise ArgumentError, "Game class needs to be specified!"
				end
				@nextGame = game;
				send_event(:gameStarting, @nextGame);

				@gameTickThread.run();
				@mqtt.publish_to "Lasertag/Game/Phase/Valid",
					get_allowed_phases.to_json(), retain: true
			end

			# Stops the currently running game.
			def stop_game()
				@nextGame = nil;
				@mqtt.publish_to "Lasertag/Game/CurrentGame", "", retain: true
			end

			# Returns an Array<Symbol> of the currently allowed phases of this game.
			# This list can also be retrieved via MQTT, under Lasertag/Game/CurrentGame
			def get_allowed_phases()
				allowedPhases = [:idle]
				if(@currentGame)
					allowedPhases = [allowedPhases, @currentGame.phases].flatten
				end

				return allowedPhases;
			end

			# Tries to change the current phase.
			# This function will set the current phase to nextPhase, if it is an allowed
			# one. However, if nextPhase does not belong to the list of allowed phases,
			# and error is raised. The :gamePhaseEnds and :gamePhaseStarts events are
			# triggered properly. This function can be called from any context, not just
			# inside the game code itself.
			# @see get_allowed_phases
			def set_phase(nextPhase)
				allowedPhases = get_allowed_phases();

				raise ArgumentError, "Phase must be valid!" unless allowedPhases.include? nextPhase

				puts "Phase started: #{nextPhase}!".green;

				oldPhase = @gamePhase
				send_event(:gamePhaseEnds, oldPhase, nextPhase)

				@mqtt.publish_to "Lasertag/Game/Phase/Current", @gamePhase.to_s, retain: true
				@gamePhase = nextPhase;
				send_event(:gamePhaseStarts, nextPhase, oldPhase);
			end
			# Alias for set_phase
			# @see set_phase
			def gamePhase=(nextPhase)
				set_phase(nextPhase)
			end

			# Update the list of in-game players.
			# @param [Array<LZRTag::Player::Base] Array of active players
			def gamePlayers=(newPlayers)
				raise ArgumentError, "Game player list shall be an array!" unless newPlayers.is_a? Array
				@gamePlayers = newPlayers.dup;

				@playerNames = Array.new();
				plNameArray = Array.new();
				@gamePlayers.each do |pl|
					plNameArray << pl.deviceID();
				end

				newPlayers = @gamePlayers - @oldGamePlayers;
				newPlayers.each do |pl|
					send_event :playerEnteredGame, pl;
				end
				oldPlayers = @oldGamePlayers - @gamePlayers;
				oldPlayers.each do |pl|
					send_event :playerLeftGame, pl;
				end

				@oldGamePlayers = @gamePlayers.dup;
				@mqtt.publish_to "Lasertag/Game/ParticipatingPlayers", plNameArray.to_json(), retain: true
			end

			# Yield for each currently in-game player
			def each_participating()
				@gamePlayers.each do |pl|
					yield(pl)
				end
			end

			# Check if a player is currently in game
			def in_game?(player)
				return @gamePlayers.include? player
			end
		end
	end
end
