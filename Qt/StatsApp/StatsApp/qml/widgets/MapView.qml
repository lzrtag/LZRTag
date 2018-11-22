
import QtQuick 2.11

import QtQuick.Shapes 1.11

import xasin.lasertag.map 1.0

Item {
	property LTMap displayMap;

	Connections {
		target: displayMap

		onZonesChanged: {
			var outZones = [];

			for(var i=0; i < displayMap.zoneCount; i++) {
				outZones.push(displayMap.getZone(i));
			}

			zoneDrawer.model = outZones;
		}
	}

	Repeater {
		id: zoneDrawer

		anchors.fill: parent;

		delegate: Item {
			property LTMapZone map: modelData;

			Component.onCompleted: {
				console.log("Makin' a polygon from: ", index, map);
			}

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
					y: xyPoint.y;
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
						mapPolygonPath.startY = map.getPolygonPoint(0).y;

						mapPolygonShape.update()
					}
				}
			}
		}
	}
}
