#pragma once

#include "ConsistencyGateModel.h"

#include <QString>

class QTableWidget;

inline constexpr int kConsistencyLevelColumn = 0;
inline constexpr int kConsistencyScopeColumn = 1;
inline constexpr int kConsistencyTargetColumn = 2;
inline constexpr int kConsistencyEvidenceColumn = 3;
inline constexpr int kConsistencyExpectedColumn = 4;
inline constexpr int kConsistencyActualColumn = 5;
inline constexpr int kConsistencyActionColumn = 6;

inline constexpr const char *kConsistencyScopeAll = "all";
inline constexpr const char *kConsistencyScopeError = "error";
inline constexpr const char *kConsistencyScopeWarning = "warning";
inline constexpr const char *kConsistencyScopeTopology = "topology";
inline constexpr const char *kConsistencyScopeStartup = "startup";
inline constexpr const char *kConsistencyScopeIo = "io";
inline constexpr const char *kConsistencyScopeReady = "ready";

struct ConsistencyTableRowState {
  QString level;
  QString scope;
};

struct ConsistencyTableFilterStats {
  int visible = 0;
  int total = 0;
};

ConsistencyTableRowState consistencyTableRowState(QTableWidget *table, int row);
ConsistencyIssueCounts consistencyTableIssueCounts(QTableWidget *table);
bool consistencyTableScopeMatches(const ConsistencyTableRowState &state,
                                  const QString &scope);
bool consistencyTableSearchMatches(QTableWidget *table, int row,
                                   const QString &needle);
ConsistencyTableFilterStats filterConsistencyTableRows(QTableWidget *table,
                                                       const QString &scope,
                                                       const QString &needle);
int firstConsistencyTableBlockingIssueRow(QTableWidget *table);
int firstConsistencyTableIoIssueRow(QTableWidget *table);
bool consistencyTableAvailable(QTableWidget *table);
