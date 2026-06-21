#pragma once

// EtherCATAnalyzerService — protocol analysis for EtherCAT frames, errors,
// performance, and trends.
//
// Provides on-demand analysis methods that produce structured results.
// Emits analysisCompleted() signals when each analysis finishes.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QVector>
#include <QMap>
#include <QString>

class EcatClient;
class EventBus;

struct FrameInfo {
  int position = -1;
  QString type;
  int size = 0;
  double timestampUs = 0.0;
  bool hasError = false;
  QString errorDetail;
};

struct FrameAnalysis {
  int totalFrames = 0;
  int errorFrames = 0;
  QMap<QString, int> frameTypes;
  QMap<int, int> frameSizes;
  double avgFrameTimeUs = 0.0;
  double maxFrameTimeUs = 0.0;
  double minFrameTimeUs = 0.0;
  QVector<FrameInfo> samples;
};

struct ErrorEntry {
  qint64 timestampMs = 0;
  int position = -1;
  QString type;
  QString description;
  int severity = 0;
};

struct ErrorAnalysis {
  int totalErrors = 0;
  QMap<QString, int> errorsByType;
  QMap<int, int> errorsByPosition;
  QVector<ErrorEntry> recentErrors;
  double errorRate = 0.0;
  QString mostFrequentType;
  int mostAffectedPosition = -1;
};

struct PerformanceSample {
  qint64 timestampMs = 0;
  double cycleTimeUs = 0.0;
  double jitterUs = 0.0;
  double throughputMbps = 0.0;
  int frameLoss = 0;
};

struct PerformanceAnalysis {
  double avgCycleTimeUs = 0.0;
  double maxJitterUs = 0.0;
  double avgThroughputMbps = 0.0;
  int totalFrameLoss = 0;
  QVector<PerformanceSample> samples;
  QString rating;
};

struct TrendPoint {
  qint64 timestampMs = 0;
  double value = 0.0;
};

struct TrendAnalysis {
  QString metric;
  QVector<TrendPoint> points;
  double slope = 0.0;
  double intercept = 0.0;
  QString direction;
  double predictedNext = 0.0;
};

class EtherCATAnalyzerService : public QObject {
  Q_OBJECT
public:
  explicit EtherCATAnalyzerService(EventBus *bus, EcatClient *client,
                                   QObject *parent = nullptr);

  FrameAnalysis analyzeFrames(int count = 100);
  ErrorAnalysis analyzeErrors(int count = 100);
  PerformanceAnalysis analyzePerformance(int durationMs = 5000);
  TrendAnalysis analyzeTrend(int durationMs = 60000);

  void addFrameSample(const FrameInfo &frame);
  void addErrorSample(const ErrorEntry &error);
  void addPerformanceSample(const PerformanceSample &sample);
  void addTrendPoint(const TrendPoint &point);

signals:
  void frameAnalysisCompleted(const FrameAnalysis &analysis);
  void errorAnalysisCompleted(const ErrorAnalysis &analysis);
  void performanceAnalysisCompleted(const PerformanceAnalysis &analysis);
  void trendAnalysisCompleted(const TrendAnalysis &analysis);

private:
  EventBus *bus_;
  EcatClient *client_;

  QVector<FrameInfo> frameBuffer_;
  QVector<ErrorEntry> errorBuffer_;
  QVector<PerformanceSample> perfBuffer_;
  QVector<TrendPoint> trendBuffer_;
};
