
currentUARTCMD = nil;

function callUARTCallback(data)
	if(currentUARTCMD.cb ~= nil) then
		currentUARTCMD.cb(data);
	end

	uart.on("data", 0, processUARTCommand, 1);
end

function processUARTCommand(c)
	currentUARTCMD = UART_COMMANDS[c];
	uart.on("data", 0, callUARTCallback, currentUARTCMD.len);
end

uart.on("data", 0, processUARTCommand ,1);

function setVestBrightness(c)
	uart.write(0, 200, c);
end
function setVestColor(c)
	uart.write(0, 101, c);
end

function ping(sFreq, eFreq, duration)
	uart.write(0, 11, duration/10 or 2, sFreq/60 or 67, eFreq/60 or sFreq/60 or 67);
end

function vibrate(duration)
	uart.write(0, 10, duration/10);
end

subscribeTo(playerTopic .. "/Brightness", 1,
	function(tList, data)
		setVestBrightness(tonumber(data));
	end
);

setVestColor(1);
function fancyPling()
	ping(1000, 5000, 150);
	vibrate(50);
end
fancyPling();
tmr.create():alarm(200, tmr.ALARM_SINGLE, fancyPling);
