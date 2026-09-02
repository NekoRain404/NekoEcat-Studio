#pragma once

// AdvancedErrorAnalysisService — deep error analysis, pattern detection,
// correlation analysis, error prediction, and root cause analysis for
// EtherCAT networks.
//
// Thread safety: main (GUI) thread only.

#include <QDateTime>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QVector>

struct AdvancedErrorInfo {
    int id = 0;
    QDateTime timestamp;
    int slavePosition = -1;
    int errorCode = 0;
    QString message;
    QString category;
    QString severity;
    QString source;
    QJsonObject metadata;
};

struct ErrorPattern {
    QString patternId;
    QString description;
    int frequency = 0;
    QString severity;
    QStringList triggers;
    QStringList conditions;
    QDateTime firstSeen;
    QDateTime lastSeen;
    double confidence = 0.0;
};

struct CorrelationEntry {
    QString errorTypeA;
    QString errorTypeB;
    double correlation = 0.0;
    int coOccurrenceCount = 0;
    QString relationship;
};

struct CorrelationMatrix {
    QVector<CorrelationEntry> entries;
    int totalErrors = 0;
    QDateTime analysisTime;
    QStringList strongestCorrelations;
};

struct ErrorPrediction {
    QString errorType;
    double probability = 0.0;
    QDateTime predictedTime;
    QStringList basedOnPatterns;
    QString confidence;
    QString recommendation;
};

struct RootCauseAnalysis {
    QString errorType;
    QString rootCause;
    double confidence = 0.0;
    QStringList contributingFactors;
    QStringList recommendedActions;
    QStringList relatedErrors;
    QString explanation;
};

class AdvancedErrorAnalysisService : public QObject {
    Q_OBJECT
public:
    explicit AdvancedErrorAnalysisService(QObject* parent = nullptr);

    QVector<ErrorPattern> detectPatterns(const QVector<AdvancedErrorInfo>& errors);
    CorrelationMatrix analyzeCorrelation(const QVector<AdvancedErrorInfo>& errors);
    QVector<ErrorPrediction> predictErrors(const QVector<AdvancedErrorInfo>& errors);
    RootCauseAnalysis analyzeRootCause(const AdvancedErrorInfo& error);

    QVector<AdvancedErrorInfo> errorHistory() const;
    void addError(const AdvancedErrorInfo& error);
    void clearHistory();

signals:
    void patternDetected(const ErrorPattern& pattern);
    void correlationAnalyzed(const CorrelationMatrix& matrix);
    void predictionGenerated(const ErrorPrediction& prediction);
    void rootCauseFound(const RootCauseAnalysis& analysis);

private:
    QVector<AdvancedErrorInfo> history_;
    static constexpr int kMaxHistory = 5000;
};
