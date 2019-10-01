
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

			def get_phase_map()
				return @phaseMap
			end

			def initialize(handler)
				super()

				@handler = handler;

				@hookList = Array.new();

				@tickTime = 1;
			end


			end


					end
				end

				@handler.each do |pl|
					pl.noise(frequency: 1000);
					pl.brightness = :active;
				end
			end

				end

			end
			end
		end
	end
end
