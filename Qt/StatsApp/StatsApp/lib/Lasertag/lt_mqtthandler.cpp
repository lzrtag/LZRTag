#include "lt_mqtthandler.h"

LT_MQTTHandler::LT_MQTTHandler(QObject *parent) : LTHandler(parent),
	mqtt_reconnectTimer(this), mqtt_client(this), mqtt_subscription(nullptr),
	map(this)
{
	mqtt_reconnectTimer.setSingleShot(true);
	mqtt_reconnectTimer.setInterval(2000);

	connect(&mqtt_reconnectTimer, &QTimer::timeout, this, &LT_MQTTHandler::mqtt_tryReconnect);

	mqtt_client.setCleanSession(false);
	mqtt_client.setKeepAlive(1000);
	mqtt_client.setPort(1883);

	connect(&mqtt_client, &QMqttClient::disconnected, this, &LT_MQTTHandler::mqtt_onDisconnect);
	connect(&mqtt_client, &QMqttClient::connected, this, &LT_MQTTHandler::mqtt_onReconnect);
	connect(&mqtt_client, &QMqttClient::brokerSessionRestored, this, &LT_MQTTHandler::mqtt_onReconnect);
}

void LT_MQTTHandler::initNewPlayer(QString name) {
	if(player_map.contains(name))
		return;

	player_map.insert(name, new LT_MQTTPlayer(name, &mqtt_client, this));
	emit playerAdded(player_map[name]);
	emit playerListChanged();
}

QMqttClient *LT_MQTTHandler::getMQTT() {
	return &mqtt_client;
}

QVariantList LT_MQTTHandler::getPlayerIDs() {
	QVariantList outList;
	for(auto s : player_map.keys())
		outList << s;
	return outList;
}

bool LT_MQTTHandler::isConnected() {
	return mqtt_client.state() == QMqttClient::Connected;
}
LTMap* LT_MQTTHandler::getMap() {
	return &map;
}

LTPlayer * LT_MQTTHandler::getPlayer(QString name) {
	initNewPlayer(name);
	return static_cast<LTPlayer *>(player_map[name]);
}

void LT_MQTTHandler::setHostname(QString name) {
	if(mqtt_client.state() == QMqttClient::Connected)
		mqtt_client.disconnectFromHost();

	mqtt_client.setHostname(name);
	mqtt_client.connectToHost();
	mqtt_reconnectTimer.start();
	qDebug()<<"Set new hostname:"<<name;
}

void LT_MQTTHandler::mqtt_onDisconnect() {
	qDebug()<<"MQTT disconnected!";

	emit connectionStatusChanged();
	mqtt_reconnectTimer.start(2000);
}
void LT_MQTTHandler::mqtt_tryReconnect() {
	qDebug()<<"Trying to reconnect!";

	if(mqtt_client.state() == QMqttClient::Connected)
		return;
	if(mqtt_client.state() != QMqttClient::Disconnected)
		mqtt_client.disconnectFromHost();

	mqtt_client.connectToHost();
	mqtt_reconnectTimer.start(5000);
}
void LT_MQTTHandler::mqtt_onReconnect() {
	qDebug()<<"MQTT connected!";

	mqtt_subscription = mqtt_client.subscribe(QString("Lasertag/#"));
	connect(mqtt_subscription, &QMqttSubscription::messageReceived, this, &LT_MQTTHandler::mqtt_processData);

	emit connectionStatusChanged();

	for(auto k : player_map.values())
		k->mqtt_onReconnected();
}

void LT_MQTTHandler::mqtt_processData(QMqttMessage msg) {
	auto category = msg.topic().levels()[1];
	if(category == "Players") {
		auto player = msg.topic().levels()[2];

		qDebug()<<"Got data for:"<<player;

		initNewPlayer(player);
		player_map[player]->mqtt_processData(msg);
	}
	else if(category == "Zones") {
		QJsonDocument document = QJsonDocument::fromJson(msg.payload());
		map.update_from_map(document.object().toVariantMap());
	}
}
