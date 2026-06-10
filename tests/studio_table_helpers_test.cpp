#include "StudioTableHelpers.h"

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

void testSelectAndFocusTableRow() {
  QTableWidget table;
  table.setColumnCount(2);
  table.setRowCount(3);
  for (int row = 0; row < table.rowCount(); ++row) {
    for (int column = 0; column < table.columnCount(); ++column) {
      table.setItem(
          row, column,
          new QTableWidgetItem(QString("%1:%2").arg(row).arg(column)));
    }
  }

  expectTrue(selectAndFocusTableRow(&table, 2, 1), "valid row can be selected");
  expectEqual(table.currentRow(), 2, "selected row");
  expectEqual(table.currentColumn(), 1, "selected column");
  expectTrue(table.item(2, 1)->isSelected(), "selected item is highlighted");

  expectTrue(selectAndFocusTableRow(&table, 1, -4),
             "negative column falls back to zero");
  expectEqual(table.currentRow(), 1, "fallback selected row");
  expectEqual(table.currentColumn(), 0, "fallback selected column");

  expectTrue(selectAndFocusTableRow(&table, 0, table.columnCount() + 3),
             "out-of-range column falls back to zero");
  expectEqual(table.currentRow(), 0, "high-column fallback selected row");
  expectEqual(table.currentColumn(), 0, "high-column fallback selected column");

  expectTrue(!selectAndFocusTableRow(&table, -1, 0),
             "negative row is rejected");
  expectTrue(!selectAndFocusTableRow(&table, table.rowCount(), 0),
             "out-of-range row is rejected");
  expectTrue(!selectAndFocusTableRow(nullptr, 0, 0), "null table is rejected");
}

void testRowLookupHelpers() {
  QTableWidget table;
  table.setColumnCount(3);
  table.setRowCount(3);
  const QList<QStringList> rows = {
      {"1", "6040", "0"},
      {"2", "0x6060", "0x00"},
      {"bad", "0x6041", "0x01"},
  };
  for (int row = 0; row < rows.size(); ++row) {
    for (int column = 0; column < rows[row].size(); ++column) {
      table.setItem(row, column, new QTableWidgetItem(rows[row][column]));
    }
  }

  expectEqual(tableRowForPosition(&table, 2, 0), 1,
              "position lookup finds matching row");
  expectEqual(tableRowForPosition(&table, 9, 0), -1,
              "position lookup rejects missing rows");
  expectEqual(tableRowForPosition(nullptr, 2, 0), -1,
              "position lookup rejects null tables");
  expectTrue(tableObjectIndexMatches(&table, 0, "0x6040", "0x00", 1, 2),
             "object index predicate normalizes table hex text");
  expectTrue(!tableObjectIndexMatches(&table, 0, "0x6060", "0x00", 1, 2),
             "object index predicate rejects mismatches");
  expectTrue(tableObjectAddressMatches(&table, 1, 2, "6060", "0", 0, 1, 2),
             "object address predicate normalizes target hex text");
  expectTrue(!tableObjectAddressMatches(&table, 1, 3, "6060", "0", 0, 1, 2),
             "object address predicate rejects position mismatches");
  expectEqual(tableRowForObjectIndex(&table, "0x6040", "0x00", 1, 2), 0,
              "object index lookup normalizes table hex text");
  expectEqual(tableRowForObjectIndex(&table, "6060", "0", 1, 2), 1,
              "object index lookup normalizes target hex text");
  expectEqual(tableRowForObjectIndex(&table, QString(), "0", 1, 2), -1,
              "object index lookup rejects incomplete addresses");
  expectEqual(tableRowForObjectAddress(&table, 1, "0x6040", "0x00", 0, 1, 2), 0,
              "object lookup normalizes table hex text");
  expectEqual(tableRowForObjectAddress(&table, 2, "6060", "0", 0, 1, 2), 1,
              "object lookup normalizes target hex text");
  expectEqual(tableRowForObjectAddress(&table, 2, QString(), "0", 0, 1, 2), -1,
              "object lookup rejects incomplete addresses");

  expectEqual(firstVisibleTableRow(&table), 0, "first visible row");
  table.setRowHidden(0, true);
  expectEqual(firstVisibleTableRow(&table), 1, "hidden rows are skipped");
  table.setRowHidden(1, true);
  table.setRowHidden(2, true);
  expectEqual(firstVisibleTableRow(&table), -1, "all hidden rows are rejected");
  expectEqual(firstVisibleTableRow(nullptr), -1,
              "visible row lookup rejects null tables");
}

} // namespace

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  testSelectAndFocusTableRow();
  testRowLookupHelpers();
  return 0;
}
