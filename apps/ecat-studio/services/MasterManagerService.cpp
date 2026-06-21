#include "MasterManagerService.h"
#include "infra/EcatClient.h"

// MasterManagerService.cpp — Master state machine with daemon text parsing
//
// Implementation notes:
//   - Connects to EcatClient signals for master text, connect/disconnect, errors
//   - Parses daemon master text to extract slave count and error metrics
//   - State transitions (Unknown → Idle → Configuring → Active) drive UI updates

MasterManagerService::MasterManagerService(EcatClient *client,
                                           QObject *parent)
    : QObject(parent), client_(client) {
  connect(client_, &EcatClient::masterText, this,
          [this](const QString &text) { updateFromMasterText(text); });

  connect(client_, &EcatClient::connected, this, [this]() {
    setState(MasterMgrState::Idle);
    refresh();
  });

  connect(client_, &EcatClient::disconnected, this, [this]() {
    setState(MasterMgrState::Unknown);
  });

  connect(client_, &EcatClient::errorMessage, this,
          [this](const QString &msg) { emit masterError(msg); });
}

bool MasterManagerService::configureMaster(const MasterMgrConfig &config) {
  if (!client_ || !client_->isConnected()) return false;

  setState(MasterMgrState::Configuring);

  if (!config.adapterName.isEmpty()) {
    client_->setAdapter(config.adapterName);
    info_.adapterName = config.adapterName;
  }

  setState(MasterMgrState::Idle);
  return true;
}

MasterMgrDiagnosticResult MasterManagerService::diagnoseMaster() {
  MasterMgrDiagnosticResult result;

  if (!client_ || !client_->isConnected()) {
    result.success = false;
    result.summary = "Not connected to daemon";
    return result;
  }

  result.success = true;
  result.summary = "Master operational";
  result.details.append(QString("State: %1").arg(static_cast<int>(state_)));
  result.details.append(QString("Adapter: %1").arg(info_.adapterName));
  result.details.append(QString("Slaves: %1").arg(info_.slaveCount));
  result.details.append(QString("Errors: %1").arg(info_.errorCount));
  return result;
}

bool MasterManagerService::restartMaster() {
  if (!client_ || !client_->isConnected()) return false;

  setState(MasterMgrState::Configuring);
  client_->rescan();
  return true;
}

void MasterManagerService::refresh() {
  if (!client_ || !client_->isConnected()) return;
  client_->master();
  info_.adapterName = client_->masterTarget();
}

// Parses daemon master text output to populate info_ (slave count, errors, version)
void MasterManagerService::updateFromMasterText(const QString &text) {
  if (text.isEmpty()) return;

  info_.version = "IgH EtherCAT";
  info_.buildDate = QDate::currentDate().toString(Qt::ISODate);

  const QStringList lines = text.split('\n', Qt::SkipEmptyParts);
  for (const QString &line : lines) {
    if (line.contains("Slave") && line.contains("count")) {
      const QStringList parts = line.split(':');
      if (parts.size() >= 2) {
        info_.slaveCount = parts.last().trimmed().toInt();
      }
    }
    if (line.contains("Error") || line.contains("error")) {
      info_.errorCount++;
    }
  }

  setState(MasterMgrState::Active);
  emit masterInfoUpdated(info_);
}

// Transitions state machine and emits masterStateChanged if state actually changed
void MasterManagerService::setState(MasterMgrState state) {
  if (state_ == state) return;
  state_ = state;
  info_.masterState = state;
  emit masterStateChanged(state);
}
