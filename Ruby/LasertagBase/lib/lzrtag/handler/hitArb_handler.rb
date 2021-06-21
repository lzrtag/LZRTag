
require 'json'
require_relative 'base_handler.rb'

module LZRTag
	module Handler
		# Hit arbitration handling class.
		# This class extends the Handler::Base class, adding important features
		# to ensure that each player shot is only counted once.
		# Additionally, hooks can manipulate this behavior and prevent friendly-fire,
		# or enable multiple hits in the case of a shotgun!
		#
		# Shot arbitration is performed by listening to "Lasertag/Game/Events",
		# waiting for a 'type: "hit"'
		# If such a JSON payload is found, hit and source players will be
		# determined, and every available hook's "process_raw_hit" function
		# is called. If this function returns false, the hit will be "vetoed" and
		# does not count at all. However, a hook can raise {LZRTag::Handler::NoArbitration},
		# preventing this shot from being logged and thusly enabling multiple hits
		# @see Base
		class HitArb < Base
			def initialize(*data, **options)
				super(*data, **options);

				@mqtt.subscribe_to "/esp32/lzrtag/+/event/ir_hit" do |data|
					begin
						data = JSON.parse(data, symbolize_names: true);

						_handle_hitArb(data);
					#rescue JSON::ParserError
					end
				end
			end

			def process_raw_hit(*)
				return true;
			end

			def consume_event(evt, data)
				super(evt, data)
				if(evt == :slowTick)
					self.each do |pl|
						pl.check_beacons();
					end
				end
			end

			def _handle_hitArb(data)
				unless  (hitPlayer = get_player(data[:target])) and
						  (sourcePlayer = get_player(data[:shooterID])) and
						  (arbCode = data[:arbCode])
					return
				end

				if (sourcePlayer.hitIDTimetable[arbCode] + 0.2) > Time.now();
					return 
				end
				veto = false;
				arbitrateShot = true;

				hookList = Array.new();
				hookList << @hooks;
				if(@currentGame)
					hookList << @currentGame.hookList.values
				end
				hookList.flatten!

				hookList.each do |h|
					begin
						veto |= !(h.process_raw_hit(hitPlayer, sourcePlayer));
					rescue NoArbitration
						arbitrateShot = false;
					end
				end
				return if veto;

				if arbitrateShot
					sourcePlayer.hitIDTimetable[arbCode] = Time.now();
				end

				send_event(:playerHit, hitPlayer, sourcePlayer);
			end
		end

		class NoArbitration < Exception
		end
	end
end
