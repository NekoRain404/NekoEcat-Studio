#pragma once

// FreeRunMonitoringService — Free Run process data and performance monitoring.
//
// Provides real-time monitoring of process data exchange, performance metrics,
// error tracking, and Free Run status during operation. Monitoring starts only
// when a live daemon connection exists; offline calls do not synthesize a
// running Free Run state.
//
// Usage:
//   FreeRunMonitoringService *svc = new FreeRunMonitoringService(client, eventBus, this);
//   svc->startMonitoring();
//   FreeRunProcessData pd = svc->processData();
//   FreeRunPerformanceMetrics perf = svc->performance();
//   FreeRunStatus st = svc->status();

#include <QObject>
#include <QVector>
#include <QString>
#include <QTimer>

class EcatClient;
class EventBus;

struct FreeRunProcessData {
  QVector<quint8> inputs;
  QVector<quint8> outputs;
  qint64 timestamp = 0;
  quint64 cycleCount = 0;
  int errorCount = 0;
  int warningCount = 0;
};

struct FreeRunPerformanceMetrics {
  double cycleTimeUs = 0.0;
  double jitterUs = 0.0;
  double cpuLoadPercent = 0.0;
  quint64 totalCycles = 0;
  quint64 missedCycles = 0;
  double throughputMbps = 0.0;
};

struct FreeRunErrorInfo {
  qint64 timestamp = 0;
  QString code;
  QString message;
  QString severity;
  int slavePosition = -1;
};

enum class FreeRunState { Idle, Configuring, Running, Error, Stopped };

struct FreeRunStatus {
  FreeRunState state = FreeRunState::Idle;
  QString stateString;
  qint64 uptimeMs = 0;
  quint64 totalCycles = 0;
  int activeSlaves = 0;
  int errorSlaves = 0;
};

class FreeRunMonitoringService : public QObject {
  Q_OBJECT
public:
  explicit FreeRunMonitoringService(EcatClient *client, EventBus *bus,
                                    QObject *parent = nullptr);

  void startMonitoring();
  void stopMonitoring();
  bool isMonitoring() const { return monitoring_; }

  FreeRunProcessData processData() const { return processData_; }
  FreeRunPerformanceMetrics performance() const { return perfMetrics_; }
  QVector<FreeRunErrorInfo> errors() const { return errors_; }
  FreeRunStatus status() const { return status_; }

  void updateProcessData(const FreeRunProcessData &data);
  void updatePerformance(const FreeRunPerformanceMetrics &metrics);
  void addError(const FreeRunErrorInfo &error);
  void updateStatus(const FreeRunStatus &status);

signals:
  void processDataUpdated(const FreeRunProcessData &data);
  void performanceUpdated(const FreeRunPerformanceMetrics &metrics);
  void errorOccurred(const FreeRunErrorInfo &error);
  void statusChanged(const FreeRunStatus &status);
  void monitoringStateChanged(bool active);

private slots:
  void pollData();

private:
  EcatClient *client_;
  EventBus *bus_;
  QTimer *pollTimer_ = nullptr;
  bool monitoring_ = false;

  FreeRunProcessData processData_;
  FreeRunPerformanceMetrics perfMetrics_;
  QVector<FreeRunErrorInfo> errors_;
  FreeRunStatus status_;
};
