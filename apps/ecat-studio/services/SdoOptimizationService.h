#pragma once

// SdoOptimizationService — SDO communication optimization.
//
// Provides cache optimization, batch optimization, performance optimization,
// and error handling optimization for EtherCAT SDO transfers.
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

class SdoOptimizationService : public QObject {
  Q_OBJECT
public:
  explicit SdoOptimizationService(EcatClient *client, EventBus *bus,
                                  QObject *parent = nullptr);

  SdoOptimizationResult optimizeCache();
  SdoOptimizationResult optimizeBatch();
  SdoOptimizationResult optimizePerformance();
  SdoOptimizationResult optimizeErrorHandling();

  bool applyOptimization(const SdoOptimizationResult &result);

  QVector<SdoOptimizationResult> optimizationHistory() const { return history_; }
  void clearHistory();

signals:
  void optimizationCompleted(const SdoOptimizationResult &result);
  void optimizationApplied(const SdoOptimizationResult &result);

private:
  SdoOptimizationResult createResult(const QString &category,
                                     const QString &description,
                                     const QJsonObject &before,
                                     const QJsonObject &after,
                                     double improvement,
                                     const QStringList &recommendations);

  EcatClient *client_;
  EventBus *bus_;
  QVector<SdoOptimizationResult> history_;
};
