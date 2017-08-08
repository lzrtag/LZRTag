
dofile("PlayerData.lua");

playerTopic = "Lasertag/Players/"..playerID..""

dofile("MQTTConnect.lua");

StartSymbol = string.byte("!");

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

					uart.write( 0, 10, 10,
									11, 10, 7, 7,
									101, 2);

					onMQTTConnect(
						function()
							dofile("Lasertag.lua");
							dofile("ConsoleRedir.lua");
							dofile("MQTTPing.lua");
						end
					);
				end
			end
		, 0);

		uart.write(0, StartSymbol);
	end
);
