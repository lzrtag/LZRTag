#include "ltplayer.h"

LTPlayer::LTPlayer(QMqttClient * client, QObject *parent) : 	QObject(parent),
	mqtt_client(client),
	status(""),
	life(0), ammo(0)
{
	if(client->state() == QMqttClient::Connected)
		this->mqtt_resubscribe();
}
