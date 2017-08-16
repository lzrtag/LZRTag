
require_relative 'MQTTSubscriber.rb'
require 'json'

module Lasertag
class Client
	attr_reader :name
	attr_reader :team
	attr_reader :brightness
	attr_reader :id

	attr_reader :battery
	attr_reader :ping
	attr_reader :heap

	@@idTable = Hash.new(false);
	@@playerTable = Hash.new();

	def self.mqtt=(mqttConnection)
		@@mqtt = mqttConnection;
	end

	def self.connected()
		activeSets = Hash.new();

		@@playerTable.each do |k, v|
			if(v.connected?) then
				activeSets[k] = v;
			end
		end

		return activeSets;
	end

	def self.each_connected()
		@@playerTable.each do |k, v|
			if(v.connected?) then
				yield(v);
			end
		end
	end

	def initialize(name)
		raise "No MQTT Connection given!" unless @@mqtt != nil;
		raise "Player is already registered!" if @@playerTable[name];

		@@playerTable[name] = self;

		@name = name;

		@mqttTopic = "Lasertag/Players/#{@name}"

		@team = 3;
		@brightness = 0;

		@@mqtt.subscribeTo "#{@mqttTopic}/Team" do |tList, data|
			tNum = data.to_i;
			@team = tNum if tNum;
		end

		@@mqtt.subscribeTo "#{@mqttTopic}/Brightness" do |tList, data|
			tBright = data.to_i;
			@brightness = tBright if tBright;
		end

		@@mqtt.subscribeTo "#{@mqttTopic}/System" do |tList, data|
			sysInfo = JSON.parse(data);
			@heap 	= sysInfo["heap"].to_i;
			@battery = sysInfo["battery"].to_f/1000.0;
			@ping 	= sysInfo["ping"].to_i;
		end

		@@mqtt.subscribeTo "#{@mqttTopic}/Connection" do |tList, data|
			if (data == "OK") then
				newID() if @id == nil;
			else
				@@idTable.delete @id unless @id == nil;
				@@mqtt.publishTo "#{@mqttTopic}/ID", "";
				@id = nil;
			end
		end
	end

	def newID()
		@id = 1;
		while true do
			break unless(@@idTable[@id])
			@id += 1;
		end
		@@idTable[@id] = @name;
		@@mqtt.publishTo "#{@mqttTopic}/ID", @id;
	end

	def connected?()
		return @id != nil;
	end

	def team=(n)
		n = n.to_i;
		return false unless n != nil and n < 3 and n >= 0;
		@team = n;
		@@mqtt.publishTo "#{@mqttTopic}/Team", @team, retain: true;
		return true;
	end

	def brightness=(n)
		n = n.to_i;
		return false unless n != nil and n <= 5 and n >= 0;
		@brightness = n;
		@@mqtt.publishTo "#{@mqttTopic}/Brightness", @brightness, retain: true;
		return true;
	end

	def console(str)
		@@mqtt.publishTo "#{@mqttTopic}/Console/In", str;
	end

	def overrideBrightness(level, duration)
		return false unless level.is_a? Integer and duration.is_a? Numeric
		console("overrideVest(#{(duration*1000).to_i},#{level});");
	end
	def stopBrightnessOverride()
		console("overrideVest(0, 0);");
	end

	def vibrate(duration)
		return false unless duration.is_a? Numeric and duration <= 2.55
		console("vibrate(#{(duration*1000).to_i});");
	end

	def fire
		console("fireWeapon();")
	end

	def noise(duration: 0.5, startF: 440, endF: startF)
		return false unless duration.is_a? Numeric and startF.is_a? Integer and endF.is_a? Integer
		console("ping(#{startF},#{endF},#{(duration*1000).to_i});");
	end

	private :console, :newID
end
end
