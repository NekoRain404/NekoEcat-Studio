// Application entry point: creates QApplication and shows MainWindow.
#include "MainWindow.h"

#include <QApplication>
#include <QTranslator>
#include <QLocale>

int main(int argc, char *argv[]) {
  // GUI entry point for NekoEcat Studio — a Qt-based EtherCAT commissioning tool.
  QApplication app(argc, argv);
  QApplication::setApplicationName("NekoEcat Studio");

  // Load Qt translation file for the system locale
  static QTranslator translator;
  QString lang = QLocale::system().name(); // e.g. "zh_CN"
  if (translator.load("nekoecat_" + lang, ":/translations")) {
      app.installTranslator(&translator);
  }
  QApplication::setApplicationVersion("1.0.0");

  MainWindow window;
  window.show();

  return app.exec();
}
