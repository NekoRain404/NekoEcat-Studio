#pragma once

// Populates and queries the commissioning workflow QTableWidget.


#include <QString>

class QTableWidget;

inline constexpr int kCommissioningWorkflowPhaseColumn = 0;
inline constexpr int kCommissioningWorkflowStatusColumn = 1;
inline constexpr int kCommissioningWorkflowStepColumn = 2;
inline constexpr int kCommissioningWorkflowRiskColumn = 3;
inline constexpr int kCommissioningWorkflowEvidenceColumn = 4;
inline constexpr int kCommissioningWorkflowActionColumn = 5;

inline constexpr const char *kCommissioningWorkflowScopeAll = "all";
inline constexpr const char *kCommissioningWorkflowScopeOpen = "open";
inline constexpr const char *kCommissioningWorkflowScopeBlocked = "blocked";
inline constexpr const char *kCommissioningWorkflowScopeAction = "action";
inline constexpr const char *kCommissioningWorkflowScopeReady = "ready";
inline constexpr const char *kCommissioningWorkflowScopeRisk = "risk";
inline constexpr const char *kCommissioningWorkflowScopeGap = "gap";

struct CommissioningWorkflowRowState {
  bool isReady = false;
  bool isAction = false;
  bool isBlocked = false;
  bool hasRisk = false;
  bool hasGap = false;
  bool reviewIssue = false;
};

struct CommissioningWorkflowFilterStats {
  int visible = 0;
  int open = 0;
  int ready = 0;
  int action = 0;
  int blocked = 0;
  int risk = 0;
  int gaps = 0;
  int firstVisible = -1;
  bool hasVisibleIssue = false;
};

struct CommissioningWorkflowTableRow {
  int row = -1;
  QString phase;
  QString status;
  QString statusKey;
  QString step;
  QString risk;
  QString evidence;
  QString nextAction;
};

void setCommissioningWorkflowStatusKey(QTableWidget *table, int row,
                                       const QString &statusKey);
QString commissioningWorkflowStatusKeyForRow(QTableWidget *table, int row);
CommissioningWorkflowTableRow
commissioningWorkflowTableRowFromTable(QTableWidget *table, int row);
CommissioningWorkflowRowState
commissioningWorkflowRowState(QTableWidget *table, int row,
                              const QString &noneText);
bool commissioningWorkflowScopeMatches(
    const CommissioningWorkflowRowState &state, const QString &scope);
bool commissioningWorkflowSearchMatches(QTableWidget *table, int row,
                                        const QString &needle);
CommissioningWorkflowFilterStats
filterCommissioningWorkflowTable(QTableWidget *table, const QString &scope,
                                 const QString &needle,
                                 const QString &noneText);
int firstCommissioningWorkflowIssueRow(QTableWidget *table);
int nextCommissioningWorkflowIssueRow(QTableWidget *table, int currentRow);
