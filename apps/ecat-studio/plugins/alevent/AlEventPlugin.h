#pragma once

// AlEventPlugin — workspace plugin that displays Application-Layer (AL) event
// log entries from the EtherCAT master.  Supports severity filtering and
// auto-scrolls to the latest event.  Connects to EventBus for live updates
// pushed through AlEventService.

#include "plugins/WorkspacePlugin.h"

#include <QJsonObject>

class QTableWidget;
class QComboBox;
class EventBus;
class AlEventService;

class AlEventPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit AlEventPlugin(EventBus *bus, AlEventService *service,
                         QObject *parent = nullptr);

  // WorkspacePlugin identity
  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

private slots:
  void handleAlEventUpdate(const QJsonObject &data);
  void applySeverityFilter();

private:
  // Build the toolbar (severity combo + clear button) and event table.
  void buildUi();
  // Parse the JSON payload and append rows to the table.
  void populateTable(const QJsonObject &data);
  // Re-apply the current severity filter to every row.
  void updateFilterVisibility();

  EventBus *bus_;
  AlEventService *service_;
  QWidget *container_     = nullptr;
  QTableWidget *table_    = nullptr;
  QComboBox *filterCombo_ = nullptr;
};
