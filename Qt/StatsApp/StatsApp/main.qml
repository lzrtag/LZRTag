import QtQuick 2.11
import QtQuick.Window 2.11

import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

import QtQuick.Controls.Material 2.3

import xasin.lasertag.gamehandle 1.0

import "qml"

ApplicationWindow {
	id: rootWindow

	visible: true
	width: 360
	height: 592
	title: qsTr("Hello World")

	property var currentPlayer: GameHandle.currentPlayer

	property color teamColor: currentPlayer.teamColor
	Behavior on teamColor {
		ColorAnimation {
			duration: 1000
		}
	}
	Material.theme:   Material.Dark
	Material.accent:  teamColor
	Material.primary: teamColor

	BackgroundTasks {
		player: GameHandle.currentPlayer
	}

	StackView {
		id: mainViewStack
		initialItem: "qrc:/qml/pages/hud/PlayerOverview.qml"

		anchors.fill: parent;
	}
}
