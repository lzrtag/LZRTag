
require 'mqtt/sub_handler'

module LZRTag
	module Player
		class Base
			attr_reader :handler
			attr_reader :DeviceID

			attr_reader :name

			attr_reader :status

			attr_reader   :id
			attr_accessor :hitIDTimetable

			attr_reader :battery, :ping, :data

			def initialize(deviceID, handler)
				@handler = handler;
				@mqtt = handler.mqtt;

				@DeviceID = deviceID;

				@status = "";
				@name   = "";

				@hitIDTimetable = Hash.new(Time.new(0));
			end

			def _pub_to(key, data, retain: false)
				mqtt.publish_to("Lasertag/Players/#{@DeviceID}/#{key}", data, retain: retain);
			end
			private :_pub_to
			def _console(cmd)
				_pub_to("Console/In", cmd);
			end
			private :_console

			def on_mqtt_data(data, topic)
				case topic[3]
				when "Connection"
					return if @status == data;
					oldStatus = @status;
					@status = data;
					if(@status == "OK")
						@handler.send_event(:playerConnected, self);
					elsif(oldStatus == "OK")
						@handler.send_event(:playerDisconnected, self);
					end
				when "System"
					begin
						info = JSON.parse(data);
						@heap = info["heap"].to_i;
						@battery = info["battery"].to_f
						@ping = info["ping"].to_f
					rescue
					end
				when "Name"
					@name = data;
				end
			end

			def connected?()
				return @status == "OK"
			end
			def clear_safemode()
				_console('file.remove("BOOT_SAFECHECK")') if @status == "SAFEMODE"
			end

			def id=(n)
				return if @id == n;

				if(@id != nil)
					raise ArgumentError, "ID must be integer or nil!" unless n.is_a? Integer;
					raise ArgumentError, "ID out of range (0<ID<256)" unless n < 256 and n > 0;

					@id = n;
				else
					@id = nil;
				end

				_pub_to("ID", @id);
			end

			def clear_all_topics()
				self.id = nil;
				_pub_to("Connection", "", retain: true);
			end

			def inspect()
				return "#<Player #{@DeviceID} (#{@status})| VBat: #{@battery} | Ping: #{@ping}ms>";
			end
		end
	end
end
