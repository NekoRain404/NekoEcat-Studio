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

class SyncManagerPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit SyncManagerPlugin(QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  struct SyncStatusEntry {
    QString id;
    QString name;
    QString type;
    QString state;
    int progress;
    QDateTime lastSync;
    QString message;
  };

  struct SyncHistoryEntry {
    QDateTime timestamp;
    QString syncId;
    QString name;
    QString result;
    int duration;
    QString details;
  };

  struct SyncSetting {
    QString id;
    QString name;
    QString description;
    QString value;
    QString defaultValue;
  };

  struct SyncLog {
    QDateTime timestamp;
    QString source;
    QString level;
    QString message;
  };

  void addStatus(const SyncStatusEntry &status);
  void removeStatus(int index);
  int statusCount() const;

  void addHistoryEntry(const SyncHistoryEntry &entry);
  int historyCount() const;

  void addSetting(const SyncSetting &setting);
  void updateSetting(int index, const QString &value);
  int settingCount() const;

  void addLog(const SyncLog &log);
  int logCount() const;
  void filterLogs(const QString &level, const QString &source);

  bool exportReport(const QString &path);

  QTableWidget *statusTable() const;
  QTableWidget *historyTable() const;
  QTableWidget *settingsTable() const;
  QTableWidget *logTable() const;
  QLabel *statusLabel() const;

signals:
  void syncStatusChanged(const QString &syncId, const QString &state);

private:
  void buildUi();
  void rebuildStatusTable();
  void rebuildHistoryTable();
  void rebuildSettingsTable();
  void rebuildLogTable();

  QWidget *containerWidget_ = nullptr;
  QTabWidget *tabs_ = nullptr;

  QTableWidget *statusTable_ = nullptr;
  QPushButton *refreshStatusBtn_ = nullptr;

  QTableWidget *historyTable_ = nullptr;
  QLineEdit *historySearchEdit_ = nullptr;
  QPushButton *clearHistoryBtn_ = nullptr;

  QTableWidget *settingsTable_ = nullptr;
  QPushButton *saveSettingsBtn_ = nullptr;
  QPushButton *resetSettingsBtn_ = nullptr;

  QTableWidget *logTable_ = nullptr;
  QComboBox *logLevelFilter_ = nullptr;
  QLineEdit *logSourceFilter_ = nullptr;
  QPushButton *filterLogBtn_ = nullptr;
  QPushButton *exportReportBtn_ = nullptr;

  QLabel *statusLabel_ = nullptr;

  QVector<SyncStatusEntry> statuses_;
  QVector<SyncHistoryEntry> history_;
  QVector<SyncSetting> settings_;
  QVector<SyncLog> logs_;
  QVector<int> filteredLogs_;
};
