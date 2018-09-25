#ifndef LTHANDLER_H
#define LTHANDLER_H

#include <QObject>
#include <QtMqtt/QtMqtt>

#include "ltplayer.h"

class LTHandler : public QObject
{
    Q_OBJECT


private:
    QMqttClient * mqtt_client;
    QVariantMap   player_map;

    LTPlayer * current_player;

public:
    explicit LTHandler(QObject *parent = nullptr);

signals:

public slots:
};

#endif // LTHANDLER_H
