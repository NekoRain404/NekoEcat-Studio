// I/O variable model: bulk naming, row filtering, and PLC handoff export.
#include "IoVariableModel.h"
#include "utils/TextHelpers.h"

#include "ProcessDataRowModel.h"

// ─────────────────────────────────────────────────────────────────────────────
// Bulk naming
// ─────────────────────────────────────────────────────────────────────────────

namespace {

// Deduplicates tags case-insensitively before appending to avoid redundant labels.
void appendUniqueTag(QStringList* tags, QString tag) {
    if (!tags) {
        return;
    }
    tag = tag.trimmed();
    if (tag.isEmpty()) {
        return;
    }
    for (const QString& existing : *tags) {
        if (existing.trimmed().compare(tag, Qt::CaseInsensitive) == 0) {
            return;
        }
    }
    tags->append(tag);
}

} // namespace

// Counts how many rows already have user-assigned aliases.
int countIoVariableBulkNamingExistingAliases(const QVector<IoVariableTableRow>& rows) {
    int count = 0;
    for (const IoVariableTableRow& row : rows) {
        if (!row.alias.trimmed().isEmpty()) {
            ++count;
        }
    }
    return count;
}

// Generates alias and tag metadata for selected I/O rows, resolving naming collisions.
IoVariableBulkNamingResult buildIoVariableBulkNamingPlan(const QVector<IoVariableTableRow>& allRows,
                                                         const QVector<int>& targetRows,
                                                         const QHash<QString, QStringList>& metadataByKey,
                                                         const IoVariableBulkNamingOptions& options) {
    IoVariableBulkNamingResult result;

    QHash<int, IoVariableTableRow> rowsByNumber;
    rowsByNumber.reserve(allRows.size());
    for (const IoVariableTableRow& row : allRows) {
        rowsByNumber.insert(row.row, row);
    }

    QSet<int> scopedRows;
    for (const int row : targetRows) {
        scopedRows.insert(row);
    }

    QSet<QString> usedAliases;
    for (const IoVariableTableRow& row : allRows) {
        if (scopedRows.contains(row.row) && !options.keepExistingAliases) {
            continue;
        }
        const QString alias = row.alias.trimmed();
        if (!alias.isEmpty()) {
            usedAliases.insert(alias.toLower());
        }
    }

    for (const int rowNumber : targetRows) {
        if (!rowsByNumber.contains(rowNumber)) {
            ++result.skippedInvalidRows;
            continue;
        }

        const IoVariableTableRow row = rowsByNumber.value(rowNumber);
        const QString key = ioVariableTableRowKey(row);
        if (key.isEmpty()) {
            ++result.skippedInvalidRows;
            continue;
        }

        QStringList metadata = metadataByKey.value(key);
        while (metadata.size() < 3) {
            metadata.append(QString());
        }

        if (options.keepExistingAliases && !metadata.value(0).trimmed().isEmpty()) {
            ++result.skippedExistingAliases;
        } else {
            QString alias = suggestedIoVariableAlias(row, options.prefix, options.includeAddress);
            const QString baseAlias = alias;
            int duplicate = 2;
            while (usedAliases.contains(alias.toLower())) {
                alias = QString("%1_%2").arg(baseAlias).arg(duplicate++);
            }
            usedAliases.insert(alias.toLower());
            metadata[0] = alias;
        }

        QStringList tags = metadata.value(1).split(',', Qt::SkipEmptyParts);
        for (const QString& tag : options.requestedTags) {
            appendUniqueTag(&tags, tag);
        }
        if (options.addDirectionTags) {
            appendUniqueTag(&tags, ioVariableHandoffPlcDirection(row).toLower());
            appendUniqueTag(&tags, ioVariableHandoffPlcType(row).toLower());
        }
        metadata[1] = tags.join(", ");

        result.metadataUpdates.insert(key, metadata);
        ++result.updated;
    }

    return result;
}

// ─────────────────────────────────────────────────────────────────────────────
// Row filtering
// ─────────────────────────────────────────────────────────────────────────────

