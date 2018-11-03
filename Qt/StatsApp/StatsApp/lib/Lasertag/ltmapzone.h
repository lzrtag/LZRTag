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

public:
	const QString zoneTag;

	explicit LTMapZone(QString tag, QObject *parent = nullptr);

	QString getZoneTag();

	bool pointInsideZone(QPointF point);

	bool appliesToPlayer(LTPlayer *player);
	bool playerInsideZone(LTPlayer *player);

signals:

public slots:
};

#endif // LTMAPZONE_H
