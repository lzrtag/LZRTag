

subscribeTo(playerTopic .. "/Console/In", 0,
	function(data)
		node.input(data);
	end
);

targetFilename = nil;
inStreamFile 	= nil;
currentBlock	= -1;
lastFileWrite 	= 0;
subscribeTo(playerTopic .. "/Console/FileWrite", 2,
	function(data)
		cmd = sjson.decode(data);

		if(((tmr.now() - lastFileWrite) > 10000000) or (not targetFilename)) then
			if((cmd.block == 0) and (cmd.file)) then
				targetFilename = cmd.file;
				file.remove(targetFilename .. ".BKUP");
				inStreamFile = file.open(targetFilename .. ".BKUP", "w+");

				currentBlock 	= 1;
				lastFileWrite 	= tmr.now();

				homeQTT:publish(playerTopic .. "/Console/FileAnswer",
						"READY: " .. targetFilename, 2, 0);
			end
		elseif((currentBlock == cmd.block) and (targetFilename == cmd.file)) then
			inStreamFile:write(encoder.fromBase64(cmd.data));

			if(cmd.close) then
				inStreamFile:close();

				file.remove(targetFilename);
				file.rename(targetFilename .. ".BKUP", targetFilename);

				if(SAFEMODE) then
					file.remove("BOOT_SAFECHECK");
				end
			end

			homeQTT:publish(playerTopic .. "/Console/FileAnswer",
					"OK: " .. targetFilename .. ", " .. currentBlock, 2, 0);
			lastFileWrite = tmr.now();

			currentBlock = currentBlock+1;

			if(cmd.close) then
				targetFilename = nil;
				inStreamFile 	= nil;
				currentBlock	= 0;
			end
		end
	end
);

node.output(
	function(str)
		if(not ((str == "\n") or (str == ">\n"))) then
			homeQTT:publish(playerTopic .. "/Console/Out", str, 0, 0);
		end
	end,
	0);
