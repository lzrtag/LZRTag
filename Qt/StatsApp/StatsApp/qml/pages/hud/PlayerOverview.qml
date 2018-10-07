
import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

import QtQuick.Controls.Material 2.3

import xasin.lasertag.gamehandle 1.0

import "../../widgets"

Item {
	id: rootItem
	property var player: currentPlayer;

	ToolBar {
		id: topBar

		anchors.top: parent.top;
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

	GridLayout {
		anchors.left:  parent.left;
		anchors.right: parent.right;
		anchors.bottom: parent.bottom;
		anchors.top:   topBar.bottom;
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
