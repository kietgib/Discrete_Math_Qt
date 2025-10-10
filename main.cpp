#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "coursegraph.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    qmlRegisterType<CourseGraph>("App", 1, 0, "CourseGraph");

    engine.load(QUrl(QStringLiteral("qrc:/qml/Main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;
    return app.exec();
}
