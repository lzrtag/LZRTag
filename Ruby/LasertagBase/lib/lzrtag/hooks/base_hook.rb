
module LZRTag
	module Hook
		class Base
			@@globalCBList = Hash.new();

			def initialize()
				@localCBList = Hash.new();
			end

			def self.on(evtName, &block)
				raise ArgumentError, "Block needs to be given!" unless block_given?

				evtName = [evtName].flatten
				evtName.each do |evt|
					unless (evt.is_a? Symbol)
						raise ArgumentError, "Event needs to be a symbol or array of symbols!"
					end
					@@globalCBList[evt] ||= Array.new();
					@@globalCBList[evt] << block;
				end
			end

			def on(evtName, &block)
				raise ArgumentError, "Block needs to be given!" unless block_given?

				evtName = [evtName].flatten
				evtName.each do |evt|
					unless (evt.is_a? Symbol)
						raise ArgumentError, "Event needs to be a symbol or array of symbols!"
					end
					@localCBList[evt] ||= Array.new();
					@localCBList[evt] << block;
				end
			end

			def consume_event(evtName, data)
				puts "Got EVENT #{evtName} : #{data}";

				puts "Global CB for this event is: #{@@globalCBList[evtName]}";

				if(cbList = @@globalCBList[evtName]) then
					cbList.each do |cb|
						instance_exec(*data, &cb);
					end
				end
				cbList = nil;
				if(cbList = @localCBList[evtName]) then
					cbList.each do |cb|
						cb.call(*data);
					end
				end
			end

			def on_hookin(handler)
				@handler = handler;
			end
			def on_hookout()
			end

			def process_raw_hit(hitPlayer, sourcePLayer)
				return true;
			end
		end
	end
end
