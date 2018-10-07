#ifndef LT_MQTTPLAYER_H
#define LT_MQTTPLAYER_H

#include <QObject>
#include <QtMqtt/QtMqtt>
#include <QJsonDocument>

#include "ltplayer.h"

class LT_MQTTPlayer : public LTPlayer
{
	Q_OBJECT

private:
	QMqttClient *mqtt_client;

public:
	LT_MQTTPlayer(QString deviceID, QMqttClient *client, QObject *parent=nullptr);

	void setName(QString newName);
	void setIconURL(QString newIcon);

	void updatePosition(QVariantMap newPosition);

public slots:
	void mqtt_processData(QMqttMessage msg);
	void mqtt_onReconnected();
};

#endif // LT_MQTTPLAYER_H
