

module Lasertag
	class EventHook
		def initialize() end

		def onHookin(game)
		end

		def onPlayerRegistration(player)
		end
		def onPlayerUnregistration(player)
		end

		def onPlayerConnect(player)
		end
		def onPlayerDisconnect(player)
		end

		def processHit(hitPlayer, sourcePlayer, shotID)
			return true;
		end

		def onHit(hitPlayer, sourcePlayer)
		end
		def onKill(killedPlayer, sourcePlayer)
		end
	end
end
