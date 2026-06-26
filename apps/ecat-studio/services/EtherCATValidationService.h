#pragma once

// EtherCATValidationService — validation request facade for EtherCAT
// configurations, network topology, timing constraints, and safety
// requirements.
//
// Provides rejected validation results until a real evidence-producing
// validation backend is available. It must not synthesize passing checks.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QVector>
#include <QString>

struct ValidationError {
    QString code;
    QString message;
    QString location;
    int severity = 0;
};

struct ValidationWarning {
    QString code;
    QString message;
    QString location;
};

struct EtherCATValidationResult {
    bool valid = false;
    QVector<ValidationError> errors;
    QVector<ValidationWarning> warnings;
    QString details;
    QVector<QString> recommendations;
    QString validationType;
};

class EtherCATValidationService : public QObject {
    Q_OBJECT
public:
    explicit EtherCATValidationService(QObject *parent = nullptr);

    EtherCATValidationResult validateConfiguration();
    EtherCATValidationResult validateNetwork();
    EtherCATValidationResult validateTiming();
    EtherCATValidationResult validateSafety();

signals:
    void validationCompleted(const EtherCATValidationResult &result);

private:
    EtherCATValidationResult createRejectedResult(const QString &type);
};
