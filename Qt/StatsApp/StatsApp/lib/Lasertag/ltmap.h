#ifndef LTMAP_H
#define LTMAP_H

#include <QObject>
#include <QtDebug>

#include <QPointF>

#include "ltmapzone.h"

class LTMap : public QObject
{
	Q_OBJECT
private:
	QPointF mapCenter;
	double  mapRotation;

	QList<LTMapZone> zones;

public:
	explicit LTMap(QObject *parent = nullptr);

	Q_INVOKABLE QPointF latLonToXY(QPointF latLong);
	Q_INVOKABLE QList<LTMapZone *> getZonesForPlayer(LTPlayer *player);

	Q_INVOKABLE void updateZonesForPlayer(LTPlayer *player);

signals:

public slots:
	void update_from_map(QVariantMap data);
};

#endif // LTMAP_H