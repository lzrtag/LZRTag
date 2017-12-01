
playerDead = false;
reviveTimer = tmr.create();

function canShoot()
	if(playerDead) then
		return false
	end
end

function displayHit()
	vibrate(hitConf.hitVibrateDuration);
	overrideVest(hitConf.hitFlashDuration, hitConf.hitFlashBrightness);
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

	vibrate(hitConf.deathVibration);
	overrideVest(hitConf.deathFlashDuration, hitConf.deathFlashBrightness);
	setVestBrightness(hitConf.deathBrightness);

	if(hitConf.deathDuration) then
		reviveTimer:alarm(hitConf.deathDuration, tmr.ALARM_SINGLE, revivePlayer);
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

		if(hitConf.dieOnHit) then
			killPlayer();
		elseif(hitConf.flashOnHit) then
			displayHit();
		end
	end);
