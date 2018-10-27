
require_relative 'base_hook.rb'

module LZRTag
	module Hook
		class Debug < Base
			attr_accessor :eventWhitelist
			attr_accessor :eventBlacklist

			def initialize()
				super();

				@eventWhitelist = Array.new();
				@eventBlacklist = Array.new();
			end

			def consume_event(evtName, data)
				super(evtName, data);

				return if @eventBlacklist.include? evtName
				unless(@eventWhitelist.empty?)
					return unless @eventWhitelist.include? evtName
				end

				puts "Caught event: #{evtName} with data: #{data}";
			end
		end

		class RandomTeam < Base
			attr_accessor :teamWhitelist

			def initialize()
				super();

				@teamWhitelist = (1..7).to_a;
			end

			def on_hookin(game)
				super(game);

				game.each do |pl|
					reassignTeam(pl);
				end
			end

			def reassignTeam(player)
				minCount = @handler.teamCount.values.min[1];

				(@teamWhitelist.shuffle()).each do |t|
					if(@handler.teamCount[t] == minCount)
						player.team = t;
						break;
					end
				end
			end

			on :playerConnected do |player|

				puts "Reassigning player: #{player}"
				reassignTeam(player);
			end
		end
	end
end
