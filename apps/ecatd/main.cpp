// ecatd entry point: starts the runtime daemon event loop.
#include "EcatDaemon.h"

#include <QCoreApplication>
#include <QDebug>

int main(int argc, char* argv[]) {
    // Headless daemon that bridges local TCP clients to the IgH EtherCAT stack.
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("ecatd");
    QCoreApplication::setApplicationVersion("0.1.0");

    EcatDaemon daemon;
    if (!daemon.listen(5877)) {
        qCritical() << "Failed to listen on 127.0.0.1:5877";
        return 1;
    }

    qInfo() << "ecatd listening on 127.0.0.1:5877";
    return app.exec();
}
