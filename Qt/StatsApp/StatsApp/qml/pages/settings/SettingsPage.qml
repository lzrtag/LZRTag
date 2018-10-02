
import QtQuick 2.11

import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

import xasin.lasertag.gamehandle 1.0

Item {
	id: item1
	ColumnLayout {
		id: columnLayout
		anchors.fill: parent

		spacing: 1

		ToolBar {
			id: topBar

			Layout.fillWidth: true
			Layout.alignment: Qt.AlignTop

			RowLayout {
				anchors.fill: parent;

				ToolButton {
					icon.source: "qrc:/pics/octicons/chevron-left.svg"
					icon.width: 16
					icon.height: 23
					onClicked: mainViewStack.pop();
				}

				Label {
					text: "Info & Settings"
					font.family: "Impact"
					horizontalAlignment: Text.AlignHCenter
					fontSizeMode: Text.Fit
					font.pixelSize: 25

					Layout.fillWidth: true
				}
			}
		}

		SettingsHeading {
			text: "Player Status"
		}
		SettingsOutput {
			name: "Connection:"
			value: GameHandle.currentPlayer.status
		}
		SettingsOutput {
			name: "Battery:"
			value: qsTr("%1V").arg(GameHandle.currentPlayer.battery.toFixed(2));

			highlighted: GameHandle.currentPlayer.battery < 3.65;
			disabled: GameHandle.currentPlayer.status !== "OK";
		}
		SettingsOutput {
			name: "Ping:"
			value: qsTr("%1ms").arg(GameHandle.currentPlayer.ping.toFixed(2));
			disabled: GameHandle.currentPlayer.status !== "OK";
		}


		SettingsHeading {
			text: "Connection Settings"
		}
		SettingsInputText {
			name: "Hostname"
			text: GameHandle.hostname

			onAccepted: GameHandle.hostname = text;
		}
		RowLayout {
			Layout.fillWidth: true

			Label {
				text: "Device ID"
				Layout.minimumWidth: 120
				leftPadding: 10
			}

			ComboBox {
				id: playerDIDBox
				editable: true;
				editText: GameHandle.playerDID;

				model: GameHandle.game.playerIDs
				onModelChanged: {
					var i = model.findIndex(function(s) { return s === GameHandle.playerDID; });
					if(i >= 0)
						playerDIDBox.currentIndex = i
					else
						playerDIDBox.editText = GameHandle.playerDID;
				}

				Layout.fillWidth: true

				onAccepted:  GameHandle.playerDID = editText;
				onActivated: GameHandle.playerDID = currentText;
			}
		}


		SettingsHeading {
			text: "Personalisation"
		}
		SettingsInputText {
			name: "Name"
			text: GameHandle.settings.playerName

			onAccepted: GameHandle.settings.playerName = text;
		}
		SettingsInputText {
			name: "Icon URL"
			text: GameHandle.settings.playerIconURL

			onAccepted: GameHandle.settings.playerIconURL = text;
		}

		Item {
			Layout.fillWidth: true
			Layout.fillHeight: true
		}

	}
}

/*##^## Designer {
	 D{i:0;autoSize:true;height:480;width:640}D{i:7;anchors_height:100;anchors_width:100}
}
 ##^##*/
