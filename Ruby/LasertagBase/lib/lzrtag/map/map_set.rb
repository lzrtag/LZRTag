
module LZRTag
	module Map
		class Set
			attr_reader :zones

			def initialize(mqtt)
				@mqtt = mqtt;
				@zones = Array.new();
			end

			def to_h()
				outZones = Array.new();

				@zones.each do ||

				end

				return outZones;
			end

			def to_json()
				return self.to_h().to_json();
			end

			def publish()
				@mqtt.publish_to "Lasertag/Game/Zones", self.to_json, qos: 1, retain: true;
			end
			def clear()
				@mqtt.publish_to "Lasertag/Game/Zones", "", retain: true;
			end
		end
	end
end
