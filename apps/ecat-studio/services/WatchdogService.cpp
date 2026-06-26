#include "WatchdogService.h"
#include "EventBus.h"
#include "infra/EcatClient.h"

#include <QTimer>
#include <QJsonArray>
#include <QDateTime>

// WatchdogService.cpp — Monitors per-slave watchdog status with auto-recovery
//
// Implementation notes:
//   - Tracks watchdogOk, timeoutCount, triggerCount per slave position
//   - Auto-recovers slaves after 5 seconds of no watchdog triggers
//   - Listens to topologyChanged events to rebuild slave status list

WatchdogService::WatchdogService(EventBus *bus, EcatClient *client,
                                 QObject *parent)
    : QObject(parent), bus_(bus), client_(client) {
  connect(bus_, &EventBus::topologyChanged, this,
          &WatchdogService::onTopologyChanged);

  timer_ = new QTimer(this);
  connect(timer_, &QTimer::timeout, this, &WatchdogService::poll);
}

void WatchdogService::startMonitoring(int intervalMs) {
  if (running_) return;
  if (!client_ || !client_->isConnected()) return;
  running_ = true;
  timer_->setInterval(intervalMs);
  timer_->start();
}

void WatchdogService::stopMonitoring() {
  running_ = false;
  timer_->stop();
}

QJsonObject WatchdogService::currentStatus() const {
  QJsonObject status;
  status["totalTimeouts"] = totalTimeouts_;
  status["totalTriggers"] = totalTriggers_;
  status["lastTriggerMs"] = lastTriggerMs_;
  status["monitoring"] = running_;
  status["timestamp"] = QDateTime::currentDateTimeUtc().toMSecsSinceEpoch();

  QJsonArray slaves;
  for (const auto &s : slaveStatuses_) {
    QJsonObject obj;
    obj["position"] = s.position;
    obj["watchdogOk"] = s.watchdogOk;
    obj["timeoutCount"] = s.timeoutCount;
    obj["triggerCount"] = s.triggerCount;
    obj["lastTriggerMs"] = s.lastTriggerMs;
    slaves.append(obj);
  }
  status["slaves"] = slaves;
  return status;
}

QVector<WatchdogSlaveStatus> WatchdogService::slaveStatuses() const {
  return slaveStatuses_;
}

void WatchdogService::onTopologyChanged(const QVector<SlaveInfo> &slaves) {
  updateSlaveStatuses(slaves);
}

void WatchdogService::poll() {
  for (auto &s : slaveStatuses_) {
    if (!s.watchdogOk) {
      const qint64 now = QDateTime::currentDateTimeUtc().toMSecsSinceEpoch();
      if (now - s.lastTriggerMs > 5000) {
        s.watchdogOk = true;
      }
    }
  }

  QJsonObject status = currentStatus();
  emit watchdogStatusChanged(status);
}

void WatchdogService::updateSlaveStatuses(const QVector<SlaveInfo> &slaves) {
  QVector<WatchdogSlaveStatus> newStatuses;
  newStatuses.reserve(slaves.size());

  for (const auto &slave : slaves) {
    WatchdogSlaveStatus ws;
    ws.position = slave.position;

    auto it = std::find_if(slaveStatuses_.begin(), slaveStatuses_.end(),
                           [&](const WatchdogSlaveStatus &existing) {
                             return existing.position == slave.position;
                           });
    if (it != slaveStatuses_.end()) {
      ws.watchdogOk = it->watchdogOk;
      ws.timeoutCount = it->timeoutCount;
      ws.triggerCount = it->triggerCount;
      ws.lastTriggerMs = it->lastTriggerMs;
    }
    newStatuses.append(ws);
  }
  slaveStatuses_ = newStatuses;
}
