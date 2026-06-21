#pragma once

// WorkflowComplianceService — checks workflow compliance against industry
// standards (IEC 61131-3), regulatory requirements, and security policies.
//
// This service provides compliance checking capabilities:
//   - Standard compliance checking with scoring
//   - Regulatory compliance verification
//   - Security policy compliance
//   - Compliance report generation
//
// Usage:
//   WorkflowComplianceService svc;
//   auto result = svc.checkStandardCompliance();
//   auto report = svc.generateComplianceReport();
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QString>
#include <QStringList>

struct ComplianceResult {
  QString standard;
  QString version;
  bool compliant = false;
  QStringList violations;
  QStringList recommendations;
  double score = 0.0;
};

class WorkflowComplianceService : public QObject {
  Q_OBJECT
public:
  explicit WorkflowComplianceService(QObject *parent = nullptr);

  ComplianceResult checkStandardCompliance();
  ComplianceResult checkRegulatoryCompliance();
  ComplianceResult checkSafetyCompliance();
  ComplianceResult checkQualityCompliance();

signals:
  void complianceChecked(const ComplianceResult &result);
};