// Computes all predicate flags for a single row in one pass to avoid repeated lookups.
IoVariableFilterRowState ioVariableFilterRowState(const IoVariableTableRow& row, int selectedPosition,
                                                  const QString& readyText) {
    IoVariableFilterRowState state;
    state.selected = row.positionValid && row.position == selectedPosition;
    state.process = ioVariableTableRowHasProcessSource(row);
    state.pdo = ioVariableTableRowHasPdoSource(row);
    state.watchEvidence = ioVariableTableRowHasWatchEvidence(row);
    state.startupDiff = ioVariableTableRowHasStartupDiff(row);
    state.missingValue = ioVariableTableRowHasMissingValue(row);
    state.changed = ioVariableTableRowHasChangedValue(row);
    state.plcIssue = ioVariableTableRowHasPlcIssue(row, readyText);
    state.rx = ioVariableTableRowIsRx(row);
    state.tx = ioVariableTableRowIsTx(row);
    state.cia402 = ioVariableTableRowIsCia402(row);
    return state;
}

// Tests whether a row's state matches the named scope predicate.
bool ioVariableFilterScopeMatches(const IoVariableFilterRowState& state, const QString& scope) {
    if (scope == kIoVariableScopeAll)
        return true;
    if (scope == kIoVariableScopeSelected)
        return state.selected;
    if (scope == kIoVariableScopeProcess)
        return state.process;
    if (scope == kIoVariableScopePdo)
        return state.pdo;
    if (scope == kIoVariableScopeWatch)
        return state.watchEvidence;
    if (scope == kIoVariableScopeStartupDiff)
        return state.startupDiff;
    if (scope == kIoVariableScopeMissingValue)
        return state.missingValue;
    if (scope == kIoVariableScopeRx)
        return state.rx;
    if (scope == kIoVariableScopeTx)
        return state.tx;
    if (scope == kIoVariableScopeCia402)
        return state.cia402;
    if (scope == kIoVariableScopeChanged)
        return state.changed;
    if (scope == kIoVariableScopePlcIssues)
        return state.plcIssue;
    return true;
}

// Case-insensitive substring match across all visible cell text.
bool ioVariableFilterTextMatches(const QStringList& cells, const QString& needle) {
    if (needle.isEmpty()) {
        return true;
    }
    const QString lowerNeedle = needle.toLower();
    for (const QString& cell : cells) {
        if (cell.toLower().contains(lowerNeedle)) {
            return true;
        }
    }
    return false;
}

// Evaluates a single row against scope + text filter, returning visibility and state.
IoVariableFilterDecision evaluateIoVariableFilterRow(const IoVariableTableRow& row, const QStringList& cells,
                                                     const QString& scope, const QString& needle, int selectedPosition,
                                                     const QString& readyText) {
    IoVariableFilterDecision decision;
    decision.state = ioVariableFilterRowState(row, selectedPosition, readyText);
    decision.visible =
        ioVariableFilterScopeMatches(decision.state, scope) && ioVariableFilterTextMatches(cells, needle);
    return decision;
}

// Accumulates per-scope counters for the filter summary bar.
void accumulateIoVariableFilterStats(IoVariableFilterStats* stats, const IoVariableFilterDecision& decision) {
    if (!stats)
        return;
    ++stats->total;
    if (decision.visible)
        ++stats->visible;
    if (decision.state.process)
        ++stats->processRows;
    if (decision.state.watchEvidence)
        ++stats->watchRows;
    if (decision.state.startupDiff)
        ++stats->startupDiffs;
    if (decision.state.missingValue)
        ++stats->missingValues;
    if (decision.state.changed)
        ++stats->changedRows;
    if (decision.state.plcIssue)
        ++stats->plcIssues;
}

// Formats the filter summary text with visible/total counts and scope-specific stats.
QString ioVariableFilterSummaryText(const IoVariableFilterStats& stats, const QString& scopeLabel,
                                    const QString& summaryPattern) {
    return QString(summaryPattern)
        .arg(stats.visible)
        .arg(stats.total)
        .arg(scopeLabel)
        .arg(stats.processRows)
        .arg(stats.watchRows)
        .arg(stats.startupDiffs)
        .arg(stats.missingValues)
        .arg(stats.changedRows)
        .arg(stats.plcIssues);
}

// ─────────────────────────────────────────────────────────────────────────────
// PLC handoff
// ─────────────────────────────────────────────────────────────────────────────

