import QtQuick 2.11

import QtQuick.Controls 2.4
import QtQuick.Controls.Material 2.3

Pane {
	padding: 0

	Material.elevation: 2

	property color boxColor  : Material.highlight
	property bool  blink: false

	property real  fillPercent: 0
	property alias radius: rootFrame.radius

	default property alias children: contentRect.data

	Behavior on boxColor {ColorAnimation {duration: 500}}

	onBlinkChanged: {
		if(!blink)
			rootFrame.opacity = 1;
	}

	Timer {
		id: blinkTimer
		interval: 500
		repeat:   true
		running:  blink

		onTriggered: {
			if(rootFrame.opacity === 1)
				rootFrame.opacity = 0.5
			else
				rootFrame.opacity = 1
		}
	}

	Rectangle {
		id: background
		color: Material.background
		radius: rootFrame.radius

		anchors.fill: parent;
	}

	Rectangle {
		id: rootFrame

		anchors.fill: parent;

		color: "transparent"
		radius: 1
		border.width: 3
		border.color: boxColor

		clip: true

		Rectangle {
			id: fillRect
			anchors.left:   parent.left
			anchors.top:    parent.top
			anchors.bottom: parent.bottom
			anchors.topMargin:		parent.border.width
			anchors.bottomMargin:	parent.border.width
			radius: parent.radius

			width: parent.width * Math.max(Math.min(fillPercent, 1), 0)

			opacity: 0.4

			color: rootFrame.border.color

			Behavior on width {
				SmoothedAnimation {
					velocity: -1
					duration: 200
				}
			}
		}

		Item {
			id: contentRect
			anchors.fill: parent;
			anchors.margins: parent.radius
		}
	}
}
