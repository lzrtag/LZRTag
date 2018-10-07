
import QtQuick 2.11
import QtGraphicalEffects 1.0

Rectangle {
	property var player;

	color: "transparent"

	radius: height/2;

	border.width: 2
	border.color: player.teamColor;

	Image {
		id: playerIcon
		anchors.fill: parent;
		asynchronous: true;

		antialiasing: true;

		source: player.iconURL;
		sourceSize: Qt.size(100, 100);
		fillMode: Image.PreserveAspectFit

		visible: false;
	}
	Rectangle {
		id: playerIconMask
		anchors.fill: playerIcon

		radius: height/2;
		visible: false;
	}

	OpacityMask {
		anchors.fill: playerIcon
		source: playerIcon
		maskSource: playerIconMask

		z: -1
	}
}
