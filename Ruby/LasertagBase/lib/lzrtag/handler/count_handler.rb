
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
			attr_reader :teamMap
			# Returns a Hash with keys equal to player's brightnesses, describing
			# how many players have which brightness
			attr_reader :brightnessMap

			# Returns a hash with keys of beacon numbers, describing
			# how many players are in which beacon
			attr_reader :beaconMap

			def initialize(*args, **argHash)
				super(*args, **argHash);

				@teamMap = Hash.new();
				7.times do |i|
					@teamMap[i] = Array.new;
				end

				@brightnessMap = Hash.new();
				Player::Hardware.getBrightnessKeys().each do |bKey|
					@brightnessMap[bKey] = Array.new;
				end

				@beaconMap = Hash.new() do |h, k|
					h[k] = Array.new();
				end
			end

			# @private
			def consume_event(evtName, data)
				super(evtName, data);

				case evtName
				when :playerRegistered
					@teamMap[data[0].team] << data[0];
					@brightnessMap[data[0].brightness] << data[0];
				when :playerUnregistered
					@teamMap[data[0].team].delete data[0];
					@brightnessMap[data[0].brightness].delete data[0];
				when :playerTeamChanged
					@teamMap[data[1]].delete data[0];
					@teamMap[data[0].team] << data[0];
				when :playerBrightnessChanged
					@brightnessMap[data[1]].delete data[0];
					@brightnessMap[data[0].brightness] << data[0];
				when :playerEnteredBeacon
					@beaconMap[data[1]] << data[0];
				when :playerLeftBeacon
					@beaconMap[data[1]].delete data[0];
				end
			end

			def get_team_killcount(team)
				total = 0;
				@teamMap[team].each do |pl|
					total += pl.stats["Kills"];
				end

				return total;
			end
		end
	end
end
