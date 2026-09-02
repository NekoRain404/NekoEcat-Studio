// Populates and queries the SDO evidence / history QTableWidget.
#include "SdoEvidenceTableAdapter.h"

#include "utils/TableHelpers.h"
#include "utils/TextHelpers.h"

#include <QTableWidget>

namespace {

// Adds a non-empty candidate value with its source label to the evidence list.
void appendCandidate(SdoEvidenceCandidates* candidates, const QString& source, const QString& value) {
    if (!candidates) {
        return;
    }
    const QString trimmed = value.trimmed();
    if (!trimmed.isEmpty()) {
        candidates->append({source, trimmed});
    }
}

// Appends a non-empty evidence item, used for both local and write evidence assembly.
void appendEvidenceItem(QVector<SdoEvidenceItem>* items, const QString& source, const QString& value) {
    if (!items) {
        return;
    }
    const QString trimmed = value.trimmed();
    if (!trimmed.isEmpty()) {
        items->append({source, trimmed});
    }
}

} // namespace

// Locates the matching row in each evidence table for a given SDO target address.
SdoEvidenceTableRows sdoEvidenceTableRowsForTarget(const SdoEvidenceTables& tables, const SdoEvidenceTarget& target) {
    SdoEvidenceTableRows rows;
    if (target.position < 0 || target.index.trimmed().isEmpty() || target.subIndex.trimmed().isEmpty()) {
        return rows;
    }

    if (target.dictionaryLoadedForPosition) {
        rows.dictionaryRow = tableRowForObjectIndex(tables.dictionaryTable, target.index, target.subIndex, 1, 2);
    }
    rows.watchRow =
        tableRowForObjectAddress(tables.watchTable, target.position, target.index, target.subIndex, 1, 2, 3);
    rows.startupRow =
        tableRowForObjectAddress(tables.startupTable, target.position, target.index, target.subIndex, 0, 1, 2);
    rows.bookmarkRow =
        tableRowForObjectAddress(tables.bookmarkTable, target.position, target.index, target.subIndex, 0, 2, 3);
    rows.targetTrailRow =
        tableRowForObjectAddress(tables.targetTrailTable, target.position, target.index, target.subIndex, 1, 2, 3);
    return rows;
}

// Assembles all available value candidates (read, watch, dictionary, startup, bookmark, trail) for comparison.
SdoEvidenceCandidates sdoEvidenceCandidatesFromTables(const QString& readValue, const SdoEvidenceTables& tables,
                                                      const SdoEvidenceTableRows& rows,
                                                      const SdoEvidenceCandidateLabels& labels) {
    SdoEvidenceCandidates candidates;

    appendCandidate(&candidates, labels.readValue, readValue);

    if (rows.watchRow >= 0) {
        appendCandidate(&candidates, labels.watch, tableText(tables.watchTable, rows.watchRow, 4));
    }

    if (rows.dictionaryRow >= 0) {
        appendCandidate(&candidates, labels.dictionary, tableText(tables.dictionaryTable, rows.dictionaryRow, 7));
    }

    if (rows.startupRow >= 0) {
        appendCandidate(&candidates, labels.startup, tableText(tables.startupTable, rows.startupRow, 3));
    }

    if (rows.bookmarkRow >= 0) {
        appendCandidate(&candidates, labels.bookmark, tableText(tables.bookmarkTable, rows.bookmarkRow, 8));
    }

    if (rows.targetTrailRow >= 0) {
        const QString trailWrite = tableText(tables.targetTrailTable, rows.targetTrailRow, 7);
        appendCandidate(&candidates, labels.targetTrailWrite, trailWrite);
        if (trailWrite.trimmed().isEmpty()) {
            appendCandidate(&candidates, labels.targetTrail,
                            tableText(tables.targetTrailTable, rows.targetTrailRow, 6));
        }
    }

    return candidates;
}

