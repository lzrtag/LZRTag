
require 'mqtt/sub_handler'

require 'xasin_logger'

require_relative '../hooks/base_hook.rb'

require_relative '../player/life_player.rb'

module LZRTag
	module Handler
		# The base handler class.
		# This class code deals with the most rudimentary systems:
		# - It handles the MQTT connection
		# - It registers new players and distributes MQTT data to the respective class
		# - It hands out Player IDs to connected players
		# - It runs the event loop system and manages hooks
		#
		# In it's simplest form it can be instantiated with
		# just a MQTT handler:
		# @example
		#  # Using LZRTag.Handler instead of LZRTag::Handler::Base to fetch the latest handler
		#  handler = LZRTag.Handler.new(mqttConn);
		class Base
			include XasLogger::Mix

			# Returns the MQTT connection
			attr_reader :mqtt

			# Returns the ID-Table, a Hash of Players and their matched IDs
			attr_reader :idTable

			def initialize(mqtt, playerClass = Player::Life, clean_on_exit: true)
				init_x_log("LZRTag Base", nil);

				@mqtt = mqtt;

				@playerClass = playerClass;

				@players = Hash.new();
				@idTable = Hash.new();

				@playerSynchMutex = Mutex.new();

				@hooks = [self];
				@eventQueue = Queue.new();

				@eventThread = Thread.new do
					loop do
						nextData = @eventQueue.pop;
						@hooks.each do |h|
							h.consume_event(nextData[0], nextData[1]);
						end
					end
				end
				@eventThread.abort_on_exception = true;
				Thread.new do
					loop do
						sleep 0.5;
						send_event(:slowTick);
					end
				end

				@mqtt.subscribe_to "/esp32/lzrtag/+/#" do |data, topic|
					dID = topic[0];
					if(not @players.key? dID)
						@playerSynchMutex.synchronize {
							@players[dID] = @playerClass.new(dID, self);
						}
						send_event(:playerRegistered, @players[dID]);
						x_logi("New player registered: #{dID}");
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

				x_logi("Initialisation complete");
			end

			# Send an event into the event loop.
			# Any events will be queued and will be executed in-order by a separate
			# thread. The provided data will be passed along to the hooks
			# @param evtName [Symbol] Name of the event
			# @param *data Any additional data to send along with the event
			def send_event(evtName, *data)
				raise ArgumentError, "Event needs to be a symbol!" unless evtName.is_a? Symbol;
				@eventQueue << [evtName, data];
			end

			# @private
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
				when :slowTick
					self.each do |pl|
						pl._tick_connection();
					end
				end
			end

			# Add or instantiate a new hook.
			# This function will take either a Class of Hook::Base or an
			# instance of it, and add it to the current list of hooks, thusly
			# including it in the event processing
			# @param hook [LZRTag::Hook::Base] The hook to instantiate and add
			# @return The added hook
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
			# Remove an existing hook from the system.
			# This will remove the provided hook instance from the event handling
			# @param hook [LZRTag::Hook::Base] The hook to remove
			def remove_hook(hook)
				unless(hook.is_a? Lasertag::EventHook)
					raise ArgumentError, "Hook needs to be a Lasertag::EventHook!"
				end

				return unless @hooks.include? hook
				hook.on_hookout();
				@hooks.delete(hook);
			end

			# Return a player either by their ID or their DeviceID
			# @return LZRTag::Player::Base
			def [](c)
				return @players[c] if c.is_a? String
				return @idTable[c] if c.is_a? Integer

				raise ArgumentError, "Unknown identifier for the player id!"
			end
			alias get_player []

			# Run the provided block on each registered player.
			# @param connected Only yield for connected players
			# @yield [player] Yields for every played. With connected = true,
			#   only yields connected players
			def each(connected: false)
				@playerSynchMutex.synchronize {
					@players.each do |_, player|
						yield(player) if(player.connected? | !connected);
					end
				}
			end

			# Returns the number of currently connected players
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
