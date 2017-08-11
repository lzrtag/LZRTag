

triggerPressed = false;
shotsRunning	= false;

shotTimer = tmr.create();

function shootIfValid()
	if(not triggerPressed) then
		shotTimer:stop();
		shotsRunning = false;
	else
		fireWeapon();
	end
end

registerUARTCommand(0, 1,
	function(data)
		triggerPressed = (data:byte() ~= 0);
		if(triggerPressed and not shotsRunning) then
			shootIfValid();
			shotTimer:start();
			shotsRunning = true;
		end
	end
);

shotTimer:register(250, tmr.ALARM_AUTO, shootIfValid);
