// Unit tests for ConsistencyTableAdapter.
#include "adapters/ConsistencyTableAdapter.h"

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

void setCell(QTableWidget *table, int row, int column, const QString &text) {
  table->setItem(row, column, new QTableWidgetItem(text));
}

void initTable(QTableWidget *table) {
  table->setColumnCount(7);
  table->setRowCount(5);
  setCell(table, 0, kConsistencyLevelColumn, "Error");
  setCell(table, 0, kConsistencyScopeColumn, "Topology");
  setCell(table, 0, kConsistencyTargetColumn, "#1 state");
  setCell(table, 1, kConsistencyLevelColumn, "Warning");
  setCell(table, 1, kConsistencyScopeColumn, "I/O Variables");
  setCell(table, 1, kConsistencyTargetColumn, "#2 0x6040:0x00");
  setCell(table, 2, kConsistencyLevelColumn, "Ready");
  setCell(table, 2, kConsistencyScopeColumn, "Project");
  setCell(table, 2, kConsistencyTargetColumn, "#3 ready");
  setCell(table, 3, kConsistencyLevelColumn, "Info");
  setCell(table, 3, kConsistencyScopeColumn, "Startup");
  setCell(table, 3, kConsistencyTargetColumn, "#4 startup row 1");
  setCell(table, 4, kConsistencyLevelColumn, "警告");
  setCell(table, 4, kConsistencyScopeColumn, "变量");
  setCell(table, 4, kConsistencyTargetColumn, "#5 0x6060:0x00");
}

void testCountsAndFirstRows() {
  QTableWidget table;
  initTable(&table);

  const ConsistencyIssueCounts counts = consistencyTableIssueCounts(&table);
  expectEqual(counts.errors, 1, "error count");
  expectEqual(counts.warnings, 2, "warning count");
  expectEqual(counts.ready, 1, "ready count");
  expectEqual(counts.infos, 1, "info count");
  expectEqual(firstConsistencyTableBlockingIssueRow(&table), 0,
              "first blocking row");
  expectEqual(firstConsistencyTableIoIssueRow(&table), 1,
              "first I/O issue row");
  expectTrue(consistencyTableAvailable(&table), "table is available");
  expectTrue(!consistencyTableAvailable(nullptr), "null table is unavailable");
}

void testScopeAndSearchFiltering() {
  QTableWidget table;
  initTable(&table);

  ConsistencyTableFilterStats stats = filterConsistencyTableRows(
      &table, QString::fromLatin1(kConsistencyScopeIo), QString());
  expectEqual(stats.total, 5, "filter total");
  expectEqual(stats.visible, 2, "I/O visible rows");
  expectTrue(table.isRowHidden(0), "topology row hidden by I/O filter");
  expectTrue(!table.isRowHidden(1), "English I/O row visible");
  expectTrue(!table.isRowHidden(4), "Chinese I/O row visible");

  stats = filterConsistencyTableRows(
      &table, QString::fromLatin1(kConsistencyScopeWarning), "0x6060");
  expectEqual(stats.visible, 1, "warning search visible rows");
  expectTrue(table.isRowHidden(1), "non-matching warning row hidden");
  expectTrue(!table.isRowHidden(4), "matching warning row visible");

  stats = filterConsistencyTableRows(
      &table, QString::fromLatin1(kConsistencyScopeReady), QString());
  expectEqual(stats.visible, 1, "ready visible rows");
  expectTrue(!table.isRowHidden(2), "ready row visible");
}

} // namespace

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  testCountsAndFirstRows();
  testScopeAndSearchFiltering();
  return 0;
}
