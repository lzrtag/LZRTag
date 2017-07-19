
subscribeTo("SYS/PING/"..clientID, 0,
	function(tList, data)
		print("Received ping!");
		homeQTT:publish("SYS/PING/"..clientID.. "/REPORT", tmr.now() - data, 1, 0);
	end
);

tmr.create():alarm(3000, tmr.ALARM_AUTO,
	function()
		if(homeQTT_connected) then
			homeQTT:publish("SYS/PING/"..clientID, tmr.now(), 0, 0);
			homeQTT:publish("SYS/PING/"..clientID.."/SIGNAL", wifi.sta.getrssi(), 0, 0);
		end
	end
);
