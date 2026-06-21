#include "WorkflowSyncService.h"

WorkflowSyncService::WorkflowSyncService(QObject *parent)
    : QObject(parent) {}

bool WorkflowSyncService::syncTime() {
  QDateTime now = QDateTime::currentDateTime();
  status_.lastSync = now;
  status_.syncCount++;
  emit timeSynced(now);
  return true;
}

bool WorkflowSyncService::syncData() {
  status_.lastSync = QDateTime::currentDateTime();
  status_.syncCount++;
  emit dataSynced(0);
  return true;
}

bool WorkflowSyncService::syncState() {
  status_.lastSync = QDateTime::currentDateTime();
  status_.syncCount++;
  return true;
}

bool WorkflowSyncService::syncConfiguration() {
  status_.lastSync = QDateTime::currentDateTime();
  status_.syncCount++;
  return true;
}

WorkflowSyncStatus WorkflowSyncService::syncStatus() const {
  return status_;
}
