#include "FreeRunMonitoringService.h"
#include "infra/EcatClient.h"

FreeRunMonitoringService::FreeRunMonitoringService(EcatClient *client,
                                                   EventBus *bus,
                                                   QObject *parent)
    : QObject(parent), client_(client), bus_(bus) {
  pollTimer_ = new QTimer(this);
  pollTimer_->setInterval(100);
  connect(pollTimer_, &QTimer::timeout, this,
          &FreeRunMonitoringService::pollData);

  status_.state = FreeRunState::Idle;
  status_.stateString = QStringLiteral("Idle");
}

void FreeRunMonitoringService::startMonitoring() {
  if (monitoring_) return;
  if (!client_ || !client_->isConnected()) return;
  monitoring_ = true;
  pollTimer_->start();
  emit monitoringStateChanged(true);
}

void FreeRunMonitoringService::stopMonitoring() {
  if (!monitoring_) return;
  monitoring_ = false;
  pollTimer_->stop();
  status_.state = FreeRunState::Stopped;
  status_.stateString = QStringLiteral("Stopped");
  emit statusChanged(status_);
  emit monitoringStateChanged(false);
}

void FreeRunMonitoringService::updateProcessData(const FreeRunProcessData &data) {
  processData_ = data;
  emit processDataUpdated(data);
}

void FreeRunMonitoringService::updatePerformance(
    const FreeRunPerformanceMetrics &metrics) {
  perfMetrics_ = metrics;
  emit performanceUpdated(metrics);
}

void FreeRunMonitoringService::addError(const FreeRunErrorInfo &error) {
  errors_.append(error);
  emit errorOccurred(error);
}

void FreeRunMonitoringService::updateStatus(const FreeRunStatus &status) {
  status_ = status;
  emit statusChanged(status);
}

void FreeRunMonitoringService::pollData() {
  if (!client_) return;
}
