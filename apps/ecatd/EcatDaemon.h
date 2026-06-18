// ecatd runtime daemon: TCP server, command dispatch, and master lifecycle.

#pragma once
#include "CommandDispatcher.h"

// ecatd runtime daemon: TCP server, command dispatch, and master lifecycle.


#include "EthercatCliBackend.h"
#include "FreeRunController.h"
#include "RtTestController.h"
#include "handlers/AlEventHandler.h"

#include <QHash>
#include <QObject>
#include <QTcpServer>

class QTimer;
class QTcpSocket;

class EcatDaemon : public QObject {
    Q_OBJECT

    // Local TCP daemon that multiplexes JSON commands over IgH CLI and ecrt APIs.
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
    // CLI-based backend for non-real-time EtherCAT operations (scan, SDO, state).
    EcatService *backend_ = nullptr;
    // ecrt-based controller for real-time process data I/O in Free Run mode.
    FreeRunController freeRun_;
    // ecrt-based controller for real-time cycle timing / stability test.
    RtTestController rtTest_;
    // Per-socket read buffers for reassembling fragmented TCP into complete JSON lines.
    QHash<QTcpSocket *, QByteArray> buffers_;
    CommandDispatcher dispatcher_;
    // Tracks AL status changes across all slaves.
    AlEventHandler alEventHandler_;
    // Periodic timer that polls slave AL status every second.
    QTimer *alPollTimer_ = nullptr;
    void setupHandlers();
};
