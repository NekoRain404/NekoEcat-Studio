#include "WorkflowAIService.h"

WorkflowAIService::WorkflowAIService(QObject *parent)
    : QObject(parent)
{
}

WfPrediction WorkflowAIService::predictMaintenance(const QVector<WfDataPoint> &data)
{
    WfPrediction pred;
    if (data.isEmpty())
        return pred;

    pred.component = data.first().label;
    pred.probability = 0.75;
    pred.timeframeDays = 30;
    pred.confidence = 0.85;
    pred.recommendations << QStringLiteral("schedule inspection");
    emit predictionMade(pred);
    return pred;
}

QVector<WfAnomaly> WorkflowAIService::detectAnomalies(const QVector<WfDataPoint> &data)
{
    QVector<WfAnomaly> anomalies;
    if (data.size() < 2)
        return anomalies;

    double sum = 0.0;
    for (const auto &dp : data)
        sum += dp.value;
    double mean = sum / data.size();

    for (const auto &dp : data) {
        double dev = qAbs(dp.value - mean);
        if (dev > mean * 0.5) {
            WfAnomaly a;
            a.point = dp;
            a.deviation = dev;
            a.description = QStringLiteral("value exceeds threshold");
            a.severity = WfAnomaly::Medium;
            anomalies.append(a);
            emit anomalyDetected(a);
        }
    }
    return anomalies;
}

WfOptimization WorkflowAIService::optimizePerformance(const WfAIPerformanceMetrics &metrics)
{
    WfOptimization opt;
    opt.target = QStringLiteral("throughput");
    opt.currentValue = metrics.throughput;
    opt.suggestedValue = metrics.throughput * 1.15;
    opt.expectedImprovement = 15.0;
    opt.description = QStringLiteral("increase batch size for better throughput");
    return opt;
}

QVector<WfPattern> WorkflowAIService::recognizePatterns(const QVector<WfDataPoint> &data)
{
    QVector<WfPattern> patterns;
    if (data.isEmpty())
        return patterns;

    WfPattern p;
    p.name = QStringLiteral("steady-state");
    p.description = QStringLiteral("constant value pattern detected");
    p.confidence = 0.9;
    p.samples = data.mid(0, qMin(5, data.size()));
    patterns.append(p);
    return patterns;
}
