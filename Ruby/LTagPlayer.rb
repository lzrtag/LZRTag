
require_relative 'Libs/MQTTSubscriber.rb'

module Lasertag
class Client
	attr_reader :name
	attr_reader :team
	attr_reader :brightness
	attr_reader :id

	attr_reader :battery
	attr_reader :ping
	attr_reader :heap

	attr_accessor :data

	def initialize(name, mqtt)
		@mqtt = mqtt;
		@name = name;

		@mqttTopic = "Lasertag/Players/#{@name}"

		@data = Hash.new();

		@team = 0;
		@brightness = 0;

		@battery = 3.3;
		@ping = 10000;
		@heap = 40000;
	end

	def connected?()
		return @id != nil;
	end

	def team=(n)
		n = n.to_i;
		return false unless n != nil and n < 3 and n >= 0;
		@team = n;
		@mqtt.publishTo "#{@mqttTopic}/Team", @team, retain: true;
		return true;
	end

	def brightness=(n)
		n = n.to_i;
		return false unless n != nil and n <= 5 and n >= 0;
		@brightness = n;
		@mqtt.publishTo "#{@mqttTopic}/Brightness", @brightness, retain: true;
		return true;
	end

	def id=(n)
		if(n != nil) then
			raise ArgumentError, "ID must be a integer!" unless n.is_a? Integer;
			raise ArgumentError, "ID out of range (0<ID<256)" unless n < 256 and n > 0;

			@id = n;
		else
			@id = nil;
		end

		@mqtt.publish_to "#{@mqttTopic}/ID", @id, retain: true;
	end

	def clean_all_topics()
		raise "Client still connected!" if connected?

		@mqtt.publishTo "#{@mqttTopic}/Team", "", retain: true;
		@mqtt.publishTo "#{@mqttTopic}/Brightness", "", retain: true;
	end

	def console(str)
		@mqtt.publishTo "#{@mqttTopic}/Console/In", str;
	end

	def override_brightness(level, duration)
		return false unless level.is_a? Integer and duration.is_a? Numeric
		console("overrideVest(#{(duration*1000).to_i},#{level});");
	end
	def stop_brightness_override()
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

	private :console
end
end
