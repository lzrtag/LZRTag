#include "ltmap.h"

#include <math.h>

LTMap::LTMap(QObject *parent) : QObject(parent),
	 mapCenter(8.209523, 52.581728), mapRotation(0),
	zones()
{
    auto circZone = new LTMapZone("CircTest");
    circZone->radius = 50;

	 zones << circZone;

	 auto teaTableZone = new LTMapZone("TeaTable");
	 teaTableZone->centerPoint = QPointF(8.5, -11);
	 teaTableZone->radius = 5;

	 zones << teaTableZone;

	 auto pondZone = new LTMapZone("Pond");
	 pondZone->centerPoint = latLonToXY(QPointF(8.209529, 52.581640));
	 pondZone->radius = 4;

	 zones << pondZone;

	 auto grassZone = new LTMapZone("LeftGarden");
	 grassZone->mapPolygon << QPointF(11.5, 2.5) << QPointF(10, 8) << QPointF(10.5, 16) << QPointF(3.5, 20) << QPointF(4, 11.7);
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
	for(auto z : zones) {
		if(z->playerInsideZone(player) && !output.contains(z->getZoneTag()))
			output << z->getZoneTag();
	}

	return output;
}
void LTMap::updateZonesForPlayer(LTPlayer *player) {
	player->updateZones(getZonesForPlayer(player));
}
