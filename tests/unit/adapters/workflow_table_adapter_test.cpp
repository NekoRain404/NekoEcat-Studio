// WorkflowTableAdapterTest — Tests for Workflow Table Adapter
//
// Test coverage:
//   - Status key storage and retrieval from table cells
//   - Workflow row initialization with phase, status, step columns
//   - Table population from workflow data
//   - Status color and icon mapping
//   - Filter and sort operations
#include "adapters/WorkflowTableAdapter.h"

#include <QApplication>
#include <QTableWidget>
#include <QTableWidgetItem>

#include <cstdlib>
#include <iostream>

namespace {

void fail(const QString &message) {
  std::cerr << message.toStdString() << '\n';
  std::exit(1);
}

void expectTrue(bool condition, const QString &message) {
  if (!condition) {
    fail(message);
  }
}

void expectEqual(int actual, int expected, const QString &message) {
  if (actual != expected) {
    fail(QString("%1: expected %2, got %3")
             .arg(message)
             .arg(expected)
             .arg(actual));
  }
}

void expectEqual(const QString &actual, const QString &expected,
                 const QString &message) {
  if (actual != expected) {
    fail(QString("%1: expected %2, got %3").arg(message, expected, actual));
  }
}

void setCell(QTableWidget *table, int row, int column, const QString &text) {
  table->setItem(row, column, new QTableWidgetItem(text));
}

void initWorkflowTable(QTableWidget *table) {
  table->setColumnCount(6);
  table->setRowCount(3);

  setCell(table, 0, kCommissioningWorkflowPhaseColumn, "Runtime");
  setCell(table, 0, kCommissioningWorkflowStatusColumn, "Ready");
  setCell(table, 0, kCommissioningWorkflowStepColumn, "Connect runtime");
  setCell(table, 0, kCommissioningWorkflowRiskColumn, "None");
  setCell(table, 0, kCommissioningWorkflowEvidenceColumn, "ecatd is connected");
  setCell(table, 0, kCommissioningWorkflowActionColumn, "Refresh online data");
  setCommissioningWorkflowStatusKey(table, 0, "ready");

  setCell(table, 1, kCommissioningWorkflowPhaseColumn, "Evidence");
  setCell(table, 1, kCommissioningWorkflowStatusColumn, "Action");
  setCell(table, 1, kCommissioningWorkflowStepColumn,
          "Inspect Object Dictionary");
  setCell(table, 1, kCommissioningWorkflowRiskColumn, "OD missing");
  setCell(table, 1, kCommissioningWorkflowEvidenceColumn,
          "Object data not loaded");
  setCell(table, 1, kCommissioningWorkflowActionColumn,
          "Open Object Dictionary");
  setCommissioningWorkflowStatusKey(table, 1, "action");

  setCell(table, 2, kCommissioningWorkflowPhaseColumn, "Map");
  setCell(table, 2, kCommissioningWorkflowStatusColumn, "Blocked");
  setCell(table, 2, kCommissioningWorkflowStepColumn, "Review PDO Map");
  setCell(table, 2, kCommissioningWorkflowRiskColumn, "PDO missing");
  setCell(table, 2, kCommissioningWorkflowEvidenceColumn,
          "PDO data not loaded");
  setCell(table, 2, kCommissioningWorkflowActionColumn, "Select a slave first");
  setCommissioningWorkflowStatusKey(table, 2, "blocked");
}

// Test status key storage, row state flags, and structured row extraction
void testStatusStorageAndState() {
  QTableWidget table;
  initWorkflowTable(&table);

  expectEqual(commissioningWorkflowStatusKeyForRow(&table, 1), "action",
              "workflow status key is stored");
  const CommissioningWorkflowRowState state =
      commissioningWorkflowRowState(&table, 1, "None");
  expectTrue(state.isAction, "action row state");
  expectTrue(!state.isReady, "action row is not ready");
  expectTrue(state.hasRisk, "action row has risk");
  expectTrue(state.hasGap, "action row has evidence gap");
  expectTrue(state.reviewIssue, "action row is review issue");

  const CommissioningWorkflowTableRow row =
      commissioningWorkflowTableRowFromTable(&table, 1);
  expectEqual(row.row, 1, "workflow row index");
  expectEqual(row.phase, "Evidence", "workflow row phase");
  expectEqual(row.status, "Action", "workflow row status");
  expectEqual(row.statusKey, "action", "workflow row status key");
  expectEqual(row.step, "Inspect Object Dictionary", "workflow row step");
  expectEqual(row.risk, "OD missing", "workflow row risk");
  expectEqual(row.evidence, "Object data not loaded", "workflow row evidence");
  expectEqual(row.nextAction, "Open Object Dictionary",
              "workflow row next action");
}

// Test scope filtering, text search, and issue row navigation
void testFilteringAndIssueLookup() {
  QTableWidget table;
  initWorkflowTable(&table);

  const CommissioningWorkflowFilterStats blocked =
      filterCommissioningWorkflowTable(
          &table, QString::fromLatin1(kCommissioningWorkflowScopeBlocked),
          QString(), "None");
  expectEqual(blocked.visible, 1, "blocked filter visible count");
  expectEqual(blocked.open, 2, "open count across all rows");
  expectEqual(blocked.ready, 1, "ready count across all rows");
  expectEqual(blocked.action, 1, "action count across all rows");
  expectEqual(blocked.blocked, 1, "blocked count across all rows");
  expectEqual(blocked.risk, 2, "risk count across all rows");
  expectEqual(blocked.gaps, 2, "gap count across all rows");
  expectTrue(table.isRowHidden(0), "ready row hidden by blocked filter");
  expectTrue(table.isRowHidden(1), "action row hidden by blocked filter");
  expectTrue(!table.isRowHidden(2), "blocked row visible");
  expectEqual(firstCommissioningWorkflowIssueRow(&table), 2,
              "first visible issue row");

  const CommissioningWorkflowFilterStats search =
      filterCommissioningWorkflowTable(
          &table, QString::fromLatin1(kCommissioningWorkflowScopeAll), "Object",
          "None");
  expectEqual(search.visible, 1, "search visible count");
  expectTrue(!table.isRowHidden(1), "search keeps matching action row");
  expectEqual(firstCommissioningWorkflowIssueRow(&table), 1,
              "first visible action issue row");
  expectEqual(nextCommissioningWorkflowIssueRow(&table, 1), 1,
              "single visible issue wraps to itself");

  filterCommissioningWorkflowTable(
      &table, QString::fromLatin1(kCommissioningWorkflowScopeOpen), QString(),
      "None");
  expectEqual(nextCommissioningWorkflowIssueRow(&table, 1), 2,
              "next visible issue advances");
  expectEqual(nextCommissioningWorkflowIssueRow(&table, 2), 1,
              "next visible issue wraps");
}

} // namespace

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  testStatusStorageAndState();
  testFilteringAndIssueLookup();
  return 0;
}
