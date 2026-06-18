#include "DcSyncService.h"
#include "infra/EcatClient.h"

#include <QTimer>

DcSyncService::DcSyncService(EcatClient *client, QObject *parent)
    : QObject(parent), client_(client) {
  // Forward the EcatClient signal into our own signal so consumers
  // do not need direct access to EcatClient.
  connect(client_, &EcatClient::dcSyncStatusResult, this,
          [this](const QJsonObject &data) { emit dcSyncUpdate(data); });

  connect(client_, &EcatClient::errorMessage, this,
          [this](const QString &msg) { emit error(msg); });

  pollTimer_ = new QTimer(this);
  connect(pollTimer_, &QTimer::timeout, this, &DcSyncService::requestUpdate);
}

void DcSyncService::startPolling(int intervalMs) {
  pollTimer_->setInterval(intervalMs);
  pollTimer_->start();
  // Fire an immediate request so the UI is populated right away.
  requestUpdate();
}

void DcSyncService::stopPolling() { pollTimer_->stop(); }

void DcSyncService::requestUpdate() {
  if (client_ && client_->isConnected()) {
    client_->dcSyncStatus();
  }
}
