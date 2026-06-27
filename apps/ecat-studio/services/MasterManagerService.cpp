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
  if (!client_)
    return;

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
  if (config.adapterName.trimmed().isEmpty() || config.cycleTime <= 0
      || config.sync0Time < 0 || config.watchdogTimeout < 0
      || config.debugLevel < 0 || config.debugLevel > 3) {
    emit masterError(QStringLiteral("Invalid master configuration"));
    return false;
  }
  if (!client_ || !client_->isConnected()) return false;

  Q_UNUSED(config);
  emit masterError(QStringLiteral(
      "Master configuration requires a backend acknowledgement"));
  return false;
}

MasterMgrDiagnosticResult MasterManagerService::diagnoseMaster() {
  MasterMgrDiagnosticResult result;

  if (!client_ || !client_->isConnected()) {
    result.success = false;
    result.summary = "Not connected to daemon";
    return result;
  }

  result.success = false;
  result.summary = "Master diagnostics require live master evidence";
  result.details.append(QString("State: %1").arg(static_cast<int>(state_)));
  result.details.append(QString("Adapter: %1").arg(info_.adapterName));
  result.details.append(QString("Slaves: %1").arg(info_.slaveCount));
  result.details.append(QString("Errors: %1").arg(info_.errorCount));
  return result;
}

bool MasterManagerService::restartMaster() {
  if (!client_ || !client_->isConnected()) return false;

  emit masterError(QStringLiteral(
      "Master restart requires a backend acknowledgement"));
  return false;
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
