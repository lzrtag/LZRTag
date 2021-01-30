
require 'xasin_logger'

module LZRTag
	module Hook
=begin
	Base class for all game hooks, implements DSL.
	This class shall be used as a base class for any DIY game hooks.
	The purpose of any hook is to implement a specific element of a game,
	such as damaging players, regenerating them, handing out teams, etc.

	This hook base class implements a DSL that makes it very easy for the application
	to implement their own behavior in a modular fashion.

	@example
		class MyHook < LZRTag::Base::Hook
			# Hooks can register which parameters are configurable, and how
			# This will be used to reconfigure them on the fly, but it is not mandatory
			# to register options.
			describe_option :myValue, "A description of my Value";

			def initialize(handler, **options)
				super(handler);

				@myValue = options[:myValue] || "default";
			end

			# The "on" DSL makes it easy to perform tasks on
			# any arbitrary event
			on :playerKilled do |player|
				puts "Player #{player.name} was killed :C";
			end
		end

		class MyGame < LZRTag::Game::Base
			# A game can register that it wants to use this hook, and
			# even which options to use for it.
			hook :aHook, MyHook, {myValue: "Not a default!"};
		end

		# Alternatively, the hook can be added to the game directly
		handler.add_hook(MyHook);
=end
		class Base
			include XasLogger::Mix

			def self.getCBs()
				@globalCBList ||= Hash.new();
				return @globalCBList;
			end
			def self.getOptionDescriptions()
				@globalOptionDescriptions ||= Hash.new();
				return @globalOptionDescriptions
			end

			def initialize(handler, logName = self)
				@localCBList = Hash.new();

				@handler = handler

				init_x_log(logName.to_s, nil);
			end

			# DSL function to describe an option of this hook.
			# The application can use this DSL to describe a given option,
			# identified by optionSymbol. The extraDetails hash is optional,
			# but in the future will allow this hook to be reconfigured remotely
			# via MQTT!
			# @param optionSymbol [Symbol] The Symbol used for this option
			# @param descString [String] String description of this option.
			# @param extraDetails [Hash] Optional hash to provide further details
			#    of this option, such as "min", "max", "type", etc.
			def self.describe_option(optionSymbol, descString, extraDetails = {})
				raise ArgumentError, "Option shall be a symbol!" unless optionSymbol.is_a? Symbol
				raise ArgumentError, "Description should be a string!" unless descString.is_a? String
				getOptionDescriptions()[optionSymbol] = extraDetails;
				getOptionDescriptions()[optionSymbol][:desc] = descString;
			end

			# DSL function to add a block on an event.
			# The application can provide a block to this function that will be executed
			# for every "evtName" game event
			# @param evtName [Symbol] Event name to trigger this block on
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

			# Function to add hooks to the already instantiated hook.
			# This function works exactly like {self.on}, except that it
			# acts on an instance of hook, and allows the app to extend a standard
			# hook by extending it afterwards.
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

			# @private
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
