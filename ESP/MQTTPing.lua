
pingTopic = playerTopic .. "/Connection"
sysInfoTopic = playerTopic .. "/System"

currentPing = 1000000;

subscribeTo(pingTopic .. "/PingOut", 0,
	function(tList, data)
		currentPing = tmr.now() - tonumber(data);

		homeQTT:publish(sysInfoTopic, sjson.encode(
			{	heap = node.heap(),
				battery = adc.readvdd33(0),
				ping = currentPing,
				sigStrength = wifi.sta.getrssi()}), 0, 0);
	end
);

tmr.create():alarm(5000, tmr.ALARM_AUTO,
	function()
		if(homeQTT_connected) then
			homeQTT:publish(pingTopic .. "/PingOut", tmr.now(), 0, 0);
		end
	end
);
