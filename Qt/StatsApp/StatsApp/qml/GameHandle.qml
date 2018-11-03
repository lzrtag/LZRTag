pragma Singleton
import QtQuick 2.0

import Qt.labs.settings 1.0

Item {
	property alias settings: connectionSettings

	property alias hostname:	connectionSettings.hostname
	property alias playerDID:  connectionSettings.playerDID

	property var game: gameHandler
	property var currentPlayer: gameHandler.getPlayer(playerDID);

    property var currentZones: []

	onHostnameChanged:  game.setHostname(hostname);
	onPlayerDIDChanged: setupPlayer();

	function setupPlayer() {
		currentPlayer.name = ""
		currentPlayer.iconURL = "";

		currentPlayer = game.getPlayer(playerDID);

		currentPlayer.name	 = connectionSettings.playerName;
		currentPlayer.iconURL = connectionSettings.playerIconURL;
	}

	Settings {
		id: connectionSettings

		category: "Connection"
		property string hostname:   "192.168.251.1"
		property string playerDID:  "Red"

		property bool	 authenticationOn: false
		property string username: ""
		property string password: ""

		property string playerName: ""
		property string playerIconURL: ""

		onPlayerNameChanged:    currentPlayer.name = playerName;
		onPlayerIconURLChanged: currentPlayer.iconURL = playerIconURL;

		Component.onCompleted: {
			setupPlayer();

			if(authenticationOn) {
				console.log("Authenticating...");
			}

			game.setHostname(hostname);
		}
	}
}
