
sublist = {};
subQueue = {};
subTimer = tmr.create();

function compareTopics(receivedTopic, topicPattern)
	if(#topicPattern > #receivedTopic) then
		return false;
	end

	i = 1;
	while(i <= #receivedTopic and i <= #topicPattern) do
		if(topicPattern[i] == "#") then
			return true;
		elseif(topicPattern[i] == "+") then
		elseif(topicPattern[i] ~= receivedTopic[i]) then
			return false;
		end

		i = i+1;
	end

	return #receivedTopic == #topicPattern;
end

function genTopiclist(topicString)
	outputList = {};

	for s in string.gmatch(topicString, "[^/]+") do
		table.insert(outputList, s);
		if(s == "#") then
			return outputList;
		end
	end

	return outputList;
end

function mqtt_slowSub()
	if(#subQueue > 0) then
		t = table.remove(subQueue);
		homeQTT:subscribe(t, sublist[t].qos);
		subTimer:start();
	end
end

function subscribeTo(topic, qos, callFunction)
	sublist[topic] = {
		callback = callFunction,
		qos = qos,
		matchPattern = genTopiclist(topic),
	}

	table.insert(subQueue, topic);
	running, state = subTimer:state();
	if(not running) then
		subTimer:start();
	end
end

function callSubCallback(mqttClient, topic, data)
	topiclist = genTopiclist(topic);
	for k, v in pairs(sublist) do
		if(compareTopics(topiclist, v.matchPattern)) then
			v.callback(topiclist, data);
		end
	end
end

subTimer:register(500, tmr.ALARM_SEMI, mqtt_slowSub);
homeQTT:on("message", callSubCallback);
