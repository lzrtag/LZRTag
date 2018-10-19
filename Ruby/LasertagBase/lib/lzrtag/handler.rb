
require 'mqtt/sub_handler'

puts("TODO - Remove base player call");
require_relative 'player/base_player.rb'

module LZRTag
	class Handler
		attr_reader :mqtt

		def initialize(mqtt)
			@mqtt = mqtt;

			@playerClass = Player::Base;

			@players = Hash.new();
			@idTable = Hash.new();

			@hooks = [self];
			@evtCallbacks = Hash.new();
			@eventQueue = Queue.new();

			@eventThread = Thread.new do
				loop do
					nextData = @eventQueue.pop;
					@hooks.each do |h|
						h.consume_event(nextData[0], nextData[1]);
					end
					if(cbList = @evtCallbacks[nextData[0]])
						cbList.each do |cb|
							cb.call(*nextData[1]);
						end
					end
				end
			end.abort_on_exception = true;

			@mqtt.subscribe_to "Lasertag/Players/#" do |data, topic|

				dID = topic[0];
				if(not @players.key? dID and topic[1] == "Connection")
					@players[dID] = @playerClass.new(dID, self);
					send_event(:playerRegistered, @players[dID]);
				end
				next unless @players[dID];

				@players[dID].on_mqtt_data(data, topic);
			end
		end

		def send_event(evtName, *data)
			raise ArgumentError, "Event needs to be a symbol!" unless evtName.is_a? Symbol;
			@eventQueue << [evtName, data];
		end

		def consume_event(evtName, data)
			case evtName
			when :playerConnected
				player = data[0];
				i = 1;
				while(@idTable[i]) do i+=1; end
				@idTable[i] = player;
				player.id = i;
			when :playerDisconnected
				player = data[0];
				@idTable[player.id] = nil;
				player.id = nil;
			end
		end

		def on(evtName, &callback)
			raise ArgumentError, "Block needs to be given!" unless block_given?
			raise ArgumentError, "Event needs to be a symbol!" unless evtName.is_a? Symbol;

			@evtCallbacks[evtName] ||= Array.new();
			@evtCallbacks[evtName] << callback;
		end
		def add_hook(hook)
			if(hook.is_a? Class)
				hook = hook.new();
			end

			@hooks << hook;

			return hook;
		end
	end
end
