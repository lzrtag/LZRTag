
uart.write(0, 100, 0);

function setNozzle(colorNum)
	uart.write(0, 100, colorNum);
end

function buzzerBlip(frequency)
	uart.write(0, 99, frequency/50);
end

subscribeTo("Lasertag/Players/"..playerID.."/Team", 1,
	function(topic, data)
		setNozzle(tonumber(data));
	end
);
