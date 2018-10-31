

require_relative '../lzrtag.rb'

class DebugHook < LZRTag::Hook::Base
	def initialize()
		super();
	end

	def consume_event(evtName, data)
		return if [:gameTick, :playerRegenerated].include? evtName
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
$handler = LZRTag.Handler.new($mqtt);

$handler.add_hook(DebugHook);
$handler.add_hook(LZRTag::Hook::RandomTeam);
$handler.add_hook(LZRTag::Hook::Damager);
$handler.add_hook(LZRTag::Hook::Regenerator);

cfg = LZRTag::Hook::Configurator.new();
cfg.fireConfig = {
	shotLocked: false
}

$handler.add_hook(cfg);

sleep 4

$handler.start_game(LZRTag::Game::Base.new($handler));

$handler.each do |pl|
	pl.fireConfig[:shotLocked] = false;
	pl.ammo = 100;
end

while(true) do
	sleep 0.5;
end
