#include "EtherCATAnalyticsService.h"
#include "EventBus.h"
#include "infra/EcatClient.h"
#include <algorithm>
#include <cmath>
#include <QDateTime>

// EtherCATAnalyticsService.cpp — Statistical analytics for EtherCAT data streams
//
// Implementation notes:
//   - Computes mean, stddev, variance, trend detection, and anomaly identification
//   - Uses 2-sigma threshold for anomaly detection on data point buffers
//   - Provides time-windowed analysis for performance, errors, and resource usage

EtherCATAnalyticsService::EtherCATAnalyticsService(EventBus* bus, EcatClient* client, QObject* parent)
    : QObject(parent), bus_(bus), client_(client) {}

AnalysisResult EtherCATAnalyticsService::makeResult(const QString& category, const QString& summary,
                                                    const QVector<QString>& trends, const QVector<QString>& patterns,
                                                    const QVector<QString>& anomalies,
                                                    const QVector<QString>& recommendations) {
    AnalysisResult r;
    r.category = category;
    r.summary = summary;
    r.trends = trends;
    r.patterns = patterns;
    r.anomalies = anomalies;
    r.recommendations = recommendations;
    emit analysisCompleted(r);
    return r;
}

AnalysisResult EtherCATAnalyticsService::analyzeData(const QVector<DataPoint>& data) {
    if (data.isEmpty()) {
        return makeResult(QStringLiteral("Data"), QStringLiteral("No data points to analyze"), {}, {}, {},
                          {QStringLiteral("Collect data before analysis")});
    }

    double sum = 0.0, minVal = data.first().value, maxVal = data.first().value;
    for (const auto& dp : data) {
        sum += dp.value;
        minVal = std::min(minVal, dp.value);
        maxVal = std::max(maxVal, dp.value);
    }
    double mean = sum / data.size();

    double variance = 0.0;
    for (const auto& dp : data)
        variance += (dp.value - mean) * (dp.value - mean);
    variance /= data.size();
    double stddev = std::sqrt(variance);

    QVector<QString> trends;
    if (data.size() >= 2) {
        double firstHalf = 0, secondHalf = 0;
        int half = data.size() / 2;
        for (int i = 0; i < half; ++i)
            firstHalf += data[i].value;
        for (int i = half; i < data.size(); ++i)
            secondHalf += data[i].value;
        firstHalf /= half;
        secondHalf /= (data.size() - half);
        if (secondHalf > firstHalf * 1.05)
            trends << QStringLiteral("Increasing trend detected");
        else if (secondHalf < firstHalf * 0.95)
            trends << QStringLiteral("Decreasing trend detected");
        else
            trends << QStringLiteral("Stable trend");
    }

    QVector<QString> anomalies;
    for (const auto& dp : data) {
        if (std::abs(dp.value - mean) > 2.0 * stddev)
            anomalies << QStringLiteral("Anomaly at %1: value %2").arg(dp.timestamp).arg(dp.value);
    }

    QVector<QString> patterns;
    if (maxVal - minVal < mean * 0.1)
        patterns << QStringLiteral("Low variance - consistent behavior");
    else if (stddev > mean * 0.5)
        patterns << QStringLiteral("High variance - erratic behavior");

    QVector<QString> recommendations;
    if (!anomalies.isEmpty())
        recommendations << QStringLiteral("Investigate anomalous data points");
    if (stddev > mean * 0.3)
        recommendations << QStringLiteral("High variability suggests tuning needed");
    if (recommendations.isEmpty())
        recommendations << QStringLiteral("System operating within normal parameters");

    QString summary = QStringLiteral("Analyzed %1 points: mean=%2, stddev=%3, range=[%4, %5]")
                          .arg(data.size())
                          .arg(mean, 0, 'f', 2)
                          .arg(stddev, 0, 'f', 2)
                          .arg(minVal, 0, 'f', 2)
                          .arg(maxVal, 0, 'f', 2);

    return makeResult(QStringLiteral("Data"), summary, trends, patterns, anomalies, recommendations);
}

