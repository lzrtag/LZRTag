
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

			# Returns a hash with keys of beacon numbers, describing
			# how many players are in which beacon
			attr_reader :beaconCount

			def initialize(*args, **argHash)
				super(*args, **argHash);

				@teamCount = Hash.new(0);

				@brightnessCount = Hash.new(0);

				@beaconCount = Hash.new(0);
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
				when :playerEnteredBeacon
					@beaconCount[data[1]] += 1;
				when :playerLeftBeacon
					@beaconCount[data[1]] -= 1;
				end
			end
		end
	end
end
