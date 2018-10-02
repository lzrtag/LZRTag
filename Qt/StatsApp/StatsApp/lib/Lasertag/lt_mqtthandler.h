#ifndef LT_MQTTHANDLER_H
#define LT_MQTTHANDLER_H

#include <QTimer>
#include <QtMqtt/QtMqtt>

#include "lthandler.h"
#include "lt_mqttplayer.h"

class LT_MQTTHandler : public LTHandler
{
	Q_OBJECT

	Q_PROPERTY(QMqttClient *mqtt READ getMQTT)
	Q_PROPERTY(QVariantList playerIDs READ getPlayerIDs NOTIFY playerListChanged)

private:
	QMap<QString, LT_MQTTPlayer *>player_map;

	QTimer		mqtt_reconnectTimer;
	QMqttClient mqtt_client;
	QMqttSubscription	*mqtt_subscription;

protected:
	void initNewPlayer(QString name);

public:
	LT_MQTTHandler(QObject *parent=nullptr);

	QMqttClient *getMQTT();

	QVariantList getPlayerIDs();

	Q_INVOKABLE LTPlayer * getPlayer(QString name);
	Q_INVOKABLE void setHostname(QString name);

public slots:
	void mqtt_onDisconnect();
	void mqtt_tryReconnect();
	void mqtt_onReconnect();

	void mqtt_processData(QMqttMessage msg);
};

#endif // LT_MQTTHANDLER_H
