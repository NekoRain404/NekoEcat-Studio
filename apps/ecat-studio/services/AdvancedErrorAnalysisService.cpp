#include "AdvancedErrorAnalysisService.h"

#include <QMap>
#include <QSet>
#include <QtMath>
#include <algorithm>

AdvancedErrorAnalysisService::AdvancedErrorAnalysisService(QObject *parent)
    : QObject(parent) {}

QVector<AdvancedErrorInfo> AdvancedErrorAnalysisService::errorHistory() const {
  return history_;
}

void AdvancedErrorAnalysisService::addError(const AdvancedErrorInfo &error) {
  history_.append(error);
  if (history_.size() > kMaxHistory)
    history_.remove(0, history_.size() - kMaxHistory);
}

void AdvancedErrorAnalysisService::clearHistory() { history_.clear(); }

QVector<ErrorPattern> AdvancedErrorAnalysisService::detectPatterns(
    const QVector<AdvancedErrorInfo> &errors) {
  QVector<ErrorPattern> patterns;
  if (errors.isEmpty()) return patterns;

  QMap<QString, QVector<int>> byCategory;
  for (int i = 0; i < errors.size(); ++i)
    byCategory[errors[i].category].append(i);

  for (auto it = byCategory.begin(); it != byCategory.end(); ++it) {
    const auto &indices = it.value();
    if (indices.size() < 2) continue;

    ErrorPattern pat;
    pat.patternId = QStringLiteral("PAT-%1-%2").arg(it.key()).arg(patterns.size());
    pat.frequency = indices.size();
    pat.firstSeen = errors[indices.first()].timestamp;
    pat.lastSeen = errors[indices.last()].timestamp;
    pat.severity = errors[indices.first()].severity;

    QMap<QString, int> triggerCounts;
    for (int idx : indices) {
      const auto &meta = errors[idx].metadata;
      for (auto k = meta.begin(); k != meta.end(); ++k)
        triggerCounts[k.key()]++;
    }
    for (auto t = triggerCounts.begin(); t != triggerCounts.end(); ++t) {
      if (t.value() >= indices.size() / 2)
        pat.triggers.append(t.key());
    }

    int intervalSum = 0;
    int intervalCount = 0;
    for (int i = 1; i < indices.size(); ++i) {
      qint64 ms = errors[indices[i - 1]].timestamp.msecsTo(
          errors[indices[i]].timestamp);
      if (ms > 0 && ms < 60000) {
        intervalSum += static_cast<int>(ms);
        ++intervalCount;
      }
    }
    if (intervalCount > 0) {
      int avgInterval = intervalSum / intervalCount;
      pat.conditions.append(QStringLiteral("avg_interval_ms=%1").arg(avgInterval));
    }

    pat.confidence = qBound(0.0, indices.size() / 10.0, 1.0);
    pat.description = QStringLiteral("Repeated %1 errors (%2 occurrences)")
                          .arg(it.key())
                          .arg(indices.size());

    patterns.append(pat);
    emit patternDetected(pat);
  }

  return patterns;
}

CorrelationMatrix AdvancedErrorAnalysisService::analyzeCorrelation(
    const QVector<AdvancedErrorInfo> &errors) {
  CorrelationMatrix matrix;
  matrix.totalErrors = errors.size();
  matrix.analysisTime = QDateTime::currentDateTime();

  if (errors.size() < 2) return matrix;

  QMap<QString, int> typeCounts;
  for (const auto &e : errors) typeCounts[e.category]++;

  QStringList types;
  for (auto it = typeCounts.begin(); it != typeCounts.end(); ++it)
    types.append(it.key());

  for (int i = 0; i < types.size(); ++i) {
    for (int j = i + 1; j < types.size(); ++j) {
      const QString &a = types[i];
      const QString &b = types[j];
      int coCount = 0;
      for (int k = 0; k < errors.size() - 1; ++k) {
        if ((errors[k].category == a && errors[k + 1].category == b) ||
            (errors[k].category == b && errors[k + 1].category == a))
          ++coCount;
      }
      if (coCount > 0) {
        CorrelationEntry entry;
        entry.errorTypeA = a;
        entry.errorTypeB = b;
        entry.coOccurrenceCount = coCount;
        entry.correlation = qBound(0.0, static_cast<double>(coCount) /
                                            qMax(typeCounts[a], typeCounts[b]),
                                   1.0);
        entry.relationship = entry.correlation > 0.7
                                 ? QStringLiteral("Strong")
                                 : entry.correlation > 0.3
                                       ? QStringLiteral("Moderate")
                                       : QStringLiteral("Weak");
        matrix.entries.append(entry);
      }
    }
  }

  std::sort(matrix.entries.begin(), matrix.entries.end(),
            [](const CorrelationEntry &a, const CorrelationEntry &b) {
              return a.correlation > b.correlation;
            });

  for (int i = 0; i < qMin(5, matrix.entries.size()); ++i)
    matrix.strongestCorrelations.append(
        QStringLiteral("%1 <-> %2 (%3)")
            .arg(matrix.entries[i].errorTypeA)
            .arg(matrix.entries[i].errorTypeB)
            .arg(matrix.entries[i].relationship));

  emit correlationAnalyzed(matrix);
  return matrix;
}

