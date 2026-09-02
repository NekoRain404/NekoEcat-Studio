#include "EtherCATCertificationService.h"

// EtherCATCertificationService.cpp — EtherCAT certification request facade
//
// Implementation notes:
//   - Pre-defines conformance, performance, safety, and interoperability requirements
//   - Tracks certification status per requirement with mandatory/optional flags
//   - Rejects certification checks until a real evidence-producing backend exists

EtherCATCertificationService::EtherCATCertificationService(QObject* parent) : QObject(parent) {
    CertificationRequirement r1;
    r1.requirementId = QStringLiteral("CONF-001");
    r1.category = QStringLiteral("Conformance");
    r1.description = QStringLiteral("Device must pass EtherCAT conformance test.");
    r1.mandatory = true;
    requirements_.append(r1);

    CertificationRequirement r2;
    r2.requirementId = QStringLiteral("PERF-001");
    r2.category = QStringLiteral("Performance");
    r2.description = QStringLiteral("Jitter must remain below 1us.");
    r2.mandatory = true;
    requirements_.append(r2);

    CertificationRequirement r3;
    r3.requirementId = QStringLiteral("SAFETY-001");
    r3.category = QStringLiteral("Safety");
    r3.description = QStringLiteral("Safety-over-EtherCAT must be supported.");
    r3.mandatory = true;
    requirements_.append(r3);

    CertificationRequirement r4;
    r4.requirementId = QStringLiteral("INTEROP-001");
    r4.category = QStringLiteral("Interoperability");
    r4.description = QStringLiteral("Device must interoperate with reference master.");
    r4.mandatory = false;
    requirements_.append(r4);
}

void EtherCATCertificationService::addRequirement(const CertificationRequirement& req) {
    requirements_.append(req);
    emit requirementAdded();
}

bool EtherCATCertificationService::removeRequirement(const QString& reqId) {
    for (int i = 0; i < requirements_.size(); ++i) {
        if (requirements_[i].requirementId == reqId) {
            requirements_.removeAt(i);
            emit requirementRemoved();
            return true;
        }
    }
    return false;
}

QVector<CertificationRequirement> EtherCATCertificationService::requirements() const {
    return requirements_;
}

CertificationReport EtherCATCertificationService::runCertification() {
    CertificationReport report;
    for (const auto& req : requirements_) {
        CertificationTestResult result;
        result.requirementId = req.requirementId;
        result.status = CertificationTestStatus::NotTested;
        result.notes = QStringLiteral("Certification requirement '%1' requires a real certification backend.")
                           .arg(req.requirementId);
        report.results.append(result);
    }
    report.totalRequirements = report.results.size();
    report.passedCount = 0;
    report.failedCount = 0;
    report.notTestedCount = report.results.size();
    report.overallPass = false;
    return report;
}

CertificationTestResult EtherCATCertificationService::testRequirement(const QString& reqId) {
    for (const auto& req : requirements_) {
        if (req.requirementId == reqId) {
            CertificationTestResult result;
            result.requirementId = reqId;
            result.status = CertificationTestStatus::NotTested;
            result.notes = QStringLiteral("Certification requirement '%1' requires a real certification backend.")
                               .arg(req.requirementId);
            return result;
        }
    }
    CertificationTestResult result;
    result.requirementId = reqId;
    result.status = CertificationTestStatus::NotTested;
    result.notes = QStringLiteral("Requirement not found.");
    return result;
}

CertificationResult EtherCATCertificationService::certifyDevice(int position) {
    Q_UNUSED(position);
    return createRejectedCert(QStringLiteral("Device"));
}

CertificationResult EtherCATCertificationService::certifyNetwork() {
    return createRejectedCert(QStringLiteral("Network"));
}

CertificationResult EtherCATCertificationService::certifySystem() {
    return createRejectedCert(QStringLiteral("System"));
}

CertificationResult EtherCATCertificationService::certifyOperator(const QString& operatorName) {
    Q_UNUSED(operatorName);
    return createRejectedCert(QStringLiteral("Operator"));
}

CertificationResult EtherCATCertificationService::createRejectedCert(const QString& scope) {
    CertificationResult result;
    result.timestamp = QDateTime::currentDateTime();
    result.valid = false;
    result.scope = scope;
    result.conditions.append(QStringLiteral("Certification requires a real evidence-producing backend."));
    return result;
}
