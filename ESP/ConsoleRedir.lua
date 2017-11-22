

subscribeTo(playerTopic .. "/Console/In", 0,
	function(data)
		node.input(data);
	end
);

targetFilename = nil;
inStreamFile = nil;
currentBlock = -1;
subscribeTo(playerTopic .. "/Console/FileWrite", 2,
	function(data)
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
				print("File-Write successful!");
			end
		else
			currentBlock = -1;
			print("File-Write failed :c");
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
