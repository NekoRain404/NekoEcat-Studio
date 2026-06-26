#pragma once

// WorkflowCertificationService — manages workflow process certification,
// quality certification, and certificate lifecycle for EtherCAT projects.
//
// This service provides certification capabilities:
//   - Process certification with configurable requirements
//   - Quality certification against standards
//   - Certificate retrieval, renewal, and revocation
//   - Certification history tracking
//
// Usage:
//   WorkflowCertificationService svc;
//   WfProcessConfig cfg{"Commissioning", {"init","config"}, {"doc-complete"}};
//   auto result = svc.certifyProcess(cfg);
//   auto cert = svc.certificate(result.certificateId);
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDateTime>

struct WfProcessConfig {
  QString processName;
  QStringList steps;
  QStringList requirements;
  double tolerance = 0.0;
};

struct WfQualityConfig {
  QString qualityStandard;
  QStringList metrics;
  double minScore = 0.0;
  QStringList thresholds;
};

struct WfSafetyConfig {
  QString safetyLevel;
  QStringList hazards;
  QStringList mitigations;
  int silLevel = 0;
};

struct WfComplianceConfig {
  QString regulation;
  QStringList requirements;
  QStringList evidence;
  QString jurisdiction;
};

struct WfCertificationResult {
  QString certificateId;
  QDateTime timestamp;
  bool valid = false;
  QDateTime expiry;
  QString scope;
  QStringList conditions;
};

class WorkflowCertificationService : public QObject {
  Q_OBJECT
public:
  explicit WorkflowCertificationService(QObject *parent = nullptr);

  WfCertificationResult certifyProcess(const WfProcessConfig &config);
  WfCertificationResult certifyQuality(const WfQualityConfig &config);
  WfCertificationResult certifySafety(const WfSafetyConfig &config);
  WfCertificationResult certifyCompliance(const WfComplianceConfig &config);

signals:
  void certificationCompleted(const WfCertificationResult &result);

private:
  WfCertificationResult rejectedResult(const QString &scope) const;
};
