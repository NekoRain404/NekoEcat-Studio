#pragma once

#include "plugins/WorkspacePlugin.h"

#include <QDateTime>
#include <QVector>

class QLabel;
class QLineEdit;
class QPushButton;
class QTabWidget;
class QTableWidget;

class AlarmManagerPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit AlarmManagerPlugin(QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;

  struct AlarmRule {
    QString id;
    QString name;
    QString condition;
    QString severity;
    bool enabled;
    QDateTime createdAt;
  };

  struct AlarmRecord {
    QString ruleId;
    QString message;
    QString severity;
    QString channel;
    QDateTime timestamp;
    bool acknowledged;
  };

  struct EscalationPolicy {
    QString id;
    QString name;
    int delaySeconds;
    QString targetChannel;
    QString severity;
  };

  void addRule(const AlarmRule &rule);
  void removeRule(int index);
  int ruleCount() const;

  void addRecord(const AlarmRecord &record);
  void removeRecord(int index);
  int recordCount() const;

  void acknowledgeRecord(int index);
  void filterRecords(const QString &query);

  void addChannel(const QString &channel);
  void removeChannel(int index);
  int channelCount() const;

  void addEscalationPolicy(const EscalationPolicy &policy);
  void removeEscalationPolicy(int index);
  int escalationPolicyCount() const;

  QString exportAlarmData() const;

  QTabWidget *tabs() const;
  QTableWidget *rulesTable() const;
  QTableWidget *historyTable() const;
  QTableWidget *channelsTable() const;
  QTableWidget *escalationTable() const;
  QLabel *statusLabel() const;

signals:
  void alarmRaised(const QString &ruleId, const QString &message);
  void alarmAcknowledged(int index);

private:
  void buildUi();
  void rebuildRulesTable();
  void rebuildHistoryTable();
  void rebuildChannelsTable();
  void rebuildEscalationTable();

  QWidget *containerWidget_ = nullptr;
  QTabWidget *tabs_ = nullptr;
  QTableWidget *rulesTable_ = nullptr;
  QTableWidget *historyTable_ = nullptr;
  QTableWidget *channelsTable_ = nullptr;
  QTableWidget *escalationTable_ = nullptr;
  QLineEdit *filterEdit_ = nullptr;
  QPushButton *addRuleBtn_ = nullptr;
  QPushButton *removeRuleBtn_ = nullptr;
  QPushButton *toggleRuleBtn_ = nullptr;
  QPushButton *ackBtn_ = nullptr;
  QPushButton *addChannelBtn_ = nullptr;
  QPushButton *removeChannelBtn_ = nullptr;
  QPushButton *addEscalationBtn_ = nullptr;
  QPushButton *removeEscalationBtn_ = nullptr;
  QLabel *statusLabel_ = nullptr;

  QVector<AlarmRule> rules_;
  QVector<AlarmRecord> records_;
  QVector<AlarmRecord> filteredRecords_;
  QVector<QString> channels_;
  QVector<EscalationPolicy> policies_;
};
