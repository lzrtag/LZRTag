
reviveTimer = tmr.create();

function canShoot()
	if(player.dead) then
		return false
	end
end

function displayHit()
	vibrate(hitConf.hitVibration);
	overrideVest(hitConf.hitFlashDuration, hitConf.hitFlashBrightness);
end

function revivePlayer()
	player.dead = false;
	setVestBrightness(game.brightness);
	homeQTT:publish(playerTopic .. "/Dead", "", 0, 1);
end
function killPlayer()
	if(player.dead) then
		return;
	end

	player.dead = true;

	vibrate(hitConf.deathVibration);
	overrideVest(hitConf.deathFlashDuration, hitConf.deathFlashBrightness);
	setVestBrightness(hitConf.deathBrightness);

	if(hitConf.deathDuration) then
		reviveTimer:alarm(hitConf.deathDuration, tmr.ALARM_SINGLE, revivePlayer);
	end
end

subscribeTo(playerTopic .. "/Dead", 0,
	function(data)
		data = (data == "true");

		if((data) and (not player.dead)) then
			killPlayer();
		elseif(not(data) and (player.dead)) then
			reviveTimer:unregister();
			revivePlayer();
		end
	end);

registerUARTCommand(0, 1,
	function(data)
		if((data == 0) == invertButton) then
			eP = '{"type":"button","player":"' .. playerID .. '"}';

			homeQTT:publish(lasertagTopic .. "/Game/Events", eP, 0, 0);
		end
	end);

-- Handling of AVR-Detected shots
registerUARTCommand(1, 2,
	function(data)
		-- sec, usec, rate = rtctime.get()

		eP = '{"type":"hit","shooterID":' .. data:byte(1)
		eP = eP .. ',"target":"' .. playerID .. '","arbCode":' .. data:byte(2)
		--eP = eP .. ',"time":{"sec":' .. sec .. ',"msec":' .. usec/1000 .. '}}'
		eP = eP .. '}';

		homeQTT:publish(lasertagTopic .. "/Game/Events", eP, 0, 0);

		if(hitConf.dieOnHit) then
			killPlayer();
			homeQTT:publish(playerTopic .. "/Dead", "true", 0, 1);
		elseif(hitConf.flashOnHit) then
			displayHit();
		end
	end);
