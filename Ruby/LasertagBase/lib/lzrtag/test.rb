
require_relative 'handler.rb'

class DBG
	def initialize()
	end

	def consume_event(evtName, data)
		puts "Caught event: #{evtName} with data: #{data}";
	end
end

$mqtt = MQTT::SubHandler.new("192.168.251.1");
$handler = LZRTag::Handler.new($mqtt);

$handler.add_hook(DBG);
