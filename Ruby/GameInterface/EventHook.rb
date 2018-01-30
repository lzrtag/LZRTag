

module Lasertag
	class EventHook
		def initialize()
		end

		def _simulate_registration()
			@game.each do |h|
				self.onPlayerRegistration(h);
			end
		end
		def _simulate_connect()
			@game.each_connected do |h|
				self.onPlayerConnect(h);
			end
		end
		def _simulate_disconnect()
			@game.each_connected do |h|
				self.onPlayerDisconnect(h);
			end
		end
		def _simulate_unregistration()
			@game.each do |h|
				self.onPlayerUnregistration(h);
			end
		end

		def onHookin(game)
			@game = game;

			_simulate_registration();
			_simulate_connect();
		end
		def onHookout()
			_simulate_disconnect();
			_simulate_unregistration();
		end

		def onPlayerRegistration(player)
		end
		def onPlayerUnregistration(player)
		end

		def onPlayerConnect(player)
		end
		def onPlayerDisconnect(player)
		end

		def processHit(hitPlayer, sourcePlayer, arbCode)
			return true;
		end

		def onGameTick(dT)
		end

		def onHit(hitPlayer, sourcePlayer)
		end
		def onKill(killedPlayer, sourcePlayer)
		end
	end

	class VerbooseDebugHook < EventHook

		def onPlayerRegistration(player)
			puts "#{player.name} was registered!"
		end
		def onPlayerUnregistration(player)
			puts "#{player.name} was unregistered!"
		end

		def onPlayerConnect(player)
			puts "#{player.name} connected!"
		end
		def onPlayerDisconnect(player)
			puts "#{player.name} disconnected!"
		end

		def processHit(hitP, sourceP, arbCode)
			puts "Confirming hit #{sourceP.name} --> #{hitP.name}! (Code: #{arbCode})";
			return true;
		end

		def onHit(hitP, sourceP)
			puts "Confirmed hit #{sourceP.name} --> #{hitP.name}!"
		end
		def onKill(killedP, sourceP)
			puts "#{sourceP.name} has killed #{killedP.name}!"
		end
	end
end
