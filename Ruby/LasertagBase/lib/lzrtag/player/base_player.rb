
require 'mqtt/sub_handler'

module LZRTag
	module Player
		# The base player class.
		# This class is not instantiated by the user, but instead on a on-demand basis
		# by the LZRTag::Handler::Base when a new PlayerID needs to be registered. The player classes
		# process and send MQTT data, handle events, and keep track of per-player infos like life,
		# damage, ammo, team, etc. etc.
		class Base
			attr_reader :handler
			# @return [String] The player's DeviceID, which is derived from the ESP's MAC
			attr_reader :DeviceID

			# @return [String] Name of the player, set externally
			attr_reader :name

			# @return [String] status-string of the player. Should be "OK"
			attr_reader :status

			attr_reader :connected

			# @return [Time] Time at which the player status was last updated
			attr_reader :last_status_update

			# @return [Integer] 0..255, shot ID of the player
			attr_reader   :id
			# @return [Hash<Time>] Hash of the last few recorded shot times,
			#  used for hit arbitration
			attr_accessor :hitIDTimetable

			def initialize(deviceID, handler)
				@handler = handler;
				@mqtt = handler.mqtt;

				@DeviceID = deviceID;

				@status = "";
				@name   = "";

				@last_status_update = Time.at(0);
				@connected = false;

				@hitIDTimetable = Hash.new(Time.new(0));
			end

			def _pub_to(key, data, retain: false)
				@mqtt.publish_to("Lasertag/Players/#{@DeviceID}/#{key}", data, retain: retain, qos: 1);
				if(data == "")
					@mqtt.publish_to("Lasertag/Players/#{@DeviceID}/#{key}", " ", retain: false);
				end
			end
			private :_pub_to

			# @private
			def on_mqtt_data(data, topic)
				case topic[1..topic.length].join("/")
				when "Connection"
					return if @status == data;
					@status = data;

					return if @status == "OK"
					return if @status == ""
					return if !@connected
					@connected = false;
					@handler.send_event(:playerDisconnected, self);

				when "CFG/Name"
					@name = data;
				when "Ping"
					@last_status_update = Time.now();

					if(@status == "OK" && (!@connected))
						@connected = true
						@handler.send_event(:playerConnected, self);
					end
				end
			end

			def _tick_connection()
				return unless @connected

				if((Time.now() - @last_status_update) > 60)
					@connected = false;
					@handler.send_event(:playerDisconnected, self);
				end
			end

			# @return [Boolean] Whether this player is connected
			def connected?()
				return @connected
			end

			# Set the Shot ID of the player.
			# @note Do not call this function yourself - the Handler must
			#   assign unique IDs to ensure proper game functionality!
			# @private
			def id=(n)
				return if @id == n;

				if(!n.nil?)
					raise ArgumentError, "ID must be integer or nil!" unless n.is_a? Integer;
					raise ArgumentError, "ID out of range (0<ID<256)" unless n < 256 and n > 0;

					@id = n;
				else
					@id = nil;
				end

				_pub_to("CFG/ID", @id, retain: true);
			end

			# Trigger a clear of all topics
			# @note Do not call this function yourself, except when deregistering a player!
			# @private
			def clear_all_topics()
				self.id = nil;
			end

			def inspect()
				iString =  "#<Player:#{@deviceID}##{@id ? @id : "OFFLINE"}, Team=#{@team}";
				iString += ", DEAD" if @dead
				iString += ", Battery=#{@battery.round(2)}"
				iString += ", Ping=#{@ping.ceil}ms>";

				return iString;
			end
			alias to_s inspect
		end
	end
end
