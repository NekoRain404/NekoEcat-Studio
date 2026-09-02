#pragma once

/// @brief Workspace plugin for Distributed Clock (DC) synchronization diagnostics.
///
/// @details The DC Sync workspace monitors the synchronization state of all
/// slaves on the EtherCAT bus using the Distributed Clock mechanism. It
/// displays per-slave synchronization metrics including drift, jitter, and
/// reference clock status.
///
/// Features:
///   - **Per-slave DC sync status table**: Shows position, name, reference
///     clock flag, drift (ns), jitter (ns), and sync state for each slave.
///   - **Reference clock identification**: Highlights which slave is the
///     system reference clock.
///   - **Drift and jitter statistics**: Real-time drift and jitter
///     measurements per slave.
///   - **Real-time updates**: Data is updated via EventBus::dcSyncUpdate
///     signals from DcSyncService (default 2s polling interval).
///
/// @par Constructor
///   DcSyncPlugin(EventBus *bus, DcSyncService *service, QObject *parent = nullptr)
///   Uses fine-grained injection pattern.
///
/// @par Plugin Identity
///   - id: "dcsync"
///   - defaultOrder: 60
///   - visible: always true
///
/// @see WorkspacePlugin, DcSyncService, EventBus

#include "plugins/WorkspacePlugin.h"

#include <QJsonObject>

class QTableWidget;
class EventBus;
class DcSyncService;

/// @brief Workspace plugin for Distributed Clock (DC) synchronization diagnostics.
class DcSyncPlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    /// Constructs the DC Sync plugin with fine-grained service injection.
    /// @param bus      EventBus for receiving DC sync update signals
    /// @param service  DcSyncService for DC synchronization data
    /// @param parent   Qt parent object (typically MainWindow)
    explicit DcSyncPlugin(EventBus* bus, DcSyncService* service, QObject* parent = nullptr);

    // WorkspacePlugin identity
    QString id() const override;            ///< Returns "dcsync"
    QString displayName() const override;   ///< Returns "DC Sync"
    QString displayNameZh() const override; ///< Returns "分布式时钟"
    QWidget* widget() override;             ///< Returns the root container widget
    int defaultOrder() const override;      ///< Returns 60
    bool visible() const override;          ///< Returns true (always visible)

private slots:
    void handleDcSyncUpdate(const QJsonObject& data); ///< Handles incoming DC sync data from EventBus

private:
    void buildUi();                              ///< Builds the DC sync status table layout
    void populateTable(const QJsonObject& data); ///< Parses JSON and fills the table rows

    EventBus* bus_;
    DcSyncService* service_;
    QWidget* container_ = nullptr;
    QTableWidget* table_ = nullptr;
};
