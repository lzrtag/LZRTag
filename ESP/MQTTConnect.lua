clientID = "Lasertag_"..playerID

serverURL = "iot.eclipse.org"

homeQTT = mqtt.Client(clientID, 120);

homeQTT_connected = false;

homeQTT_FirstConnect = nil;

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
	homeQTT:connect(serverURL,
		function(client)
			homeQTT_connected = true;
			if(homeQTT_FirstConnect) then
				homeQTT_FirstConnect();
				homeQTT_FirstConnect = nil;
			end
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

if(wifi.sta.getip()) then
	mqtt_Connect();
end

function onMQTTConnect(cbFunc)
	if(homeQTT_connected) then
		cbFunc();
	else
		homeQTT_FirstConnect = cbFunc;
	end
end

dofile("MQTTTools.lua");
