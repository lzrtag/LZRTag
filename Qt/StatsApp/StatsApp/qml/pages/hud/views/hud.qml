import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

import QtQuick.Controls.Material 2.3

import xasin.lasertag.gamehandle 1.0

import "../../../widgets"

GridLayout {
	property string title: player.name + "'s HUD";

	property bool   layoutVertial: width < height

	anchors.fill: parent;
	anchors.margins: 3;

	layoutDirection: Qt.RightToLeft

	columnSpacing: 3
	rowSpacing:    3

	Frame {
		Layout.fillHeight: true
		Layout.fillWidth:  true
		Layout.preferredHeight: 0.5
		Layout.preferredWidth:  1

		Layout.row: 1
		Layout.column: 1

        Label {
            anchors.fill: parent;

				text: GameHandle.currentPosition + "\nZones: " + GameHandle.currentZones
        }
	}

	Frame {
		Layout.fillHeight: true
		Layout.fillWidth: true
		Layout.preferredHeight: 0.5;
		Layout.preferredWidth:  1;

		Layout.row: 2
		Layout.column: 1

		Material.elevation: 1;

		GridLayout {
			anchors.fill: parent;

			columns: 3;

			Item {
				Layout.rowSpan: 2
				Layout.fillHeight: true
				Layout.preferredWidth: 30

				Label {
					width: parent.height
					height: parent.width

					anchors.centerIn: parent;

					padding: 0

					text: "Criticals"
					fontSizeMode: Text.Fit
					font.pixelSize: 300
					horizontalAlignment: Qt.AlignHCenter

					rotation: -90
				}
			}



			InfoLabel {
				Layout.fillHeight: true
				Layout.fillWidth:	 true
				Layout.margins:  3;

				Layout.columnSpan: 2;

				title: "HP"
				text: qsTr("%1%").arg(player.life.toFixed(1));

				fillPercent: player.life/100;

				boxColor: Material.color((player.life > 60) ? Material.Green : (player.life < 40) ? Material.Red : Material.Yellow);

				blink: player.life < 40;
			}

			InfoLabel {
				Layout.fillHeight: true
				Layout.fillWidth:	 true
				Layout.margins:  3;

				Layout.columnSpan: 2;

				title: "Ammo"
				text: qsTr("%1/%2").arg(player.ammo).arg(player.maxAmmo);

				fillPercent: player.ammo / player.maxAmmo;

				boxColor: Material.color((fillPercent > 0.5) ? Material.Green : (fillPercent < 0.1) ? Material.Red : Material.Yellow);

				blink: fillPercent === 0;
			}
		}
	}

	Frame {
		id: playerListFrame

		clip: true;

		Layout.fillHeight: true
		Layout.fillWidth: true
		Layout.preferredHeight: 1
		Layout.preferredWidth:  1

		Layout.rowSpan: 2
		Layout.row: layoutVertial ? 3 : 1
		Layout.column: layoutVertial ? 1 : 2

		Material.elevation: 1;

		ListView {
			anchors.fill: parent;

			model: GameHandle.game.playerIDs

			delegate: Component {
				Item {
					width: playerListFrame.availableWidth
					height: 50

					SmolPlayerBar {
						anchors.fill: parent;
						anchors.margins: 3;

						player: GameHandle.game.getPlayer(modelData);
					}
				}
			}
		}
	}
}

/*##^## Designer {
	 D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/
