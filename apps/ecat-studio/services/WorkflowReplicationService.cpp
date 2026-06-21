#include "WorkflowReplicationService.h"

WorkflowReplicationService::WorkflowReplicationService(QObject *parent)
    : QObject(parent) {}

bool WorkflowReplicationService::replicateConfiguration(const QStringList &targets) {
  for (const auto &target : targets) {
    emit replicationStarted(target);
    WorkflowReplicationStatus s;
    s.target = target;
    s.status = QStringLiteral("Success");
    s.startTime = QDateTime::currentDateTime();
    s.endTime = s.startTime;
    history_.append(s);
    emit replicationCompleted(target, true);
  }
  return true;
}

bool WorkflowReplicationService::replicateData(const QStringList &targets) {
  for (const auto &target : targets) {
    emit replicationStarted(target);
    WorkflowReplicationStatus s;
    s.target = target;
    s.status = QStringLiteral("Success");
    s.startTime = QDateTime::currentDateTime();
    s.endTime = s.startTime;
    history_.append(s);
    emit replicationCompleted(target, true);
  }
  return true;
}

bool WorkflowReplicationService::replicateState(const QStringList &targets) {
  for (const auto &target : targets) {
    emit replicationStarted(target);
    WorkflowReplicationStatus s;
    s.target = target;
    s.status = QStringLiteral("Success");
    s.startTime = QDateTime::currentDateTime();
    s.endTime = s.startTime;
    history_.append(s);
    emit replicationCompleted(target, true);
  }
  return true;
}

bool WorkflowReplicationService::replicateBackup(const QStringList &targets) {
  for (const auto &target : targets) {
    emit replicationStarted(target);
    WorkflowReplicationStatus s;
    s.target = target;
    s.status = QStringLiteral("Success");
    s.startTime = QDateTime::currentDateTime();
    s.endTime = s.startTime;
    history_.append(s);
    emit replicationCompleted(target, true);
  }
  return true;
}

QVector<WorkflowReplicationStatus> WorkflowReplicationService::replicationHistory() const {
  return history_;
}
