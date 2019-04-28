
require_relative 'hardware_player.rb'

module LZRTag
	module Player
		class Effects < Hardware
			attr_reader :heartbeat

			def initialize(*data)
				super(*data);
			end

			def vibrate(duration)
				raise ArgumentError, "Vibration-duration out of range (between 0 and 65.536)" unless duration.is_a? Numeric and duration <= 65.536 and duration >= 0
				_console("vibrate(#{(duration*1000).to_i});");

				_pub_to("FX/Vibrate", duration);
			end

			def heartbeat=(data)
				return if (@heartbeat == data);

				@heartbeat = data;
				_pub_to("FX/Heartbeat", @heartbeat ? "1" : "0", retain: true);
			end

			def fire
				_console("fireWeapon();")
			end

			def noise(duration: 0.5, startF: 440, endF: startF)
				return false unless duration.is_a? Numeric and startF.is_a? Integer and endF.is_a? Integer
				_console("ping(#{startF},#{endF},#{(duration*1000).to_i});");
			end

			def sound(sName)
				_pub_to("FX/Sound", sName);
			end

			def hit()
				_pub_to("FX/Hit", @hitConfig[:hitDuration] || 0.7)
				_console("displayHit();");
			end

			def clear_all_topics()
				super();

				["FX/Heartbeat"].each do |t|
					_pub_to(t, "", retain: true)
				end
			end
		end
	end
end
