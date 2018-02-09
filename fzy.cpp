#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "FzyModel.h"

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);

  qmlRegisterType<FzyModel>("com.github.Sarcasm.fzy", 1, 0, "FzyModel");

  QCoreApplication::addLibraryPath("./");

  QQmlApplicationEngine engine;
  engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

  return app.exec();
}
