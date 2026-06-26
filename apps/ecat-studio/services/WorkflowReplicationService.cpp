#include "WorkflowReplicationService.h"

WorkflowReplicationService::WorkflowReplicationService(QObject *parent)
    : QObject(parent) {}

bool WorkflowReplicationService::replicateConfiguration(const QStringList &targets) {
  Q_UNUSED(targets);
  return false;
}

bool WorkflowReplicationService::replicateData(const QStringList &targets) {
  Q_UNUSED(targets);
  return false;
}

bool WorkflowReplicationService::replicateState(const QStringList &targets) {
  Q_UNUSED(targets);
  return false;
}

bool WorkflowReplicationService::replicateBackup(const QStringList &targets) {
  Q_UNUSED(targets);
  return false;
}

QVector<WorkflowReplicationStatus> WorkflowReplicationService::replicationHistory() const {
  return history_;
}