// Derives a short PLC-friendly alias from the row's metadata.
QString suggestedIoVariableAlias(const IoVariableTableRow& row, const QString& prefix, bool includeAddress) {
    QString seed = row.alias;
    if (seed.isEmpty()) {
        seed = row.symbol;
    }
    if (seed.isEmpty()) {
        seed = row.meaning;
    }
    if (seed.isEmpty()) {
        seed = QString("Obj_%1_%2").arg(row.index, row.subIndex);
    }

    QString fallbackIndex = row.index;
    QString fallbackSubIndex = row.subIndex;
    fallbackIndex.remove("0x", Qt::CaseInsensitive);
    fallbackSubIndex.remove("0x", Qt::CaseInsensitive);
    const QString fallback = QString("S%1_%2_%3")
                                 .arg(row.positionValid ? QString::number(row.position) : QString())
                                 .arg(fallbackIndex, fallbackSubIndex);
    QString alias = plcIdentifier(seed, fallback);
    if (includeAddress) {
        const QString address = plcIdentifier(fallback, fallback);
        if (!alias.contains(address, Qt::CaseInsensitive)) {
            alias = QString("%1_%2").arg(address, alias);
        }
    }

    const QString normalizedPrefix = prefix.trimmed().isEmpty() ? QString() : plcIdentifier(prefix, QString());
    if (!normalizedPrefix.isEmpty() && !alias.startsWith(normalizedPrefix + "_", Qt::CaseInsensitive)) {
        alias = QString("%1_%2").arg(normalizedPrefix, alias);
    }
    return alias;
}

// Resolves the best alias/fallback/symbol triple for PLC export.
IoVariableHandoffName ioVariableHandoffName(const IoVariableTableRow& row) {
    IoVariableHandoffName result;
    result.alias = row.alias;
    result.fallbackAlias = suggestedIoVariableAlias(row, QString(), true);
    result.symbol = plcIdentifier(result.alias, result.fallbackAlias);
    return result;
}

// Detects all handoff issues for a row: missing alias, auto-name, no tags, duplicate symbol.
QVector<IoVariableHandoffIssue> ioVariableHandoffIssues(const IoVariableTableRow& row,
                                                        const QSet<QString>* duplicateSymbols) {
    const IoVariableHandoffName name = ioVariableHandoffName(row);
    QVector<IoVariableHandoffIssue> issues;
    if (name.alias.trimmed().isEmpty()) {
        issues.append(IoVariableHandoffIssue::MissingAlias);
    } else if (name.alias.compare(name.fallbackAlias, Qt::CaseInsensitive) == 0 ||
               name.alias.startsWith("S" + QString::number(row.position) + "_", Qt::CaseInsensitive)) {
        issues.append(IoVariableHandoffIssue::AutoName);
    }
    if (row.tags.trimmed().isEmpty()) {
        issues.append(IoVariableHandoffIssue::NoTags);
    }
    if (duplicateSymbols && duplicateSymbols->contains(name.symbol.toLower())) {
        issues.append(IoVariableHandoffIssue::DuplicateSymbol);
    }
    return issues;
}

// Scans all rows to find symbols that appear more than once.
QSet<QString> duplicateIoVariableHandoffSymbols(const QVector<IoVariableTableRow>& rows) {
    QHash<QString, int> counts;
    QSet<QString> duplicates;
    for (const IoVariableTableRow& row : rows) {
        const QString symbol = ioVariableHandoffName(row).symbol.toLower();
        const int count = counts.value(symbol, 0) + 1;
        counts.insert(symbol, count);
        if (count > 1) {
            duplicates.insert(symbol);
        }
    }
    return duplicates;
}

// Converts issue enum to human-readable keys for tooltip display.
QStringList ioVariableHandoffIssueKeys(const QVector<IoVariableHandoffIssue>& issues) {
    QStringList keys;
    for (const auto& issue : issues) {
        switch (issue) {
            case IoVariableHandoffIssue::MissingAlias:
                keys << "missingAlias";
                break;
            case IoVariableHandoffIssue::AutoName:
                keys << "autoName";
                break;
            case IoVariableHandoffIssue::NoTags:
                keys << "noTags";
                break;
            case IoVariableHandoffIssue::DuplicateSymbol:
                keys << "duplicateSymbol";
                break;
        }
    }
    return keys;
}

// Checks whether a specific handoff issue is present.
bool ioVariableHandoffHasIssue(const QVector<IoVariableHandoffIssue>& issues, IoVariableHandoffIssue issue) {
    return issues.contains(issue);
}

// Maps row direction metadata to PLC I/O classification (Output/Input/Parameter).
QString ioVariableHandoffPlcDirection(const IoVariableTableRow& row) {
    if (ioVariableTableRowIsRx(row))
        return QStringLiteral("Output");
    if (ioVariableTableRowIsTx(row))
        return QStringLiteral("Input");
    return QStringLiteral("Parameter");
}

