
#include "ltplayer.h"
#include <QDebug>

LTPlayer::LTPlayer(QString deviceID, QObject *parent) : 	QObject(parent),
	status(""),
	name(""), icon(""),
	battery(0), ping(0),
	team(0),
	life(0), ammo(0), maxAmmo(0),
	position(), currentZones(), zoneData(),
	deviceID(deviceID)
{
}

QString LTPlayer::getStatus() {
	if(status == "")
		return "Offline";
	return status;
}

QString LTPlayer::getName() {
	if(name == "")
		return deviceID;
	return name;
}
void LTPlayer::setName(QString newName) {
	this->name = newName;
	emit nameChanged();
}
QString LTPlayer::getIconURL() {
	return icon;
}
void LTPlayer::setIconURL(QString newIcon) {
	this->icon = newIcon;
	emit iconChanged();
}

float LTPlayer::getBattery() {
	return battery;
}
float LTPlayer::getPing() {
	return ping;
}

int LTPlayer::getTeam() {
	return team;
}
QColor LTPlayer::getTeamColor() {
	const QColor colors[] = {
		"#607D8B",
		"#F44336",
		"#4CAF50",
		"#FFC107",
		"#3F51B5",
		"#9C27B0",
		"#00BCD4",
		"#BEBEBE"
	};

	if(team > 0 && team < 8)
		return colors[team];

	return colors[0];
}

float LTPlayer::getLife() {
	return life;
}
int LTPlayer::getAmmo() {
	return ammo;
}
int LTPlayer::getMaxAmmo() {
	return maxAmmo;
}

QPointF LTPlayer::getMapPosition() {
    return QPointF(position["x"].toDouble(), position["y"].toDouble());
}
void LTPlayer::updatePosition(QVariantMap newPosition) {
	this->position = newPosition;
	emit positionChanged();
}
void LTPlayer::updateZones(QList<QString> newZones, QVariantMap zoneData) {
	QList<QString> enteredZones(newZones);
	QList<QString> leftZones(currentZones);

	for(auto z : newZones) {
		leftZones.removeAll(z);
	}
	for(auto z : currentZones) {
		enteredZones.removeAll(z);
	}

	if(!leftZones.empty() || !enteredZones.empty()) {
		currentZones = newZones;
		this->zoneData = zoneData;
		emit zonesChanged(enteredZones, leftZones);
	}
}
const QList<QString>& LTPlayer::getZones() {
	return currentZones;
}
const QVariantMap& LTPlayer::getZoneData() {
	return zoneData;
}
