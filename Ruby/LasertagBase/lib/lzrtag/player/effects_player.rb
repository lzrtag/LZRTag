
require_relative 'hardware_player.rb'

module LZRTag
	module Player
		# This class extends the pure hardware class, adding various hooks
		# that can be used to send effects and other events to the weapon.
		# These do not change the game itself, but instead just look and feel good!
		class Effects < Hardware
			# Heartbeat status of the player
			# The "heartbeat" is a regular vibration pattern on the weapon,
			# which can be used to indicate things like low life or other tense events.
			# Set it to true/false.
			attr_reader :heartbeat

			# Mark a player in a given color
			# This function can be used to "mark" a player, sending flashes of
			# light across their LEDs in a given color. This can either be
			# false, to turn the marking off, or 0..7 to set it to a team color.
			# Alternatively, any RGB Number (0x?? ?? ??) can be used for arbitrary marking color
			attr_reader :marked

			def initialize(*data)
				super(*data);

				@marked = false;
			end

			# Vibrate the weapon for a number of seconds
			# @param duration [Numeric] Number (in s) to vibrate for.
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

			# Make the weapon play a given note.
			# This function can make the set play a note of given frequency, volume
			# and duration.
			# @param duration [Numeric] Length in seconds
			# @param frequency [Numeric] Frequency of the note
			# @param volume [Numeric] Volume (0..1) of the note
			def noise(duration: 0.5, frequency: 440, volume: 0.5)
				return false unless duration.is_a? Numeric and frequency.is_a? Integer
				_pub_to("Sound/Note", [frequency, volume*20000, duration*1000].pack("L3"))
			end

			# Play a given sound file.
			# Depending on the weapon, various sounds are available to be played,
			# such as:
			# - "GAME START"
			# - "HIT"
			# - "OWN DEATH"
			# - "KILL SCORE"
			# etc.
			# This list may be expanded in the future
			def sound(sName)
				_pub_to("Sound/File", sName);
			end

			# Make the weapon display a hit.
			# When a weapon is hit, it will flash bright white and vibrate
			# for a short moment. The length can be specified.
			# @param hitLength [Numeric,nil] Length (in s) of the hit.
			def hit(hitLength = nil)
				_pub_to("CFG/Hit", hitLength || 0.7)
			end

			# @private
			def clear_all_topics()
				super();

				["CFG/Heartbeat", "CFG/Marked"].each do |t|
					_pub_to(t, "", retain: true)
				end
			end
		end
	end
end