// Builds the full list of local evidence items from all tables, for the evidence detail panel.
QVector<SdoEvidenceItem> sdoLocalEvidenceItemsFromTables(int position, const QString& index, const QString& subIndex,
                                                         const QString& readValue, const QString& cachedDictionaryValue,
                                                         bool includeReadValue, bool dictionaryTableLoaded,
                                                         const SdoEvidenceTables& tables,
                                                         const SdoLocalEvidenceLabels& labels) {
    QVector<SdoEvidenceItem> items;

    if (includeReadValue) {
        appendEvidenceItem(&items, labels.read, readValue);
    }

    // Iterate over collection
    for (int row = 0; tables.watchTable && row < tables.watchTable->rowCount(); ++row) {
        if (tableObjectAddressMatches(tables.watchTable, row, position, index, subIndex, 1, 2, 3)) {
            appendEvidenceItem(&items, QString("%1 #%2").arg(labels.watchPrefix).arg(row + 1),
                               tableText(tables.watchTable, row, 4));
        }
    }

    bool usedDictionaryTableEvidence = false;
    if (dictionaryTableLoaded) {
        // Iterate over collection
        for (int row = 0; tables.dictionaryTable && row < tables.dictionaryTable->rowCount(); ++row) {
            if (!tableObjectIndexMatches(tables.dictionaryTable, row, index, subIndex, 1, 2)) {
                continue;
            }
            const QString value = tableText(tables.dictionaryTable, row, 7);
            if (!value.trimmed().isEmpty()) {
                appendEvidenceItem(&items, labels.dictionary, value);
                usedDictionaryTableEvidence = true;
            }
            break;
        }
    }
    if (!usedDictionaryTableEvidence) {
        appendEvidenceItem(&items, labels.dictionary, cachedDictionaryValue);
    }

    // Iterate over collection
    for (int row = 0; tables.startupTable && row < tables.startupTable->rowCount(); ++row) {
        if (tableObjectAddressMatches(tables.startupTable, row, position, index, subIndex, 0, 1, 2)) {
            appendEvidenceItem(&items, QString("%1 #%2").arg(labels.startupPrefix).arg(row + 1),
                               tableText(tables.startupTable, row, 3));
        }
    }

    // Iterate over collection
    for (int row = 0; tables.bookmarkTable && row < tables.bookmarkTable->rowCount(); ++row) {
        if (tableObjectAddressMatches(tables.bookmarkTable, row, position, index, subIndex, 0, 2, 3)) {
            appendEvidenceItem(&items, QString("%1 #%2").arg(labels.bookmarkPrefix).arg(row + 1),
                               tableText(tables.bookmarkTable, row, 8));
        }
    }

    return items;
}

// Assembles write-side evidence items from pre-extracted value strings for delta review.
QVector<SdoEvidenceItem> sdoWriteEvidenceItemsFromValues(const QString& readValue, const QString& dictionaryValue,
                                                         const QString& watchValue, const QString& startupValue,
                                                         const QString& bookmarkValue, const QString& targetTrailValue,
                                                         const QString& targetTrailWriteValue,
                                                         const SdoWriteEvidenceLabels& labels) {
    QVector<SdoEvidenceItem> items;

    if (!readValue.trimmed().isEmpty()) {
        appendEvidenceItem(&items, labels.read, readValue);
    } else {
        appendEvidenceItem(&items, labels.dictionary, dictionaryValue);
    }
    appendEvidenceItem(&items, labels.watch, watchValue);
    appendEvidenceItem(&items, labels.startup, startupValue);
    appendEvidenceItem(&items, labels.bookmark, bookmarkValue);
    appendEvidenceItem(&items, labels.targetTrail,
                       targetTrailWriteValue.trimmed().isEmpty() ? targetTrailValue : targetTrailWriteValue);

    return items;
}

// Whether enough evidence is present across tables to render the write-delta review panel.
bool sdoWriteDeltaReviewEvidenceAvailable(const QString& readValue, const SdoEvidenceTables& tables,
                                          const SdoEvidenceTableRows& rows) {
    if (!readValue.trimmed().isEmpty()) {
        return true;
    }

    if (rows.dictionaryRow >= 0 && (!tableText(tables.dictionaryTable, rows.dictionaryRow, 7).isEmpty() ||
                                    !tableText(tables.dictionaryTable, rows.dictionaryRow, 8).isEmpty())) {
        return true;
    }

    if (rows.watchRow >= 0 && !tableText(tables.watchTable, rows.watchRow, 4).isEmpty()) {
        return true;
    }

    if (rows.startupRow >= 0 && !tableText(tables.startupTable, rows.startupRow, 3).isEmpty()) {
        return true;
    }

    if (rows.bookmarkRow >= 0 && !tableText(tables.bookmarkTable, rows.bookmarkRow, 8).isEmpty()) {
        return true;
    }

    return rows.targetTrailRow >= 0 && (!tableText(tables.targetTrailTable, rows.targetTrailRow, 7).isEmpty() ||
                                        !tableText(tables.targetTrailTable, rows.targetTrailRow, 6).isEmpty());
}

// Extracts a target trail row capturing the full SDO read/write history entry.
SdoTargetTrailRow sdoTargetTrailRowFromTable(QTableWidget* table, int row) {
    SdoTargetTrailRow result;
    result.row = row;
    if (!table || row < 0 || row >= table->rowCount()) {
        return result;
    }

    result.time = tableText(table, row, 0);
    result.positionText = tableText(table, row, 1);
    result.position = result.positionText.toInt(&result.positionValid);
    result.index = normalizeHexText(tableText(table, row, 2), 4);
    result.subIndex = normalizeHexText(tableText(table, row, 3), 2);
    result.type = tableText(table, row, 4);
    result.source = tableText(table, row, 5);
    result.value = tableText(table, row, 6);
    result.writeValue = tableText(table, row, 7);
    result.detail = tableText(table, row, 8);
    return result;
}

