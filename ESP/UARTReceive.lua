
UART_COMMANDS = {};
currentUARTCMD = nil;

function callUARTCallback(data)
	currentUARTCMD[2](data);
	uart.on("data", 1, processUARTCommand, 0);
end

function processUARTCommand(data)
	currentUARTCMD = UART_COMMANDS[data:byte()];
	if(currentUARTCMD ~= nil) then
		if(currentUARTCMD[1] == 0 and currentUARTCMD[2] ~= nil) then
		  currentUARTCMD[2](nil);
		else
			uart.on("data", currentUARTCMD[1], callUARTCallback, 0);
		end
	end
end

function registerUARTCommand(c, len, cb)
	UART_COMMANDS[c] = {
		len,
		cb
	};
end

uart.on("data", 1, processUARTCommand, 0);
