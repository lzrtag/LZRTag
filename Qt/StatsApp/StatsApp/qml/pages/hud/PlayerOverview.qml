
import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

import QtQuick.Controls.Material 2.3

import xasin.lasertag.gamehandle 1.0

import "../../widgets"

Item {
	id: rootItem
	property var player: currentPlayer;

	Column {
		anchors.fill: parent;

		spacing: 4

		ToolBar {
			id: topBar

			anchors.left: parent.left;
			anchors.right: parent.right;

			RowLayout {
				anchors.fill: parent;

				/*Label {
					text: "Lasertag HUD"
					font.family: "Impact"
					horizontalAlignment: Text.AlignHCenter
					fontSizeMode: Text.Fit
					font.pixelSize: 25

					Layout.fillWidth: true
				}*/


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
			anchors.left: parent.left;
			anchors.right: parent.right;

			anchors.margins: 10

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

			height: (GameHandle.game.connected && player.status === "OK") ? 0 : 30
			Behavior on height {
				NumberAnimation {duration: 500}
			}
		}

		GridLayout {
			anchors.left:  parent.left;
			anchors.right: parent.right;
			anchors.margins: 5;

			columns: 2

			InfoLabel {
				Layout.preferredHeight: 100
				Layout.fillWidth:	 true
				Layout.margins:  3;
				Layout.columnSpan: 1

				title: "Ammo"
				text: qsTr("%1/%2").arg(player.ammo).arg(player.maxAmmo);

				fillPercent: player.ammo / player.maxAmmo;

				boxColor: Material.color((fillPercent > 0.5) ? Material.Green : (fillPercent < 0.1) ? Material.Red : Material.Yellow);

				blink: fillPercent === 0;
			}

			InfoLabel {
				Layout.preferredHeight: 100
				Layout.fillWidth:	 true

				Layout.margins:  3;
				Layout.columnSpan: 1

				title: "HP"
				text: qsTr("%1%").arg(player.life.toFixed(1));

				fillPercent: player.life/100;

				boxColor: Material.color((player.life > 60) ? Material.Green : (player.life < 40) ? Material.Red : Material.Yellow);

				blink: player.life < 40;
			}

			Item {
				Layout.columnSpan: 2
				Layout.fillHeight: true
			}
		}
	}
}
