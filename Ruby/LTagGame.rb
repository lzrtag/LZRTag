
require_relative 'MQTTSubscriber.rb'
require_relative 'LTagPlayer.rb'
require 'json'

module Lasertag
class Game
	def initialize(mqtt, delete_disconnected: false, id_assign: true)
		@mqtt = mqtt;
		@mqttTopic = "Lasertag/Players/+"

		@clients = Hash.new();
		@idTable = Hash.new();

		@clientConnectCBs 		= Array.new();
		@clientDisconnectCBs		= Array.new();
		@clientRegisteredCBs		= Array.new();
		@clientUnregisteredCBs	= Array.new();

		@mqtt.subscribeTo "#{@mqttTopic}/Team" do |tList, data|
			if @clients.key? tList[0] then
				teamNumber = data.to_i;
				@clients[tList[0]].instance_variable_set(:@team, teamNumber) if teamNumber;
			end
		end

		@mqtt.subscribeTo "#{@mqttTopic}/Brightness" do |tList, data|
			if @clients.key? tList[0] then
				brightness = data.to_i;
				@clients[tList[0]].instance_variable_set(:@brightness, brightness) if brightness;
			end
		end

		@mqtt.subscribeTo "#{@mqttTopic}/System" do |tList, data|
			if @clients.key? tList[0] then
				sysInfo = JSON.parse(data);
				c = @clients[tList[0]];
				c.instance_variable_set(:@heap, sysInfo["heap"].to_i);
				c.instance_variable_set(:@battery, sysInfo["battery"].to_i/1000.0);
				c.instance_variable_set(:@ping, sysInfo["ping"].to_i/1000.0);
			end
		end

		@mqtt.subscribeTo "#{@mqttTopic}/Connection" do |tList, data|
			pName = tList[0];

			if(data == "OK") then
				# Check if the player is on record.
				# If not, generate one and call the callbacks
				if(not @clients.key? pName) then
					@clients[pName] = Client.new(pName, @mqtt);
					@clientRegisteredCBs.each do |cb|
						cb.call(pName, @clients[pName]);
					end
				end

				# Check if the player is not registered as connected right now
				# If he isn't, that means he reconnected. Call the callbacks
				if(not @clients[pName].connected?) then
					@clientConnectCBs.each do |cb|
						cb.call(pName, @clients[pName]);
					end
				end

			else
				# Check whether or not the player is connected, and this is the LWT disconnect
				if(@clients.key?(pName) and @clients[pName].connected?) then
					@clientDisconnectCBs.each do |cb|
						cb.call(pName, @clients[pName]);
					end
				end
			end
		end

		if(delete_disconnected) then
			on_disconnect do |pName, player|
				remove_player(pName);
			end
		end

		if(id_assign) then
			on_connect do |pName, player|
				i = 1;
				while @idTable[i] do
					i += 1;
				end
				@idTable[i] = pName;
				player.id 	= i;

				@mqtt.publish_to "Lasertag/Game/ID", @idTable.to_json, retain: true;
			end

			on_disconnect do |pName, player|
				@idTable.delete player.id;
				player.id = nil;

				@mqtt.publish_to "Lasertag/Game/ID", @idTable.to_json, retain: true;
			end
		end

		at_exit {
			print "Disconnecting Lasertag Clients ... "
			remove_disconnected();
			puts "Done!"
		}
	end

	def [](c)
		return @clients[c] if c.is_a? String
		return @clients[@idTable[c]] if c.is_a? Integer

		raise ArgumentError, "Unknown identifier for the player id!"
	end

	def on_connect(&connectProc)
		@clientConnectCBs << connectProc;
		return connectProc;
	end
	def on_disconnect(&disconnectProc)
		# Using "unshift" here so that the higher level functions get called first
		@clientDisconnectCBs.unshift(disconnectProc);
		return disconnectProc;
	end
	def on_register(&registerProc);
		@clientRegisteredCBs << registerProc;
		return registerProc;
	end
	def on_unregister(&unregisterProc);
		# Using "unshift" here so that the higher level functions get called first
		@clientUnregisteredCBs.unshift(unregisterProc);
		return unregisterProc;
	end

	def remove_callback(callback)
		@clientConnectCBs.delete callback
		@clientDisconnectCBs.delete callback
		@clientRegisteredCBs.delete callback
		@clientUnregisteredCBs.delete callback
	end

	def remove_player(pName)
		@clientUnregisteredCBs.each do |cb|
			cb.call(pName, @clients[pName]);
		end
		@clients[pName].clean_all_topics unless @clients[pName].connected?
		@clients.delete pName;
	end

	def each()
		@clients.each do |k, v|
			yield(k, v);
		end
	end

	def each_connected()
		@clients.each do |k, v|
			yield(k, v) if v.connected?
		end
	end

	def remove_disconnected()
		@clients.each do |k, v|
			remove_player k unless v.connected?
		end
	end

	def get_connected()
		outputHash = Hash.new();
		self.each_connected do |k, v|
			outputHash[k] = v;
		end
		return outputHash;
	end

	def num_connected()
		n = 0;
		self.each_connected do |k, v|
			n += 1;
		end
		return n;
	end

end
end
