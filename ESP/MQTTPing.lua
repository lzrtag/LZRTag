
pingTopic = playerTopic .. "/Connection"
sysInfoTopic = playerTopic .. "/System"

currentPing = 1000000;

pubString = "";

subscribeTo(pingTopic .. "/PingOut", 0,
	function(data)
		currentPing = tmr.now() - tonumber(data);
		homeQTT:publish(sysInfoTopic, struct.pack("L<L<L<", adc.readvdd33(0), 0, currentPing/1000), 0, 0);
	end
);

tmr.create():alarm(5000, tmr.ALARM_AUTO,
	function()
		if(homeQTT_connected) then
			homeQTT:publish(pingTopic .. "/PingOut", tmr.now(), 0, 0);
		end
	end
);
