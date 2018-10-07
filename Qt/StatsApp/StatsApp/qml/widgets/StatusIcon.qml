
import QtQuick 2.0
import QtQuick.Layouts 1.3

Image {
	property bool enabled: true

	id: iconImg
	sourceSize: Qt.size(64, 64);

	opacity: enabled ? 1 : 0
	Behavior on opacity {
		NumberAnimation {
			duration: 200
		}
	}

	Layout.preferredWidth: height * opacity;
}
