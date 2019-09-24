
require_relative 'hitArb_handler.rb'
require_relative '../player/hardware_player.rb'

module LZRTag
	module Handler
		class Count < HitArb
			attr_reader :teamCount
			attr_reader :brightnessCount

			def initialize(*args, **argHash)
				super(*args, **argHash);

				@teamCount = Hash.new();
				7.times do |i|
					@teamCount[i] = 0;
				end

				@brightnessCount = Hash.new();
				Player::Hardware.getBrightnessKeys().each do |bKey|
					@brightnessCount[bKey] = 0;
				end
			end

			def consume_event(evtName, data)
				super(evtName, data);

				case evtName
				when :playerRegistered
					@teamCount[data[0].team] += 1;
					@brightnessCount[data[0].brightness] += 1;
				when :playerUnregistered
					@teamCount[data[0].team] -= 1;
					@brightnessCount[data[0].brightness] -= 1;
				when :playerTeamChanged
					@teamCount[data[1]] -= 1;
					@teamCount[data[0].team] += 1;
				when :playerBrightnessChanged
					@brightnessCount[data[1]] -= 1;
					@brightnessCount[data[0].brightness] += 1;
				end
			end
		end
	end
end
