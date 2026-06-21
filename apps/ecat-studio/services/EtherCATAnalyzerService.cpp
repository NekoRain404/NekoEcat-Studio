#include "EtherCATAnalyzerService.h"
#include "infra/EcatClient.h"
#include "EventBus.h"
#include <QDateTime>
#include <algorithm>

// EtherCATAnalyzerService.cpp — Ring-buffer analysis for frames, errors, performance, and trends
//
// Implementation notes:
//   - All buffers capped at 10 000 samples with FIFO eviction
//   - Trend analysis uses linear regression (least-squares) to predict direction
//   - Performance rating based on max jitter: Excellent <10µs, Good <50µs, Fair <100µs

EtherCATAnalyzerService::EtherCATAnalyzerService(EventBus *bus, EcatClient *client,
                                                 QObject *parent)
    : QObject(parent), bus_(bus), client_(client)
{
}

void EtherCATAnalyzerService::addFrameSample(const FrameInfo &frame)
{
    frameBuffer_.append(frame);
    if (frameBuffer_.size() > 10000)
        frameBuffer_.remove(0, frameBuffer_.size() - 10000);
}

void EtherCATAnalyzerService::addErrorSample(const ErrorEntry &error)
{
    errorBuffer_.append(error);
    if (errorBuffer_.size() > 10000)
        errorBuffer_.remove(0, errorBuffer_.size() - 10000);
}

void EtherCATAnalyzerService::addPerformanceSample(const PerformanceSample &sample)
{
    perfBuffer_.append(sample);
    if (perfBuffer_.size() > 10000)
        perfBuffer_.remove(0, perfBuffer_.size() - 10000);
}

void EtherCATAnalyzerService::addTrendPoint(const TrendPoint &point)
{
    trendBuffer_.append(point);
    if (trendBuffer_.size() > 10000)
        trendBuffer_.remove(0, trendBuffer_.size() - 10000);
}

FrameAnalysis EtherCATAnalyzerService::analyzeFrames(int count)
{
    FrameAnalysis result;
    int n = qMin(count, frameBuffer_.size());
    int start = frameBuffer_.size() - n;

    result.totalFrames = frameBuffer_.size();
    double sumTime = 0.0;
    result.minFrameTimeUs = 1e18;
    result.maxFrameTimeUs = 0.0;

    for (int i = start; i < frameBuffer_.size(); ++i) {
        const FrameInfo &info = frameBuffer_[i];
        if (info.hasError)
            ++result.errorFrames;
        result.frameTypes[info.type]++;
        result.frameSizes[info.size]++;
        sumTime += info.timestampUs;
        result.minFrameTimeUs = qMin(result.minFrameTimeUs, info.timestampUs);
        result.maxFrameTimeUs = qMax(result.maxFrameTimeUs, info.timestampUs);
        result.samples.append(info);
    }

    if (n > 0)
        result.avgFrameTimeUs = sumTime / n;
    else
        result.minFrameTimeUs = 0.0;

    emit frameAnalysisCompleted(result);
    return result;
}

ErrorAnalysis EtherCATAnalyzerService::analyzeErrors(int count)
{
    ErrorAnalysis result;
    int n = qMin(count, errorBuffer_.size());
    int start = errorBuffer_.size() - n;

    result.totalErrors = errorBuffer_.size();

    for (int i = start; i < errorBuffer_.size(); ++i) {
        const ErrorEntry &entry = errorBuffer_[i];
        result.errorsByType[entry.type]++;
        result.errorsByPosition[entry.position]++;
        result.recentErrors.append(entry);
    }

    int maxTypeCount = 0;
    for (auto it = result.errorsByType.begin(); it != result.errorsByType.end(); ++it) {
        if (it.value() > maxTypeCount) {
            maxTypeCount = it.value();
            result.mostFrequentType = it.key();
        }
    }

    int maxPosCount = 0;
    result.mostAffectedPosition = -1;
    for (auto it = result.errorsByPosition.begin(); it != result.errorsByPosition.end(); ++it) {
        if (it.value() > maxPosCount) {
            maxPosCount = it.value();
            result.mostAffectedPosition = it.key();
        }
    }

    if (n > 0)
        result.errorRate = static_cast<double>(result.totalErrors) / n;

    emit errorAnalysisCompleted(result);
    return result;
}

PerformanceAnalysis EtherCATAnalyzerService::analyzePerformance(int durationMs)
{
    PerformanceAnalysis result;
    qint64 cutoff = QDateTime::currentMSecsSinceEpoch() - durationMs;

    double sumCycle = 0.0, sumJitter = 0.0, sumThroughput = 0.0;
    int count = 0;

    for (const auto &s : perfBuffer_) {
        if (s.timestampMs >= cutoff) {
            sumCycle += s.cycleTimeUs;
            sumJitter += s.jitterUs;
            sumThroughput += s.throughputMbps;
            result.totalFrameLoss += s.frameLoss;
            result.maxJitterUs = qMax(result.maxJitterUs, s.jitterUs);
            result.samples.append(s);
            ++count;
        }
    }

    if (count > 0) {
        result.avgCycleTimeUs = sumCycle / count;
        result.avgThroughputMbps = sumThroughput / count;
    }

    if (result.maxJitterUs < 10.0)
        result.rating = QStringLiteral("Excellent");
    else if (result.maxJitterUs < 50.0)
        result.rating = QStringLiteral("Good");
    else if (result.maxJitterUs < 100.0)
        result.rating = QStringLiteral("Fair");
    else
        result.rating = QStringLiteral("Poor");

    emit performanceAnalysisCompleted(result);
    return result;
}

TrendAnalysis EtherCATAnalyzerService::analyzeTrend(int durationMs)
{
    TrendAnalysis result;
    result.metric = QStringLiteral("cycle_time");
    qint64 cutoff = QDateTime::currentMSecsSinceEpoch() - durationMs;

    for (const auto &p : trendBuffer_) {
        if (p.timestampMs >= cutoff)
            result.points.append(p);
    }

    int n = result.points.size();
    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;

    for (int i = 0; i < n; ++i) {
        double x = static_cast<double>(i);
        double y = result.points[i].value;
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumX2 += x * x;
    }

    if (n > 1) {
        double denom = n * sumX2 - sumX * sumX;
        if (qAbs(denom) > 1e-12) {
            result.slope = (n * sumXY - sumX * sumY) / denom;
            result.intercept = (sumY - result.slope * sumX) / n;
        }
        result.predictedNext = result.slope * n + result.intercept;

        if (result.slope > 0.01)
            result.direction = QStringLiteral("increasing");
        else if (result.slope < -0.01)
            result.direction = QStringLiteral("decreasing");
        else
            result.direction = QStringLiteral("stable");
    }

    emit trendAnalysisCompleted(result);
    return result;
}
