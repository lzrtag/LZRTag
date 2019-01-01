
require 'json'
require_relative 'base_handler.rb'

module LZRTag
	module Handler
		class HitArb < Base
			def initialize(mqtt, **args)
				super(mqtt, **args);

				mqtt.subscribe_to "Lasertag/Game/Events" do |data|
					begin
						data = JSON.parse(data, symbolize_names: true);

						if(data[:type] == "hit")
							_handle_hitArb(data);
						end
					rescue JSON::ParserError
					end
				end
			end

			def process_raw_hit(*)
				return true;
			end

			def _handle_hitArb(data)
				unless  (hitPlayer = get_player(data[:target])) and
						  (sourcePlayer = get_player(data[:shooterID])) and
						  (arbCode = data[:arbCode])
					return
				end

				return if (sourcePlayer.hitIDTimetable[arbCode] + 1) > Time.now();

				veto = false;
				@hooks.each do |h|
					veto |= !(h.process_raw_hit(hitPlayer, sourcePlayer));
				end
				return if veto;

				sourcePlayer.hitIDTimetable[arbCode] = Time.now();

				send_event(:playerHit, hitPlayer, sourcePlayer);
			end
		end
	end
end
