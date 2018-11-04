#include "lt_mqttplayer.h"

LT_MQTTPlayer::LT_MQTTPlayer(QString deviceID, QMqttClient *client, QObject *parent) : LTPlayer(deviceID, parent),
 mqtt_client(client)
{
	qDebug()<<"New player"<<deviceID<<"created!";

	connect(this, &LTPlayer::zonesChanged, this,
			  [this](QList<QString> &entered, QList<QString> &exited) {
		if(mqtt_client->state() != QMqttClient::Connected)
			return;

		QVariantMap data;
		data["entered"] = QVariant(entered);
		data["exited"]	 = QVariant(exited);
		data["current"] = QVariant(this->currentZones);

		auto jsonObj = QJsonObject::fromVariantMap(data);
		auto jsonDoc = QJsonDocument(jsonObj);

		mqtt_client->publish(QString("Lasertag/Players/%1/ZoneUpdate").arg(this->deviceID), jsonDoc.toJson(), 1, 0);
	});
}

void LT_MQTTPlayer::setName(QString newName) {
	LTPlayer::setName(newName);
	mqtt_client->publish(QString("Lasertag/Players/%1/Name").arg(deviceID), newName.toUtf8(), 1, true);
}
void LT_MQTTPlayer::setIconURL(QString newIcon) {
	LTPlayer::setIconURL(newIcon);
	mqtt_client->publish(QString("Lasertag/Players/%1/Icon").arg(deviceID), newIcon.toUtf8(), 1, true);
}

void LT_MQTTPlayer::updatePosition(QVariantMap newPosition) {
    position = newPosition;
    emit positionChanged();

	auto jsonObj = QJsonObject::fromVariantMap(newPosition);
	auto jsonDoc = QJsonDocument(jsonObj);

	mqtt_client->publish(QString("Lasertag/Players/%1/Position").arg(deviceID), jsonDoc.toJson(), 1);
}

void LT_MQTTPlayer::mqtt_processData(QMqttMessage msg) {
	auto parameter = msg.topic().levels()[3];

	if(parameter == "Connection" && msg.topic().levelCount() == 4) {
		QString new_status = msg.payload();
		if(new_status != this->status) {
			this->status = new_status;
			emit statusChanged(status);

			qDebug()<<"Player"<<deviceID<<" - Status:"<<status;
		}
	}
	else if (parameter == "Name") {
		QString newName = msg.payload();
		if(newName == name)
			return;
		name = newName;
		emit nameChanged();
	}
	else if (parameter == "Icon") {
		QString newIcon = msg.payload();
		if(newIcon == icon)
			return;
		icon = newIcon;
		emit iconChanged();
	}
	else if(parameter == "System") {
		auto jsonDoc = QJsonDocument::fromJson(msg.payload()).object();
		if(jsonDoc.contains("battery"))
			battery = jsonDoc["battery"].toVariant().toFloat()/1000;
		if(jsonDoc.contains("ping"))
			ping = jsonDoc["ping"].toVariant().toFloat()/1000;

		emit systemDataChanged();
	}
	else if(parameter == "FireConf") {
		fireConfig = QJsonDocument::fromJson(msg.payload()).object().toVariantMap();
		emit fireConfigChanged();
	}
	else if(parameter == "Team") {
		auto newTeam = msg.payload().toInt();
		if(newTeam != team) {
			team = newTeam;
			emit teamChanged();
		}
	}
	else if(parameter == "HP") {
		auto oldLife = life;
		life = msg.payload().toFloat();
		emit lifeChanged(life, oldLife);
	}
	else if(parameter == "Ammo") {
		auto oldAmmo = ammo;
		ammo = msg.payload().toInt();
		emit ammoChanged(ammo, oldAmmo);
	}
	else if(parameter == "Position") {
		position = QJsonDocument::fromJson(msg.payload()).object().toVariantMap();
		emit positionChanged();
	}
}

void LT_MQTTPlayer::mqtt_onReconnected() {
	if(name != "")
		mqtt_client->publish(QString("Lasertag/Players/%1/Name").arg(deviceID), name.toUtf8(), 1, true);
	if(icon != "")
		mqtt_client->publish(QString("Lasertag/Players/%1/Icon").arg(deviceID), icon.toUtf8(), 1, true);
}