QVector<ErrorPrediction> AdvancedErrorAnalysisService::predictErrors(
    const QVector<AdvancedErrorInfo> &errors) {
  QVector<ErrorPrediction> predictions;
  if (errors.size() < 5) return predictions;

  QMap<QString, QVector<QDateTime>> timestamps;
  for (const auto &e : errors)
    timestamps[e.category].append(e.timestamp);

  for (auto it = timestamps.begin(); it != timestamps.end(); ++it) {
    const auto &ts = it.value();
    if (ts.size() < 3) continue;

    qint64 totalGap = 0;
    int gapCount = 0;
    for (int i = 1; i < ts.size(); ++i) {
      qint64 gap = ts[i - 1].msecsTo(ts[i]);
      if (gap > 0) {
        totalGap += gap;
        ++gapCount;
      }
    }
    if (gapCount == 0) continue;

    qint64 avgGap = totalGap / gapCount;
    qint64 timeSinceLast = ts.last().msecsTo(QDateTime::currentDateTime());
    double probability = qBound(0.0, static_cast<double>(timeSinceLast) / avgGap,
                                1.0);

    ErrorPrediction pred;
    pred.errorType = it.key();
    pred.probability = probability;
    pred.predictedTime = ts.last().addMSecs(avgGap);
    pred.basedOnPatterns = {QStringLiteral("interval_analysis")};
    pred.confidence = probability > 0.7 ? QStringLiteral("High")
                    : probability > 0.3 ? QStringLiteral("Medium")
                                        : QStringLiteral("Low");
    pred.recommendation = probability > 0.7
        ? QStringLiteral("Proactive maintenance recommended for %1 errors").arg(it.key())
        : QStringLiteral("Monitor %1 error trends").arg(it.key());

    predictions.append(pred);
    emit predictionGenerated(pred);
  }

  std::sort(predictions.begin(), predictions.end(),
            [](const ErrorPrediction &a, const ErrorPrediction &b) {
              return a.probability > b.probability;
            });

  return predictions;
}

RootCauseAnalysis AdvancedErrorAnalysisService::analyzeRootCause(
    const AdvancedErrorInfo &error) {
  RootCauseAnalysis rca;
  rca.errorType = error.category;

  if (error.category == "Communication") {
    rca.rootCause = "Network communication failure";
    rca.confidence = 0.85;
    rca.contributingFactors = {"Cable disconnection", "EMI interference",
                               "Switch port failure"};
    rca.recommendedActions = {"Check cable connections",
                              "Verify switch port status",
                              "Run cable diagnostics"};
  } else if (error.category == "Device") {
    rca.rootCause = "Slave device failure";
    rca.confidence = 0.80;
    rca.contributingFactors = {"Device firmware issue", "Power supply problem",
                               "Hardware fault"};
    rca.recommendedActions = {"Check device power supply",
                              "Update device firmware",
                              "Replace device if persistent"};
  } else if (error.category == "Configuration") {
    rca.rootCause = "Configuration mismatch";
    rca.confidence = 0.90;
    rca.contributingFactors = {"Invalid SDO parameter", "PDO mapping conflict",
                               "DC sync configuration error"};
    rca.recommendedActions = {"Validate SDO parameters",
                              "Check PDO mapping consistency",
                              "Verify DC sync settings"};
  } else if (error.category == "Protocol") {
    rca.rootCause = "Protocol violation";
    rca.confidence = 0.75;
    rca.contributingFactors = {"Frame corruption", "Timing violation",
                               "State machine error"};
    rca.recommendedActions = {"Check frame integrity",
                              "Verify timing parameters",
                              "Review state machine transitions"};
  } else {
    rca.rootCause = "Unknown error source";
    rca.confidence = 0.50;
    rca.contributingFactors = {"Insufficient data for classification"};
    rca.recommendedActions = {"Collect more error data",
                              "Enable detailed logging"};
  }

  for (const auto &e : history_) {
    if (e.category == error.category && e.id != error.id &&
        !rca.relatedErrors.contains(e.message))
      rca.relatedErrors.append(e.message);
    if (rca.relatedErrors.size() >= 5) break;
  }

  rca.explanation = QStringLiteral("Error '%1' at slave %2: %3")
                        .arg(error.message)
                        .arg(error.slavePosition)
                        .arg(rca.rootCause);

  emit rootCauseFound(rca);
  return rca;
}
