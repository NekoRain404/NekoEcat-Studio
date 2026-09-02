#include "EcatHealthService.h"
#include "infra/EcatClient.h"
#include "services/AlEventService.h"
#include "services/DcSyncService.h"
#include "services/EventBus.h"
#include "services/TopologyService.h"
#include "services/WatchdogService.h"

// EcatHealthService.cpp — Aggregates EtherCAT bus health into a 0–100 score
//
// Implementation notes:
//   - Polls slave states (OP/SafeOp/PreOp/Init/Error), DC sync, and watchdog status
//   - Scoring: 100 base minus penalties for errors, non-OP slaves, watchdog triggers
//   - Grades: Excellent (100), Good (80+), Fair (60+), Poor (40+), Critical (<40)
//   - Monitoring starts only when a live daemon connection exists

EcatHealthService::EcatHealthService(EcatClient* client, EventBus* bus, TopologyService* topology,
                                     DcSyncService* dcSync, AlEventService* alEvent, WatchdogService* watchdog,
                                     QObject* parent)
    : QObject(parent), client_(client), bus_(bus), topology_(topology), dcSync_(dcSync), alEvent_(alEvent),
      watchdog_(watchdog) {
    pollTimer_ = new QTimer(this);
    connect(pollTimer_, &QTimer::timeout, this, &EcatHealthService::poll);
}

void EcatHealthService::startMonitoring(int intervalMs) {
    if (pollTimer_->isActive())
        return;
    if (!client_ || !client_->isConnected())
        return;
    pollTimer_->start(intervalMs);
}

void EcatHealthService::stopMonitoring() {
    pollTimer_->stop();
}

bool EcatHealthService::isMonitoring() const {
    return pollTimer_->isActive();
}

MasterState EcatHealthService::masterState() const {
    return master_;
}

SlaveState EcatHealthService::slaveState(int position) const {
    for (const auto& s : slaves_) {
        if (s.position == position)
            return s;
    }
    return {};
}

DcSyncStatus EcatHealthService::dcSyncStatus() const {
    return dcSyncStatus_;
}

AlEventStatus EcatHealthService::alEventStatus() const {
    return alEventStatus_;
}

WatchdogStatus EcatHealthService::watchdogStatus() const {
    return watchdogStatus_;
}

HealthScore EcatHealthService::overallHealth() const {
    return health_;
}

void EcatHealthService::poll() {
    if (!client_ || !client_->isConnected())
        return;

    auto prevSlaves = slaves_;

    master_.responsive = true;

    int op = 0, safeOp = 0, preOp = 0, init = 0, err = 0;
    for (const auto& s : slaves_) {
        if (s.state == "OP")
            ++op;
        else if (s.state == "SAFEOP")
            ++safeOp;
        else if (s.state == "PREOP")
            ++preOp;
        else if (s.state == "INIT")
            ++init;
        if (s.hasError)
            ++err;
    }

    int total = slaves_.size();
    if (total == 0) {
        health_ = {};
        emit healthChanged(health_);
        return;
    }

    int score = 100;
    if (err > 0)
        score -= err * 10;
    if (init > 0)
        score -= init * 15;
    if (preOp > 0)
        score -= preOp * 8;
    if (safeOp > 0)
        score -= safeOp * 3;
    if (watchdogStatus_.triggered)
        score -= 20;
    if (!dcSyncStatus_.inSync)
        score -= 5;
    score = qBound(0, score, 100);

    QString grade;
    if (score >= 100)
        grade = "Excellent";
    else if (score >= 80)
        grade = "Good";
    else if (score >= 60)
        grade = "Fair";
    else if (score >= 40)
        grade = "Poor";
    else
        grade = "Critical";

    QString summary;
    if (score >= 100)
        summary = QStringLiteral("All slaves in OP, no errors, DC synced");
    else if (score >= 80)
        summary = QStringLiteral("Some slaves in SafeOp, minor errors");
    else if (score >= 60)
        summary = QStringLiteral("Some slaves in PreOp, moderate errors");
    else if (score >= 40)
        summary = QStringLiteral("Some slaves in Init, significant errors");
    else
        summary = QStringLiteral("Critical errors, communication loss");

    health_ = {score, grade, summary, total, op, safeOp, preOp, init, err};

    for (int i = 0; i < slaves_.size(); ++i) {
        bool changed = i >= prevSlaves.size() || slaves_[i].state != prevSlaves[i].state ||
                       slaves_[i].hasError != prevSlaves[i].hasError;
        if (changed)
            emit stateChanged(slaves_[i].position, slaves_[i]);
    }

    emit healthChanged(health_);
}
