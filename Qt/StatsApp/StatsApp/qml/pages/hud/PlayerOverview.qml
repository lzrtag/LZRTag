
import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

import QtGraphicalEffects 1.0

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
			Layout.preferredHeight: 55

			RowLayout {
				anchors.fill: parent;

				PlayerIcon {
					icon: playerHUDStack.currentItem.icon || rootItem.player.iconURL;
					border.color: "transparent"

					Layout.preferredHeight: 50;
					Layout.preferredWidth: 50;

					Layout.leftMargin: 10
				}

				Label {
					Layout.fillHeight: true;
					Layout.fillWidth:  true
					Layout.margins: 3

					text: playerHUDStack.currentItem.title || player.name;

					fontSizeMode:   Text.Fit
					horizontalAlignment: Text.AlignHCenter
					verticalAlignment:   Text.AlignBottom

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
			id: connectionIndicator

			property bool allConnected: (GameHandle.game.connected && player.status === "OK")

			Layout.fillWidth: true
			Layout.leftMargin: 5
			Layout.rightMargin: Layout.leftMargin

			Material.elevation: 5;
			Material.background: Material.color(Material.Red);

			leftPadding: 0;
			rightPadding: 0;
			topPadding:  0;
			bottomPadding: 0;

			RowLayout {
				anchors.fill: parent;

				BusyIndicator {
					padding: 3;
					leftPadding: 3;

					Layout.fillHeight: true;

					running: !connectionIndicator.allConnected
				}

				Label {
					clip: true

					Layout.fillHeight: true
					Layout.fillWidth:  true

					text: "%1 connecting...".arg(GameHandle.game.connected ? "Device" : "Server");

					fontSizeMode: Text.Fit
					font.pixelSize: 300
					font.family: "Impact"
				}
			}



			Layout.preferredHeight: allConnected ? 0 : 25
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
