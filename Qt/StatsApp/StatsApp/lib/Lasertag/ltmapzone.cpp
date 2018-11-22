
#include "ltmapzone.h"
#include <math.h>

LTMapZone::LTMapZone() :
	zoneTag(""),
	centerPoint(0, 0), radius(0),
	mapPolygon(),
	color(), teamMask(255),
	zoneData(){

}

LTMapZone::LTMapZone(QString tag, QObject *parent) : QObject(parent),
	zoneTag(tag),
	centerPoint(0, 0), radius(0),
	mapPolygon(),
	color(), teamMask(255),
	zoneData()
{
}
LTMapZone::LTMapZone(const LTMapZone &source) : LTMapZone(source.zoneTag) {
	this->operator=(source);
}

LTMapZone& LTMapZone::operator=(const LTMapZone &source) {
	centerPoint = source.centerPoint;
	radius = source.radius;
	mapPolygon = source.mapPolygon;

	color = source.color;

	teamMask = source.teamMask;

	zoneData = source.zoneData;
	zoneTag  = source.zoneTag;

	return *this;
}

LTMapZone::~LTMapZone() {
}

QString LTMapZone::getZoneTag() {
	return zoneTag;
}

QPointF LTMapZone::getCenterPoint() {
	return centerPoint;
}
double LTMapZone::getRadius() {
	return radius;
}
QPolygonF LTMapZone::getPolygon() {
	return mapPolygon;
}

QVariantList LTMapZone::getPolygonPoints() {
	QVariantList outList;

	for(auto p : mapPolygon) {
		outList << p;
	}

	return outList;
}

QColor LTMapZone::getColor() {
	return color;
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
