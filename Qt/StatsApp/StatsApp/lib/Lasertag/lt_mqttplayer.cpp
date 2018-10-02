#include "lt_mqttplayer.h"

LT_MQTTPlayer::LT_MQTTPlayer(QString deviceID, QMqttClient *client, QObject *parent) : LTPlayer(deviceID, parent),
 mqtt_client(client)
{
	qDebug()<<"New player"<<deviceID<<"created!";
}

void LT_MQTTPlayer::setName(QString newName) {
	LTPlayer::setName(newName);
	mqtt_client->publish(QString("Lasertag/Players/%1/Name").arg(deviceID), newName.toUtf8(), 1, true);
}
void LT_MQTTPlayer::setIconURL(QString newIcon) {
	LTPlayer::setIconURL(newIcon);
	mqtt_client->publish(QString("Lasertag/Players/%1/Icon").arg(deviceID), newIcon.toUtf8(), 1, true);
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
	else if (parameter == "icon") {
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
			emit teamChanged(team);
		}
	}
	else if(parameter == "HP") {
		life = msg.payload().toFloat();
		emit lifeChanged(life);
	}
	else if(parameter == "Ammo") {
		ammo = msg.payload().toInt();
		emit ammoChanged(ammo);
	}
}

void LT_MQTTPlayer::mqtt_onReconnected() {
	mqtt_client->publish(QString("Lasertag/Players/%1/Name").arg(deviceID), name.toUtf8(), 1, true);
	mqtt_client->publish(QString("Lasertag/Players/%1/Icon").arg(deviceID), icon.toUtf8(), 1, true);
}
