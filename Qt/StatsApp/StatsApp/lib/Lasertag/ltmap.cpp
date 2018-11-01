#include "ltmap.h"

#include <math.h>

LTMap::LTMap(QObject *parent) : QObject(parent),
	mapCenter(9.7168621, 52.3840558), mapRotation(0),
	zones()
{

}

QPointF LTMap::latLonToXY(QPointF latLong) {
	auto yFact = 6378100 * M_2_PI;
	auto xFact = yFact * cos(latLong.y()/M_2_PI);

	auto centeredCoords = latLong - mapCenter;

	auto xUnr = centeredCoords.x() * xFact;
	auto yUnr = centeredCoords.y() * yFact;

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
