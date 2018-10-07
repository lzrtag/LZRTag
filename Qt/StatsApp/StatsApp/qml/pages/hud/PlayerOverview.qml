
import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

import QtQuick.Controls.Material 2.3

import xasin.lasertag.gamehandle 1.0

import "../../widgets"

Item {
	id: rootItem
	property var player: currentPlayer;

	Drawer {
		width: rootItem.width * 0.66
		height: rootItem.height - topBar.height
		y: topBar.height

		Overlay.modal: Rectangle {
			color: "#A0000000"
		}
	}

	ColumnLayout {
		anchors.fill: parent;

		ToolBar {
			id: topBar

			Layout.fillWidth: true

			RowLayout {
				anchors.fill: parent;

				PlayerIcon {
					player: rootItem.player;

					border.color: "transparent"

					Layout.preferredHeight: 50;
					Layout.preferredWidth: 50;

					Layout.leftMargin: 10
				}

				Label {
					Layout.fillHeight: true;
					Layout.fillWidth:  true

					text: player.name;

					fontSizeMode:   Text.Fit
					horizontalAlignment: Text.AlignHCenter

					font.pixelSize: 50
					font.family: "Impact"
				}

				ToolButton {
					icon.source: "qrc:/pics/octicons/gear.svg"
					icon.width: 25
					icon.height: 28
					onClicked: mainViewStack.push("qrc:/qml/pages/settings/SettingsPage.qml");
				}
			}
		}

		Pane {
			Layout.fillWidth: true

			Material.elevation: 5;
			Material.background: Material.color(Material.Red);

			padding: 2;

			Label {
				clip: true

				anchors.fill: parent;

				text: "Reconnecting..."
				fontSizeMode: Text.Fit
				font.pixelSize: 300
				font.family: "Impact"

				horizontalAlignment: Text.AlignHCenter
			}

			Layout.preferredHeight: (GameHandle.game.connected && player.status === "OK") ? 0 : 25
			Behavior on Layout.preferredHeight {
				NumberAnimation {duration: 500}
			}
		}

		StackView {
			id: playerHUDStack
			initialItem: "qrc:/qml/pages/hud/views/hud.qml"

			Layout.fillHeight: true
			Layout.fillWidth:  true
		}
	}
}
