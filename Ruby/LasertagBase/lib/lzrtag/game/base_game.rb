
require_relative '../hooks/base_hook.rb'

module LZRTag
	module Game
		class Base < Hook::Base
			attr_reader :hookList
			attr_reader :tickTime

			def initialize(handler)
				@handler = handler;

				@hookList = Array.new();
				@activeHooks = Array.new();

				@tickTime = 0.3;
			end

			def _on_start_raw()
				@hookList.each do |h|
					@activeHooks << @handler.add_hook(h);
				end

				on_start();
			end

			def on_start()
				@handler.each do |pl|
					pl.sound("GAME START");
				end

				3.times do
					@handler.each do |pl|
						pl.noise();
					end
					sleep 1;
				end

				@handler.each do |pl|
					pl.noise(startF: 1000);
					pl.brightness = 7;
				end
			end

			def _on_end_raw()
				@activeHooks.each do |h|
					@handler.remove_hook(h);
				end

				on_end();
			end
			def on_end()
			end
		end
	end
end
