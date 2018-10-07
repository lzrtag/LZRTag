import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

import QtQuick.Controls.Material 2.3

import "qrc:/qml/widgets"

GridLayout {
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
