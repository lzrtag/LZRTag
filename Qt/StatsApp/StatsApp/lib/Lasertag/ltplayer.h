#ifndef LTPLAYER_H
#define LTPLAYER_H

#include <QObject>
#include <QVariantMap>

#include <QColor>

class LTPlayer : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString status READ getStatus	NOTIFY statusChanged)

	Q_PROPERTY(QString name	   READ getName		WRITE setName NOTIFY nameChanged)
	Q_PROPERTY(QString iconURL READ getIconURL	WRITE setIconURL NOTIFY iconChanged)

	Q_PROPERTY(float  battery READ getBattery NOTIFY systemDataChanged)
	Q_PROPERTY(float	ping	  READ getPing		NOTIFY systemDataChanged)

	Q_PROPERTY(int		team	READ getTeam	NOTIFY teamChanged)
	Q_PROPERTY(QColor	teamColor	READ getTeamColor	NOTIFY teamChanged)

	Q_PROPERTY(float	life	READ getLife	NOTIFY lifeChanged)
	Q_PROPERTY(int		ammo	READ getAmmo	NOTIFY ammoChanged)
	Q_PROPERTY(float	maxAmmo READ getMaxAmmo NOTIFY fireConfigChanged)

protected:
	QString status;

	QString name;
	QString icon;

	QVariantMap fireConfig;

	float battery;
	float ping;

	int	team;

	float life;
	int	ammo;

public:
	explicit LTPlayer(QString deviceID, QObject *parent = nullptr);

	const QString deviceID;

	QString getStatus();

	QString getName();
	virtual void	  setName(QString newName);
	QString getIconURL();
	virtual void	  setIconURL(QString newIcon);

	float		getBattery();
	float		getPing();

	int		getTeam();
	QColor	getTeamColor();

	float getLife();
	int	getAmmo();
	int	getMaxAmmo();

signals:
	void statusChanged(QString status);

	void nameChanged();
	void iconChanged();

	void systemDataChanged();
	void fireConfigChanged();

	void teamChanged(int newTeam);

	void lifeChanged(float life);
	void ammoChanged(int ammo);

public slots:
};

#endif // LTPLAYER_H
