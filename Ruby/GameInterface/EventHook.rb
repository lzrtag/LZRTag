

module Lasertag
	class EventHook
		def initialize()
		end

		def onHookin(game)
			@game = game;
		end
		def onHookout()
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

		def onGameTick()
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
