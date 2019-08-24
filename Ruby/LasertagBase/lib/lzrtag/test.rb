

require_relative '../lzrtag.rb'

class DebugHook < LZRTag::Hook::Base
	def initialize()
		super();
	end

	def consume_event(evtName, data)
		super(evtName, data);

		return if [:gameTick, :playerRegenerated, :gameStarted, :gameStarting].include? evtName
		puts "Caught event: #{evtName} with data: #{data}";
	end
end

DebugHook.on :playerDisconnected do |player|
	puts "Yay, player #{player.DeviceID} disconnected!"
end
DebugHook.on :playerHurt do |player, fromPlayer|
	player.sound("HIT");
	fromPlayer.sound("MINOR SCORE");
end
DebugHook.on :playerKilled do |targetPlayer, sourcePlayer|
	targetPlayer.sound("DEATH");
	sourcePlayer.sound("KILL SCORE") if(sourcePlayer)
end
DebugHook.on :playerFullyRegenerated do |player|
	player.dead = false;
end

DebugHook.on [:playerRegenerated, :playerHurt] do |player|
	player.heartbeat = (player.life < 30);
end

DebugHook.on :playerEnteredZone do |player, entered|
	if(entered.include? "teamSetter")
		player.team = (player.zoneIDs[:teamSetter][:team]).to_i;
	end
end

$mqtt = MQTT::SubHandler.new("mqtt://localhost");

$handler = LZRTag.Handler.new($mqtt);

$handler.add_hook(DebugHook);
$handler.add_hook(LZRTag::Hook::RandomTeam);
$handler.add_hook(LZRTag::Hook::Damager.new(dmgPerShot: 25));
$handler.add_hook(LZRTag::Hook::Regenerator.new(regRate: 7, regDelay: 3, autoReviveThreshold: 21));

cfg = LZRTag::Hook::Configurator.new();
cfg.fireConfig = {
	shotLocked: false
}

$handler.add_hook(cfg);

sleep 4

$handler.start_game(LZRTag::Game::Base.new($handler));

$handler.each do |pl|
	pl.ammo = 100;
	pl.gunNo = 3;
end

while(true) do
	sleep 0.5;
end
