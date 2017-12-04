
dofile("PlayerData.lua");
playerTopic = lasertagTopic .. "/Players/"..playerID;

dofile("MQTTConnect.lua");

StartSymbol = string.byte("!");

SAFEMODE = file.exists("BOOT_SAFECHECK");
systemIsSetUp = false;

tmr.create():alarm(2000, tmr.ALARM_SINGLE,
	function(t)

		uart.setup(0, 31250, 8, uart.PARITY_NONE, uart.STOPBITS_1, 0);

		noTagTimer = tmr.create();
		noTagTimer:alarm(1000, tmr.ALARM_SINGLE,
			function(tt)
				uart.on("data");
				uart.setup(0, 115200, 8, uart.PARITY_NONE, uart.STOPBITS_1, 1);

				print("\nNo Lasertag attached, returning to normal mode!\n");
			end
		);

		uart.on("data", 0,
			function(data)
				data = string.byte(data);
				if(data == StartSymbol) then
					noTagTimer:unregister();

					uart.write( 0, 10, 10,		-- Vibrate a little
									11, 10, 7, 7,	-- Connect buzz
									101, 2);			-- Set blue team

					uart.on("data", 0, function(d) end, 0);

					onMQTTConnect(
						function()
							dofile("ConsoleRedir.lua");

							if(not(SAFEMODE)) then
								file.open("BOOT_SAFECHECK","w"):close();

								dofile("MQTTPing.lua");
								dofile("Lasertag.lua");
								dofile("ServerConfig.lua")
								dofile("FireHandling.lua");

								tmr.create():alarm(1500, tmr.ALARM_SINGLE,
									function()
										file.remove("BOOT_SAFECHECK");
									end);
							else
								uart.write(	0, 200, 10,	-- Bright blink mode
												101, 0,		-- Red team
												11, 100, 32, 34); -- 2kHz "error" buzz
								homeQTT:publish(playerTopic .. "/Connection", "SAFEMODE", 0, 1);
							end
						end
					);
				end
			end
		, 0);

		uart.write(0, StartSymbol);
	end
);
