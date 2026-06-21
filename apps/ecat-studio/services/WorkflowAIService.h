#pragma once

// WorkflowAIService — AI-powered predictive maintenance, anomaly detection,
// performance optimization, and pattern recognition for workflow systems.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QVector>
#include <QString>
#include <QStringList>

struct WfDataPoint {
  double value = 0.0;
  qint64 timestamp = 0;
  QString label;
};

struct WfPrediction {
  QString component;
  double probability = 0.0;
  int timeframeDays = 0;
  double confidence = 0.0;
  QStringList recommendations;
};

struct WfAnomaly {
  WfDataPoint point;
  double deviation = 0.0;
  QString description;
  enum Severity { Low, Medium, High, Critical };
  Severity severity = Low;
};

struct WfOptimization {
  QString target;
  double currentValue = 0.0;
  double suggestedValue = 0.0;
  double expectedImprovement = 0.0;
  QString description;
};

struct WfPattern {
  QString name;
  QString description;
  double confidence = 0.0;
  QVector<WfDataPoint> samples;
};

struct WfAIPerformanceMetrics {
  double cpu = 0.0;
  double memory = 0.0;
  double latency = 0.0;
  double throughput = 0.0;
  double errorRate = 0.0;
  qint64 timestamp = 0;
};

class WorkflowAIService : public QObject {
  Q_OBJECT
public:
  explicit WorkflowAIService(QObject *parent = nullptr);

  WfPrediction predictMaintenance(const QVector<WfDataPoint> &data);
  QVector<WfAnomaly> detectAnomalies(const QVector<WfDataPoint> &data);
  WfOptimization optimizePerformance(const WfAIPerformanceMetrics &metrics);
  QVector<WfPattern> recognizePatterns(const QVector<WfDataPoint> &data);

signals:
  void predictionMade(const WfPrediction &prediction);
  void anomalyDetected(const WfAnomaly &anomaly);
};
