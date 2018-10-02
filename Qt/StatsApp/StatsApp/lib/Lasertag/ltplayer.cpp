#include "ltplayer.h"

LTPlayer::LTPlayer(QString deviceID, QObject *parent) : 	QObject(parent),
	status(""),
	name(""), icon(""),
	battery(0), ping(0),
	team(0),
	life(0), ammo(0),
	deviceID(deviceID)
{
}

QString LTPlayer::getStatus() {
	if(status == "")
		return "Offline";
	return status;
}

QString LTPlayer::getName() {
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
	if(fireConfig.contains("ammoCap"))
		return fireConfig["ammoCap"].toInt();
	return 5;
}
