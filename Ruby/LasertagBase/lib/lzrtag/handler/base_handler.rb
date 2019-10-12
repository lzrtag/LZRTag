
require 'mqtt/sub_handler'

require_relative '../hooks/base_hook.rb'

require_relative '../player/life_player.rb'

module LZRTag
	module Handler
		class Base
			attr_reader :mqtt

			attr_reader :idTable

			def initialize(mqtt, playerClass = Player::Life, clean_on_exit: true)
				@mqtt = mqtt;

				@playerClass = playerClass;

				@players = Hash.new();
				@idTable = Hash.new();

				@playerSynchMutex = Mutex.new();

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
				end
				@eventThread.abort_on_exception = true;

				@mqtt.subscribe_to "Lasertag/Players/#" do |data, topic|
					dID = topic[0];
					if(not @players.key? dID)
						@playerSynchMutex.synchronize {
							@players[dID] = @playerClass.new(dID, self);
						}
						send_event(:playerRegistered, @players[dID]);
					end

					@players[dID].on_mqtt_data(data, topic);
				end

				if(clean_on_exit)
					at_exit {
						@playerSynchMutex.synchronize {
							@players.each do |id, player|
								player.clear_all_topics();
								sleep 0.1;
							end
						}
						sleep 0.5;
					}
				end

				puts "I LZR::Handler init finished".green
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

				return [evtName, callback];
			end
			def remove_event_callback(cb)
				@evtCallbacks[cb[0]].delete cb[1];
			end
			def add_hook(hook)
				hook = hook.new(self) if hook.is_a? Class and hook <= LZRTag::Hook::Base;

				unless(hook.is_a? LZRTag::Hook::Base)
					raise ArgumentError, "Hook needs to be a Lasertag::EventHook!"
				end

				return if(@hooks.include? hook);
				hook.on_hookin(self);
				@hooks << hook;

				return hook;
			end
			def remove_hook(hook)
				unless(hook.is_a? Lasertag::EventHook)
					raise ArgumentError, "Hook needs to be a Lasertag::EventHook!"
				end

				return unless @hooks.include? hook
				hook.on_hookout();
				@hooks.delete(hook);
			end

			def [](c)
				return @players[c] if c.is_a? String
				return @idTable[c] if c.is_a? Integer

				raise ArgumentError, "Unknown identifier for the player id!"
			end
			alias get_player []

			def each(connected: false)
				@playerSynchMutex.synchronize {
					@players.each do |_, player|
						yield(player) if(player.connected? | !connected);
					end
				}
			end

			def num_connected()
				n = 0;
				self.each_connected do
					n += 1;
				end
				return n;
			end
		end
	end
end
