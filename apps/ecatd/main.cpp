#include "EcatDaemon.h"

#include <QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
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
