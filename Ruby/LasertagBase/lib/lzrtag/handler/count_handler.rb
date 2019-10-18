
require_relative 'hitArb_handler.rb'
require_relative '../player/hardware_player.rb'

module LZRTag
	module Handler
		# This class provides useful statistics about the current game and situation.
		#
		# The various readable attributes contain various information on the game, such as
		# current kill count, damage done, team and brightness composition, etc.
		# Most of this information can be used to determine game progress
		class Count < HitArb
			# Returns a Hash with keys 0..7, describing which teams have
			# how many players
			attr_reader :teamCount
			# Returns a Hash with keys equal to player's brightnesses, describing
			# how many players have which brightness
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

			# @private
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
