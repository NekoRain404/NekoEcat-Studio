#pragma once

// I/O variable model: bulk naming, row filtering, and PLC handoff export.
//
// Combines three sub-models that all operate on IoVariableTableRow:
//   - BulkNaming: PLC symbol naming rules and collision detection
//   - Filter: predicate set for filtering rows by scope and state
//   - Handoff: PLC handoff issue detection and IEC 61131-3 export


#include "infra/ProcessDataTypes.h"

#include <QHash>
#include <QRegularExpression>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QVector>

// ─────────────────────────────────────────────────────────────────────────────
// Bulk naming
// ─────────────────────────────────────────────────────────────────────────────

struct IoVariableBulkNamingOptions {
  QString prefix;
  QStringList requestedTags;
  bool includeAddress = true;
  bool keepExistingAliases = true;
  bool addDirectionTags = true;
};

struct IoVariableBulkNamingResult {
  QHash<QString, QStringList> metadataUpdates;
  int updated = 0;
  int skippedExistingAliases = 0;
  int skippedInvalidRows = 0;
};

int countIoVariableBulkNamingExistingAliases(
    const QVector<IoVariableTableRow> &rows);
IoVariableBulkNamingResult
buildIoVariableBulkNamingPlan(const QVector<IoVariableTableRow> &allRows,
                              const QVector<int> &targetRows,
                              const QHash<QString, QStringList> &metadataByKey,
                              const IoVariableBulkNamingOptions &options);

// ─────────────────────────────────────────────────────────────────────────────
// Row filtering
// ─────────────────────────────────────────────────────────────────────────────

inline constexpr const char *kIoVariableScopeAll = "all";
inline constexpr const char *kIoVariableScopeSelected = "selected";
inline constexpr const char *kIoVariableScopeProcess = "process";
inline constexpr const char *kIoVariableScopePdo = "pdo";
inline constexpr const char *kIoVariableScopeWatch = "watch";
inline constexpr const char *kIoVariableScopeStartupDiff = "startupDiff";
inline constexpr const char *kIoVariableScopeMissingValue = "missingValue";
inline constexpr const char *kIoVariableScopeRx = "rx";
inline constexpr const char *kIoVariableScopeTx = "tx";
inline constexpr const char *kIoVariableScopeCia402 = "cia402";
inline constexpr const char *kIoVariableScopeChanged = "changed";
inline constexpr const char *kIoVariableScopePlcIssues = "plcIssues";

struct IoVariableFilterRowState {
  bool selected = false;
  bool process = false;
  bool pdo = false;
  bool watchEvidence = false;
  bool startupDiff = false;
  bool missingValue = false;
  bool changed = false;
  bool plcIssue = false;
  bool rx = false;
  bool tx = false;
  bool cia402 = false;
};

struct IoVariableFilterDecision {
  IoVariableFilterRowState state;
  bool visible = false;
};

struct IoVariableFilterStats {
  int visible = 0;
  int total = 0;
  int processRows = 0;
  int watchRows = 0;
  int startupDiffs = 0;
  int missingValues = 0;
  int changedRows = 0;
  int plcIssues = 0;
};

IoVariableFilterRowState ioVariableFilterRowState(const IoVariableTableRow &row,
                                                  int selectedPosition,
                                                  const QString &readyText);
bool ioVariableFilterScopeMatches(const IoVariableFilterRowState &state,
                                  const QString &scope);
bool ioVariableFilterTextMatches(const QStringList &cells,
                                 const QString &needle);
IoVariableFilterDecision
evaluateIoVariableFilterRow(const IoVariableTableRow &row,
                            const QStringList &cells, const QString &scope,
                            const QString &needle, int selectedPosition,
                            const QString &readyText);
void accumulateIoVariableFilterStats(IoVariableFilterStats *stats,
                                     const IoVariableFilterDecision &decision);
QString ioVariableFilterSummaryText(const IoVariableFilterStats &stats,
                                    const QString &scopeLabel,
                                    const QString &summaryPattern);

// ─────────────────────────────────────────────────────────────────────────────
// PLC handoff
// ─────────────────────────────────────────────────────────────────────────────

enum class IoVariableHandoffIssue {
  MissingAlias,
  AutoName,
  NoTags,
  DuplicateSymbol,
};

struct IoVariableHandoffName {
  QString alias;
  QString fallbackAlias;
  QString symbol;
};

struct IoVariableHandoffCsvRow {
  QStringList values;
};

QString suggestedIoVariableAlias(const IoVariableTableRow &row,
                                 const QString &prefix, bool includeAddress);
IoVariableHandoffName ioVariableHandoffName(const IoVariableTableRow &row);
QVector<IoVariableHandoffIssue>
ioVariableHandoffIssues(const IoVariableTableRow &row,
                        const QSet<QString> *duplicateSymbols);
QSet<QString>
duplicateIoVariableHandoffSymbols(const QVector<IoVariableTableRow> &rows);
QStringList
ioVariableHandoffIssueKeys(const QVector<IoVariableHandoffIssue> &issues);
bool ioVariableHandoffHasIssue(const QVector<IoVariableHandoffIssue> &issues,
                               IoVariableHandoffIssue issue);
QString ioVariableHandoffPlcDirection(const IoVariableTableRow &row);
QString ioVariableHandoffPlcType(const IoVariableTableRow &row);
QString ioVariableHandoffComment(const IoVariableTableRow &row,
                                 const QStringList &qualityLabels);
QString ioVariableHandoffUniqueSymbol(const IoVariableTableRow &row,
                                      QSet<QString> *usedSymbols);
QString ioVariableHandoffDeclarationLine(const IoVariableTableRow &row,
                                         QSet<QString> *usedSymbols,
                                         const QStringList &qualityLabels);
QString ioVariableHandoffDeclarationBlock(
    const QVector<IoVariableTableRow> &rows,
    const QVector<QStringList> &qualityLabelsByRow);
QStringList ioVariableHandoffCsvHeaders();
IoVariableHandoffCsvRow ioVariableHandoffCsvRow(const IoVariableTableRow &row,
                                                QSet<QString> *usedSymbols,
                                                const QString &exportedAt);
