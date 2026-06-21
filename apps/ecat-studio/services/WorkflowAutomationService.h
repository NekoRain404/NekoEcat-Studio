#pragma once

// WorkflowAutomationService -- automates task execution, testing, deployment,
// and monitoring workflows in NekoEcat Studio.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QHash>
#include <QJsonObject>
#include <QDateTime>

struct AutoTaskConfig {
  QString task;
  QString schedule;
  QStringList triggers;
  QJsonObject parameters;
  int priority = 0;
  int timeoutMs = 30000;
};

struct TestConfig {
  QStringList tests;
  QString environment;
  QJsonObject criteria;
  QStringList prerequisites;
  bool failFast = true;
};

struct DeployConfig {
  QString target;
  QString version;
  QStringList rollbackSteps;
  QJsonObject settings;
  bool dryRun = false;
};

struct MonitorConfig {
  QStringList metrics;
  QStringList alerts;
  QStringList notifications;
  int intervalMs = 5000;
  QJsonObject thresholds;
};

enum class AutomationResult { Pending, Running, Success, Failed, Cancelled };

struct AutomationStatus {
  QString type;
  AutomationResult result = AutomationResult::Pending;
  QDateTime startTime;
  QDateTime endTime;
  double progress = 0.0;
  QString message;
  QJsonObject details;
};

class WorkflowAutomationService : public QObject {
  Q_OBJECT
public:
  explicit WorkflowAutomationService(QObject *parent = nullptr);

  bool automateTask(const AutoTaskConfig &config);
  bool automateTest(const TestConfig &config);
  bool automateDeploy(const DeployConfig &config);
  bool automateMonitor(const MonitorConfig &config);

  AutomationStatus status(const QString &type) const;
  QVector<AutomationStatus> allStatuses() const;
  bool cancel(const QString &type);

signals:
  void automationStarted(const QString &type);
  void automationCompleted(const QString &type, bool success);
  void automationProgress(const QString &type, double progress);

private:
  AutomationStatus executeAutomation(const QString &type, const QJsonObject &config);
  QHash<QString, AutomationStatus> statuses_;
};
