
module LZRTag
	module Hook
		class Base
			def self.getCBs()
				@globalCBList ||= Hash.new();
				return @globalCBList;
			end
			def self.getOptionDescriptions()
				@globalOptionDescriptions ||= Hash.new();
				return @globalOptionDescriptions
			end

			def initialize(handler)
				@localCBList = Hash.new();

				@handler = handler
			end

			def self.describe_option(optionSymbol, descString, extraDetails = {})
				raise ArgumentError, "Option shall be a symbol!" unless optionSymbol.is_a? Symbol
				raise ArgumentError, "Description should be a string!" unless descString.is_a? String
				getOptionDescriptions()[optionSymbol] = extraDetails;
				getOptionDescriptions()[optionSymbol][:desc] = descString;
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
						begin
							instance_exec(*data, &cb);
						rescue StandardError => e
							puts e.message
							puts e.backtrace.inspect
						end
					end
				end
				if(cbList = @localCBList[evtName]) then
					cbList.each do |cb|
						begin
							cb.call(*data);
						rescue StandardError => e
							puts e.message
							puts e.backtrace.inspect
						end
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
