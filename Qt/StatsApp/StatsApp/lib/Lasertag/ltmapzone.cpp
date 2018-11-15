
#include "ltmapzone.h"
#include <math.h>

LTMapZone::LTMapZone(QString tag, QObject *parent) : QObject(parent),
    centerPoint(0, 0), radius(0),
    mapPolygon(),
    teamMask(255),
	 zoneData(), zoneTag(tag)
{
}
LTMapZone::LTMapZone(const LTMapZone &source) : LTMapZone(source.zoneTag) {
	centerPoint = source.centerPoint;
	radius = source.radius;

	mapPolygon = source.mapPolygon;

	teamMask = source.teamMask;
}

LTMapZone& LTMapZone::operator=(const LTMapZone &source) {
	centerPoint = source.centerPoint;
	radius = source.radius;
	mapPolygon = source.mapPolygon;

	teamMask = source.teamMask;

	zoneData = source.zoneData;
	zoneTag  = source.zoneTag;

	return *this;
}

QString LTMapZone::getZoneTag() {
	return zoneTag;
}
QVariantMap LTMapZone::getZoneData() {
	return zoneData;
}

bool LTMapZone::pointInsideZone(QPointF point) {
	if(mapPolygon.empty()) {
		auto qVec = point - centerPoint;
		auto len = sqrt(pow(qVec.x(),2) + pow(qVec.y(),2));

		return len <= radius;
	}
	else
		return mapPolygon.containsPoint(point, Qt::FillRule::OddEvenFill);
}

bool LTMapZone::appliesToPlayer(LTPlayer *player) {
	return ((1 <<player->getTeam()) & teamMask) != 0;
}
bool LTMapZone::playerInsideZone(LTPlayer *player) {
	if(!appliesToPlayer(player))
		return false;

    return pointInsideZone(player->getMapPosition());
}
