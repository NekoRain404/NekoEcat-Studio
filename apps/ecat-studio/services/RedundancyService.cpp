#include "RedundancyService.h"
#include "EcatClient.h"

// RedundancyService.cpp — EtherCAT cable redundancy management
//
// Implementation notes:
//   - Delegates to EcatClient for daemon communication
//   - Maintains cached state from last query
//   - Updates state on successful status/command responses

RedundancyService::RedundancyService(EcatClient *client, QObject *parent)
    : QObject(parent), client_(client) {
  primaryPath_.pathId = 0;
  primaryPath_.state = PathState::Unknown;
  secondaryPath_.pathId = 1;
  secondaryPath_.state = PathState::Unknown;

  // Connect daemon signals.
  connect(client_, &EcatClient::redundancyStatusResult, this,
          [this](const QJsonObject &data) {
            // Update cached state from daemon response.
            const QString stateStr = data.value("state").toString();
            if (stateStr == "DualPath") {
              state_ = RedundancyState::DualPath;
            } else if (stateStr == "PrimaryOnly" || stateStr == "SecondaryOnly") {
              state_ = RedundancyState::Failover;
            } else if (stateStr == "BothDown") {
              state_ = RedundancyState::Error;
            } else {
              state_ = RedundancyState::SinglePath;
            }

            // Update path info.
            const auto pObj = data.value("primaryPath").toObject();
            primaryPath_.isHealthy = pObj.value("isHealthy").toBool();
            primaryPath_.state = primaryPath_.isHealthy ? PathState::Active : PathState::Failed;
            primaryPath_.lastCheck = QDateTime::currentDateTime();

            const auto sObj = data.value("secondaryPath").toObject();
            secondaryPath_.isHealthy = sObj.value("isHealthy").toBool();
            secondaryPath_.state = secondaryPath_.isHealthy ? PathState::Active : PathState::Failed;
            secondaryPath_.lastCheck = QDateTime::currentDateTime();

            emit statusReceived(data);
          });

  connect(client_, &EcatClient::redundancyCommandResult, this,
          [this](const QString &cmd, bool success, const QString &msg) {
            emit commandResult(cmd, success, msg);
            if (!success) {
              emit error(msg);
            }
          });

  connect(client_, &EcatClient::redundancyHistoryResult, this,
          [this](const QJsonObject &data) {
            emit historyReceived(data);
          });

  connect(client_, &EcatClient::errorMessage, this,
          [this](const QString &msg) { emit error(msg); });
}

void RedundancyService::setPrimaryPath(int slaveCount) {
  primaryPath_.slaveCount = slaveCount;
  primaryPath_.state = PathState::Active;
  primaryPath_.isHealthy = true;
  primaryPath_.lastCheck = QDateTime::currentDateTime();
}

void RedundancyService::setSecondaryPath(int slaveCount) {
  secondaryPath_.slaveCount = slaveCount;
  secondaryPath_.state = PathState::Standby;
  secondaryPath_.isHealthy = true;
  secondaryPath_.lastCheck = QDateTime::currentDateTime();
}

void RedundancyService::queryStatus() {
  if (!client_ || !client_->isConnected()) {
    emit error("Cannot query redundancy status: daemon not connected");
    return;
  }
  client_->redundancyStatus();
}

bool RedundancyService::enableRedundancy() {
  if (!client_ || !client_->isConnected()) {
    emit error("Cannot enable redundancy: daemon not connected");
    return false;
  }
  client_->redundancyEnable();
  return true;
}

bool RedundancyService::disableRedundancy() {
  if (!client_ || !client_->isConnected()) {
    emit error("Cannot disable redundancy: daemon not connected");
    return false;
  }
  client_->redundancyDisable();
  return true;
}

bool RedundancyService::failover() {
  if (!client_ || !client_->isConnected()) {
    emit error("Cannot perform failover: daemon not connected");
    return false;
  }
  client_->redundancyFailover();
  return true;
}

bool RedundancyService::failback() {
  if (!client_ || !client_->isConnected()) {
    emit error("Cannot perform failback: daemon not connected");
    return false;
  }
  client_->redundancyFailback();
  return true;
}

void RedundancyService::queryHistory(int limit) {
  if (!client_ || !client_->isConnected()) {
    emit error("Cannot query redundancy history: daemon not connected");
    return;
  }
  client_->redundancyHistory(limit);
}

RedundancyState RedundancyService::currentState() const { return state_; }
RedundancyPath RedundancyService::primaryPath() const { return primaryPath_; }
RedundancyPath RedundancyService::secondaryPath() const { return secondaryPath_; }
QVector<RedundancyEvent> RedundancyService::redundancyHistory() const { return history_; }
bool RedundancyService::isRedundant() const { return state_ == RedundancyState::DualPath; }
