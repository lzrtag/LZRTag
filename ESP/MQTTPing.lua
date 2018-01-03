
pingTopic = playerTopic .. "/Connection"
sysInfoTopic = playerTopic .. "/System"

currentPing = 1000000;

pubString = "";

subscribeTo(pingTopic .. "/PingOut", 0,
	function(data)
		currentPing = tmr.now() - tonumber(data);

		pubString = '{"heap":' .. node.heap();
		pubString = pubString .. ',"battery":' .. adc.readvdd33(0);
		pubString = pubString .. ',"ping":' .. currentPing .. '}';

		homeQTT:publish(sysInfoTopic, pubString, 0, 0);
	end
);

tmr.create():alarm(5000, tmr.ALARM_AUTO,
	function()
		if(homeQTT_connected) then
			homeQTT:publish(pingTopic .. "/PingOut", tmr.now(), 0, 0);
		end
	end
);
