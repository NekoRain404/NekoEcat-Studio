#pragma once

// FreeRunOptimizationService — Free Run optimization for process data exchange.
//
// Provides cycle time optimization, data mapping optimization, performance
// optimization, and error handling optimization for EtherCAT Free Run mode.
//
// Usage:
//   FreeRunOptimizationService *svc = new FreeRunOptimizationService(client, bus, this);
//   OptimizationResult result = svc->optimizeCycleTime();
//   svc->applyOptimization(result);

#include <QObject>
#include <QJsonObject>
#include <QVector>
#include <QDateTime>
#include <QString>

class EcatClient;
class EventBus;

struct FreeRunOptimizationResult {
  QString category;
  QString description;
  QJsonObject before;
  QJsonObject after;
  double improvement = 0.0;
  QStringList recommendations;
  bool applied = false;
  QDateTime timestamp;
};

Q_DECLARE_METATYPE(FreeRunOptimizationResult)

class FreeRunOptimizationService : public QObject {
  Q_OBJECT
public:
  explicit FreeRunOptimizationService(EcatClient *client, EventBus *bus,
                                      QObject *parent = nullptr);

  FreeRunOptimizationResult optimizeCycleTime();
  FreeRunOptimizationResult optimizeDataMapping();
  FreeRunOptimizationResult optimizePerformance();
  FreeRunOptimizationResult optimizeErrorHandling();

  bool applyOptimization(const FreeRunOptimizationResult &result);

  QVector<FreeRunOptimizationResult> optimizationHistory() const { return history_; }
  void clearHistory();

signals:
  void optimizationCompleted(const FreeRunOptimizationResult &result);
  void optimizationApplied(const FreeRunOptimizationResult &result);

private:
  FreeRunOptimizationResult createResult(const QString &category,
                                         const QString &description,
                                         const QJsonObject &before,
                                         const QJsonObject &after,
                                         double improvement,
                                         const QStringList &recommendations);

  EcatClient *client_;
  EventBus *bus_;
  QVector<FreeRunOptimizationResult> history_;
};
