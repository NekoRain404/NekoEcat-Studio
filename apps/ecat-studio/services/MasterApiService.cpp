#include "MasterApiService.h"
#include "infra/EcatClient.h"

// MasterApiService.cpp — EtherCAT master lifecycle (create/activate/deactivate)
//
// Implementation notes:
//   - Thin state machine: tracks created_ and active_ flags
//   - Validates prerequisites (client exists, master created) before transitions
//   - Slave config returns a lightweight descriptor; no hardware I/O

MasterApiService::MasterApiService(EcatClient *client, QObject *parent)
    : QObject(parent), client_(client) {}

bool MasterApiService::createMaster() {
  if (created_) return true;
  if (!client_) {
    emit error(QStringLiteral("No client available"));
    return false;
  }
  created_ = true;
  emit masterCreated();
  return true;
}

bool MasterApiService::activateMaster() {
  if (!created_) {
    emit error(QStringLiteral("Master not created"));
    return false;
  }
  if (active_) return true;
  active_ = true;
  state_.linkUp = true;
  emit masterActivated();
  return true;
}

bool MasterApiService::deactivateMaster() {
  if (!active_) return true;
  active_ = false;
  state_.linkUp = false;
  emit masterDeactivated();
  return true;
}

MasterApiState MasterApiService::masterState() const {
  return state_;
}

SlaveApiConfig MasterApiService::slaveConfig(int position) const {
  SlaveApiConfig cfg;
  if (!created_ || position < 0) return cfg;
  cfg.position = position;
  cfg.valid = true;
  return cfg;
}
