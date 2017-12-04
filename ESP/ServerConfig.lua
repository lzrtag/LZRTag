
game = {
	brightness 	= 3,
	team			= 1,
}

player = {
	dead = false,
	ammo = 0,
}

hitConfDefaults = {
	hitFlashBrightness 	= 10,
	hitFlashDuration   	= 500,
	hitVibration			= 500,

	deathFlashBrightness = 10,
	deathFlashDuration	= 1500,
	deathBrightness		= 1,
	deathVibration			= 1500,
}

hitConf = {};

hitConfOptions = {
	metatable = {
		__index = hitConfDefaults}};
subscribeTo(playerTopic .. "/HitConf", 1,
	function(data)
		hitConf = sjson.decode(data, hitConfOptions);
	end);

subscribeTo(playerTopic .. "/ID", 1,
	function(data)
		player.id = tonumber(data);
		if(player.id) then
			uart.write(0, 100, player.id);
		end
	end);

subscribeTo(playerTopic .. "/Team", 1,
	function(data)
		game.team = tonumber(data) or 0;
		setVestColor(game.team);
	end);
subscribeTo(playerTopic .. "/Brightness", 1,
	function(data)
		game.brightness = tonumber(data) or 3;
		if(not(player.dead)) then
			setVestBrightness(game.brightness);
		end
	end);
