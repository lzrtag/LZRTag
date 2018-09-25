#ifndef LTPLAYER_H
#define LTPLAYER_H

#include <QObject>
#include <QtMqtt>

class LTPlayer : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString status READ getStatus NOTIFY statusChanged)

	Q_PROPERTY(float	life READ getLife	NOTIFY lifeChanged)
	Q_PROPERTY(int		ammo READ getAmmo	NOTIFY ammoChanged)

private:
	QMqttClient * mqtt_client;
	QMqttSubscription * mqtt_playerSubscription;

	QString status;

	float life;
	int	ammo;

public:
	explicit LTPlayer(QMqttClient * client, QObject *parent = nullptr);

	QString getStatus();

	float getLife();
	int	getAmmo();

signals:
	void lifeChanged(float life);
	void ammoChanged(int ammo);

public slots:
	void mqtt_resubscribe();
	void mqtt_dataReceived(QMqttMessage msg);
};

#endif // LTPLAYER_H
