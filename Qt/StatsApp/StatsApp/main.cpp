#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QTextToSpeech>

#include <QQuickStyle>
#include <QQmlContext>

#include "lib/Lasertag/lt_mqtthandler.h"

#include "lib/Lasertag/ltmap.h"

int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	QGuiApplication app(argc, argv);

	QQuickStyle::setStyle("Material");

	QQmlApplicationEngine engine;

	QTextToSpeech  tts(nullptr);
	LT_MQTTHandler game(nullptr);

	LTMap testMap(nullptr);
	qDebug()<<"Testing zone conversion:"<<testMap.latLonToXY(QPointF(9.717405, 52.387669));

	engine.rootContext()->setContextProperty("ttsEngine", &tts);

	qmlRegisterType<LTPlayer>();
	engine.rootContext()->setContextProperty("gameHandler", &game);

	qmlRegisterSingletonType(QUrl("qrc:/qml/GameHandle.qml"), "xasin.lasertag.gamehandle", 1, 0, "GameHandle");

	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
	if (engine.rootObjects().isEmpty())
		return -1;

	return app.exec();
}
