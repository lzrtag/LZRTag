#ifndef LTPLAYER_H
#define LTPLAYER_H

#include <QObject>
#include <QVariantMap>

#include <QPointF>

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

	Q_PROPERTY(QVariantMap position MEMBER position WRITE updatePosition NOTIFY positionChanged)
	Q_PROPERTY(QList<QString> zones READ getZones NOTIFY zonesChanged)

	Q_PROPERTY(QVariantMap zoneData READ getZoneData NOTIFY zonesChanged)
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
	int	maxAmmo;

	QVariantMap position;
	QList<QString> currentZones;
	QVariantMap zoneData;
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

	Q_INVOKABLE QPointF getMapPosition();
	virtual void updatePosition(QVariantMap posData);
	Q_INVOKABLE void updateZones(QList<QString> newZones, QVariantMap zoneData);
	const QList<QString>& getZones();
	const QVariantMap&  getZoneData();

signals:
	void statusChanged(QString status);

	void nameChanged();
	void iconChanged();

	void systemDataChanged();
	void fireConfigChanged();

	void teamChanged();

	void lifeChanged(float newLife, float oldLife);
	void ammoChanged(int newAmmo, int oldAmmo);

	void positionChanged();
	void zonesChanged(QList<QString> &entered, QList<QString> &left);

public slots:
};

#endif // LTPLAYER_H
