

require_relative 'handler/hitArb_handler.rb'

class DebugHook < LZRTag::Hook::Base
	def initialize()
		super();
	end

	def processRawHit(*args)
		return true;
	end

	def consume_event(evtName, data)
		puts "Caught event: #{evtName} with data: #{data}";

		super(evtName, data);
	end
end

DebugHook.on :playerDisconnected do |player|
	puts "Yay, player #{player.DeviceID} disconnected!"
end

$mqtt = MQTT::SubHandler.new("192.168.251.1");
$handler = LZRTag::Handler::HitArb.new($mqtt);

$handler.add_hook(DebugHook);

sleep 1;

$handler["White"].fire

sleep 1;
