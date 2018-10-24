


module Lasertag
	class EventHook
		def initialize()
		end

		def on_hookin()
		end

		def processRawHit(*args)
			return true;
		end

		def consume_event(evtName, data)
			puts "Caught event: #{evtName} with data: #{data}";
		end
	end
end

require_relative 'handler/hitArb_handler.rb'

$mqtt = MQTT::SubHandler.new("192.168.251.1");
$handler = LZRTag::Handler::HitArb.new($mqtt);

$handler.add_hook(Lasertag::EventHook);

sleep 1;

$handler["White"].fire

sleep 1;
