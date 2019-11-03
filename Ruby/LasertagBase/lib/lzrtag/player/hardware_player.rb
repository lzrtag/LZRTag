
require 'mqtt/mqtt_hash.rb'

require_relative 'base_player.rb'

module LZRTag
	module Player
		# Hardware-handling player class.
		# This class extends the base player, adding more hardware-related
		# functionality and interfaces, such as:
		# - Team setting
		# - Brightness setting
		# - Note playing
		# - Gyro and button readout
		# - Ping and Battery reading
		# etc.
		class Hardware < Base
			# The team (0..7) of this player.
			# Setting it to a number will publish to /DeviceID/CFG/Team,
			# changing the color of the weapon. Interpret it as binary string,
			# with 1 being red, 2 green and 4 blue.
			#
			# Changes trigger the :playerTeamChanged event, with [player, oldTeam] data
			attr_reader :team
			# Current brightness of the weapon.
			# @return [Symbol] Symbol describing the current brightness.
			# Possible brightnesses are:
			# - :idle (low, slow brightness, white with slight team hue)
			# - :teamSelect (team-colored with rainbow overlay)
			# - :dead (low brightness, team colored with white overlay)
			# - :active (bright, flickering and in team color)
			#
			# A change will trigger the :playerBrightnessChanged event, with data [player, oldBrightness]
			attr_reader :brightness

			# Whether or not the player is currently dead.
			# Set this to kill the player. Will trigger a :playerKilled or :playerRevived event,
			# although kill_by is preferred to also specify which player killed.
			attr_reader :dead
			# Last time the death status changed (killed/revivied).
			# Especially useful to determine when to revive a player
			attr_reader :deathChangeTime

			# Number of the current gun.
			# The application can freely choose which gun profile the set is using,
			# which influences shot speeds, sounds, reloading, etc.
			attr_reader :gunNo

			# Current amount of reserve ammo (i.e. clips the player can reload with)
			# Can be set to -1 to enable infinite reserve ammo (default), or
			# any other value to give the player limited ammo
			attr_reader :reserveAmmo
			# Current ammo in the clip. Can be set to -1 for infinite shots (no
			# reloading necessary at all), or any other numeric value. Default is
			# a single clip (gun-dependant)
			attr_reader :clipAmmo
			# Size of the clip of the currently equipped gun. Can not be set!
			attr_reader :clipSize

			# Returns the gyro pose of the set.
			# This is either:
			# - :active
			# - :laidDown
			# - :pointsUp
			# - :pointsDown
			# Changes are triggered by the set itself, if it has a gyro.
			# The :poseChanged event is sent on change with [player, newPose] data
			attr_reader :gyroPose

			# Returns a Hash of the last timestamps when a beacon was detected
			attr_reader :beaconTimes

			attr_reader :battery, :ping, :heap

			def self.getBrightnessKeys()
				return [:idle, :teamSelect, :dead, :active]
			end

			def initialize(*data)
				super(*data);

				@team = 0;
				@brightness = :idle;

				@dead = false;
				@deathChangeTime = Time.now();

				@reserveAmmo = 0;
				@clipAmmo 	 = 0;
				@clipSize 	 = 0;

				@gunNo = 0;

				@gyroPose = :unknown;

				@position = {x: 0, y: 0}
				@zoneIDs  = Hash.new();

				@beaconTimes = Hash.new();
				@BeaconTimeout = 3;

				@battery = 0; @ping = 0; @heap = 0;

				@BrightnessMap = self.class.getBrightnessKeys();

				# These values are configured for a DPS ~1, equal to all weapons
				# Including reload timings and other penalties
				@GunDamageMultipliers = [
					0.9138,
					1.85,
					0.6166,
				];
			end

			# @private
			# This function processes incoming MQTT data.
			# The user must not call this, since it is handled by the
			# LZRTag base handler
			def on_mqtt_data(data, topic)
				case topic[1..topic.length].join("/")
				when "HW/Ping"
					if(data.size == 3*4)
						parsedData = data.unpack("L<*");

						@battery = parsedData[0].to_f/1000;
						@ping 	= parsedData[2].to_f;
					end
				when "CFG/Dead"
					dead = (data == "1")
					return if @dead == dead;
					@dead = dead;

					@deathChangeTime = Time.now();

					@handler.send_event(@dead ? :playerKilled : :playerRevived, self);
				when "Stats/Ammo"
					return if(data.size != 12)

					outData = data.unpack("L<*");
					@clipAmmo = outData[0];
					@clipSize = outData[1];
					@reserveAmmo = outData[2];
				when "Position"
					begin
						@position = JSON.parse(data, symbolize_names: true);
					rescue
					end
				when "HW/NSwitch"
					@handler.send_event(:navSwitchPressed, self, data.to_i)
				when "HW/Gyro"
					@gyroPose = data.to_sym
					@handler.send_event(:poseChanged, self, @gyroPose);
				when "HW/BeaconDetect"
					beaconID = data.to_i;
					unless(@beaconTimes[beaconID])
						@handler.send_event(:playerEnteredBeacon, self, beaconID);
					end
					@beaconTimes[beaconID] = Time.now();
					@handler.send_event(:playerInBeacon, self, beaconID);
				when "ZoneUpdate"
					begin
						data = JSON.parse(data, symbolize_names: true);
					rescue JSON::ParserError
						return;
					end

					@zoneIDs = data[:data];
					if(data[:entered])
						@handler.send_event(:playerEnteredZone, self, data[:entered])
					end
					if(data[:exited])
						@handler.send_event(:playerExitedZone, self, data[:exited])
					end
				else
					super(data, topic);
				end
			end

			def team=(n)
				n = n.to_i;
				raise ArgumentError, "Team out of range (must be between 0 and 7)" unless n <= 7 and n >= 0;

				return if @team == n;
				oldT = @team;
				@team = n;

				_pub_to "CFG/Team", @team, retain: true;
				@handler.send_event :playerTeamChanged, self, oldT;

				@team;
			end
			def brightness=(n)
				raise ArgumentError, "Brightness must be a valid symbol!" unless @BrightnessMap.include? n;

				return if @brightness == n;
				oldB = @brightness;
				@brightness = n;

				n = @BrightnessMap.find_index(n)

				_pub_to "CFG/Brightness", n, retain: true;
				@handler.send_event :playerBrightnessChanged, self, oldB;

				@brightness;
			end

			def _set_dead(d, player = nil)
				dead = (d ? true : false);
				return if @dead == dead;
				@dead = dead;

				@deathChangeTime = Time.now();

				_pub_to "CFG/Dead", @dead ? "1" : "0", retain: true;
				@handler.send_event(@dead ? :playerKilled : :playerRevived, self, player);
			end
			def dead=(d)
				_set_dead(d);
			end
			def kill_by(player)
				return if @dead;
				_set_dead(true, player);
			end
			def revive_by(player)
				return unless @dead
				_set_dead(false, player)
			end

			def reserveAmmo=(n)
				unless (n.is_a?(Integer))
					raise ArgumentError, "Ammo amount needs to be a number!"
				end

				_pub_to("Stats/Ammo/SetReserve", n);
			end
			def clipAmmo=(n)
				unless (n.is_a?(Integer))
					raise ArgumentError, "Ammo amount needs to be a number!"
				end

				_pub_to("Stats/Ammo/SetClip", n);
			end

			def gunNo=(n)
				unless (n.is_a?(Integer) and (n >= 0))
					raise ArgumentError, "Gun ID needs to be a positive integer!"
				end

				return if(@gunNo == n)

				oldGun = @gunNo;
				@gunNo = n;
				@handler.send_event(:playerGunChanged, self, n, oldGun);

				_pub_to("CFG/GunNo", n, retain: true);
			end

			# Return the averaged damage the player's gun should do.
			# This function is very useful to calculate the damage a player did
			# per shot. The returned number tries to average damage to "1 DPS" for
			# all weapons regardless of speed etc., which the application can
			# multiply for a given total damage, creating a more balanced game.
			def gunDamage(number = nil)
				number ||= @gunNo

				return @GunDamageMultipliers[number-1] || 1;
			end

			def reload()
				_pub_to("CFG/Reload", "1");
			end

			def check_beacons()
				bIDs = @beaconTimes.keys;
				bIDs.each do |bID|
					if((Time.now() - @beaconTimes[bID]) > @BeaconTimeout)
						@handler.send_event(:playerLeftBeacon, self, bID);
						@beaconTimes.delete(bID)
					end
				end
			end

			def clear_all_topics()
				super();

				[	"CFG/Dead", "CFG/GunNo", "CFG/Brightness", "CFG/Team"].each do |t|
					_pub_to(t, "", retain: true);
				end
			end
		end
	end
end
