#include "WorkflowAutomationService.h"
#include <QJsonArray>

// WorkflowAutomationService.cpp — Automates tasks, tests, and deployments via JSON configs
//
// Implementation notes:
//   - Builds QJsonObject configs from typed config structs
//   - Supports triggers, prerequisites, and environment specifications
//   - Delegates execution to internal executeAutomation dispatcher

WorkflowAutomationService::WorkflowAutomationService(QObject* parent) : QObject(parent) {}

bool WorkflowAutomationService::automateTask(const AutoTaskConfig& config) {
    if (config.task.isEmpty())
        return false;

    QJsonObject cfg;
    cfg[QStringLiteral("task")] = config.task;
    cfg[QStringLiteral("schedule")] = config.schedule;
    cfg[QStringLiteral("priority")] = config.priority;
    cfg[QStringLiteral("timeoutMs")] = config.timeoutMs;

    QJsonArray triggers;
    for (const auto& t : config.triggers)
        triggers.append(t);
    cfg[QStringLiteral("triggers")] = triggers;
    cfg[QStringLiteral("parameters")] = config.parameters;

    executeAutomation(QStringLiteral("task"), cfg);
    return true;
}

bool WorkflowAutomationService::automateTest(const TestConfig& config) {
    if (config.tests.isEmpty())
        return false;

    QJsonObject cfg;
    cfg[QStringLiteral("environment")] = config.environment;
    cfg[QStringLiteral("failFast")] = config.failFast;
    cfg[QStringLiteral("criteria")] = config.criteria;

    QJsonArray tests;
    for (const auto& t : config.tests)
        tests.append(t);
    cfg[QStringLiteral("tests")] = tests;

    QJsonArray prereqs;
    for (const auto& p : config.prerequisites)
        prereqs.append(p);
    cfg[QStringLiteral("prerequisites")] = prereqs;

    executeAutomation(QStringLiteral("test"), cfg);
    return true;
}

bool WorkflowAutomationService::automateDeploy(const DeployConfig& config) {
    if (config.target.isEmpty())
        return false;

    QJsonObject cfg;
    cfg[QStringLiteral("target")] = config.target;
    cfg[QStringLiteral("version")] = config.version;
    cfg[QStringLiteral("dryRun")] = config.dryRun;
    cfg[QStringLiteral("settings")] = config.settings;

    QJsonArray rollback;
    for (const auto& r : config.rollbackSteps)
        rollback.append(r);
    cfg[QStringLiteral("rollbackSteps")] = rollback;

    executeAutomation(QStringLiteral("deploy"), cfg);
    return true;
}

bool WorkflowAutomationService::automateMonitor(const MonitorConfig& config) {
    if (config.metrics.isEmpty())
        return false;

    QJsonObject cfg;
    cfg[QStringLiteral("intervalMs")] = config.intervalMs;
    cfg[QStringLiteral("thresholds")] = config.thresholds;

    QJsonArray metrics, alerts, notifications;
    for (const auto& m : config.metrics)
        metrics.append(m);
    for (const auto& a : config.alerts)
        alerts.append(a);
    for (const auto& n : config.notifications)
        notifications.append(n);
    cfg[QStringLiteral("metrics")] = metrics;
    cfg[QStringLiteral("alerts")] = alerts;
    cfg[QStringLiteral("notifications")] = notifications;

    executeAutomation(QStringLiteral("monitor"), cfg);
    return true;
}

AutomationStatus WorkflowAutomationService::status(const QString& type) const {
    return statuses_.value(type);
}

QVector<AutomationStatus> WorkflowAutomationService::allStatuses() const {
    QVector<AutomationStatus> result;
    for (auto it = statuses_.begin(); it != statuses_.end(); ++it)
        result.append(it.value());
    return result;
}

bool WorkflowAutomationService::cancel(const QString& type) {
    if (!statuses_.contains(type))
        return false;

    auto& s = statuses_[type];
    if (s.result == AutomationResult::Running) {
        s.result = AutomationResult::Cancelled;
        s.endTime = QDateTime::currentDateTime();
        emit automationCompleted(type, false);
        return true;
    }
    return false;
}

AutomationStatus WorkflowAutomationService::executeAutomation(const QString& type, const QJsonObject& config) {
    AutomationStatus s;
    s.type = type;
    s.result = AutomationResult::Running;
    s.startTime = QDateTime::currentDateTime();
    s.progress = 0.0;
    s.message = QStringLiteral("Starting %1 automation").arg(type);
    s.details = config;
    statuses_[type] = s;

    emit automationStarted(type);
    return s;
}
