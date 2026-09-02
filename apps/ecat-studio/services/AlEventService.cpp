// AlEventService — implementation.  See header for interface documentation.

#include "AlEventService.h"
#include "infra/EcatClient.h"

#include <QTimer>

AlEventService::AlEventService(EcatClient* client, QObject* parent) : QObject(parent), client_(client) {
    // Forward EcatClient's raw response into our own signal so consumers
    // do not need direct access to EcatClient.
    connect(client_, &EcatClient::alEventLogResult, this,
            [this](const QJsonObject& data) { emit alEventUpdate(data); });

    connect(client_, &EcatClient::errorMessage, this, [this](const QString& msg) { emit error(msg); });

    pollTimer_ = new QTimer(this);
    connect(pollTimer_, &QTimer::timeout, this, &AlEventService::requestUpdate);
}

void AlEventService::startPolling(int intervalMs) {
    pollTimer_->setInterval(intervalMs);
    pollTimer_->start();
    // Fire an immediate request so the UI is populated right away.
    requestUpdate();
}

void AlEventService::stopPolling() {
    pollTimer_->stop();
}

void AlEventService::requestUpdate() {
    if (client_ && client_->isConnected()) {
        client_->alEventLog();
    }
}

void AlEventService::clearEvents() {
    if (client_ && client_->isConnected()) {
        client_->alEventClear();
    }
}
