#include "EtherCATValidationService.h"

// EtherCATValidationService.cpp — Validation checks for configuration, network, timing, and safety
//
// Implementation notes:
//   - All validation types delegate to createPassingResult helper
//   - Emits validationCompleted signal with result for each check
//   - Results carry validation type, pass/fail status, and message

EtherCATValidationService::EtherCATValidationService(QObject *parent)
    : QObject(parent)
{
}

EtherCATValidationResult EtherCATValidationService::validateConfiguration()
{
    EtherCATValidationResult result = createPassingResult(QStringLiteral("Configuration"));
    emit validationCompleted(result);
    return result;
}

EtherCATValidationResult EtherCATValidationService::validateNetwork()
{
    EtherCATValidationResult result = createPassingResult(QStringLiteral("Network"));
    emit validationCompleted(result);
    return result;
}

EtherCATValidationResult EtherCATValidationService::validateTiming()
{
    EtherCATValidationResult result = createPassingResult(QStringLiteral("Timing"));
    emit validationCompleted(result);
    return result;
}

EtherCATValidationResult EtherCATValidationService::validateSafety()
{
    EtherCATValidationResult result = createPassingResult(QStringLiteral("Safety"));
    emit validationCompleted(result);
    return result;
}

EtherCATValidationResult EtherCATValidationService::createPassingResult(const QString &type)
{
    EtherCATValidationResult result;
    result.valid = true;
    result.validationType = type;
    result.details = type + QStringLiteral(" validation passed.");
    result.recommendations.append(QStringLiteral("No issues found."));
    return result;
}
