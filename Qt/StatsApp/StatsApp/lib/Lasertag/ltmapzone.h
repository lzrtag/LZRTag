#ifndef LTMAPZONE_H
#define LTMAPZONE_H

#include <QObject>
#include <QPointF>
#include <QPolygonF>

#include "ltplayer.h"

class LTMap;

class LTMapZone : public QObject
{
	Q_OBJECT

friend LTMap;

protected:
	QPointF centerPoint;
	double  radius;
	QPolygonF mapPolygon;

	uint8_t teamMask;

	QVariantMap data;

public:
	const QString zoneTag;

	explicit LTMapZone(QString tag, QObject *parent = nullptr);
	LTMapZone(const LTMapZone &source);

	QString getZoneTag();

	bool pointInsideZone(QPointF point);

	bool appliesToPlayer(LTPlayer *player);
	bool playerInsideZone(LTPlayer *player);

signals:
	void zonesRecalculated();

public slots:
};

#endif // LTMAPZONE_H
