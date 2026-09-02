#pragma once

/// @brief OPC UA server that exposes EtherCAT bus data.
///
/// @details Listens on a configurable TCP port (default 4840) and exposes
/// EtherCAT slave information, topology, and SDO values as OPC UA nodes.
/// Uses the open62541 library.
///
/// Phase 2 — Slave info + SDO value exposure with periodic polling:
///   Objects → EtherCAT → Master0 → Slaves → Slave[i] →
///     {Name, State, Position, Flags}           (static, from topology)
///     SDO → {DeviceType, ErrorRegister, ...}  (live, polled from bus)
///
/// Thread safety:
///   open62541 runs its own network thread. Data updates are marshalled
///   from the main thread via QTimer + mutex-protected shared state.

#include "EthercatTypes.h"
#include <open62541.h>
#include <QHash>
#include <QMutex>
#include <QObject>
#include <QPair>
#include <QString>
#include <QTimer>
#include <QVector>

class EcatClient;

/// One monitored SDO entry for periodic polling.
struct MonitoredSdo {
    int slavePos = 0;
    QString index;
    QString subIndex;
    QString type;
    QString lastValue;
};

class OpcUaServer : public QObject {
    Q_OBJECT
public:
    explicit OpcUaServer(EcatClient* client, QObject* parent = nullptr);
    ~OpcUaServer() override;

    /// Start the OPC UA server on the given port.
    bool start(quint16 port = 4840);
    /// Stop the OPC UA server.
    void stop();
    bool isRunning() const { return running_; }

    /// Update the exposed slave list (called from topology change).
    void updateSlaves(const QVector<SlaveInfo>& slaves);

signals:
    void serverStarted(quint16 port);
    void serverStopped();

private:
    void setupObjectTypes();
    void addSlaveNodes();
    void addSdoNodes(int slaveIndex, unsigned int nodeBase, const UA_NodeId& slaveNodeId);
    void removeSlaveNodes();
    void setupSdoPolling();
    void pollSdoValues();

    EcatClient* client_;
    UA_Server* server_ = nullptr;
    UA_ServerConfig* config_ = nullptr;
    QVector<SlaveInfo> slaves_;
    mutable QMutex mutex_;
    bool running_ = false;
    quint16 port_ = 4840;
    QTimer* sdoPollTimer_ = nullptr;

    // SDO value cache: "slave:index:subIndex" → value string
    QHash<QString, QString> sdoCache_;
    mutable QMutex sdoCacheMutex_;

    // List of SDO entries to poll
    QVector<MonitoredSdo> monitoredSdos_;
    // Maps SDO cache key → (nodeBase + sdoIndex) for value updates
    QHash<QString, QPair<unsigned int, int>> sdoNodeMap_;
};