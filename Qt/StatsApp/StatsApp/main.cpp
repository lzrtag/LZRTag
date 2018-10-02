#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QQuickStyle>
#include <QQmlContext>

#include "lib/Lasertag/lt_mqtthandler.h"

int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	QGuiApplication app(argc, argv);

	QQuickStyle::setStyle("Material");

	QQmlApplicationEngine engine;

	LT_MQTTHandler game(nullptr);

	qmlRegisterSingletonType(QUrl("qrc:/qml/GameHandle.qml"), "xasin.lasertag.gamehandle", 1, 0, "GameHandle");

	qmlRegisterType<LTPlayer>();
	engine.rootContext()->setContextProperty("gameHandler", &game);

	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
	if (engine.rootObjects().isEmpty())
		return -1;

	return app.exec();
}
