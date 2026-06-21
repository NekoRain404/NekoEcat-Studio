#pragma once

// EtherCATAnalyticsService — advanced analytics for data, performance,
// errors, and usage patterns.
//
// Provides on-demand analysis methods that produce structured results
// with trends, patterns, anomalies, and recommendations.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QVector>
#include <QMap>
#include <QString>

class EcatClient;
class EventBus;

struct DataPoint {
  qint64 timestamp = 0;
  double value = 0.0;
  QString source;
};

struct AnalysisResult {
  QString category;
  QString summary;
  QVector<QString> trends;
  QVector<QString> patterns;
  QVector<QString> anomalies;
  QVector<QString> recommendations;
};

class EtherCATAnalyticsService : public QObject {
  Q_OBJECT
public:
  explicit EtherCATAnalyticsService(EventBus *bus, EcatClient *client,
                                    QObject *parent = nullptr);

  AnalysisResult analyzeData(const QVector<DataPoint> &data);
  AnalysisResult analyzePerformance(int durationSec);
  AnalysisResult analyzeErrors(int durationSec);
  AnalysisResult analyzeUsage(int durationSec);

signals:
  void analysisCompleted(const AnalysisResult &result);

private:
  AnalysisResult makeResult(const QString &category, const QString &summary,
                            const QVector<QString> &trends,
                            const QVector<QString> &patterns,
                            const QVector<QString> &anomalies,
                            const QVector<QString> &recommendations);

  EventBus *bus_;
  EcatClient *client_;

  QVector<DataPoint> perfBuffer_;
  QVector<DataPoint> errorBuffer_;
  QVector<DataPoint> usageBuffer_;
};
