
require 'mqtt/mqtt_hash.rb'

require_relative 'base_player.rb'

module LZRTag
	module Player
		class Hardware < Base
			attr_reader :team
			attr_reader :brightness

			attr_reader :dead, :deathChangeTime

			attr_reader :ammo
			attr_reader :maxAmmo
			attr_reader :gunNo

			attr_reader :gyroPose

			attr_reader :position
			attr_reader :zoneIDs

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

				@ammo = 0;
				@maxAmmo = 0;
				@gunNo = 0;

				@gyroPose = :unknown;

				@position = {x: 0, y: 0}
				@zoneIDs  = Hash.new();

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
					return if(data.size != 8)

					outData = data.unpack("L<*");
					@ammo = outData[0];
					@maxAmmo = outData[1];
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

			def ammo=(n)
				unless (n.is_a?(Integer) and (n >= 0))
					raise ArgumentError, "Ammo amount needs to be a positive number!"
				end

				@ammo = n;

				_pub_to("Stats/Ammo/Set", n);
			end

			def gunNo=(n)
				unless (n.is_a?(Integer) and (n >= 0))
					raise ArgumentError, "Gun ID needs to be a positive integer!"
				end

				return if(@gunNo == n)

				@gunNo = n;
				_pub_to("CFG/GunNo", n, retain: true);
			end

			def gunDamage(number = nil)
				number = @gunNo if(number.nil?)

				return @GunDamageMultipliers[number-1] || 1;
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