// Delegates to IEC type resolution for PLC variable declaration.
QString ioVariableHandoffPlcType(const IoVariableTableRow& row) {
    return ioVariableTableRowIecType(row);
}

// Builds a structured IEC comment with position, OD address, and quality info.
QString ioVariableHandoffComment(const IoVariableTableRow& row, const QStringList& qualityLabels) {
    QString comment =
        QString("#%1 %2:%3")
            .arg(row.positionValid ? QString::number(row.position) : QStringLiteral("?"), row.index, row.subIndex);
    QStringList facts;
    const QString direction = ioVariableHandoffPlcDirection(row);
    if (!direction.isEmpty())
        facts << direction;
    if (!row.pdo.isEmpty())
        facts << row.pdo;
    if (!row.meaning.isEmpty())
        facts << row.meaning;
    if (!qualityLabels.isEmpty()) {
        facts << QString("Quality: %1").arg(qualityLabels.join(" | "));
    }
    if (!facts.isEmpty())
        comment += " | " + facts.join(" | ");
    comment.replace("(*", "(");
    comment.replace("*)", ")");
    return comment;
}

// Appends numeric suffixes to ensure each exported PLC symbol is unique.
QString ioVariableHandoffUniqueSymbol(const IoVariableTableRow& row, QSet<QString>* usedSymbols) {
    QString symbol = ioVariableHandoffName(row).symbol;
    if (!usedSymbols)
        return symbol;
    const QString baseSymbol = symbol;
    int duplicate = 2;
    while (usedSymbols->contains(symbol.toLower())) {
        symbol = QString("%1_%2").arg(baseSymbol).arg(duplicate++);
    }
    usedSymbols->insert(symbol.toLower());
    return symbol;
}

// Renders one IEC 61131-3 VAR_GLOBAL declaration line with inline comment.
QString ioVariableHandoffDeclarationLine(const IoVariableTableRow& row, QSet<QString>* usedSymbols,
                                         const QStringList& qualityLabels) {
    const QString symbol = ioVariableHandoffUniqueSymbol(row, usedSymbols);
    const QString comment = ioVariableHandoffComment(row, qualityLabels);
    return QString("    %1 : %2; (* %3 *)").arg(symbol, ioVariableHandoffPlcType(row), comment);
}

// Renders the complete IEC variable declaration block for PLC export.
QString ioVariableHandoffDeclarationBlock(const QVector<IoVariableTableRow>& rows,
                                          const QVector<QStringList>& qualityLabelsByRow) {
    QSet<QString> usedSymbols;
    QStringList lines;
    lines << QStringLiteral("VAR_GLOBAL");
    for (int i = 0; i < rows.size(); ++i) {
        lines << ioVariableHandoffDeclarationLine(rows.at(i), &usedSymbols, qualityLabelsByRow.value(i));
    }
    lines << QStringLiteral("END_VAR");
    return lines.join('\n');
}

// Column headers for the CSV export format.
QStringList ioVariableHandoffCsvHeaders() {
    return {"Symbol",  "Direction", "Type",    "Slave",   "Index",   "SubIndex",    "Bits",      "Pdo",
            "Source",  "Raw",       "Decoded", "Meaning", "Watch",   "Startup",     "Map",       "Changed",
            "Quality", "Alias",     "Tags",    "Note",    "Address", "DefaultName", "ExportedAt"};
}

// Builds a flat CSV row from an I/O variable table row for export.
IoVariableHandoffCsvRow ioVariableHandoffCsvRow(const IoVariableTableRow& row, QSet<QString>* usedSymbols,
                                                const QString& exportedAt) {
    const IoVariableHandoffName name = ioVariableHandoffName(row);
    const QString symbol = ioVariableHandoffUniqueSymbol(row, usedSymbols);
    const QString address =
        QString("#%1 %2:%3")
            .arg(row.positionValid ? QString::number(row.position) : QStringLiteral("?"), row.index, row.subIndex);
    const QString defaultName = row.symbol.isEmpty() ? name.fallbackAlias : row.symbol;
    return {{symbol,
             ioVariableHandoffPlcDirection(row),
             ioVariableHandoffPlcType(row),
             row.positionValid ? QString::number(row.position) : QString(),
             row.index,
             row.subIndex,
             row.bits,
             row.pdo,
             row.source,
             row.raw,
             row.decoded,
             row.meaning,
             row.watch,
             row.startup,
             row.map,
             row.changed,
             row.plcQuality,
             name.alias,
             row.tags,
             row.note,
             address,
             defaultName,
             exportedAt}};
}
