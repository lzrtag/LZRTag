
import QtQuick 2.11

import QtQuick.Shapes 1.11

import xasin.lasertag.map 1.0
import xasin.lasertag.gamehandle 1.0

Flickable {
	property LTMap displayMap;

	property var   idsToDraw: gameHandler.playerIDs;

	contentWidth: displayMap.boundingRect.width
	contentHeight: displayMap.boundingRect.height

	Connections {
		target: displayMap

		onZonesChanged: {
			var outZones = [];

			for(var i=0; i < displayMap.zoneCount; i++) {
				outZones.push(displayMap.getZone(i));
			}

			zoneDrawer.model = outZones;

			console.log("New bounding rect is:", displayMap.boundingRect);
		}
	}

	Item {
		x: -displayMap.boundingRect.x
		y: displayMap.boundingRect.y + displayMap.boundingRect.height

		Repeater {
			id: playerDrawer
			anchors.fill: parent;

			model: gameHandler.playerIDs

			delegate: Rectangle {
				property var player: gameHandler.getPlayer(modelData);

				width: 6
				height: width
				radius: width/2

				color: player.teamColor

				x: -width/2 + player.position["x"];
				y: -width/2 + player.position["y"];

				Component.onCompleted: console.log("Drawing", player.name, "at", x, y);
			}
		}

		Repeater {
			id: zoneDrawer

			anchors.fill: parent;

			delegate: Item {
				property LTMapZone map: modelData;

				anchors.centerIn: parent;

				Rectangle {
					color: map.color;

					x: map.centerPoint.x;
					y: map.centerPoint.y;

					width:  map.radius;
					height: width;
					radius: width/2;
				}

				Component {
					id: pathElement
					PathLine {
						property point xyPoint;

						x: xyPoint.x;
						y: -xyPoint.y;
					}
				}

				Shape {
					id: mapPolygonShape
					anchors.centerIn: parent;

					anchors.fill: parent;

					ShapePath {
						id: mapPolygonPath

						fillColor: map.color

						Component.onCompleted:  {
							console.log("Map has", map.getPolygonPointCount(), " points!");

							for(var i=0; i<map.getPolygonPointCount(); i++) {
								mapPolygonPath.pathElements.push(pathElement.createObject(mapPolygonPath, {"xyPoint": map.getPolygonPoint(i)}));
							}

							mapPolygonPath.startX = map.getPolygonPoint(0).x;
							mapPolygonPath.startY = -map.getPolygonPoint(0).y;

							mapPolygonShape.update()
						}
					}
				}
			}
		}
	}
}
