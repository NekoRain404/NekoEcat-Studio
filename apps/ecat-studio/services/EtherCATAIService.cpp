#include "EtherCATAIService.h"
#include <QtMath>

// EtherCATAIService.cpp — AI-driven predictive maintenance and anomaly detection
//
// Implementation notes:
//   - Uses statistical analysis (mean/max deviation) for maintenance predictions
//   - Anomaly detection based on standard deviation thresholds
//   - Emits Qt signals for prediction and anomaly events

EtherCATAIService::EtherCATAIService(QObject *parent)
    : QObject(parent)
{
}

Prediction EtherCATAIService::predictMaintenance(const QVector<AIDataPoint> &data)
{
    Prediction pred;
    if (data.isEmpty())
        return pred;

    double sum = 0;
    double maxVal = data[0].value;
    for (const auto &dp : data) {
        sum += dp.value;
        if (dp.value > maxVal) maxVal = dp.value;
    }
    double mean = sum / data.size();

    pred.component = data[0].label;
    pred.probability = qBound(0.0, (maxVal - mean) / (mean + 1.0), 1.0);
    pred.timeframeDays = pred.probability > 0.7 ? 7 : 30;
    pred.confidence = qBound(0.0, 0.5 + data.size() * 0.01, 0.99);

    if (pred.probability > 0.7)
        pred.recommendations << QStringLiteral("Schedule immediate inspection");
    else if (pred.probability > 0.4)
        pred.recommendations << QStringLiteral("Monitor closely");

    emit predictionMade(pred);
    return pred;
}

QVector<Anomaly> EtherCATAIService::detectAnomalies(const QVector<AIDataPoint> &data)
{
    QVector<Anomaly> anomalies;
    if (data.size() < 2)
        return anomalies;

    double sum = 0;
    for (const auto &dp : data)
        sum += dp.value;
    double mean = sum / data.size();

    double varSum = 0;
    for (const auto &dp : data) {
        double diff = dp.value - mean;
        varSum += diff * diff;
    }
    double stddev = qSqrt(varSum / data.size());
    if (stddev < 1e-9)
        stddev = 1.0;

    for (const auto &dp : data) {
        double deviation = qAbs(dp.value - mean) / stddev;
        if (deviation > 2.0) {
            Anomaly a;
            a.point = dp;
            a.deviation = deviation;
            a.description = QStringLiteral("Value %1 deviates %2 sigma from mean %3")
                                .arg(dp.value, 0, 'f', 2)
                                .arg(deviation, 0, 'f', 2)
                                .arg(mean, 0, 'f', 2);
            if (deviation > 4.0)
                a.severity = Anomaly::Critical;
            else if (deviation > 3.0)
                a.severity = Anomaly::High;
            else
                a.severity = Anomaly::Medium;
            anomalies.append(a);
            emit anomalyDetected(a);
        }
    }
    return anomalies;
}

Optimization EtherCATAIService::optimizePerformance(const AIPerformanceMetrics &metrics)
{
    Optimization opt;
    opt.currentValue = metrics.latency;

    if (metrics.cpu > 80.0) {
        opt.target = QStringLiteral("CPU");
        opt.suggestedValue = metrics.cpu * 0.8;
        opt.expectedImprovement = 20.0;
        opt.description = QStringLiteral("Reduce CPU load by scaling workloads");
    } else if (metrics.memory > 80.0) {
        opt.target = QStringLiteral("Memory");
        opt.suggestedValue = metrics.memory * 0.75;
        opt.expectedImprovement = 25.0;
        opt.description = QStringLiteral("Optimize memory usage with caching");
    } else if (metrics.latency > 10.0) {
        opt.target = QStringLiteral("Latency");
        opt.suggestedValue = metrics.latency * 0.5;
        opt.expectedImprovement = 50.0;
        opt.description = QStringLiteral("Reduce latency with connection pooling");
    } else {
        opt.target = QStringLiteral("Throughput");
        opt.suggestedValue = metrics.throughput * 1.2;
        opt.expectedImprovement = 20.0;
        opt.description = QStringLiteral("Increase throughput with batch processing");
    }

    return opt;
}

QVector<Pattern> EtherCATAIService::recognizePatterns(const QVector<AIDataPoint> &data)
{
    QVector<Pattern> patterns;
    if (data.size() < 3)
        return patterns;

    bool increasing = true;
    bool decreasing = true;
    for (int i = 1; i < data.size(); ++i) {
        if (data[i].value <= data[i - 1].value)
            increasing = false;
        if (data[i].value >= data[i - 1].value)
            decreasing = false;
    }

    if (increasing || decreasing) {
        Pattern p;
        p.name = increasing ? QStringLiteral("Monotonic Increase")
                            : QStringLiteral("Monotonic Decrease");
        p.description = QStringLiteral("Consistent %1 trend across %2 samples")
                            .arg(increasing ? QStringLiteral("upward")
                                            : QStringLiteral("downward"))
                            .arg(data.size());
        p.confidence = 0.95;
        p.samples = data.mid(0, qMin(10, data.size()));
        patterns.append(p);
    }

    double sum = 0;
    for (const auto &dp : data)
        sum += dp.value;
    double mean = sum / data.size();
    double varSum = 0;
    for (const auto &dp : data)
        varSum += (dp.value - mean) * (dp.value - mean);
    double stddev = qSqrt(varSum / data.size());

    if (stddev < mean * 0.1) {
        Pattern p;
        p.name = QStringLiteral("Stable");
        p.description = QStringLiteral("Values remain within 10%% of mean (%1)")
                            .arg(mean, 0, 'f', 2);
        p.confidence = 0.9;
        patterns.append(p);
    }

    return patterns;
}
