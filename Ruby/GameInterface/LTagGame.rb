
require_relative '../Libs/MQTTSubscriber.rb'
require_relative 'EventHook.rb'
require_relative 'LTagPlayer.rb'
require 'json'

module Lasertag
class Game < Lasertag::EventHook
	attr_accessor :mqtt

	def initialize(mqtt, delete_disconnected: false, clean_on_exit: true)
		@mqtt = mqtt;
		@mqttTopic = "Lasertag/Players/+"

		@clientClass = Class.new(Lasertag::Client)
		@clientClass.instance_variable_set(:@mqtt, mqtt);
		@clientClass.instance_variable_set(:@game, self);

		@clients = Hash.new();
		@idTable = Hash.new();

		@hooks   = Array.new();

		@mqtt.subscribe_to "#{@mqttTopic}/Team" do |tList, data|
			if @clients.key? tList[0] then
				teamNumber = data.to_i;
				@clients[tList[0]].instance_variable_set(:@team, teamNumber) if teamNumber;
			end
		end
		@mqtt.subscribe_to "#{@mqttTopic}/Brightness" do |tList, data|
			if @clients.key? tList[0] then
				brightness = data.to_i;
				@clients[tList[0]].instance_variable_set(:@brightness, brightness) if brightness;
			end
		end
		@mqtt.subscribe_to "#{@mqttTopic}/Dead" do |tList, data|
			if @clients.key? tList[0] then
				@clients[tList[0]].instance_variable_set(:@dead, data == "true");
			end
		end
		@mqtt.subscribe_to "#{@mqttTopic}/Ammo" do |tList, data|
			if @clients.key? tList[0] then
				@clients[tList[0]].instance_variable_set(:@ammo, data.to_i);
			end
		end

		@mqtt.subscribe_to "#{@mqttTopic}/System" do |tList, data|
			if @clients.key? tList[0] then
				sysInfo = JSON.parse(data);
				c = @clients[tList[0]];
				c.instance_variable_set(:@heap, sysInfo["heap"].to_i);
				c.instance_variable_set(:@battery, sysInfo["battery"].to_i/1000.0);
				c.instance_variable_set(:@ping, sysInfo["ping"].to_i/1000.0);
			end
		end

		@mqtt.subscribe_to "#{@mqttTopic}/Connection" do |tList, data|
			pName = tList[0];

			# Check if the player is on record.
			# If not, generate one and call the callbacks
			if(not @clients.key? pName) then
				@clients[pName] = @clientClass.new(pName);
				@hooks.each do |h|
					h.onPlayerRegistration(@clients[pName]);
				end
			end

			player = @clients[pName];

			oldStatus = player.status;
			player.instance_variable_set(:@status, data);

			if(data == "OK") then
				# Check if the player is not registered as connected right now
				# If he isn't, that means he reconnected. Call the callbacks
				if(oldStatus != "OK") then
					# Search for a free ID number.
					# Since the table of free ID numbers does not need to be continuous,
					# each ID has to be looked at.
					i = 1;
					while @idTable[i] do
						i += 1;
					end
					@idTable[i] = pName;
					player.id 	= i;
					@mqtt.publish_to "Lasertag/Game/ID", @idTable.to_json, retain: true;

					@hooks.each do |h|
						h.onPlayerConnect(player);
					end
				end
			# Check whether or not the player is connected, and this is the LWT disconnect
			elsif(oldStatus == "OK") then
				@hooks.each do |h|
					h.onPlayerDisconnect(player);
				end

				@idTable.delete player.id;
				player.id = nil;
				@mqtt.publish_to "Lasertag/Game/ID", @idTable.to_json, retain: true;

				remove_player(pName) if delete_disconnected;
			end
		end

		if(clean_on_exit) then
			at_exit {
				@clients.each do |pName, player|
					print "Disconnecting client #{pName}...          \r"
					remove_player(pName);
				end
				sleep 1;
				puts "Done disconnecting clients!          "
			}
		end
	end

	def [](c)
		return @clients[c] if c.is_a? String
		return @clients[@idTable[c]] if c.is_a? Integer

		raise ArgumentError, "Unknown identifier for the player id!"
	end

	def add_hook(hook)
		unless(hook.is_a? Lasertag::EventHook) then
			raise ArgumentError, "Hook needs to be a Lasertag::EventHook!"
		end

		return if(@hooks.include? hook);
		hook.onHookin(self);
		@hooks << hook;

		if(hook.catch_up_players) then
			@clients.each do |pName, player|
				hook.onPlayerRegistration(player);
				hook.onPlayerConnect(player) if player.connected?
			end
		end
	end
	def remove_hook(hook)
		unless(hook.is_a? Lasertag::EventHook) then
			raise ArgumentError, "Hook needs to be a Lasertag::EventHook!"
		end
		
		return unless @hooks.include? hook
		hook.onHookout();
		@hooks.delete(hook);
	end

	def remove_player(pName)
		@hooks.each do |h|
			h.onPlayerUnregistration(@clients[pName]);
		end
		@idTable.delete @clients[pName].id if @clients[pName].id;

		@clients[pName].clean_all_topics;
		@clients.delete pName;
	end

	def each()
		@clients.each do |k, v|
			yield(k, v);
		end

		return;
	end

	def each_connected()
		@clients.each do |k, v|
			yield(k, v) if v.connected?
		end

		return;
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
