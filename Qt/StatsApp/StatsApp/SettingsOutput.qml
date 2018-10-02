
import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

import QtQuick.Controls.Material 2.3

ItemDelegate {
	property alias name:  nameLabel.text
	property alias value: valueLabel.text

	property bool  disabled: false;

	Layout.fillWidth: true
	implicitHeight: 30

	Rectangle {
		color: Material.background
		opacity: disabled ? 0.6 : 0

		anchors.fill: parent;

		Behavior on opacity {
			SmoothedAnimation { velocity: -1; duration: 300 }
		}

		z: 1
	}

	RowLayout {
		anchors.fill: parent;

		Label {
			id: nameLabel
			Layout.minimumWidth: 120
			leftPadding: 10
		}

		Label {
			id: valueLabel
			Layout.fillWidth: true

			color: "lightgrey"
		}
	}
}
