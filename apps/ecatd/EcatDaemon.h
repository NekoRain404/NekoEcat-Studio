#pragma once

#include "EthercatCliBackend.h"
#include "FreeRunController.h"

#include <QHash>
#include <QObject>
#include <QTcpServer>

class QTcpSocket;

class EcatDaemon : public QObject {
    Q_OBJECT

public:
    explicit EcatDaemon(QObject *parent = nullptr);
    bool listen(quint16 port);

private slots:
    void acceptClient();
    void readClient();

private:
    void handle(QTcpSocket *socket, const QJsonObject &request);
    void send(QTcpSocket *socket, const QJsonObject &response);

    QTcpServer server_;
    EthercatCliBackend backend_;
    FreeRunController freeRun_;
    QHash<QTcpSocket *, QByteArray> buffers_;
};
