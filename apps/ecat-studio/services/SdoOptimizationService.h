#pragma once

// SdoOptimizationService — SDO communication optimization.
//
// Provides SDO optimization request helpers. Until a live SDO optimization
// backend is wired, requests return recommendations only and do not synthesize
// before/after measurements or successful communication changes.
//
// Usage:
//   SdoOptimizationService *svc = new SdoOptimizationService(client, bus, this);
//   OptimizationResult result = svc->optimizeCache();
//   svc->applyOptimization(result);

#include <QObject>
#include <QJsonObject>
#include <QVector>
#include <QDateTime>
#include <QString>

class EcatClient;
class EventBus;

struct SdoOptimizationResult {
  QString category;
  QString description;
  QJsonObject before;
  QJsonObject after;
  double improvement = 0.0;
  QStringList recommendations;
  bool applied = false;
  QDateTime timestamp;
};

Q_DECLARE_METATYPE(SdoOptimizationResult)

class SdoOptimizationService : public QObject {
  Q_OBJECT
public:
  explicit SdoOptimizationService(EcatClient *client, EventBus *bus,
                                  QObject *parent = nullptr);

  SdoOptimizationResult optimizeCache();
  SdoOptimizationResult optimizeBatch();
  SdoOptimizationResult optimizePerformance();
  SdoOptimizationResult optimizeErrorHandling();

  // Apply an optimization to live SDO communication settings.
  // Returns false while offline instead of recording a synthetic applied item.
  bool applyOptimization(const SdoOptimizationResult &result);

  QVector<SdoOptimizationResult> optimizationHistory() const { return history_; }
  void clearHistory();

signals:
  void optimizationCompleted(const SdoOptimizationResult &result);
  void optimizationApplied(const SdoOptimizationResult &result);

private:
  SdoOptimizationResult createRejectedResult(
      const QString &category, const QStringList &recommendations);

  EcatClient *client_;
  EventBus *bus_;
  QVector<SdoOptimizationResult> history_;
};
