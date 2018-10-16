
import QtQuick 2.0
import QtQuick.Controls.Material 2.3

import QtQuick.Layouts 1.3

StatusBox {
	property alias title  : titleText.text
	property alias text   : messageText.text

	GridLayout {
		anchors.fill:		  parent;
		anchors.margins:	  3;

		columns: (width/height) > 3 ? 2 : 1;

		Text {
			id: titleText

			Layout.fillHeight: true
			Layout.fillWidth:  true
			Layout.preferredHeight: 3
			Layout.preferredWidth:  4

			font.family: "Impact"
			horizontalAlignment: Text.AlignHCenter
			verticalAlignment:   Text.AlignVCenter

			fontSizeMode:   Text.Fit
			font.pixelSize: 300

			color: boxColor;
			text: "Title"
		}
		Text {
			id: messageText

			Layout.fillHeight: true
			Layout.fillWidth:  true
			Layout.preferredHeight: 10
			Layout.preferredWidth:  10

			font.family: "Impact"
			fontSizeMode: Text.Fit
			horizontalAlignment: Text.AlignHCenter
			verticalAlignment:   Text.AlignVCenter
			minimumPixelSize: 30
			font.pixelSize:	 500

			color: boxColor;
			text: "Text"
		}
	}
}
