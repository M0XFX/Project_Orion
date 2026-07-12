#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "Orion.h"


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    HFSDR::Orion orion;

    engine.rootContext()->setContextProperty("orion", &orion);


    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection
        );

    engine.loadFromModule("HF_SDR_Display", "Main");

    return app.exec();
}
