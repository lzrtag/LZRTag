

module Lasertag
	class EventHook
		attr_accessor :catch_up_players

		def initialize()
			@catch_up_players = true;
		end

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
