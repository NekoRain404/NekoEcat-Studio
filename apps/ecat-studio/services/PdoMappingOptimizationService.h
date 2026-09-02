#pragma once

// PdoMappingOptimizationService — PDO mapping optimization for EtherCAT slaves.
//
// Provides PDO optimization request helpers. Until a real PDO mapping backend
// is wired, requests return recommendations only and do not synthesize
// before/after measurements or successful hardware changes.
//
// Usage:
//   PdoMappingOptimizationService *svc = new PdoMappingOptimizationService(this);
//   PdoMappingOptimizationResult result = svc->optimizeMapping();
//   svc->applyOptimization(result);

#include <QDateTime>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QVector>

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

Q_DECLARE_METATYPE(PdoMappingOptimizationResult)

class PdoMappingOptimizationService : public QObject {
    Q_OBJECT
public:
    explicit PdoMappingOptimizationService(QObject* parent = nullptr);

    PdoMappingOptimizationResult optimizeMapping();
    PdoMappingOptimizationResult optimizeSize();
    PdoMappingOptimizationResult optimizeAlignment();
    PdoMappingOptimizationResult optimizePerformance();

    // Apply an optimization through a real PDO mapping backend.
    // Returns false until such a backend is wired in.
    bool applyOptimization(const PdoMappingOptimizationResult& result);

    QVector<PdoMappingOptimizationResult> optimizationHistory() const { return history_; }
    void clearHistory();

signals:
    void optimizationCompleted(const PdoMappingOptimizationResult& result);
    void optimizationApplied(const PdoMappingOptimizationResult& result);

private:
    PdoMappingOptimizationResult createRejectedResult(const QString& category, const QStringList& recommendations);

    QVector<PdoMappingOptimizationResult> history_;
};
