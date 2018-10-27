
require_relative 'hitArb_handler.rb'

module LZRTag
	module Handler
		class Count < HitArb
			attr_reader :teamCount

			def initialize(*args)
				super(*args);

				@teamCount = Hash.new();
				7.times do |i|
					@teamCount[i] = 0;
				end
			end

			def consume_event(evtName, data)
				super(evtName, data);

				case evtName
				when :playerConnected
					@teamCount[data[0].team] += 1;
				when :playerDisconnected
					@teamCount[data[0].team] -= 1;
				when :playerTeamChanged
					@teamCount[data[1]] -= 1;
					@teamCount[data[0].team] += 1;
				end
			end
		end
	end
end
