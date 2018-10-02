
import QtQuick 2.0
import QtQuick.Controls 2.4

Item {
	property alias modal: popupCore.modal
	property alias dim:   popupCore.dim
	property alias popped: popupCore.visible
	property alias closePolicy: popupCore.closePolicy

	property alias enter: popupCore.enter
	property alias exit:  popupCore.exit

	default property alias children: popupContent.data

	id: popupFrame

	Popup {
		id: popupCore

		modal: true

		Overlay.modal: Rectangle {
			color: "#A0000000"
		}

		Item {
			id: popupContent

			implicitHeight: 0
			implicitWidth:  0

			width:   popupFrame.width
			height:  popupFrame.height
		}
	}
}
