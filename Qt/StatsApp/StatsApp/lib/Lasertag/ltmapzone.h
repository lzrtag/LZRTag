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

	Q_PROPERTY(QString tag			 READ getZoneTag)

	Q_PROPERTY(QPointF centerPoint READ getCenterPoint)
	Q_PROPERTY(double  radius		 READ getRadius)
	Q_PROPERTY(QPolygonF polygon	 READ getPolygon)

	Q_PROPERTY(QColor  color READ getColor)

friend LTMap;

protected:
	QString zoneTag;

	QPointF centerPoint;
	double  radius;
	QPolygonF mapPolygon;

	QColor  color;

	uint8_t teamMask;

	QVariantMap zoneData;

public:
	LTMapZone();

	explicit LTMapZone(QString tag, QObject *parent = nullptr);
	LTMapZone(const LTMapZone &source);
	LTMapZone& operator=(const LTMapZone &source);

	~LTMapZone();

	QString getZoneTag();

	QPointF getCenterPoint();
	double  getRadius();
	QPolygonF getPolygon();

	Q_INVOKABLE QVariantList getPolygonPoints();

	QColor  getColor();

	QVariantMap getZoneData();

	bool pointInsideZone(QPointF point);

	Q_INVOKABLE bool appliesToPlayer(LTPlayer *player);
	Q_INVOKABLE bool playerInsideZone(LTPlayer *player);

signals:

public slots:
};

#endif // LTMAPZONE_H
