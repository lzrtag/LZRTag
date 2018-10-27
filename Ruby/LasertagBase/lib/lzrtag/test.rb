

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
DebugHook.on :playerHurt do |player|
	player.hit();
end
DebugHook.on :playerFullyRegenerated do |player|
	player.dead = false;
end

DebugHook.on [:playerRegenerated, :playerHurt] do |player|
	player.heartbeat = (player.life < 30);
end

$mqtt = MQTT::SubHandler.new("192.168.251.1");
$handler = LZRTag::Handler::HitArb.new($mqtt);

$handler.add_hook(DebugHook);

sleep 1;

w = $handler["White"];
w.team = 5;
w.brightness = 7;

until(w.dead)
	$handler["White"].damage_by(10);
	sleep 1;
end

while(w.dead)
	w.regenerate(20);
	sleep 1;
end

sleep 3;
