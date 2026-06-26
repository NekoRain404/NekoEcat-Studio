#include "WorkflowCertificationService.h"

// WorkflowCertificationService.cpp — Process, quality, safety, and compliance certification
//
// Implementation notes:
//   - Certification requires an external authority/backend.
//   - Local requests fail closed and do not mint certificate IDs or expiry dates.
//   - All certify* methods emit certificationCompleted with the rejected result.

WorkflowCertificationService::WorkflowCertificationService(QObject *parent)
    : QObject(parent)
{
}

WfCertificationResult WorkflowCertificationService::rejectedResult(
    const QString &scope) const
{
    WfCertificationResult r;
    r.timestamp = QDateTime::currentDateTime();
    r.scope = scope;
    r.valid = false;
    r.conditions << QStringLiteral("Certification backend required before issuing or validating certificates.");
    return r;
}

WfCertificationResult WorkflowCertificationService::certifyProcess(
    const WfProcessConfig &config)
{
    WfCertificationResult r = rejectedResult(config.processName);
    emit certificationCompleted(r);
    return r;
}

WfCertificationResult WorkflowCertificationService::certifyQuality(
    const WfQualityConfig &config)
{
    WfCertificationResult r = rejectedResult(config.qualityStandard);
    emit certificationCompleted(r);
    return r;
}

WfCertificationResult WorkflowCertificationService::certifySafety(
    const WfSafetyConfig &config)
{
    WfCertificationResult r = rejectedResult(config.safetyLevel);
    emit certificationCompleted(r);
    return r;
}

WfCertificationResult WorkflowCertificationService::certifyCompliance(
    const WfComplianceConfig &config)
{
    WfCertificationResult r = rejectedResult(config.regulation);
    emit certificationCompleted(r);
    return r;
}
