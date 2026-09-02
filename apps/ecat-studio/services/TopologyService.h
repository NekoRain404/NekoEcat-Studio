#pragma once

// TopologyService — manages bus scanning, slave info, and topology baseline.
//
// This service provides the primary interface for EtherCAT bus topology operations.
// It handles:
//   - Bus scanning to discover connected slaves
//   - Slave information retrieval and caching
//   - Topology baseline capture and comparison
//   - Change detection between current and baseline topology
//
// Usage:
//   ServiceContainer *container = ...;
//   TopologyService *topo = container->topology();
//   topo->scan();  // Scan bus for slaves
//   QVector<SlaveInfo> slaves = topo->currentSlaves();
//   topo->captureBaseline();  // Save current topology as baseline
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. The service
//   marshals daemon communication to the main thread internally.
//
// Performance:
//   - Scan results are cached with configurable TTL (default 60s)
//   - Baseline comparison is O(n) where n is number of slaves
//   - Topology changes are detected via EventBus notifications

#include "EthercatTypes.h"
#include <QObject>
#include <QVector>

class EcatClient;

class TopologyService : public QObject {
    Q_OBJECT
public:
    explicit TopologyService(EcatClient* client, QObject* parent = nullptr);

    // Scan the EtherCAT bus for connected slaves.
    // Emits scanComplete() with the discovered slaves.
    void scan();

    // Force a rescan of the EtherCAT bus, ignoring cached results.
    // Emits scanComplete() with the discovered slaves.
    void rescan();

    // Get the current list of slaves from the last scan.
    // @return Vector of SlaveInfo structures
    QVector<SlaveInfo> currentSlaves() const;

    // Get the baseline topology for comparison.
    // @return Vector of SlaveInfo structures from baseline capture
    QVector<SlaveInfo> baselineSlaves() const;

    // Capture the current topology as the baseline for change detection.
    // Emits baselineChanged() after capture.
    void captureBaseline();

    // Clear the captured baseline topology.
    // Emits baselineChanged() after clearing.
    void clearBaseline();

    // Check if a baseline topology has been captured.
    // @return true if baseline is available
    bool baselineCaptured() const;

signals:
    // Emitted when a bus scan completes successfully.
    // @param slaves  Vector of discovered slave information
    void scanComplete(const QVector<SlaveInfo>& slaves);

    // Emitted when the baseline topology is captured or cleared.
    void baselineChanged();

private:
    EcatClient* client_;            // TCP client to ecatd daemon
    QVector<SlaveInfo> slaves_;     // Current slave list from last scan
    QVector<SlaveInfo> baseline_;   // Captured baseline topology
    bool baselineCaptured_ = false; // Whether baseline has been captured
};
