#include "OnlineDiagnosticsService.h"
#include "infra/EcatClient.h"

#include <QDateTime>

OnlineDiagnosticsService::OnlineDiagnosticsService(EcatClient *client, QObject *parent)
    : QObject(parent), client_(client) {
  pollTimer_ = new QTimer(this);
  connect(pollTimer_, &QTimer::timeout, this, &OnlineDiagnosticsService::poll);
}

void OnlineDiagnosticsService::startMonitoring(int intervalMs) {
  if (pollTimer_->isActive()) return;
  pollTimer_->start(intervalMs);
}

void OnlineDiagnosticsService::stopMonitoring() { pollTimer_->stop(); }

bool OnlineDiagnosticsService::isMonitoring() const { return pollTimer_->isActive(); }

void OnlineDiagnosticsService::poll() {
  if (!client_ || !client_->isConnected()) {
    emit error(QStringLiteral("Not connected to daemon"));
    return;
  }

  client_->hostDiagnostics();

  if (prevTxFrames_ > 0) {
    quint64 deltaTx = traffic_.txFrames - prevTxFrames_;
    quint64 deltaRx = traffic_.rxFrames - prevRxFrames_;
    perf_.pdoUpdateRate = static_cast<double>(deltaTx + deltaRx);
    traffic_.bandwidth = (perf_.pdoUpdateRate * 12000.0) / 1000000.0;
    traffic_.utilization = qMin(traffic_.bandwidth / 100.0 * 100.0, 100.0);
  }
  prevTxFrames_ = traffic_.txFrames;
  prevRxFrames_ = traffic_.rxFrames;

  quint64 currentErrors = errorRate_.totalErrors;
  if (prevTotalErrors_ > 0) {
    errorRate_.rate = static_cast<double>(currentErrors - prevTotalErrors_);
  }
  prevTotalErrors_ = currentErrors;

  if (currentErrors > 100 || errorRate_.lostErrors > 50) {
    health_.grade = QStringLiteral("Critical");
  } else if (currentErrors > 10) {
    health_.grade = QStringLiteral("Warning");
  } else {
    health_.grade = QStringLiteral("Healthy");
  }

  emit trafficUpdated(traffic_);
  emit errorRateUpdated(errorRate_);
  emit performanceUpdated(perf_);
  emit healthUpdated(health_);
}
