
import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

import QtQuick.Controls.Material 2.3

Rectangle {
	id: rootRect

	property var player;

	implicitHeight: 52;

	border.width: 2
	border.color: Material.color(Material.Green)

	radius: height/2;

	color: Material.background;

	Rectangle {
		id: fillRect

		property real fill: player.life/100;

		anchors.top: parent.top;
		anchors.left: parent.left;
		anchors.bottom: parent.bottom;

		radius: height/2;

		color: parent.border.color;

		opacity: 0.3;

		width: (2*radius) + (parent.width - 2*radius)*fill;
	}

	RowLayout {
		spacing: 0;

		anchors.fill: parent;

		PlayerIcon {
			player: rootRect.player;

			Layout.preferredWidth: height;
			Layout.fillHeight: true;
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

		StatusIcon {
			id: playerConnected

			enabled: player.status !== "OK"
			source: "qrc:/pics/octicons/plug.svg"

			Layout.preferredHeight: rootRect.height * 0.6
		}
		StatusIcon {
			enabled: (player.battery < 3.6) && !playerConnected.enabled
			source: "qrc:/pics/diverse_icons/low-battery.svg"

			Layout.preferredHeight: rootRect.height * 0.6
		}

		Item {Layout.preferredWidth: 10}
	}
}
