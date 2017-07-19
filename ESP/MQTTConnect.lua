playerID = "Xasin";
clientID = "Lasertag_"..playerID

homeQTT = mqtt.Client(clientID, 10, "Internal", "internal", 0);

homeQTT_connected = false;

homeQTT:on("offline",
function(client)
	homeQTT_connected = false;
	if(wifi.sta.getip()) then
		tmr.create():alarm(3000, tmr.ALARM_SINGLE, mqtt_Connect);
	end
end
);


function mqtt_Connect()
	if(homeQTT_connected) then
		return;
	end
	homeQTT:connect("xasin.hopto.org",
		function(client)
			homeQTT_connected = true
		end,
		function(client, reason)
			homeQTT_connected = false;
			if(wifi.sta.getip()) then
				tmr.create():alarm(3000, tmr.ALARM_SINGLE, mqtt_Connect);
			end
		end
	);
end

wifi.eventmon.register(wifi.eventmon.STA_GOT_IP,
	function(t)
		mqtt_Connect();
	end
);

mqtt_Connect();

dofile("MQTTTools.lua");
