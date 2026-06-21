#pragma once

// OnlineDiagnosticsService — Real-time EtherCAT bus diagnostics monitoring.
//
// Provides bus traffic monitoring, error rate tracking, performance metrics,
// and health status assessment. Polls the ecatd daemon periodically and
// emits signals for UI consumption.
//
// Reuses BusTraffic, ErrorRate, PerformanceMetrics, HealthStatus from
// EtherCATMonitorService.h for consistency.

#include <QObject>
#include <QTimer>
#include <QVector>

#include "EtherCATMonitorService.h"

class EcatClient;

class OnlineDiagnosticsService : public QObject {
  Q_OBJECT
public:
  explicit OnlineDiagnosticsService(EcatClient *client, QObject *parent = nullptr);

  void startMonitoring(int intervalMs = 1000);
  void stopMonitoring();
  bool isMonitoring() const;

  BusTraffic busTraffic() const { return traffic_; }
  ErrorRate errorRate() const { return errorRate_; }
  PerformanceMetrics performance() const { return perf_; }
  HealthStatus health() const { return health_; }

signals:
  void trafficUpdated(const BusTraffic &traffic);
  void errorRateUpdated(const ErrorRate &rate);
  void performanceUpdated(const PerformanceMetrics &metrics);
  void healthUpdated(const HealthStatus &health);
  void error(const QString &msg);

private:
  void poll();

  EcatClient *client_;
  QTimer *pollTimer_ = nullptr;
  BusTraffic traffic_;
  ErrorRate errorRate_;
  PerformanceMetrics perf_;
  HealthStatus health_;
  quint64 prevTxFrames_ = 0;
  quint64 prevRxFrames_ = 0;
  quint64 prevTotalErrors_ = 0;
};
