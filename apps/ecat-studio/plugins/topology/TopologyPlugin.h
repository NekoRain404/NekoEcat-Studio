#pragma once

// TopologyPlugin — workspace plugin that displays the EtherCAT bus topology
// as a graphical node graph.  Connects to EventBus::slaveChanged for live
// updates.  Provides linear and tree layout modes, zoom, and pan.
//
// This plugin provides a visual representation of the EtherCAT bus topology.
// It handles:
//   - Graphical display of slave nodes in a topology graph
//   - Linear and tree layout modes
//   - Zoom and pan capabilities
//   - Live updates via EventBus::slaveChanged
//   - Node interaction (click, double-click)
//   - Slave state visualization
//
// UI Features:
//   - TopologyGraphWidget for graphical node display
//   - Layout mode selector (Linear, Tree)
//   - Zoom controls
//   - Pan with mouse drag
//   - Node color coding by slave state
//
// Usage:
//   EventBus *bus = ...;
//   TopologyPlugin plugin(bus);
//   plugin.widget();  // Get the plugin's main widget
//   plugin.graphWidget();  // Get the graph widget
//   bus->emitSlaveChanged(slaves);  // Update topology
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. The plugin
//   uses EventBus for thread-safe communication.
//
// Performance:
//   - Graph rendering is O(n) where n is number of slaves
//   - Layout calculation is O(n) for linear, O(n log n) for tree
//   - Zoom/pan operations are O(1)

#include "EthercatTypes.h"
#include "plugins/WorkspacePlugin.h"

#include <QVector>

class QComboBox;
class QPushButton;
class TopologyGraphWidget;
class EventBus;

class TopologyPlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    explicit TopologyPlugin(EventBus* bus, QObject* parent = nullptr);

    // WorkspacePlugin identity
    QString id() const override;
    QString displayName() const override;
    QString displayNameZh() const override;
    QWidget* widget() override;
    int defaultOrder() const override;
    bool visible() const override;

    // Get the topology graph widget.
    // @return TopologyGraphWidget pointer
    TopologyGraphWidget* graphWidget() const { return graph_; }

private slots:
    // Handle slave list changes from EventBus.
    // @param slaves  Updated slave list
    void handleSlaveChanged(const QVector<SlaveInfo>& slaves);

private:
    // Build the plugin's UI.
    void buildUi();

    EventBus* bus_;                        // Event bus for slave updates
    QWidget* container_ = nullptr;         // Main container widget
    TopologyGraphWidget* graph_ = nullptr; // Topology graph widget
    QComboBox* layoutCombo_ = nullptr;     // Layout mode selector
};
