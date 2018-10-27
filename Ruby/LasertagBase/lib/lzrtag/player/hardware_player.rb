
require 'mqtt/mqtt_hash.rb'

require_relative 'base_player.rb'

module LZRTag
	module Player
		class Hardware < Base

			attr_reader :team
			attr_reader :brightness

			attr_reader :dead, :lifeChangeTime

			attr_reader :ammo

			attr_reader :battery, :ping, :heap

			attr_reader :fireConfig, :hitConfig

			def initialize(*data)
				super(*data);

				@team = 0;
				@brightness = 0;

				@dead = false;
				@lifeChangeTime = Time.now();

				@ammo = 0;

				@battery = 0; @ping = 0; @heap = 0;

				@fireConfig = MQTT::TXHash.new(@mqtt, "Lasertag/Players/#{@DeviceID}/FireConf")
				@hitConfig  = MQTT::TXHash.new(@mqtt, "Lasertag/Players/#{@DeviceID}/HitConf")
			end

			def on_mqtt_data(data, topic)
				case topic[1..topic.length].join("/")
				when "System"
					begin
						info = JSON.parse(data);
						@heap = info["heap"].to_i;
						@battery = info["battery"].to_f
						@ping = info["ping"].to_f/1000
					rescue
					end
				when "Dead"
					dead = (data == "true")
					return if @dead == dead;
					@dead = dead;

					@lifeChangeTime = Time.now();

					@handler.send_event(@dead ? :playerKilled : :playerRevived, self);
				when "Ammo"
					@ammo = data.to_i;
				else
					super(data, topic);
				end
			end

			def team=(n)
				n = n.to_i;
				raise ArgumentError, "Team out of range (must be between 0 and 255)" unless n != nil and n <= 255 and n >= 0;

				return if @team == n;
				oldT = @team;
				@team = n;

				_pub_to "Team", @team, retain: true;
				@handler.send_event :playerTeamChanged, self, oldT;
				@team;
			end
			def brightness=(n)
				n = n.to_i;
				raise ArgumentError, "Brightness out of range (must be between 0 and 7 or nil)" unless n != nil and n <= 7 and n >= 0;
				return if @brightness == n;

				@brightness = n;
				_pub_to "Brightness", @brightness, retain: true;

				@brightness;
			end

			def _set_dead(d, player = nil)
				dead = (d ? true : false);
				return if @dead == dead;
				@dead = dead;

				@lifeChangeTime = Time.now();

				_pub_to "Dead", @dead ? "true" : "", retain: true;
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
				unless (n.is_a?(Integer) and (n >= 0)) then
					raise ArgumentError, "Ammo amount needs to be a positive number!"
				end

				@ammo = n;

				_pub_to("AmmoSet", n);
			end

			def fireConfig=(h)
				@fireConfig.hash = h;
			end
			def hitConfig=(h)
				@hitConfig.hash = h;
			end

			def clear_all_topics()
				super();

				[	"Dead", "Brightness", "Team",
					"HitConf", "FireConf"].each do |t|
					_pub_to(t, "", retain: true);
				end
			end
		end
	end
end
