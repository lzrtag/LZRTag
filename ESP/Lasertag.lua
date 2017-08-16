
dofile("UARTReceive.lua");

playerIDList = {}

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
	function(tList, data)
		setVestBrightness(tonumber(data));
	end
);
subscribeTo(playerTopic .. "/Team", 1,
	function(tList, data)
		setVestColor(tonumber(data));
	end
);

subscribeTo(playersTopic .. "/+/ID", 1,
	function(tList, data)
		if(data ~= nil) then
			num = tonumber(data);
			if(num ~= nil) then
				playerIDList[tonumber(data)] = tList[3];
			end

			if(tList[3] == playerID) then
				playerIDNum = num;
				uart.write(0, 100, playerIDNum);
			end
		end
	end
);
homeQTT:publish(playerTopic .. "/Connection", "OK", 1, 1);

subscribeTo(lasertagTopic .. "/Game/Status", 1,
	function(tList, data)
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

setVestColor(1);
function fancyPling()
	ping(1000, 5000, 150);
	vibrate(50);
end
fancyPling();
tmr.create():alarm(200, tmr.ALARM_SINGLE, fancyPling);

dofile("NoAmmoShoot.lua");
