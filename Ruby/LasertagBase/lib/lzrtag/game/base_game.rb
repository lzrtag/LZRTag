
require_relative '../hooks/base_hook.rb'

# @author Xasin
module LZRTag
	module Game
		# The base game class.
		# It implements a DSL that allows users to easily
		# define their own games, and hooks in with the
		# game event system
		class Base < Hook::Base
			attr_reader :hookList
			attr_reader :tickTime

			attr_reader :phases

			# @private
			# This function is meant for the DSL,
			# to allow adding to the class itself
			def self.get_phase_map()
				@globalPhaseMap ||= Hash.new();
				return @globalPhaseMap;
			end
			# @private
			# See get_phsae_map()
			def self.get_phase_prep_map()
				@globalPhasePrepMap ||= Hash.new();
				return @globalPhasePrepMap;
			end
			# @private
			# See get_phase_map()
			def self.get_phase_end_map()
				@globalPhaseEndMap ||= Hash.new();
				return @globalPhaseEndMap;
			end
			# @private
			# See get_phase_map()
			def self.get_hooks()
				@globalHookList ||= Array.new();
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

				@hookList = Array.new();
				self.class.get_hooks().each do |hookData|
					@hookList << hookData[0].new(@handler, **hookData[1])
				end

				@tickTime = 0.1;

				@phaseMap = self.class.get_phase_map();
				@phasePrepMap = self.class.get_phase_prep_map();
				@phaseEndMap = self.class.get_phase_end_map();

				@phases = [@phaseMap.keys, @phasePrepMap.keys].flatten.uniq

				@phaseTime = 0;
				@phaseLastTime = 0;
			end

			def self.hook(hookType, hookOptions = {})
				unless hookType.is_a? Class and hookType < LZRTag::Hook::Base
					raise ArgumentError, "Hook needs to be a LZR::Hook!"
				end
				raise ArgumentError, "Hook options need to be a hash" unless hookOptions.is_a? Hash
				get_hooks() << [hookType, hookOptions];
			end

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

				@hookList.each do |hook|
					hook.consume_event(evt, data);
				end
			end

			def handle_phase_change
				@phaseTime = 0;

				return unless @phasePrepMap[@handler.gamePhase]
				@phasePrepMap[@handler.gamePhase].each do |cb|
					instance_exec(&cb);
				end
			end

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
