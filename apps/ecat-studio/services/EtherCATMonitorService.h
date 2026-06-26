#pragma once

// EtherCATMonitorService — real-time monitoring of EtherCAT bus traffic,
// error rates, performance metrics, and overall health status.
//
// Provides timer-based polling at a configurable interval (default 1s).
// Emits trafficUpdated(), errorRateUpdated(), performanceUpdated(), and
// healthUpdated() signals on each poll cycle. Offline start requests do not
// synthesize an active monitoring session.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QTimer>
#include <QVector>

class EcatClient;
class EventBus;

struct BusTraffic {
  quint64 txFrames = 0;
  quint64 rxFrames = 0;
  quint64 txBytes = 0;
  quint64 rxBytes = 0;
  double bandwidth = 0.0;
  double utilization = 0.0;
};

struct ErrorRate {
  double rate = 0.0;
  quint64 totalErrors = 0;
  quint64 crcErrors = 0;
  quint64 frameErrors = 0;
  quint64 lostErrors = 0;
  quint64 overrunErrors = 0;
};

struct PerformanceMetrics {
  double cycleTimeUs = 0.0;
  double jitterUs = 0.0;
  double frameLossRate = 0.0;
  double sdoResponseMs = 0.0;
  double pdoUpdateRate = 0.0;
};

struct HealthStatus {
  int score = 100;
  QString grade;
  int totalSlaves = 0;
  int opSlaves = 0;
  int errorSlaves = 0;
  bool dcInSync = false;
  double dcDriftNs = 0.0;
  bool watchdogOk = true;
};

class EtherCATMonitorService : public QObject {
  Q_OBJECT
public:
  explicit EtherCATMonitorService(EventBus *bus, EcatClient *client,
                                  QObject *parent = nullptr);

  void startMonitoring(int intervalMs = 1000);
  void stopMonitoring();
  bool isMonitoring() const { return running_; }

  BusTraffic busTraffic() const { return traffic_; }
  ErrorRate errorRate() const { return errors_; }
  PerformanceMetrics performance() const { return perf_; }
  HealthStatus health() const { return health_; }

  void updateTraffic(const BusTraffic &t);
  void updateErrorRate(const ErrorRate &r);
  void updatePerformance(const PerformanceMetrics &m);
  void updateHealth(const HealthStatus &h);

signals:
  void trafficUpdated(const BusTraffic &traffic);
  void errorRateUpdated(const ErrorRate &rate);
  void performanceUpdated(const PerformanceMetrics &metrics);
  void healthUpdated(const HealthStatus &health);

private:
  void poll();

  EventBus *bus_;
  EcatClient *client_;
  QTimer *timer_ = nullptr;
  bool running_ = false;

  BusTraffic traffic_;
  ErrorRate errors_;
  PerformanceMetrics perf_;
  HealthStatus health_;
};
