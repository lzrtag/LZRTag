
import QtQuick 2.11

import QtPositioning 5.8

Item {
	property var player

	Connections {
		target: gameHandler
		onConnectionStatusChanged: {
			ttsEngine.say(gameHandler.connected ? "Server connected" : "Server disconnected");
		}
	}
	Connections {
		target: GameHandle.currentPlayer
		onStatusChanged: {
			var cStatus = GameHandle.currentPlayer.status
			if(cStatus === "OK")
				ttsEngine.say("Device connected");
			else if(cStatus === "Offline")
				ttsEngine.say("Device disconnected")
		}
	}
	Connections {
		target: GameHandle.currentPlayer
		onLifeChanged: function(newLife, oldLife) {
			if(newLife <= 25 && oldLife > 25)
				ttsEngine.say("Life critical!");
		}
	}

	PositionSource {
		id: gpsSource

		updateInterval: 3000
		active: true

		onPositionChanged: {
			var positionData = {}

			var coord = gpsSource.position.coordinate;
			positionData.latitude  = coord.latitude;
			positionData.longitude = coord.longitude;

			positionData.speed = gpsSource.position.speed;

			player.position = positionData;
		}
	}
}
