
require_relative '../hooks/base_hook.rb'

module LZRTag
	module Game
		class Base < Hook::Base
			attr_reader :hooksList
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

			def get_phase_map()
				return @phaseMap
			end

			def initialize(handler)
				super()

				@handler = handler;

				@hookList = Array.new();

				@tickTime = 0.3;

				@phaseMap = self.class.get_phase_map();
				@phasePrepMap = self.class.get_phase_prep_map();

				@phases = @phaseMap.keys

				@phaseTime = 0;
				@phaseLastTime = 0;
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

			def consume_event(evt, data)
				super(evt, data);

				case evt
				when :gameTick
					handle_game_tick(*data);
				when :gamePhaseChanged
					handle_phase_change();
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
				return unless @phaseMap[@handler.gamePhase];
				return if @handler.gamePhase == :idle;

				@phaseLastTime = @phaseTime;
				@phaseTime += dT;
				@handler.mqtt.publish_to "Lasertag/Game/Timer", @phaseTime

				instance_exec(dT, &@phaseMap[@handler.gamePhase]);
			end
		end
	end
end
