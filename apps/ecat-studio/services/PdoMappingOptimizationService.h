#pragma once

// PdoMappingOptimizationService — PDO mapping optimization for EtherCAT slaves.
//
// Provides mapping optimization, size optimization, alignment optimization,
// and performance optimization for PDO configurations.
//
// Usage:
//   PdoMappingOptimizationService *svc = new PdoMappingOptimizationService(this);
//   PdoMappingOptimizationResult result = svc->optimizeMapping();
//   svc->applyOptimization(result);

#include <QObject>
#include <QJsonObject>
#include <QVector>
#include <QDateTime>
#include <QString>

struct PdoMappingOptimizationResult {
  QString category;
  QString description;
  QJsonObject before;
  QJsonObject after;
  double improvement = 0.0;
  QStringList recommendations;
  bool applied = false;
  QDateTime timestamp;
};

class PdoMappingOptimizationService : public QObject {
  Q_OBJECT
public:
  explicit PdoMappingOptimizationService(QObject *parent = nullptr);

  PdoMappingOptimizationResult optimizeMapping();
  PdoMappingOptimizationResult optimizeSize();
  PdoMappingOptimizationResult optimizeAlignment();
  PdoMappingOptimizationResult optimizePerformance();

  bool applyOptimization(const PdoMappingOptimizationResult &result);

  QVector<PdoMappingOptimizationResult> optimizationHistory() const { return history_; }
  void clearHistory();

signals:
  void optimizationCompleted(const PdoMappingOptimizationResult &result);
  void optimizationApplied(const PdoMappingOptimizationResult &result);

private:
  PdoMappingOptimizationResult createResult(const QString &category,
                                            const QString &description,
                                            const QJsonObject &before,
                                            const QJsonObject &after,
                                            double improvement,
                                            const QStringList &recommendations);

  QVector<PdoMappingOptimizationResult> history_;
};
