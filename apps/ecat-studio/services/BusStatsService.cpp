#include "BusStatsService.h"
#include "infra/EcatClient.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>

// BusStatsService.cpp — Periodic bus frame/error statistics polling
//
// Implementation notes:
//   - Uses a QTimer-driven poll loop against EcatClient::hostDiagnostics
//   - Computes frame rate and bandwidth from delta counters each tick
//   - Publishes stats as QJsonObject for direct UI/JSON consumers
//   - Monitoring starts only when a live daemon connection exists

BusStatsService::BusStatsService(EcatClient* client, QObject* parent) : QObject(parent), client_(client) {
    pollTimer_ = new QTimer(this);
    connect(pollTimer_, &QTimer::timeout, this, &BusStatsService::poll);
}

void BusStatsService::startMonitoring(int intervalMs) {
    if (pollTimer_->isActive())
        return;
    if (!client_ || !client_->isConnected())
        return;
    pollTimer_->start(intervalMs);
}

void BusStatsService::stopMonitoring() {
    pollTimer_->stop();
}

bool BusStatsService::isMonitoring() const {
    return pollTimer_->isActive();
}

BusStats BusStatsService::currentStats() const {
    return stats_;
}

QJsonObject BusStatsService::currentStatsJson() const {
    QJsonObject obj;
    obj["txFrames"] = static_cast<qint64>(stats_.txFrames);
    obj["rxFrames"] = static_cast<qint64>(stats_.rxFrames);
    obj["txErrors"] = static_cast<qint64>(stats_.txErrors);
    obj["rxErrors"] = static_cast<qint64>(stats_.rxErrors);
    obj["crcErrors"] = static_cast<qint64>(stats_.crcErrors);
    obj["lostFrames"] = static_cast<qint64>(stats_.lostFrames);
    obj["bandwidthMbps"] = stats_.bandwidthMbps;
    obj["frameRate"] = stats_.frameRate;
    obj["timestampMs"] = stats_.timestampMs;
    return obj;
}

// Fetches host diagnostics, computes frame rate/bandwidth deltas, emits update
void BusStatsService::poll() {
    if (!client_ || !client_->isConnected()) {
        emit error(QStringLiteral("Not connected to daemon"));
        return;
    }

    client_->hostDiagnostics();
    stats_.timestampMs = QDateTime::currentMSecsSinceEpoch();

    quint64 totalTxFrames = stats_.txFrames + stats_.txErrors;
    quint64 totalRxFrames = stats_.rxFrames + stats_.rxErrors;
    if (prevTxFrames_ > 0) {
        quint64 deltaTx = totalTxFrames - prevTxFrames_;
        quint64 deltaRx = totalRxFrames - prevRxFrames_;
        stats_.frameRate = static_cast<double>(deltaTx + deltaRx);
        stats_.bandwidthMbps = (static_cast<double>(deltaTx + deltaRx) * 12000.0) / 1000000.0;
    }
    prevTxFrames_ = totalTxFrames;
    prevRxFrames_ = totalRxFrames;

    emit statsUpdated(currentStatsJson());
}
