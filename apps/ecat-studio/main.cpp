// Application entry point: creates QApplication and shows MainWindow.
#include "MainWindow.h"

#include <QApplication>

int main(int argc, char *argv[]) {
  // GUI entry point for NekoEcat Studio — a Qt-based EtherCAT commissioning tool.
  QApplication app(argc, argv);
  QApplication::setApplicationName("NekoEcat Studio");
  QApplication::setApplicationVersion("0.1.0");

  MainWindow window;
  window.show();

  return app.exec();
}
