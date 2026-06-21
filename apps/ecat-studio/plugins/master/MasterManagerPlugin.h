#pragma once

// MasterManagerPlugin — workspace plugin for EtherCAT master management.
// Provides master info display, configuration editor, diagnostics panel,
// restart button, and log viewer.

#include "plugins/WorkspacePlugin.h"

class QLabel;
class QPushButton;
class QTableWidget;
class QPlainTextEdit;
class MasterManagerService;
class DistributedClockService;

class MasterManagerPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit MasterManagerPlugin(MasterManagerService *masterService,
                               DistributedClockService *dcService,
                               QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QIcon icon() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  MasterManagerService *masterService() const { return masterService_; }
  DistributedClockService *dcService() const { return dcService_; }

private:
  void buildUi();
  void updateMasterInfo();
  void updateDiagnostics();
  void appendLog(const QString &message);

  MasterManagerService *masterService_;
  DistributedClockService *dcService_;
  QWidget *container_ = nullptr;
  QLabel *stateLabel_ = nullptr;
  QLabel *adapterLabel_ = nullptr;
  QLabel *slaveCountLabel_ = nullptr;
  QLabel *errorCountLabel_ = nullptr;
  QTableWidget *infoTable_ = nullptr;
  QPushButton *diagnoseBtn_ = nullptr;
  QPushButton *restartBtn_ = nullptr;
  QPlainTextEdit *logViewer_ = nullptr;
};