// Whether this trail row addresses a specific position:index:subIndex.
bool sdoTargetTrailRowHasTarget(const SdoTargetTrailRow& row) {
    return row.positionValid && row.position >= 0 && !row.index.isEmpty() && !row.subIndex.isEmpty();
}

// Prefers the write value if present, otherwise falls back to the read value.
QString sdoTargetTrailRowStartupValue(const SdoTargetTrailRow& row) {
    return row.writeValue.isEmpty() ? row.value : row.writeValue;
}

// Builds a deterministic key from address + metadata for deduplication and set membership.
QString sdoTargetTrailRowKey(int position, const QString& index, const QString& subIndex, const QString& type,
                             const QString& source, const QString& detail) {
    return QString("%1|%2|%3|%4|%5|%6")
        .arg(position)
        .arg(normalizeHexText(index, 4), normalizeHexText(subIndex, 2), type, source, detail.left(96).simplified());
}

// Convenience overload that delegates to the field-based key builder.
QString sdoTargetTrailRowKey(const SdoTargetTrailRow& row) {
    return sdoTargetTrailRowKey(row.position, row.index, row.subIndex, row.type, row.source, row.detail);
}

// Builds the trail key directly from table cells without constructing the full row model.
QString sdoTargetTrailRowKeyFromTable(QTableWidget* table, int row) {
    return sdoTargetTrailRowKey(sdoTargetTrailRowFromTable(table, row));
}

// Retrieves the startup-relevant value for a trail row by table index.
QString sdoTargetTrailStartupValueFromTable(QTableWidget* table, int row) {
    return sdoTargetTrailRowStartupValue(sdoTargetTrailRowFromTable(table, row));
}

// Collects all trail row keys into a set for quick membership checks during updates.
QSet<QString> sdoTargetTrailKeysFromTable(QTableWidget* table) {
    QSet<QString> keys;
    if (!table) {
        return keys;
    }

    // Iterate over collection
    for (int row = 0; row < table->rowCount(); ++row) {
        keys.insert(sdoTargetTrailRowKeyFromTable(table, row));
    }
    return keys;
}

// Extracts a bookmarked SDO object row for the bookmark detail panel.
SdoObjectBookmarkRow sdoObjectBookmarkRowFromTable(QTableWidget* table, int row) {
    SdoObjectBookmarkRow result;
    result.row = row;
    if (!table || row < 0 || row >= table->rowCount()) {
        return result;
    }

    result.positionText = tableText(table, row, 0);
    result.position = result.positionText.toInt(&result.positionValid);
    result.slaveName = tableText(table, row, 1);
    result.index = normalizeHexText(tableText(table, row, 2), 4);
    result.subIndex = normalizeHexText(tableText(table, row, 3), 2);
    result.access = tableText(table, row, 4);
    result.type = tableText(table, row, 5);
    result.bits = tableText(table, row, 6);
    result.name = tableText(table, row, 7);
    result.lastValue = tableText(table, row, 8);
    result.source = tableText(table, row, 9);
    return result;
}

// Whether this bookmark row addresses a valid position:index:subIndex.
bool sdoObjectBookmarkRowHasTarget(const SdoObjectBookmarkRow& row) {
    return row.positionValid && row.position >= 0 && !row.index.isEmpty() && !row.subIndex.isEmpty();
}

// Checks if an access-rights string indicates read-only, supporting localized labels.
bool sdoObjectAccessIsReadOnly(const QString& access, const QString& readOnlyText) {
    return access.toLower().contains(QStringLiteral("ro")) ||
           (!readOnlyText.isEmpty() && access.contains(readOnlyText));
}

// Extracts an SDO history row capturing past read/write operations with timestamps.
SdoHistoryRow sdoHistoryRowFromTable(QTableWidget* table, int row) {
    SdoHistoryRow result;
    result.row = row;
    if (!table || row < 0 || row >= table->rowCount()) {
        return result;
    }

    result.time = tableText(table, row, 0);
    result.action = tableText(table, row, 1);
    result.positionText = tableText(table, row, 2);
    result.position = result.positionText.toInt(&result.positionValid);
    result.index = normalizeHexText(tableText(table, row, 3), 4);
    result.subIndex = normalizeHexText(tableText(table, row, 4), 2);
    result.type = tableText(table, row, 5);
    result.value = tableText(table, row, 6);
    result.status = tableText(table, row, 7);
    result.detail = tableText(table, row, 8);
    return result;
}

// Whether this history row addresses a valid position:index:subIndex.
bool sdoHistoryRowHasTarget(const SdoHistoryRow& row) {
    return row.positionValid && row.position >= 0 && !row.index.isEmpty() && !row.subIndex.isEmpty();
}
