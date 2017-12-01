
game = {
	brightness 	= 3,
	team			= 1,
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

subscribeTo(playerTopic .. "/HitConf", 1,
	function(data)
		hitConf = sjson.decode(data);
		setmetatable(hitConf, {__index = hitConfDefaults});
	end);
