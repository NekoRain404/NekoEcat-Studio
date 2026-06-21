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

class DcSyncPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit DcSyncPlugin(EventBus *bus, DcSyncService *service,
                        QObject *parent = nullptr);

  // WorkspacePlugin identity
  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

private slots:
  void handleDcSyncUpdate(const QJsonObject &data);

private:
  void buildUi();
  void populateTable(const QJsonObject &data);

  EventBus *bus_;
  DcSyncService *service_;
  QWidget *container_ = nullptr;
  QTableWidget *table_ = nullptr;
};
