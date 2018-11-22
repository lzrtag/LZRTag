
import QtQuick 2.11

import QtQuick.Shapes 1.11

import xasin.lasertag.map 1.0

Item {
	property LTMap displayMap;

	Repeater {
		id: zoneDrawer

		model: displayMap.zoneCount;

		delegate: Item {
			property LTMapZone map: displayMap.getZone(index);

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

			ShapePath {
				fillColor: map.color;

				property var points: map.polygon;

				Component.onCompleted: {
					console.log("Polygon points are:", points);
				}

//				Repeater {
//					model: parent.map.getPolygonPoints();

//					delegate: PathLine {
//						x: modelData.x
//						y: modelData.y
//					}
//				}
			}
		}
	}
}
