#pragma once

// EtherCATComplianceService — compliance rule management and compliance check
// request facade for EtherCAT network configurations.
//
// Provides rule CRUD plus rejected per-category and full compliance reports
// until a real compliance backend is available. It must not synthesize passing
// evidence or 100-point scores.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QVector>
#include <QString>

struct ComplianceRule {
    QString ruleId;
    QString category;
    QString description;
    int severity = 0;
    bool enabled = true;
};

struct ComplianceCheckResult {
    QString ruleId;
    bool passed = false;
    QString details;
    QString recommendation;
};

struct ComplianceReport {
    QVector<ComplianceCheckResult> results;
    int totalRules = 0;
    int passedCount = 0;
    int failedCount = 0;
    double score = 0.0;
};

class EtherCATComplianceService : public QObject {
    Q_OBJECT
public:
    explicit EtherCATComplianceService(QObject *parent = nullptr);

    void addRule(const ComplianceRule &rule);
    bool removeRule(const QString &ruleId);
    QVector<ComplianceRule> rules() const;
    ComplianceReport runComplianceCheck();
    ComplianceReport checkCategory(const QString &category);

signals:
    void ruleAdded(const ComplianceRule &rule);
    void ruleRemoved(const QString &ruleId);
    void checkCompleted(const ComplianceReport &report);

private:
    QVector<ComplianceRule> rules_;
};
