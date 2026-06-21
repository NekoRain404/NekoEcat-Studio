#pragma once

// MultiMasterPlugin — workspace plugin for managing multiple EtherCAT masters.
// Provides master list, details, comparison, and synchronization.

#include "plugins/WorkspacePlugin.h"

class QLabel;
class QPushButton;
class QTableWidget;
class QTabWidget;
class QPlainTextEdit;
class MasterComparisonWidget;
class MultiMasterService;

class MultiMasterPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit MultiMasterPlugin(MultiMasterService *service,
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

  MultiMasterService *service() const { return service_; }
  int masterCount() const;
  int selectedMasterId() const;

private:
  void buildUi();
  void refreshMasterList();
  void updateMasterDetails(int masterId);
  void exportReport();

  MultiMasterService *service_;
  QWidget *containerWidget_ = nullptr;
  QTabWidget *tabWidget_ = nullptr;
  QTableWidget *masterListTable_ = nullptr;
  QTableWidget *detailTable_ = nullptr;
  MasterComparisonWidget *comparisonWidget_ = nullptr;
  QPlainTextEdit *syncLog_ = nullptr;
  QPushButton *refreshBtn_ = nullptr;
  QPushButton *addMasterBtn_ = nullptr;
  QPushButton *removeMasterBtn_ = nullptr;
  QPushButton *syncBtn_ = nullptr;
  QPushButton *exportBtn_ = nullptr;
  QLabel *statusLabel_ = nullptr;
  int selectedMasterId_ = -1;
};
