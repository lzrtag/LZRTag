#include "ltmap.h"

#include <math.h>

LTMap::LTMap(QObject *parent) : QObject(parent),
    mapCenter(9.717041, 52.384172), mapRotation(0),
	zones()
{
    auto circZone = new LTMapZone("CircTest");
    circZone->radius = 50;

    zones << circZone;
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
		if(z->playerInsideZone(player))
			output << z->getZoneTag();
	}

	return output;
}
