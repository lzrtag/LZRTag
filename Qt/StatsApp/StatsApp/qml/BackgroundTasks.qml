
import QtQuick 2.11

import QtPositioning 5.8

import xasin.lasertag.gamehandle 1.0

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

		updateInterval: 1000
		active: true

		onPositionChanged: {
			var positionData = {}

			var coord = gpsSource.position.coordinate;
            positionData.latitude  = coord.latitude  || 0;
            positionData.longitude = coord.longitude || 0;

            positionData.speed = gpsSource.position.speed || 0;

            var mapPos = gameMap.latLonToXY(Qt.point(positionData.longitude, positionData.latitude));

            positionData.x = mapPos.x;
            positionData.y = mapPos.y;
			player.position = positionData;

			gameMap.updateZonesForPlayer(player);
				GameHandle.currentZones = player.zones;
			GameHandle.currentPosition = player.getMapPosition();

            console.log("Position of player is:", player.getMapPosition());
            console.log("Zones for player are:", GameHandle.currentZones);
		}
	}
}
