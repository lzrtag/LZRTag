
require_relative '../hooks/base_hook.rb'

# @author Xasin
module LZRTag
	module Game
		# The base game class.
		# It implements a DSL that allows users to easily
		# define their own games, and hooks in with the
		# game event system
		# @example
		#   class OwnGame < LZRTag::Game::Base
		#      def initialize(handler)
		#         super(handler);
		#         @tickTime = 1;
		#      end
		#
		#      hook :dmgHook, LZRTag::Hook::Damager
		#
		#      phase :starting do |deltaTick|
		#         puts "I tick!"
		#      end
		#
		#      on :playerRegistered do |newPlayer|
		#         puts "Hello new player!";
		#      end
		#   end
		#
		#   handler.start_game(OwnGame)
		#   # Alternatively, register the game and let it be activated via MQTT
		#   handler.register_game("My Game", OwnGame);
		class Base < Hook::Base
			# Returns a list of all currently instantiated hooks
			attr_reader :hookList
			# Returns the current per-tick target time. Can, and should, be set during
			# constructor to control the granularity of the game
			attr_reader :tickTime

			# Returns a list of the known phases of this game
			attr_reader :phases

			# @private
			# This function is meant for the DSL,
			# to allow adding to the class itself
			def self.get_phase_map()
				@globalPhaseMap ||= Hash.new();
				return @globalPhaseMap;
			end
			# @private
			# @see get_phase_map()
			def self.get_phase_prep_map()
				@globalPhasePrepMap ||= Hash.new();
				return @globalPhasePrepMap;
			end
			# @private
			# @see get_phase_map()
			def self.get_phase_end_map()
				@globalPhaseEndMap ||= Hash.new();
				return @globalPhaseEndMap;
			end
			# @private
			# @see get_phase_map()
			def self.get_hooks()
				@globalHookList ||= Hash.new();
				return @globalHookList;
			end

			# This function returns a list of possible phases
			# and their tick callbacks
			def get_phase_map()
				return @phaseMap
			end

			# Initializes a generic game handler.
			# This function is usually not called by the user,
			# but instead by the LZRTag::Handler::Game when
			# starting a game.
			# @param handler [LZRTag::Handler::Base] any valid
			# 		LZRTag handler that the game shall follow
			#
			def initialize(handler)
				super(handler)

				@hookList = Hash.new();
				self.class.get_hooks().each do |hookID, hookData|
					@hookList[hookID] = hookData[0].new(@handler, **hookData[1])
				end

				@tickTime = 0.1;

				@phaseMap = self.class.get_phase_map();
				@phasePrepMap = self.class.get_phase_prep_map();
				@phaseEndMap = self.class.get_phase_end_map();

				@phases = [@phaseMap.keys, @phasePrepMap.keys].flatten.uniq

				@phaseTime = 0;
				@phaseLastTime = 0;
			end

			# @!group DSL functions

			# DSL function to add a Hook type to this game.
			# Any hook type added by this function will be instantiated when the game
			# itself is instantiated, and will be linked in with the internal game
			# signals.
			# @param hookID [Symbol] The ID of the hook, used for later referencing
			# @param hookType [Hook::Base] The class of the hook to instantiate
			# @param hookOptions [Hash] A hash of options to pass to the constructor of the hook
			def self.hook(hookID, hookType, hookOptions = {})
				raise ArgumentError, "Hook ID needs to be a symbol!" unless hookID.is_a? Symbol
				unless hookType.is_a? Class and hookType < LZRTag::Hook::Base
					raise ArgumentError, "Hook needs to be a LZR::Hook!"
				end
				raise ArgumentError, "Hook options need to be a hash" unless hookOptions.is_a? Hash
				get_hooks()[hookID] = [hookType, hookOptions];
			end

			# DSL function to provide a phase tick code to this game
			# The block provided to this function will be executed every game tick,
			# with the delta-time since last tick as parameter.
			# @param phaseName [Symbol] The name of the phase during which to execute this code
			# @yield [deltaTime] Calls this block every game-tick during the specified phase
			def self.phase(phaseName, &block)
				raise ArgumentError, "Block needs to be given!" unless block_given?

				phaseName = [phaseName].flatten
				phaseName.each do |evt|
					unless (evt.is_a? Symbol)
						raise ArgumentError, "Phase needs to be a symbol or array of symbols!"
					end
					self.get_phase_map()[evt] = block;
				end
			end

			# DSL function to provide a callback immediately after switching to a new phase.
			# The provided block will be called only once, right after a phase switch happened.
			# As such, it can be used to prepare game timer settings, player classes, etc.
			# @param phaseName [Symbol] The name of the phase before which to execute
			# @yield [oldPhase] Calls the block right after a switch, with the old phase as parameter
			def self.phase_prep(phaseName, &block)
				raise ArgumentError, "Block needs to be given!" unless block_given?

				phaseName = [phaseName].flatten
				phaseName.each do |evt|
					unless (evt.is_a? Symbol)
						raise ArgumentError, "Phase needs to be a symbol or array of symbols!"
					end
					self.get_phase_prep_map()[evt] ||= Array.new()
					self.get_phase_prep_map()[evt] << block;
				end
			end

			# DSL function to provide a callback immediately before switching to a new phase.
			# The provided block is guaranteed to execute before any phase and phase_prep blocks,
			# giving the user an option to reset and clean up after themselves.
			# @param phaseName [Symbol] Name of the phase after which to call this block
			# @yield [newPhase] Calls the block right before the switch, with the upcoming phase as parameter
			def self.phase_end(phaseName, &block)
				raise ArgumentError, "Block needs to be given!" unless block_given?

				phaseName = [phaseName].flatten
				phaseName.each do |evt|
					unless (evt.is_a? Symbol)
						raise ArgumentError, "Phase needs to be a symbol or array of symbols!"
					end
					self.get_phase_end_map()[evt] ||= Array.new()
					self.get_phase_end_map()[evt] << block;
				end
			end

			# @!endgroup

			# @private
			def consume_event(evt, data)
				super(evt, data);

				case evt
				when :gameTick
					handle_game_tick(*data);
				when :gamePhaseStarts
					handle_phase_change();
				when :gamePhaseEnds
					if @phaseEndMap[data[0]]
						@phaseEndMap[data[0]].each do |cb|
							instance_exec(&cb);
						end
					end
				end

				@hookList.each do |hookID, hook|
					hook.consume_event(evt, data);
				end
			end

			# @private
			def handle_phase_change
				@phaseTime = 0;

				return unless @phasePrepMap[@handler.gamePhase]
				@phasePrepMap[@handler.gamePhase].each do |cb|
					instance_exec(&cb);
				end
			end

			# @private
			def handle_game_tick(dT)
				phase = @handler.gamePhase
				return unless @phaseMap[phase];
				return if phase == :idle;

				@phaseLastTime = @phaseTime;
				@phaseTime += dT;
				@handler.mqtt.publish_to "Lasertag/Game/Timer", @phaseTime

				instance_exec(dT, &@phaseMap[phase]);
			end
		end
	end
end
