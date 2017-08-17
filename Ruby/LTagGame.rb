
require_relative 'MQTTSubscriber.rb'
require_relative 'LTagPlayer.rb'
require 'json'

module Lasertag
class Game
	def initialize(mqtt, autodetect: true, delete_disconnected: false)
		@mqtt = mqtt;
		@mqttTopic = "Lasertag/Players/+"

		@clients = Hash.new();
		@idTable = Hash.new();

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
			if (data == "OK") then
				newclient = false;
				if (autodetect and not @clients.key? pName) then
					@clients[pName] = Client.new(pName, @mqtt);
					newclient = true;
				end

				unless (@clients[pName].connected?) then
					i = 1;
					while @idTable[i] do
						i += 1;
					end
					@idTable[i] = true;
					@clients[pName].id = i;

					if newclient then
						@clientRegisteredCB.call(pName, @clients[pName]) if @clientRegisteredCB
					end

					@clientConnectCB.call(pName, @clients[pName]) if @clientConnectCB
				end
			else
				@clientDisconnectCB.call(pName, @clients[pName]) if @clientDisconnectCB
				@idTable.delete @clients[pName].id
				@clients[pName].id = nil;
				if(delete_disconnected) then
					remove_player(pName);
				end
			end
		end
	end

	def [](c)
		return @clients[c];
	end

	def on_connect(&connectProc)
		@clientConnectCB = connectProc;
	end
	def on_disconnect(&disconnectProc)
		@clientDisconnectCB = disconnectProc;
	end
	def on_register(&registerProc);
		@clientRegisteredCB = registerProc;
	end
	def on_unregister(&unregisterProc);
		@clientUnregisteredCB = unregisterProc;
	end

	def remove_player(pName)
		@clientUnregisteredCB.call(pName, @clients[pName]) if @clientUnregisteredCB
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
