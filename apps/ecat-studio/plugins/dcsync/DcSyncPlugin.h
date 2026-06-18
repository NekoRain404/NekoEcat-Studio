#pragma once

// DcSyncPlugin — workspace plugin that displays per-slave DC synchronisation
// diagnostics.  Connects to EventBus::dcSyncUpdate for live data pushed via
// DcSyncService.

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
