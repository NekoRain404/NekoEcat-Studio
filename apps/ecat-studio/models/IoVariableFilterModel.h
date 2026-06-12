#pragma once

// Predicate set for filtering I/O variable table rows by scope and state.


#include "infra/ProcessDataTypes.h"

#include <QString>
#include <QStringList>

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
