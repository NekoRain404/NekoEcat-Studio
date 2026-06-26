#include "EtherCATValidationService.h"

// EtherCATValidationService.cpp — Validation request facade
//
// Implementation notes:
//   - All validation types delegate to createRejectedResult helper
//   - Results carry validation type and rejection details
//   - No completion signal is emitted without real validation evidence

EtherCATValidationService::EtherCATValidationService(QObject *parent)
    : QObject(parent)
{
}

EtherCATValidationResult EtherCATValidationService::validateConfiguration()
{
    return createRejectedResult(QStringLiteral("Configuration"));
}

EtherCATValidationResult EtherCATValidationService::validateNetwork()
{
    return createRejectedResult(QStringLiteral("Network"));
}

EtherCATValidationResult EtherCATValidationService::validateTiming()
{
    return createRejectedResult(QStringLiteral("Timing"));
}

EtherCATValidationResult EtherCATValidationService::validateSafety()
{
    return createRejectedResult(QStringLiteral("Safety"));
}

EtherCATValidationResult EtherCATValidationService::createRejectedResult(const QString &type)
{
    EtherCATValidationResult result;
    result.valid = false;
    result.validationType = type;
    result.details = type + QStringLiteral(" validation requires a real validation backend.");

    ValidationError error;
    error.code = QStringLiteral("VALIDATION_BACKEND_REQUIRED");
    error.message = result.details;
    error.location = type;
    error.severity = 3;
    result.errors.append(error);

    result.recommendations.append(
        QStringLiteral("Run validation against a live evidence-producing backend before claiming validity."));
    return result;
}
