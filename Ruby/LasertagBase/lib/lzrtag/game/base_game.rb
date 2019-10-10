
require_relative '../hooks/base_hook.rb'

module LZRTag
	module Game
		class Base < Hook::Base
			attr_reader :hookList
			attr_reader :tickTime

			attr_reader :phases

			def self.get_phase_map()
				@globalPhaseMap ||= Hash.new();
				return @globalPhaseMap;
			end
			def self.get_phase_prep_map()
				@globalPhasePrepMap ||= Hash.new();
				return @globalPhasePrepMap;
			end
			def self.get_phase_end_map()
				@globalPhaseEndMap ||= Hash.new();
				return @globalPhaseEndMap;
			end
			def self.get_hooks()
				@globalHookList ||= Array.new();
				return @globalHookList;
			end

			def get_phase_map()
				return @phaseMap
			end

			def initialize(handler)
				super(handler)

				@hookList = Array.new();
				self.class.get_hooks().each do |hookData|
					@hookList << hookData[0].new(@handler, **hookData[1])
				end

				@tickTime = 0.3;

				@phaseMap = self.class.get_phase_map();
				@phasePrepMap = self.class.get_phase_prep_map();
				@phaseEndMap = self.class.get_phase_end_map();

				@phases = [@phaseMap.keys, @phasePrepMap.keys].flatten

				@phaseTime = 0;
				@phaseLastTime = 0;
			end

			def self.hook(hookType, hookOptions = {})
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
