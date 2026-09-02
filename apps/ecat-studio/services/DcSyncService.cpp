#include "DcSyncService.h"
#include "infra/EcatClient.h"

#include <QTimer>

// DcSyncService.cpp — DC sync status polling via EcatClient
//
// Implementation notes:
//   - Forwards EcatClient::dcSyncStatusResult as dcSyncUpdate for decoupled consumers
//   - Polling interval configurable; fires an immediate request on start
//   - Lightweight wrapper — no local state beyond the timer

DcSyncService::DcSyncService(EcatClient* client, QObject* parent) : QObject(parent), client_(client) {
    pollTimer_ = new QTimer(this);
    connect(pollTimer_, &QTimer::timeout, this, &DcSyncService::requestUpdate);

    if (!client_)
        return;

    // Forward the EcatClient signal into our own signal so consumers
    // do not need direct access to EcatClient.
    connect(client_, &EcatClient::dcSyncStatusResult, this,
            [this](const QJsonObject& data) { emit dcSyncUpdate(data); });

    connect(client_, &EcatClient::dcConfigureResult, this,
            [this](const QJsonObject& data) { emit dcConfigureResult(data); });

    connect(client_, &EcatClient::dcActivateResult, this,
            [this](const QJsonObject& data) { emit dcActivateResult(data); });

    connect(client_, &EcatClient::dcDeactivateResult, this,
            [this](const QJsonObject& data) { emit dcDeactivateResult(data); });

    connect(client_, &EcatClient::errorMessage, this, [this](const QString& msg) { emit error(msg); });
}

void DcSyncService::startPolling(int intervalMs) {
    if (intervalMs <= 0) {
        emit pollingRejected(QStringLiteral("Invalid polling interval"));
        return;
    }
    pollTimer_->setInterval(intervalMs);
    pollTimer_->start();
    // Fire an immediate request so the UI is populated right away.
    requestUpdate();
}

void DcSyncService::stopPolling() {
    pollTimer_->stop();
}

void DcSyncService::requestUpdate() {
    if (client_ && client_->isConnected()) {
        client_->dcSyncStatus();
    }
}

void DcSyncService::configure(int position) {
    if (client_ && client_->isConnected()) {
        client_->dcConfigure(position);
    }
}

void DcSyncService::activate(int refClockSlave) {
    if (client_ && client_->isConnected()) {
        client_->dcActivate(refClockSlave);
    }
}

void DcSyncService::deactivate() {
    if (client_ && client_->isConnected()) {
        client_->dcDeactivate();
    }
}
