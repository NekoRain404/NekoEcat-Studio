#pragma once

// Populates and queries the SDO evidence / history QTableWidget.


#include "models/SdoEvidenceModel.h"

#include <QSet>
#include <QString>

class QTableWidget;

// Cached row indices across evidence tables for a single SDO target.
struct SdoEvidenceTableRows {
    int dictionaryRow = -1;
    int watchRow = -1;
    int startupRow = -1;
    int bookmarkRow = -1;
    int targetTrailRow = -1;
};

// Pointers to all QTableWidgets that participate in evidence gathering.
struct SdoEvidenceTables {
    QTableWidget* dictionaryTable = nullptr;
    QTableWidget* watchTable = nullptr;
    QTableWidget* startupTable = nullptr;
    QTableWidget* bookmarkTable = nullptr;
    QTableWidget* targetTrailTable = nullptr;
};

// Identifies the SDO address and whether the dictionary table is loaded for this position.
struct SdoEvidenceTarget {
    int position = -1;
    QString index;
    QString subIndex;
    bool dictionaryLoadedForPosition = false;
};

// Localized source labels for each evidence candidate in the comparison view.
struct SdoEvidenceCandidateLabels {
    QString readValue;
    QString watch;
    QString dictionary;
    QString startup;
    QString bookmark;
    QString targetTrailWrite;
    QString targetTrail;
};

// Localized labels for the local evidence detail list.
struct SdoLocalEvidenceLabels {
    QString read;
    QString watchPrefix;
    QString dictionary;
    QString startupPrefix;
    QString bookmarkPrefix;
};

// Localized labels for the write-delta evidence detail list.
struct SdoWriteEvidenceLabels {
    QString read;
    QString dictionary;
    QString watch;
    QString startup;
    QString bookmark;
    QString targetTrail;
};

// A single SDO target trail entry capturing address, values, and operation context.
struct SdoTargetTrailRow {
    int row = -1;
    QString time;
    QString positionText;
    int position = -1;
    bool positionValid = false;
    QString index;
    QString subIndex;
    QString type;
    QString source;
    QString value;
    QString writeValue;
    QString detail;
};

// A bookmarked SDO object with full metadata for the bookmark detail panel.
struct SdoObjectBookmarkRow {
    int row = -1;
    QString positionText;
    int position = -1;
    bool positionValid = false;
    QString slaveName;
    QString index;
    QString subIndex;
    QString access;
    QString type;
    QString bits;
    QString name;
    QString lastValue;
    QString source;
};

// A timestamped SDO history entry for the operation log view.
struct SdoHistoryRow {
    int row = -1;
    QString time;
    QString action;
    QString positionText;
    int position = -1;
    bool positionValid = false;
    QString index;
    QString subIndex;
    QString type;
    QString value;
    QString status;
    QString detail;
};

// Locates matching rows across evidence tables for a given SDO target.
SdoEvidenceTableRows sdoEvidenceTableRowsForTarget(const SdoEvidenceTables& tables, const SdoEvidenceTarget& target);
// Assembles all value candidates for comparison across evidence sources.
SdoEvidenceCandidates sdoEvidenceCandidatesFromTables(const QString& readValue, const SdoEvidenceTables& tables,
                                                      const SdoEvidenceTableRows& rows,
                                                      const SdoEvidenceCandidateLabels& labels);
// Builds the full local evidence list from all evidence tables.
QVector<SdoEvidenceItem> sdoLocalEvidenceItemsFromTables(int position, const QString& index, const QString& subIndex,
                                                         const QString& readValue, const QString& cachedDictionaryValue,
                                                         bool includeReadValue, bool dictionaryTableLoaded,
                                                         const SdoEvidenceTables& tables,
                                                         const SdoLocalEvidenceLabels& labels);
// Assembles write-side evidence from pre-extracted values for delta review.
QVector<SdoEvidenceItem> sdoWriteEvidenceItemsFromValues(const QString& readValue, const QString& dictionaryValue,
                                                         const QString& watchValue, const QString& startupValue,
                                                         const QString& bookmarkValue, const QString& targetTrailValue,
                                                         const QString& targetTrailWriteValue,
                                                         const SdoWriteEvidenceLabels& labels);
// Whether enough evidence exists to render the write-delta review panel.
bool sdoWriteDeltaReviewEvidenceAvailable(const QString& readValue, const SdoEvidenceTables& tables,
                                          const SdoEvidenceTableRows& rows);
// Extracts a target trail row from table cells.
SdoTargetTrailRow sdoTargetTrailRowFromTable(QTableWidget* table, int row);
// Whether the trail row addresses a valid SDO target.
bool sdoTargetTrailRowHasTarget(const SdoTargetTrailRow& row);
// Prefers write value, falls back to read value.
QString sdoTargetTrailRowStartupValue(const SdoTargetTrailRow& row);
// Deterministic key from address + metadata for deduplication.
QString sdoTargetTrailRowKey(int position, const QString& index, const QString& subIndex, const QString& type,
                             const QString& source, const QString& detail);
// Convenience overload using row fields.
QString sdoTargetTrailRowKey(const SdoTargetTrailRow& row);
// Key built directly from table cells.
QString sdoTargetTrailRowKeyFromTable(QTableWidget* table, int row);
// Startup-relevant value for a trail row by table index.
QString sdoTargetTrailStartupValueFromTable(QTableWidget* table, int row);
// All trail keys in a set for quick membership checks.
QSet<QString> sdoTargetTrailKeysFromTable(QTableWidget* table);
// Extracts a bookmarked SDO object row.
SdoObjectBookmarkRow sdoObjectBookmarkRowFromTable(QTableWidget* table, int row);
// Whether the bookmark addresses a valid SDO target.
bool sdoObjectBookmarkRowHasTarget(const SdoObjectBookmarkRow& row);
// Whether access rights indicate read-only (supports localized labels).
bool sdoObjectAccessIsReadOnly(const QString& access, const QString& readOnlyText);
// Extracts an SDO history row with timestamps and operation context.
SdoHistoryRow sdoHistoryRowFromTable(QTableWidget* table, int row);
// Whether the history row addresses a valid SDO target.
bool sdoHistoryRowHasTarget(const SdoHistoryRow& row);
