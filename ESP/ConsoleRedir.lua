

subscribeTo(playerTopic .. "/Console/In", 0,
	function(tList, data)
		node.input(data);
	end
);

targetFilename = nil;
inStreamFile = nil;
currentBlock = 0;
subscribeTo(playerTopic .. "/Console/FileWrite", 2,
	function(tList, data)
		cmd = sjson.decode(data);

		if(cmd.targetFile) then
			targetFilename = cmd.targetFile;
			file.remove(targetFilename .. ".BKUP");
			inStreamFile = file.open(targetFilename .. ".BKUP", "w+");
			currentBlock = 1;
		end

		if(currentBlock == cmd.block) then
			inStreamFile:write(encoder.fromBase64(cmd.data));
			currentBlock = currentBlock+1;

			if(cmd.eof) then
				inStreamFile:close();
				file.remove(targetFilename);
				file.rename(targetFilename .. ".BKUP", targetFilename);
			end
		end
	end
);

node.output(
	function(str)
		if(not (str == "\n" or str == ">\n")) then
			homeQTT:publish(playerTopic .. "/Console/Out", str, 0, 0);
		end
	end,
	0);
