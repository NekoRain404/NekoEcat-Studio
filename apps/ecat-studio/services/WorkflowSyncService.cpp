#include "WorkflowSyncService.h"

WorkflowSyncService::WorkflowSyncService(QObject *parent)
    : QObject(parent) {}

bool WorkflowSyncService::syncTime() {
  return false;
}

bool WorkflowSyncService::syncData() {
  return false;
}

bool WorkflowSyncService::syncState() {
  return false;
}

bool WorkflowSyncService::syncConfiguration() {
  return false;
}

WorkflowSyncStatus WorkflowSyncService::syncStatus() const {
  return status_;
}
