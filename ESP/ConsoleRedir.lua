

subscribeTo(playerTopic .. "/Console/In", 2,
	function(tList, data)
		node.input(data);
	end
);


inStreamFile = nil;
subscribeTo(playerTopic .. "/Console/FileWrite", 2,
	function(tList, data)
		cmd = sjson.decode(data);
		if(cmd.targetFile) then
			file.remove(cmd.targetFile);
			inStreamFile = file.open(cmd.targetFile, "w+");
		end

		inStreamFile:write(encoder.fromBase64(cmd.data));

		if(cmd.eof) then
			inStreamFile:close();
		end
	end
);

node.output(
	function(str)
		homeQTT:publish(playerTopic .. "/Console/Out", str, 2, 0);
	end,
	0);
