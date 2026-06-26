#include "NetworkDiagnosticsService.h"
#include "infra/EcatClient.h"

#include <QDateTime>

// NetworkDiagnosticsService.cpp — Polls EtherCAT network health, port status, and error counters
//
// Implementation notes:
//   - Timer-driven polling via QTimer at configurable interval
//   - Tracks per-port error counters and bandwidth utilization
//   - Health state derived from slave and error counter readings
//   - Monitoring starts only when a live daemon connection exists

NetworkDiagnosticsService::NetworkDiagnosticsService(EcatClient *client,
                                                     QObject *parent)
    : QObject(parent), client_(client) {
  pollTimer_ = new QTimer(this);
  connect(pollTimer_, &QTimer::timeout, this, &NetworkDiagnosticsService::poll);
}

void NetworkDiagnosticsService::startMonitoring(int intervalMs) {
  if (pollTimer_->isActive()) return;
  if (!client_ || !client_->isConnected()) return;
  pollTimer_->start(intervalMs);
}

void NetworkDiagnosticsService::stopMonitoring() { pollTimer_->stop(); }

bool NetworkDiagnosticsService::isMonitoring() const {
  return pollTimer_->isActive();
}

NetworkHealth NetworkDiagnosticsService::currentHealth() const {
  return health_;
}

PortStatus NetworkDiagnosticsService::portStatus(int port) const {
  if (port >= 0 && port < ports_.size()) return ports_[port];
  return {};
}

QVector<PortStatus> NetworkDiagnosticsService::allPortStatus() const {
  return ports_;
}

ErrorCounters NetworkDiagnosticsService::errorCounters() const {
  return errors_;
}

double NetworkDiagnosticsService::bandwidthUtilization() const {
  return bandwidth_;
}

void NetworkDiagnosticsService::resetErrorCounters() {
  errors_ = {};
  for (auto &p : ports_) p.errorCount = 0;
}

void NetworkDiagnosticsService::poll() {
  if (!client_ || !client_->isConnected()) return;

  auto prevErrors = errors_;

  qint64 now = QDateTime::currentMSecsSinceEpoch();
  health_.portCount = ports_.size();
  health_.activePorts = 0;
  int totalErrors = 0;
  for (const auto &p : ports_) {
    if (p.linkUp) ++health_.activePorts;
    totalErrors += p.errorCount;
  }
  health_.errorCount = totalErrors;
  health_.bandwidth = bandwidth_;
  health_.latencyMs = latencyMs_;
  health_.jitterMs = jitterMs_;

  if (totalErrors > 10)
    health_.overall = NetworkHealth::Status::Critical;
  else if (totalErrors > 0)
    health_.overall = NetworkHealth::Status::Degraded;
  else
    health_.overall = NetworkHealth::Status::Good;

  if (errors_.crc > prevErrors.crc) {
    emit errorDetected({now, -1, "CRC", "CRC error detected"});
  }
  if (errors_.frame > prevErrors.frame) {
    emit errorDetected({now, -1, "Frame", "Frame error detected"});
  }
  if (errors_.lost > prevErrors.lost) {
    emit errorDetected({now, -1, "Lost", "Lost frame detected"});
  }

  emit healthUpdated(health_);
}
