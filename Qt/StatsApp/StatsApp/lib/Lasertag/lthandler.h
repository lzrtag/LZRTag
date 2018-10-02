#ifndef LTHANDLER_H
#define LTHANDLER_H

#include <QObject>
#include <QtMqtt/QtMqtt>

#include "ltplayer.h"

class LTHandler : public QObject
{
	Q_OBJECT

public:
	explicit LTHandler(QObject *parent = nullptr);

	Q_INVOKABLE virtual LTPlayer * getPlayer(QString name);

signals:
	void playerAdded(LTPlayer * newPlayer);
	void playerListChanged();

public slots:
};

#endif // LTHANDLER_H
