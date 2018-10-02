
import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

import QtQuick.Controls.Material 2.3

Item {
	property var player: currentPlayer;

	ToolBar {
		id: topBar

		anchors.top: parent.top;
		anchors.left: parent.left;
		anchors.right: parent.right;

		RowLayout {
			anchors.fill: parent;

			Label {
				text: "Lasertag HUD"
				font.family: "Impact"
				horizontalAlignment: Text.AlignHCenter
				fontSizeMode: Text.Fit
				font.pixelSize: 25

				Layout.fillWidth: true
			}
			ToolButton {
				icon.source: "pics/octicons/gear"
				icon.width: 20
				icon.height: 22
				onClicked: mainViewStack.push("SettingsPage.qml");
			}
		}
	}

	GridLayout {
		anchors.left:  parent.left;
		anchors.right: parent.right;
		anchors.bottom: parent.bottom;
		anchors.top:   topBar.bottom;
		anchors.margins: 30;

		columns: 2

		InfoLabel {
			Layout.fillHeight: true
			Layout.fillWidth:  true
			Layout.margins:  3;

			title: "Battery Level"
			text:  qsTr("%1V").arg(Math.round(player.battery*100)/100);

			fillPercent: (player.battery - 3.5) / 0.5;

			boxColor: Material.color((fillPercent > 0.3) ? Material.Green : (fillPercent < 0.1) ? Material.Red : Material.Yellow);
			blink: fillPercent < 0.25;
		}

		InfoLabel {
			Layout.fillHeight: true
			Layout.fillWidth:	 true
			Layout.margins:  3;
			Layout.columnSpan: 1

			title: "Ammo"
			text: qsTr("%1/%2").arg(player.ammo).arg(player.maxAmmo);

			fillPercent: player.ammo / player.maxAmmo;

			boxColor: Material.color((fillPercent > 0.5) ? Material.Green : (fillPercent < 0.25) ? Material.Red : Material.Yellow);

			blink: fillPercent < 0.25;
		}

		InfoLabel {
			Layout.fillHeight: true
			Layout.fillWidth:	 true
			Layout.margins:  3;
			Layout.columnSpan: 1

			title: "HP"
			text: qsTr("%1%").arg(player.life.toFixed(1));

			fillPercent: player.life/100;

			boxColor: Material.color((player.life > 60) ? Material.Green : (player.life < 40) ? Material.Red : Material.Yellow);

			blink: player.life < 40;
		}
	}
}
