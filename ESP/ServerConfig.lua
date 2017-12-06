
game = {
	brightness 	= 3,
	team			= 1,
}

player = {
	dead = false,
	ammo = 0,
	lastShot = 0,
	shotCooldown = false,
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
hitConf = hitConfDefaults;

fireConfDefaults = {
	ammoCap 			= 5,
	perReloadAmmo 	= 5,

	reloadDelay = 3000,

	perShotDelay	= 333,

	flashBrightness = 10,
}
fireConf = fireConfDefaults;

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

hitConfOptions = {
	metatable = {
		__index = hitConfDefaults}};
subscribeTo(playerTopic .. "/HitConf", 1,
	function(data)
		if(data == "") then
			hitConf = hitConfDefaults
		else
			hitConf = sjson.decode(data, hitConfOptions);
		end
	end);

fireConfOptions = {
	metatable = {
		__index = fireConfDefaults}}
subscribeTo(playerTopic .. "/FireConf", 1,
	function(data)
		if(data == "") then
			fireConf = fireConfDefaults
		else
			fireConf = sjson.decode(data, fireConfOptions);
		end
	end);

subscribeTo(playerTopic .. "/AmmoSet", 1,
	function(data)
		data = tonumber(data)
		if(data) then
			player.ammo = data;
		end
	end);
