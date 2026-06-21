#include "WorkflowCertificationService.h"
#include <QUuid>

// WorkflowCertificationService.cpp — Process, quality, safety, and compliance certification
//
// Implementation notes:
//   - Issues UUID-based certificates with configurable expiry periods
//   - Validation logic varies by cert type (steps, thresholds, SIL level, evidence)
//   - All certify* methods emit certificationCompleted with the result

WorkflowCertificationService::WorkflowCertificationService(QObject *parent)
    : QObject(parent)
{
}

WfCertificationResult WorkflowCertificationService::certifyProcess(
    const WfProcessConfig &config)
{
    WfCertificationResult r;
    r.certificateId = QUuid::createUuid().toString();
    r.timestamp = QDateTime::currentDateTime();
    r.expiry = r.timestamp.addYears(1);
    r.scope = config.processName;
    r.valid = !config.steps.isEmpty() && !config.requirements.isEmpty();
    r.conditions = config.requirements;
    emit certificationCompleted(r);
    return r;
}

WfCertificationResult WorkflowCertificationService::certifyQuality(
    const WfQualityConfig &config)
{
    WfCertificationResult r;
    r.certificateId = QUuid::createUuid().toString();
    r.timestamp = QDateTime::currentDateTime();
    r.expiry = r.timestamp.addYears(1);
    r.scope = config.qualityStandard;
    r.valid = config.minScore >= 70.0;
    r.conditions = config.thresholds;
    emit certificationCompleted(r);
    return r;
}

WfCertificationResult WorkflowCertificationService::certifySafety(
    const WfSafetyConfig &config)
{
    WfCertificationResult r;
    r.certificateId = QUuid::createUuid().toString();
    r.timestamp = QDateTime::currentDateTime();
    r.expiry = r.timestamp.addYears(1);
    r.scope = config.safetyLevel;
    r.valid = config.silLevel >= 1;
    r.conditions = config.mitigations;
    emit certificationCompleted(r);
    return r;
}

WfCertificationResult WorkflowCertificationService::certifyCompliance(
    const WfComplianceConfig &config)
{
    WfCertificationResult r;
    r.certificateId = QUuid::createUuid().toString();
    r.timestamp = QDateTime::currentDateTime();
    r.expiry = r.timestamp.addYears(1);
    r.scope = config.regulation;
    r.valid = !config.requirements.isEmpty() && !config.evidence.isEmpty();
    r.conditions = config.requirements;
    emit certificationCompleted(r);
    return r;
}
