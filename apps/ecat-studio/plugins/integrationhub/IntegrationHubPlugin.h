#pragma once

#include "plugins/WorkspacePlugin.h"

#include <QDateTime>
#include <QVector>

class QLabel;
class QLineEdit;
class QComboBox;
class QPushButton;
class QTabWidget;
class QTableWidget;
class QTextEdit;

class IntegrationHubPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit IntegrationHubPlugin(QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  struct SystemConnection {
    QString id;
    QString name;
    QString type;
    QString endpoint;
    QString status;
    QDateTime lastSync;
  };

  struct DataMapping {
    QString id;
    QString source;
    QString destination;
    QString transformation;
    bool enabled;
  };

  struct SyncStatus {
    QString connectionId;
    QString connectionName;
    QString state;
    int progress;
    QDateTime lastUpdate;
    QString message;
  };

  struct IntegrationLog {
    QDateTime timestamp;
    QString source;
    QString level;
    QString message;
    QString details;
  };

  void addConnection(const SystemConnection &conn);
  void removeConnection(int index);
  int connectionCount() const;

  void addMapping(const DataMapping &mapping);
  void removeMapping(int index);
  int mappingCount() const;

  void updateSyncStatus(const SyncStatus &status);
  int syncStatusCount() const;

  void addLog(const IntegrationLog &log);
  int logCount() const;
  void filterLogs(const QString &level, const QString &source);

  void exportReport(const QString &path);

  QTableWidget *connectionTable() const;
  QTableWidget *mappingTable() const;
  QTableWidget *syncTable() const;
  QTableWidget *logTable() const;
  QLabel *statusLabel() const;

signals:
  void connectionAdded(const QString &connId);
  void connectionRemoved(const QString &connId);

private:
  void buildUi();
  void rebuildConnectionTable();
  void rebuildMappingTable();
  void rebuildSyncTable();
  void rebuildLogTable();

  QWidget *containerWidget_ = nullptr;
  QTabWidget *tabs_ = nullptr;

  QTableWidget *connectionTable_ = nullptr;
  QLineEdit *connSearchEdit_ = nullptr;
  QPushButton *addConnBtn_ = nullptr;
  QPushButton *removeConnBtn_ = nullptr;
  QPushButton *testConnBtn_ = nullptr;

  QTableWidget *mappingTable_ = nullptr;
  QPushButton *addMappingBtn_ = nullptr;
  QPushButton *removeMappingBtn_ = nullptr;

  QTableWidget *syncTable_ = nullptr;
  QPushButton *refreshSyncBtn_ = nullptr;

  QTableWidget *logTable_ = nullptr;
  QComboBox *logLevelFilter_ = nullptr;
  QLineEdit *logSourceFilter_ = nullptr;
  QPushButton *filterLogBtn_ = nullptr;
  QPushButton *exportReportBtn_ = nullptr;

  QLabel *statusLabel_ = nullptr;

  QVector<SystemConnection> connections_;
  QVector<DataMapping> mappings_;
  QVector<SyncStatus> syncStatuses_;
  QVector<IntegrationLog> logs_;
  QVector<int> filteredLogs_;
};
