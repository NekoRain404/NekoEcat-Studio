#pragma once

/// @brief OPC UA server that exposes EtherCAT bus data.
///
/// @details Listens on a configurable TCP port (default 4840) and exposes
/// EtherCAT slave information, topology, and SDO values as OPC UA nodes.
/// Uses the open62541 library (single-calc C99).
///
/// Phase 1 — Slave list and status only:
///   Objects → EtherCAT → Master0 → Slaves → Slave[i] → {Name, State, Position, Vendor}
///
/// Thread safety:
///   open62541 runs its own network thread.  Data updates are marshalled
///   from the main thread via QTimer + mutex-protected shared state.

#include <QObject>
#include <QVector>
#include <QMutex>
#include "EthercatTypes.h"

struct UA_Server;
struct UA_ServerConfig;

class EcatClient;

class OpcUaServer : public QObject {
    Q_OBJECT
public:
    explicit OpcUaServer(EcatClient *client, QObject *parent = nullptr);
    ~OpcUaServer() override;

    /// Start the OPC UA server on the given port.
    /// Returns true if the server started successfully.
    bool start(quint16 port = 4840);

    /// Stop the OPC UA server.
    void stop();

    bool isRunning() const { return running_; }

    /// Update the exposed slave list (called from topology change).
    void updateSlaves(const QVector<SlaveInfo> &slaves);

signals:
    void serverStarted(quint16 port);
    void serverStopped();

private:
    void setupObjectTypes();
    void addSlaveNodes();
    void removeSlaveNodes();

    EcatClient *client_;
    UA_Server *server_ = nullptr;
    UA_ServerConfig *config_ = nullptr;
    QVector<SlaveInfo> slaves_;
    mutable QMutex mutex_;
    bool running_ = false;
    quint16 port_ = 4840;
};