#pragma once

// EtherCATValidationService — validation of EtherCAT configurations, network
// topology, timing constraints, and safety requirements.
//
// Provides on-demand validation methods that produce structured results.
// Emits validationCompleted() signal when each validation finishes.
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
    EtherCATValidationResult createPassingResult(const QString &type);
};
