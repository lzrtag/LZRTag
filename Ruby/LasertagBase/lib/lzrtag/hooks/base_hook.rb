
module LZRTag
	module Hook
		class Base
			def self.getCBs()
				@globalCBList ||= Hash.new();
				return @globalCBList;
			end

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
					getCBs()[evt] ||= Array.new();
					getCBs()[evt] << block;
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
				if(cbList = self.class.getCBs()[evtName])
					cbList.each do |cb|
						instance_exec(*data, &cb);
					end
				end
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

			def process_raw_hit(*)
				return true;
			end
		end
	end
end
