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

class ReplicationManagerPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit ReplicationManagerPlugin(QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  struct ReplicationTarget {
    QString id;
    QString name;
    QString endpoint;
    QString type;
    bool enabled;
    QDateTime lastReplicated;
  };

  struct ReplicationStatus {
    QString targetId;
    QString targetName;
    QString state;
    int progress;
    QDateTime lastUpdate;
    QString message;
  };

  struct ReplicationHistoryEntry {
    QDateTime timestamp;
    QString targetId;
    QString targetName;
    QString result;
    int objectsReplicated;
    QString details;
  };

  struct ReplicationSetting {
    QString id;
    QString name;
    QString description;
    QString value;
    QString defaultValue;
  };

  void addTarget(const ReplicationTarget &target);
  void removeTarget(int index);
  int targetCount() const;

  void updateStatus(const ReplicationStatus &status);
  int statusCount() const;

  void addHistoryEntry(const ReplicationHistoryEntry &entry);
  int historyCount() const;

  void addSetting(const ReplicationSetting &setting);
  void updateSetting(int index, const QString &value);
  int settingCount() const;

  void exportReport(const QString &path);

  QTableWidget *targetTable() const;
  QTableWidget *statusTable() const;
  QTableWidget *historyTable() const;
  QTableWidget *settingsTable() const;
  QLabel *statusLabel() const;

signals:
  void targetAdded(const QString &targetId);
  void targetRemoved(const QString &targetId);

private:
  void buildUi();
  void rebuildTargetTable();
  void rebuildStatusTable();
  void rebuildHistoryTable();
  void rebuildSettingsTable();

  QWidget *containerWidget_ = nullptr;
  QTabWidget *tabs_ = nullptr;

  QTableWidget *targetTable_ = nullptr;
  QLineEdit *targetSearchEdit_ = nullptr;
  QPushButton *addTargetBtn_ = nullptr;
  QPushButton *removeTargetBtn_ = nullptr;

  QTableWidget *statusTable_ = nullptr;
  QPushButton *refreshStatusBtn_ = nullptr;

  QTableWidget *historyTable_ = nullptr;
  QPushButton *clearHistoryBtn_ = nullptr;

  QTableWidget *settingsTable_ = nullptr;
  QPushButton *saveSettingsBtn_ = nullptr;
  QPushButton *resetSettingsBtn_ = nullptr;
  QPushButton *exportReportBtn_ = nullptr;

  QLabel *statusLabel_ = nullptr;

  QVector<ReplicationTarget> targets_;
  QVector<ReplicationStatus> statuses_;
  QVector<ReplicationHistoryEntry> history_;
  QVector<ReplicationSetting> settings_;
};
