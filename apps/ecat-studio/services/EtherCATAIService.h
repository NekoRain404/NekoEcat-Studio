#pragma once

// EtherCATAIService — AI-powered predictive maintenance, anomaly detection,
// performance optimization, and pattern recognition for EtherCAT systems.
//
// Provides machine learning based analysis of EtherCAT telemetry data
// for proactive system management.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QVector>
#include <QString>

struct AIDataPoint {
  double value = 0.0;
  qint64 timestamp = 0;
  QString label;
};

struct Prediction {
  QString component;
  double probability = 0.0;
  int timeframeDays = 0;
  double confidence = 0.0;
  QStringList recommendations;
};

struct Anomaly {
  AIDataPoint point;
  double deviation = 0.0;
  QString description;
  enum Severity { Low, Medium, High, Critical };
  Severity severity = Low;
};

struct Optimization {
  QString target;
  double currentValue = 0.0;
  double suggestedValue = 0.0;
  double expectedImprovement = 0.0;
  QString description;
};

struct Pattern {
  QString name;
  QString description;
  double confidence = 0.0;
  QVector<AIDataPoint> samples;
};

struct AIPerformanceMetrics {
  double cpu = 0.0;
  double memory = 0.0;
  double latency = 0.0;
  double throughput = 0.0;
  double errorRate = 0.0;
  qint64 timestamp = 0;
};

class EtherCATAIService : public QObject {
  Q_OBJECT
public:
  explicit EtherCATAIService(QObject *parent = nullptr);

  Prediction predictMaintenance(const QVector<AIDataPoint> &data);
  QVector<Anomaly> detectAnomalies(const QVector<AIDataPoint> &data);
  Optimization optimizePerformance(const AIPerformanceMetrics &metrics);
  QVector<Pattern> recognizePatterns(const QVector<AIDataPoint> &data);

signals:
  void predictionMade(const Prediction &prediction);
  void anomalyDetected(const Anomaly &anomaly);
};
