
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
				_pub_to("CFG/Vibrate", duration);
			end

			def heartbeat=(data)
				return if (@heartbeat == data);

				@heartbeat = data;
				_pub_to("CFG/Heartbeat", @heartbeat ? "1" : "0", retain: true);
			end

			def marked=(data)
				return if (@marked == data);

				@marked = data;
				if data.is_a? Numeric
					_pub_to("CFG/Marked", @marked.to_s, retain: true)
				else
					_pub_to("CFG/Marked", "0", retain: true)
				end
			end


			def noise(duration: 0.5, startF: 440, endF: startF)
				return false unless duration.is_a? Numeric and startF.is_a? Integer and endF.is_a? Integer
				_console("ping(#{startF},#{endF},#{(duration*1000).to_i});");
			end

			def sound(sName)
				_pub_to("Sound/File", sName);
			end

			def hit(hitLength = nil)
				_pub_to("CFG/Hit", hitLength || @hitConfig[:hitDuration] || 0.7)
			end

			def clear_all_topics()
				super();

				["CFG/Heartbeat", "CFG/Marked"].each do |t|
					_pub_to(t, "", retain: true)
				end
			end
		end
	end
end
