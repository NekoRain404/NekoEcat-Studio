#include "PdoMappingValidator.h"
#include "PdoMappingCanvas.h"

#include <QSet>

static constexpr int kMaxBitsPerSm = 1500 * 8;

PdoValidationReport PdoMappingValidator::validate(const QVector<SyncManagerBlock>& sms) {
    PdoValidationReport report;
    report.maxBitsPerSm = kMaxBitsPerSm;

    for (const auto& sm : sms) {
        if (!sm.enabled)
            continue;
        for (const auto& entry : sm.entries) {
            if (!entry.enabled)
                continue;
            if (entry.direction == PdoEntryDirection::Input) {
                report.totalInputBits += entry.bitSize;
            } else {
                report.totalOutputBits += entry.bitSize;
            }
        }
    }

    auto sizeResult = validateSmSize(sms);
    auto dupResult = validateDuplicates(sms);
    auto addrResult = validateAddresses(sms);
    auto reqResult = validateRequired(sms);

    auto merge = [&](const PdoValidationReport& r) {
        for (const auto& e : r.errors) {
            report.errors.append(e);
        }
    };
    merge(sizeResult);
    merge(dupResult);
    merge(addrResult);
    merge(reqResult);

    report.valid = report.errors.isEmpty();
    return report;
}

PdoValidationReport PdoMappingValidator::validateSmSize(const QVector<SyncManagerBlock>& sms) {
    PdoValidationReport report;
    report.maxBitsPerSm = kMaxBitsPerSm;

    for (int i = 0; i < sms.size(); ++i) {
        const auto& sm = sms[i];
        if (!sm.enabled)
            continue;
        int totalBits = 0;
        for (const auto& entry : sm.entries) {
            if (entry.enabled)
                totalBits += entry.bitSize;
        }
        if (totalBits > kMaxBitsPerSm) {
            PdoValidationError err;
            err.type = PdoValidationError::Type::SizeExceeded;
            err.smIndex = sm.index;
            err.message = QString("SM%1 total size %2 bits exceeds maximum %3 bits")
                              .arg(sm.index)
                              .arg(totalBits)
                              .arg(kMaxBitsPerSm);
            report.errors.append(err);
            report.valid = false;
        }
    }
    return report;
}

PdoValidationReport PdoMappingValidator::validateDuplicates(const QVector<SyncManagerBlock>& sms) {
    PdoValidationReport report;
    QSet<QString> seen;

    for (int i = 0; i < sms.size(); ++i) {
        for (int j = 0; j < sms[i].entries.size(); ++j) {
            const auto& entry = sms[i].entries[j];
            QString key = entry.index.toUpper() + "." + entry.subIndex.toUpper();
            if (seen.contains(key)) {
                PdoValidationError err;
                err.type = PdoValidationError::Type::DuplicateEntry;
                err.smIndex = sms[i].index;
                err.entryIndex = j;
                err.message = QString("Duplicate PDO entry %1.%2 (%3)").arg(entry.index, entry.subIndex, entry.name);
                report.errors.append(err);
                report.valid = false;
            } else {
                seen.insert(key);
            }
        }
    }
    return report;
}

PdoValidationReport PdoMappingValidator::validateAddresses(const QVector<SyncManagerBlock>& sms) {
    PdoValidationReport report;
    QVector<QPair<int, int>> inputRanges;
    QVector<QPair<int, int>> outputRanges;

    for (int i = 0; i < sms.size(); ++i) {
        int offset = 0;
        for (int j = 0; j < sms[i].entries.size(); ++j) {
            const auto& entry = sms[i].entries[j];
            if (!entry.enabled)
                continue;
            int start = offset;
            int end = offset + entry.bitSize;
            if (entry.direction == PdoEntryDirection::Input) {
                for (const auto& range : inputRanges) {
                    if (start < range.second && end > range.first) {
                        PdoValidationError err;
                        err.type = PdoValidationError::Type::OverlappingAddress;
                        err.smIndex = sms[i].index;
                        err.entryIndex = j;
                        err.message =
                            QString("Input PDO %1.%2 overlaps with another entry").arg(entry.index, entry.subIndex);
                        report.errors.append(err);
                        report.valid = false;
                    }
                }
                inputRanges.append({start, end});
            } else {
                for (const auto& range : outputRanges) {
                    if (start < range.second && end > range.first) {
                        PdoValidationError err;
                        err.type = PdoValidationError::Type::OverlappingAddress;
                        err.smIndex = sms[i].index;
                        err.entryIndex = j;
                        err.message =
                            QString("Output PDO %1.%2 overlaps with another entry").arg(entry.index, entry.subIndex);
                        report.errors.append(err);
                        report.valid = false;
                    }
                }
                outputRanges.append({start, end});
            }
            offset = end;
        }
    }
    return report;
}

PdoValidationReport PdoMappingValidator::validateRequired(const QVector<SyncManagerBlock>& sms) {
    PdoValidationReport report;

    bool hasInput = false;
    bool hasOutput = false;
    for (const auto& sm : sms) {
        if (!sm.enabled)
            continue;
        for (const auto& entry : sm.entries) {
            if (!entry.enabled)
                continue;
            if (entry.direction == PdoEntryDirection::Input)
                hasInput = true;
            if (entry.direction == PdoEntryDirection::Output)
                hasOutput = true;
        }
    }

    if (!hasInput) {
        PdoValidationError err;
        err.type = PdoValidationError::Type::MissingRequired;
        err.message = "No input (RxPDO) entries configured";
        report.errors.append(err);
        report.valid = false;
    }
    if (!hasOutput) {
        PdoValidationError err;
        err.type = PdoValidationError::Type::MissingRequired;
        err.message = "No output (TxPDO) entries configured";
        report.errors.append(err);
        report.valid = false;
    }
    return report;
}

QString PdoMappingValidator::errorString(const PdoValidationError& error) {
    return error.message;
}
