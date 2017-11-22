
dofile("UARTReceive.lua");

function setVestBrightness(c)
	uart.write(0, 200, c);
end

function setVestColor(c)
	uart.write(0, 101, c);
end
function overrideVest(duration, brightness)
	duration = duration/90;
	uart.write(0, 12, duration%255, duration/255, brightness);
end

function ping(sFreq, eFreq, duration)
	uart.write(0, 11, duration/10 or 2, sFreq/60 or 67, eFreq/60 or sFreq/60 or 67);
end

function vibrate(duration)
	uart.write(0, 10, duration/10);
end

function fireWeapon()
	if(playerIDNum) then
		uart.write(0, 0, 99);
	end
end

subscribeTo(playerTopic .. "/Brightness", 1,
	function(data)
		setVestBrightness(tonumber(data));
	end
);
subscribeTo(playerTopic .. "/Team", 1,
	function(data)
		setVestColor(tonumber(data));
	end
);

subscribeTo(lasertagTopic .. "/Game/Status", 1,
	function(data)
		if(data == "stop") then
			if(gameRunning) then
				overrideVest(5000, 10);
				ping(5000, 1000, 2000);
			end
			gameRunning = false;
		elseif(data == "start") then
			if(not gameRunning) then
				ping(2000, 2000, 2000);
			end
			gameRunning = true;
		elseif(data == "startwarn") then
			ping(1500, 1500, 300);
		end
	end
);

subscribeTo(playerTopic .. "/ID", 1,
	function(data)
		playerIDNum = tonumber(data);
		uart.write(0, 100, playerIDNum);
	end
);

tmr.create():alarm(3000, tmr.ALARM_SINGLE, function()
	homeQTT:publish(playerTopic .. "/Connection", "OK", 1, 1);
end);

setVestColor(1);
function fancyPling()
	ping(1000, 5000, 150);
	vibrate(50);
end
fancyPling();
tmr.create():alarm(200, tmr.ALARM_SINGLE, fancyPling);

dofile("NoAmmoShoot.lua");
