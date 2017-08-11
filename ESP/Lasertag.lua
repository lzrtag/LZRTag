
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
	uart.write(0, 0, 99);
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
		playerIDList[tonumber(data)] = tList[3];
	end
);
homeQTT:publish(playerTopic .. "/ID", playerIDNum, 1, 1);
uart.write(0, 100, playerIDNum);

setVestColor(1);
function fancyPling()
	ping(1000, 5000, 150);
	vibrate(50);
end
fancyPling();
tmr.create():alarm(200, tmr.ALARM_SINGLE, fancyPling);

dofile("NoAmmoShoot.lua");
