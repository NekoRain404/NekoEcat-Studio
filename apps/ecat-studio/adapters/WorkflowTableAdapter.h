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

inline constexpr const char* kCommissioningWorkflowScopeAll = "all";
inline constexpr const char* kCommissioningWorkflowScopeOpen = "open";
inline constexpr const char* kCommissioningWorkflowScopeBlocked = "blocked";
inline constexpr const char* kCommissioningWorkflowScopeAction = "action";
inline constexpr const char* kCommissioningWorkflowScopeReady = "ready";
inline constexpr const char* kCommissioningWorkflowScopeRisk = "risk";
inline constexpr const char* kCommissioningWorkflowScopeGap = "gap";

// Aggregated boolean flags describing a single commissioning workflow row.
struct CommissioningWorkflowRowState {
    bool isReady = false;
    bool isAction = false;
    bool isBlocked = false;
    bool hasRisk = false;
    bool hasGap = false;
    bool reviewIssue = false;
};

// Accumulated counts after filtering, used to drive status badges and navigation.
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

// Snapshot of all column values for a single commissioning workflow row.
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

// Stores the status key on every cell in the row for later retrieval.
void setCommissioningWorkflowStatusKey(QTableWidget* table, int row, const QString& statusKey);
// Retrieves the stored status key for filtering/routing decisions.
QString commissioningWorkflowStatusKeyForRow(QTableWidget* table, int row);
// Extracts structured row data from table cells for model binding.
CommissioningWorkflowTableRow commissioningWorkflowTableRowFromTable(QTableWidget* table, int row);
// Derives boolean flags for filtering, badges, and issue navigation.
CommissioningWorkflowRowState commissioningWorkflowRowState(QTableWidget* table, int row, const QString& noneText);
// Tests whether a row's state passes the active filter scope.
bool commissioningWorkflowScopeMatches(const CommissioningWorkflowRowState& state, const QString& scope);
// Case-insensitive full-row text search.
bool commissioningWorkflowSearchMatches(QTableWidget* table, int row, const QString& needle);
// Applies scope + text filters and returns aggregate counts.
CommissioningWorkflowFilterStats filterCommissioningWorkflowTable(QTableWidget* table, const QString& scope,
                                                                  const QString& needle, const QString& noneText);
// Returns the first visible review-issue row for initial auto-scroll.
int firstCommissioningWorkflowIssueRow(QTableWidget* table);
// Wraps around to find the next review-issue row for cycling.
int nextCommissioningWorkflowIssueRow(QTableWidget* table, int currentRow);
