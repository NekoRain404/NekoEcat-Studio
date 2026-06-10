#pragma once

#include "SdoEvidenceModel.h"

#include <QSet>
#include <QString>

class QTableWidget;

struct SdoEvidenceTableRows {
  int dictionaryRow = -1;
  int watchRow = -1;
  int startupRow = -1;
  int bookmarkRow = -1;
  int targetTrailRow = -1;
};

struct SdoEvidenceTables {
  QTableWidget *dictionaryTable = nullptr;
  QTableWidget *watchTable = nullptr;
  QTableWidget *startupTable = nullptr;
  QTableWidget *bookmarkTable = nullptr;
  QTableWidget *targetTrailTable = nullptr;
};

struct SdoEvidenceTarget {
  int position = -1;
  QString index;
  QString subIndex;
  bool dictionaryLoadedForPosition = false;
};

struct SdoEvidenceCandidateLabels {
  QString readValue;
  QString watch;
  QString dictionary;
  QString startup;
  QString bookmark;
  QString targetTrailWrite;
  QString targetTrail;
};

struct SdoLocalEvidenceLabels {
  QString read;
  QString watchPrefix;
  QString dictionary;
  QString startupPrefix;
  QString bookmarkPrefix;
};

struct SdoWriteEvidenceLabels {
  QString read;
  QString dictionary;
  QString watch;
  QString startup;
  QString bookmark;
  QString targetTrail;
};

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

SdoEvidenceTableRows
sdoEvidenceTableRowsForTarget(const SdoEvidenceTables &tables,
                              const SdoEvidenceTarget &target);
SdoEvidenceCandidates sdoEvidenceCandidatesFromTables(
    const QString &readValue, const SdoEvidenceTables &tables,
    const SdoEvidenceTableRows &rows, const SdoEvidenceCandidateLabels &labels);
QVector<SdoEvidenceItem> sdoLocalEvidenceItemsFromTables(
    int position, const QString &index, const QString &subIndex,
    const QString &readValue, const QString &cachedDictionaryValue,
    bool includeReadValue, bool dictionaryTableLoaded,
    const SdoEvidenceTables &tables, const SdoLocalEvidenceLabels &labels);
QVector<SdoEvidenceItem> sdoWriteEvidenceItemsFromValues(
    const QString &readValue, const QString &dictionaryValue,
    const QString &watchValue, const QString &startupValue,
    const QString &bookmarkValue, const QString &targetTrailValue,
    const QString &targetTrailWriteValue, const SdoWriteEvidenceLabels &labels);
bool sdoWriteDeltaReviewEvidenceAvailable(const QString &readValue,
                                          const SdoEvidenceTables &tables,
                                          const SdoEvidenceTableRows &rows);
SdoTargetTrailRow sdoTargetTrailRowFromTable(QTableWidget *table, int row);
bool sdoTargetTrailRowHasTarget(const SdoTargetTrailRow &row);
QString sdoTargetTrailRowStartupValue(const SdoTargetTrailRow &row);
QString sdoTargetTrailRowKey(int position, const QString &index,
                             const QString &subIndex, const QString &type,
                             const QString &source, const QString &detail);
QString sdoTargetTrailRowKey(const SdoTargetTrailRow &row);
QString sdoTargetTrailRowKeyFromTable(QTableWidget *table, int row);
QString sdoTargetTrailStartupValueFromTable(QTableWidget *table, int row);
QSet<QString> sdoTargetTrailKeysFromTable(QTableWidget *table);
SdoObjectBookmarkRow sdoObjectBookmarkRowFromTable(QTableWidget *table,
                                                   int row);
bool sdoObjectBookmarkRowHasTarget(const SdoObjectBookmarkRow &row);
bool sdoObjectAccessIsReadOnly(const QString &access,
                               const QString &readOnlyText);
SdoHistoryRow sdoHistoryRowFromTable(QTableWidget *table, int row);
bool sdoHistoryRowHasTarget(const SdoHistoryRow &row);
