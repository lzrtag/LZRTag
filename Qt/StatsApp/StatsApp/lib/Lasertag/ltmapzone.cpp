
#include "ltmapzone.h"
#include <math.h>

LTMapZone::LTMapZone(QString tag, QObject *parent) : QObject(parent),
	zoneTag(tag)
{
}

QString LTMapZone::getZoneTag() {
	return zoneTag;
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

	return pointInsideZone(QPointF(player->position["x"].toFloat(), player->position["y"].toFloat()));
}
