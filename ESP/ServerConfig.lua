
remoteConfMT = {
	__index = loadConf
}

function loadConf(table, key)
	return table.defaults[key];
end

function setupConf(table, defTable)
	table.defaults = defTable;
	setmetatable(table, remoteConfMT);
end

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
setupConf(hitConf, hitConfDefaults);

subscribeTo(playerTopic .. "/HitConf", 1,
	function(data)
		fireConf = sjson.decode(data);
		setupConf(hitConf, hitConfDefaults);
	end);
