

fireConfServer = {};
fireConfDefaults = {
	hitFlashBrightness 	= 10,
	hitFlashDuration   	= 500,
	hitVibration			= 500,

	deathFlashBrightness = 10,
	deathFlashDuration	= 1500,
	deathBrightness		= 1,
	deathVibration			= 1500,
}

playerDead = false;
reviveTimer = tmr.create();

subscribeTo(playerTopic .. "/FireConf", 1,
	function(data)
		fireConfServer = sjson.decode(data);
	end);

function fireConf(key)
	if(fireConf[key] ~= nil) then
		return fireConf[key];
	end
	return fireConfDefaults[key];
end

function canShoot()
	if(playerDead) then
		return false
	end
end

function displayHit()
	vibrate(fireConf("hitVibrateDuration"));
	overrideVest(fireConf("hitFlashDuration", fireConf("hitFlashBrightness")));
end

function revivePlayer()
	playerDead = false;
	setVestBrightness(game.brightness);
end
function killPlayer()
	if(playerDead) then
		return;
	end

	playerDead = true;

	vibrate(fireConf("deathVibration"));
	overrideVest(fireConf("deathFlashDuration"), fireConf("deathFlashBrightness"));
	setVestBrightness(fireConf("deathBrightness"));

	if(fireConf("deathDuration")) then
		reviveTimer:alarm(fireConf("deathDuration"), tmr.ALARM_SINGLE, revivePlayer);
	end
end

-- Handling of AVR-Detected shots
registerUARTCommand(1, 2,
	function(data)
		sec, usec, rate = rtctime.get();

		eP = '{"type":"hit","shooterID":' .. data:byte(1)
		eP = eP .. ',"target":"' .. playerID .. '","arbCode":' .. data:byte(2)
		eP = eP .. ',"time":{"sec":' .. sec .. ',"msec":' .. usec/1000 .. '}}'

		homeQTT:publish(lasertagTopic .. "/Game/Events", eP, 0, 0);

		if(fireConf("dieOnHit")) then
			killPlayer();
		elseif(fireConf("flashOnHit")) then
			displayHit();
		end
	end);
