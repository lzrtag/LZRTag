clientID = "Lasertag_"..playerID

serverURL = "iot.eclipse.org"

homeQTT = mqtt.Client(clientID, 5);
homeQTT:lwt(playerTopic .. "/Connection", "", 1, 1);

--------------
--- SUB HANDLER FUNCTIONS
--------------

mqttSubTimer = tmr.new();
mqttSubQueue = {};
mqttSubList  = {};

function mqttSoftSubscribe(topic)
	table.insert(mqttSubQueue, topic);
	running, state = mqttSubTimer:state();
	if(not running) then
		mqttSubTimer:start();
	end
end

function mqtt_raw_slow_subscribe()
	t = table.remove(mqttSubQueue);
	homeQTT:subscribe(t, mqttSubList[t].q);
	if(#mqttSubQueue > 0) then
		mqttSubTimer:start(); end;
end

function subscribeTo(topic, qos, callback)
	mqttSubQueue[topic] = {
		q=qos,
		c=callback
	};
	mqttSoftSubscribe(topic);
end

function resubToAll()
	for k,v in pairs(mqttSubList) do
		table.insert(mqttSubQueue, k);
	end

	running, state = mqttSubTimer:state();
	if(not running) then
		mqttSubTimer:start();
	end
end

---------------
--- RECONNECTING FUNCTIONS
---------------

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
			resubToAll();

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

function onMQTTConnect(cbFunc)
	if(homeQTT_connected) then
		cbFunc();
	else
		homeQTT_FirstConnect = cbFunc;
	end
end

-------------
--- EVENT REGISTRATION
-------------

wifi.eventmon.register(wifi.eventmon.STA_GOT_IP,
	function(t)
		mqtt_Connect();
	end
);
if(wifi.sta.getip()) then
	mqtt_Connect();
end

homeQTT::on("message",
	function(client, topic, data)
		mqttSubList[topic].c(data);
	end);