AnalysisResult EtherCATAnalyticsService::analyzePerformance(int durationSec) {
    qint64 cutoff = QDateTime::currentMSecsSinceEpoch() - (durationSec * 1000LL);
    QVector<DataPoint> filtered;
    for (const auto& dp : perfBuffer_) {
        if (dp.timestamp >= cutoff)
            filtered.append(dp);
    }

    if (filtered.isEmpty()) {
        return makeResult(QStringLiteral("Performance"),
                          QStringLiteral("No performance data for the specified duration"), {}, {}, {},
                          {QStringLiteral("Start performance monitoring to collect data")});
    }

    double sum = 0.0;
    for (const auto& dp : filtered)
        sum += dp.value;
    double avg = sum / filtered.size();

    QVector<QString> trends;
    trends << QStringLiteral("Average performance: %1").arg(avg, 0, 'f', 2);

    QVector<QString> recommendations;
    if (avg > 1000.0)
        recommendations << QStringLiteral("Cycle time exceeds 1ms - consider optimization");
    else if (avg < 500.0)
        recommendations << QStringLiteral("Good cycle time performance");
    else
        recommendations << QStringLiteral("Acceptable cycle time - monitor for degradation");

    return makeResult(QStringLiteral("Performance"),
                      QStringLiteral("Performance analysis over %1 seconds").arg(durationSec), trends, {}, {},
                      recommendations);
}

AnalysisResult EtherCATAnalyticsService::analyzeErrors(int durationSec) {
    qint64 cutoff = QDateTime::currentMSecsSinceEpoch() - (durationSec * 1000LL);
    QVector<DataPoint> filtered;
    for (const auto& dp : errorBuffer_) {
        if (dp.timestamp >= cutoff)
            filtered.append(dp);
    }

    int totalErrors = filtered.size();
    QVector<QString> trends;
    if (totalErrors > 0) {
        double errorRate = static_cast<double>(totalErrors) / durationSec;
        trends << QStringLiteral("Error rate: %1/sec").arg(errorRate, 0, 'f', 3);
    } else {
        trends << QStringLiteral("No errors detected");
    }

    QVector<QString> recommendations;
    if (totalErrors > 100)
        recommendations << QStringLiteral("High error count - check cabling and slave health");
    else if (totalErrors > 10)
        recommendations << QStringLiteral("Moderate errors - monitor for patterns");
    else
        recommendations << QStringLiteral("Error rate within acceptable limits");

    return makeResult(QStringLiteral("Errors"),
                      QStringLiteral("Error analysis: %1 errors in %2 seconds").arg(totalErrors).arg(durationSec),
                      trends, {}, {}, recommendations);
}

AnalysisResult EtherCATAnalyticsService::analyzeUsage(int durationSec) {
    qint64 cutoff = QDateTime::currentMSecsSinceEpoch() - (durationSec * 1000LL);
    QVector<DataPoint> filtered;
    for (const auto& dp : usageBuffer_) {
        if (dp.timestamp >= cutoff)
            filtered.append(dp);
    }

    double sum = 0.0;
    for (const auto& dp : filtered)
        sum += dp.value;
    double avg = filtered.isEmpty() ? 0.0 : sum / filtered.size();

    QVector<QString> trends;
    trends << QStringLiteral("Average usage: %1%").arg(avg, 0, 'f', 1);

    QVector<QString> recommendations;
    if (avg > 80.0)
        recommendations << QStringLiteral("High resource usage - consider scaling");
    else if (avg > 50.0)
        recommendations << QStringLiteral("Moderate usage - monitor for spikes");
    else
        recommendations << QStringLiteral("Resource usage is healthy");

    return makeResult(QStringLiteral("Usage"), QStringLiteral("Usage analysis over %1 seconds").arg(durationSec),
                      trends, {}, {}, recommendations);
}
