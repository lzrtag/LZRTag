#include "ltmap.h"

#include <math.h>

LTMap::LTMap(QObject *parent) : QObject(parent),
	 mapCenter(8.209523, 52.581728), mapRotation(0),
	zones()
{
	 LTMapZone circZone("CircTest");
	 circZone.radius = 50;

	 zones << circZone;

	 LTMapZone teaTableZone("TeaTable");
	 teaTableZone.centerPoint = QPointF(8.5, -11);
	 teaTableZone.radius = 5;

	 zones << teaTableZone;

	 LTMapZone pondZone("Pond");
	 pondZone.centerPoint = latLonToXY(QPointF(8.209529, 52.581640));
	 pondZone.radius = 4;

	 zones << pondZone;

	 LTMapZone grassZone("LeftGarden");
	 grassZone.mapPolygon << QPointF(11.5, 2.5) << QPointF(10, 8) << QPointF(10.5, 16) << QPointF(3.5, 20) << QPointF(4, 11.7);
	 zones << grassZone;
}



QPointF LTMap::latLonToXY(QPointF latLong) {
    double yFact = 6371000 * M_PI / 180;
    double xFact = yFact * cos(latLong.y()*M_PI / 180);

    QPointF centeredCoords = latLong - mapCenter;

    double xUnr = centeredCoords.x() * xFact;
    double yUnr = centeredCoords.y() * yFact;

	return QPointF(cos(mapRotation)*xUnr + sin(mapRotation)*yUnr,
						cos(mapRotation)*yUnr - sin(mapRotation)*xUnr);
}

QList<QString> LTMap::getZonesForPlayer(LTPlayer *player) {
	QList<QString> output;
	for(LTMapZone& z : zones) {
		if(z.playerInsideZone(player) && !output.contains(z.getZoneTag()))
			output << z.getZoneTag();
	}

	return output;
}

void LTMap::updateZonesForPlayer(LTPlayer *player) {
	player->updateZones(getZonesForPlayer(player));
}

void LTMap::update_from_list(QVariantList data) {
	zones.clear();

	for(QVariant &map: data) {
		auto hash = map.toHash();

		if(hash.contains("tag")) {
			LTMapZone outZone(hash.value("tag").toString());

			outZone.teamMask = uint8_t(hash.value("teamMask").toInt());

			bool convertFromGPS = hash.value("coordinatesAsGPS").toBool();

			if(hash.contains("polygon")) {
				for(QVariant &pointVariant : hash.value("polygon").toList()) {
					QList<QVariant> point = pointVariant.toList();
					QPointF outPoint(point[0].toReal(), point[1].toReal());

					if(convertFromGPS)
						outPoint = latLonToXY(outPoint);
					outZone.mapPolygon << outPoint;
				}
			}
			else {
				outZone.radius = hash.value("radius").toDouble();

				QList<QVariant> point = hash.value("centerPoint").toList();
				outZone.centerPoint = QPointF(point[0].toReal(), point[1].toReal());
				if(convertFromGPS)
					outZone.centerPoint = latLonToXY(outZone.centerPoint);
			}

			if(hash.contains("data"))
				outZone.data = hash.value("data").toMap();

			zones << outZone;
		}
	}
}
