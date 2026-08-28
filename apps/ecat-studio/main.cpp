// Application entry point: creates QApplication and shows MainWindow.
#include "MainWindow.h"

#include <QApplication>
#include <QTranslator>
#include <QLocale>

int main(int argc, char *argv[]) {
  // GUI entry point for NekoEcat Studio — a Qt-based EtherCAT commissioning tool.
  QApplication app(argc, argv);
  QApplication::setApplicationName("NekoEcat Studio");

  // Load Qt translation file for the system locale.
  // The .ts/.qm files are named nekoecat_<lang>.qm (e.g. nekoecat_zh.qm) and
  // embedded under the /i18n resource prefix.  Try the full locale name first
  // (matches "zh_TW"), then fall back to the bare ISO 639-1 code ("zh_CN" → "zh").
  static QTranslator translator;
  QString lang = QLocale::system().name(); // e.g. "zh_CN"
  bool loaded = translator.load("nekoecat_" + lang, ":/i18n");
  if (!loaded) {
    const int underscore = lang.indexOf(QLatin1Char('_'));
    if (underscore > 0) {
      loaded = translator.load("nekoecat_" + lang.left(underscore), ":/i18n");
    }
  }
  if (loaded) {
    app.installTranslator(&translator);
  }
  QApplication::setApplicationVersion("1.0.0");

  MainWindow window;
  window.show();

  return app.exec();
}
