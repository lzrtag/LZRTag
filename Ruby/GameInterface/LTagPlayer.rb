
module Lasertag
class Client
	def self.mqtt
		@mqtt
	end
	def self.game
		@game
	end

	attr_reader :name
	attr_reader :team
	attr_reader :brightness
	attr_reader :id

	attr_reader :ammo

	attr_reader :battery
	attr_reader :ping
	attr_reader :heap

	attr_accessor :data

	def initialize(name)
		@name = name;

		@mqttTopic = "Lasertag/Players/#{@name}"

		@data = Hash.new();

		@team = 0;
		@brightness = 0;

		@ammo = 0;

		@dead = false;

		@battery = 3.3;
		@ping = 10000;
		@heap = 40000;
	end

	def mqtt
		self.class.mqtt
	end
	def game
		self.class.game
	end

	def connected?()
		return @id != nil;
	end

	def send_message(topic, data, retain: nil)
		mqtt().publish_to topic, data, retain: retain;
	end

	def team=(n)
		n = n.to_i;
		raise ArgumentError, "Team out of range (must be between 0 and 255)" unless n != nil and n <= 255 and n >= 0;
		@team = n;
		send_message "#{@mqttTopic}/Team", @team, retain: true;
		return true;
	end
	def brightness=(n)
		n = n.to_i;
		raise ArgumentError, "Brightness out of range (must be between 0 and 5)" unless n != nil and n <= 7 and n >= 0;
		@brightness = n;
		send_message "#{@mqttTopic}/Brightness", @brightness, retain: true;
		return true;
	end
	def id=(n)
		if(n != nil) then
			raise ArgumentError, "ID must be integer or nil!" unless n.is_a? Integer;
			raise ArgumentError, "ID out of range (0<ID<256)" unless n < 256 and n > 0;

			@id = n;
		else
			@id = nil;
		end

		send_message "#{@mqttTopic}/ID", @id, retain: true;
	end
	def dead?
		return @dead;
	end
	def dead=(d)
		@dead = (d ? true : false);
		send_message "#{@mqttTopic}/Dead", (@dead ? "true" : ""), retain: true;
	end

	def ammo=(a)
		unless (a.is_a?(Integer) and (a >= 0)) then
			raise ArgumentError, "Ammo amount needs to be a positive number!"
		end

		@ammo = a;

		send_message "#{@mqttTopic}/AmmoSet", a
	end

	def hitConfig
		return Hash.new unless @hitConfig;
		return @hitConfig;
	end
	def hitConfig=(h)
		if(h == nil) then
			mqtt.publish_to "#{@mqttTopic}/HitConf", "", retain: true;
			@hitConfig = nil;
			return;
		end

		raise ArgumentError, "Hit Config needs to be a hash or nil!" unless h.is_a? Hash
		@hitConfig = h;

		send_message "#{@mqttTopic}/HitConf", @hitConfig.to_json, retain: true;
	end

	def fireConfig
		return Hash.new unless @fireConfig;
		return @fireConfig
	end
	def fireConfig=(h)
		if(h == nil) then
			mqtt.publish_to "#{@mqttTopic}/FireConf", "", retain: true;
			@fireConfig = nil;
			return;
		end

		raise ArgumentError, "Fire Config needs to be a hash or nil!" unless h.is_a? Hash
		@fireConfig = h;

		send_message "#{@mqttTopic}/FireConf", @fireConfig.to_json, retain: true;
	end

	def clean_all_topics()
		mqtt.publish_to "#{@mqttTopic}/Team", "", retain: true;
		mqtt.publish_to "#{@mqttTopic}/Brightness", "", retain: true;
		mqtt.publish_to "#{@mqttTopic}/ID", "", retain: true;
		mqtt.publish_to "#{@mqttTopic}/Dead", "", retain: true;

		self.hitConfig = nil;
		self.fireConfig = nil;
	end

	def console(str)
		send_message "#{@mqttTopic}/Console/In", str;
	end
	private :console

	def override_brightness(level, duration)
		raise ArgumentError unless level.is_a? Integer and duration.is_a? Numeric
		console("overrideVest(#{(duration*1000).to_i},#{level});");
	end
	def stop_brightness_override()
		console("overrideVest(0, 0);");
	end

	def vibrate(duration)
		raise ArgumentError, "Vibration-duration out of range (between 0 and 65.536)" unless duration.is_a? Numeric and duration <= 65.536 and duration >= 0
		console("vibrate(#{(duration*1000).to_i});");
	end

	def fire
		console("fireWeapon();")
	end

	def noise(duration: 0.5, startF: 440, endF: startF)
		return false unless duration.is_a? Numeric and startF.is_a? Integer and endF.is_a? Integer
		console("ping(#{startF},#{endF},#{(duration*1000).to_i});");
	end

	def hit()
		console("displayHit();");
	end


	def inspect()
		iString = "<Player:#{@name}##{@id ? @id : "OFFLINE"}, Team=#{@team}";
		iString += ", DEAD" if dead?
		iString += ", Battery=#{@battery.round(2)}"
		iString += ", Heap=#{@heap}" if @heap < 10000;
		iString +=  ", Ping=#{@ping.ceil}ms>";
	end
end
end
