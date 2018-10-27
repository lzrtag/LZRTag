
module LZRTag
	module Hook
		class Base
			@@globalCBList = Hash.new();

			def initialize()
				@localCBList = Hash.new();
			end

			def self.on(evtName, &block)
				raise ArgumentError, "Block needs to be given!" unless block_given?
				raise ArgumentError, "Event needs to be a symbol!" unless evtName.is_a? Symbol;

				@@globalCBList[evtName] ||= Array.new();
				@@globalCBList[evtName] << block;
			end

			def on(evtName, &block)
				raise ArgumentError, "Block needs to be given!" unless block_given?
				raise ArgumentError, "Event needs to be a symbol!" unless evtName.is_a? Symbol;

				@localCBList[evtName] ||= Array.new();
				@localCBList[evtName] << block;
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

			def onHookin(game)
				@game = game;
			end
			def onHookout()
			end

			def processHit(hitPlayer, sourcePLayer)
				return true;
			end
		end
	end
end
