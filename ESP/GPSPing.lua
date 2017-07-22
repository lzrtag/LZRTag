

subscribeTo("testing/GPS/distance", 0,
	function(topics, data)
		dist = tonumber(data);

		dist = dist + 10;

		buzzerBlip(50000 / dist);
	end
);
