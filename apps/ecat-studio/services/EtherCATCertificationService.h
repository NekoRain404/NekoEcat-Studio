#pragma once

// EtherCATCertificationService — certification requirement management and
// automated certification testing for EtherCAT devices and networks.
//
// Provides requirement CRUD, individual requirement testing, and full
// certification reports. Emits signals when requirements change or
// certification completes.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QVector>
#include <QString>
#include <QDateTime>

enum class CertificationTestStatus {
    Pass,
    Fail,
    NotTested
};

struct CertificationRequirement {
    QString requirementId;
    QString category;
    QString description;
    bool mandatory = true;
};

struct CertificationTestResult {
    QString requirementId;
    CertificationTestStatus status = CertificationTestStatus::NotTested;
    QString evidence;
    QString notes;
};

struct CertificationReport {
    QVector<CertificationTestResult> results;
    int totalRequirements = 0;
    int passedCount = 0;
    int failedCount = 0;
    int notTestedCount = 0;
    bool overallPass = false;
    QString certificationLevel;
};

struct CertificationResult {
    QString certificateId;
    QDateTime timestamp;
    bool valid = false;
    QDateTime expiry;
    QString scope;
    QVector<QString> conditions;
};

class EtherCATCertificationService : public QObject {
    Q_OBJECT
public:
    explicit EtherCATCertificationService(QObject *parent = nullptr);

    void addRequirement(const CertificationRequirement &req);
    bool removeRequirement(const QString &reqId);
    QVector<CertificationRequirement> requirements() const;
    CertificationReport runCertification();
    CertificationTestResult testRequirement(const QString &reqId);

    CertificationResult certifyDevice(int position);
    CertificationResult certifyNetwork();
    CertificationResult certifySystem();
    CertificationResult certifyOperator(const QString &operatorName);

signals:
    void requirementAdded();
    void requirementRemoved();
    void certificationCompleted(const CertificationReport &report);
    void deviceCertified(const CertificationResult &result);

private:
    QVector<CertificationRequirement> requirements_;
    CertificationResult createPassingCert(const QString &scope);
};
