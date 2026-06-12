// Unit tests for SdoDictionaryTableAdapter.
#include "adapters/SdoDictionaryTableAdapter.h"

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

void expectTrue(bool condition, const QString &message) {
  if (!condition) {
    fail(message);
  }
}

void expectFalse(bool condition, const QString &message) {
  if (condition) {
    fail(message);
  }
}

void setCell(QTableWidget *table, int row, int column, const QString &value) {
  table->setItem(row, column, new QTableWidgetItem(value));
}

void populateDictionaryTable(QTableWidget *table) {
  table->setColumnCount(9);
  table->setRowCount(4);

  setCell(table, 0, 0, "0x6040");
  setCell(table, 0, 1, "6040");
  setCell(table, 0, 2, "0");
  setCell(table, 0, 3, "rw");
  setCell(table, 0, 4, "uint16");
  setCell(table, 0, 5, "16");
  setCell(table, 0, 6, "Controlword");
  setCell(table, 0, 7, "0x0006");
  setCell(table, 0, 8, "Complete");

  setCell(table, 1, 1, "0x6041");
  setCell(table, 1, 2, "0x00");
  setCell(table, 1, 3, "ro");
  setCell(table, 1, 4, "uint16");
  setCell(table, 1, 6, "Statusword");
  setCell(table, 1, 8, "failed: timeout");

  setCell(table, 2, 1, "");
  setCell(table, 2, 2, "");
  setCell(table, 2, 8, "失败");

  setCell(table, 3, 1, "6060");
  setCell(table, 3, 2, "0");
  setCell(table, 3, 4, "int8");
  table->hideRow(3);
}

void testRowParsing() {
  QTableWidget table;
  populateDictionaryTable(&table);

  const SdoDictionaryRow row = sdoDictionaryRowFromTable(&table, 0);
  expectTrue(sdoDictionaryRowHasTarget(row), "dictionary row has target");
  expectTrue(sdoDictionaryRowHasValue(row), "dictionary row has value");
  expectTrue(sdoDictionaryRowIsWritable(row), "dictionary row is writable");
  expectEqual(row.index, "0x6040", "normalized index");
  expectEqual(row.subIndex, "0x00", "normalized subindex");
  expectEqual(row.type, "uint16", "type");
  expectEqual(row.value, "0x0006", "value");

  const SdoDictionaryRow readOnly = sdoDictionaryRowFromTable(&table, 1);
  expectFalse(sdoDictionaryRowIsWritable(readOnly),
              "read-only row is not writable");

  const SdoDictionaryRow missing = sdoDictionaryRowFromTable(&table, 20);
  expectFalse(sdoDictionaryRowHasTarget(missing), "missing row has no target");
}

void testRowCollections() {
  QTableWidget table;
  populateDictionaryTable(&table);

  const QVector<int> visible = visibleSdoDictionaryRows(&table);
  expectEqual(visible.size(), 3, "visible row count");
  expectEqual(visible.at(2), 2, "hidden row is skipped");

  const QVector<int> failed = failedSdoDictionaryRows(&table);
  expectEqual(failed.size(), 2, "failed row count");
  expectEqual(failed.at(0), 1, "english failed row");
  expectEqual(failed.at(1), 2, "localized failed row");

  expectTrue(sdoDictionaryRowsContainValue(&table, {0, 1}),
             "selected rows contain value");
  expectFalse(sdoDictionaryRowsContainValue(&table, {1, 2}),
              "selected rows contain no values");
}

void testReadObjectsFromRows() {
  QTableWidget table;
  populateDictionaryTable(&table);

  int skipped = -1;
  const QVector<SdoDictionaryReadObject> objects =
      sdoDictionaryReadObjectsFromRows(&table, {0, 1, 2, 3, 9}, &skipped);

  expectEqual(objects.size(), 2, "read object count");
  expectEqual(skipped, 3, "read object skipped count");
  expectEqual(objects.at(0).row, 0, "first object row");
  expectEqual(objects.at(0).index, "0x6040", "first object index");
  expectEqual(objects.at(0).subIndex, "0x00", "first object subindex");
  expectEqual(objects.at(1).index, "0x6041", "second object index");
}

void testTargetLookup() {
  QTableWidget table;
  populateDictionaryTable(&table);

  const SdoDictionaryRow row = sdoDictionaryRowForTarget(&table, "6040", "0");
  expectEqual(row.row, 0, "target lookup row");
  expectEqual(row.name, "Controlword", "target lookup name");

  const SdoDictionaryRow missing =
      sdoDictionaryRowForTarget(&table, "0x9999", "0x00");
  expectFalse(sdoDictionaryRowHasTarget(missing),
              "missing target lookup has no target");
}

} // namespace

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  testRowParsing();
  testRowCollections();
  testReadObjectsFromRows();
  testTargetLookup();
  return 0;
}
