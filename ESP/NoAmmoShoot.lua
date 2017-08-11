

triggerPressed = false;
shotsRunning	= false;

shotTimer = tmr.create();

lastHitTimestamp = 0;

function canShoot()
	tSinceHit = tmr.now() - lastHitTimestamp;
	if(tSinceHit < 0) then
		tSinceHit = tSinceHit + 2147483647;
	end

	if(tSinceHit < 3000000) then
		return false;
	end

	return true;
end

function shootIfValid()
	if(not triggerPressed or not canShoot()) then
		shotTimer:stop();
		shotsRunning = false;
	else
		fireWeapon();
	end
end

registerUARTCommand(0, 1,
	function(data)
		triggerPressed = (data:byte() ~= 0);
		if(triggerPressed and not shotsRunning and canShoot()) then
			shootIfValid();
			shotTimer:start();
			shotsRunning = true;
		end
	end
);

shotTimer:register(250, tmr.ALARM_AUTO, shootIfValid);
registerUARTCommand(1, 2,
	function(data)
		lastHitTimestamp = tmr.now();

		eventData = {
			type		= "hit",
			shooter 	= playerIDList[data:byte(1)],
			target  	= playerID,
			arbCode 	= data:byte(2)
		};
		homeQTT:publish(lasertagTopic .. "/Game/Events", sjson.encode(eventData), 0, 0);
	end
);
