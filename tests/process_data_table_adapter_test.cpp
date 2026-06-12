// Unit tests for ProcessDataTableAdapter.
#include "models/ProcessDataRowModel.h"
#include "adapters/ProcessDataTableAdapter.h"

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

void testPdoMapRowParsing() {
  QTableWidget table;
  table.setColumnCount(6);
  table.setRowCount(1);
  setCell(&table, 0, 0, "SM2");
  setCell(&table, 0, 1, "0x1600");
  setCell(&table, 0, 2, "6040");
  setCell(&table, 0, 3, "0");
  setCell(&table, 0, 4, "16");
  setCell(&table, 0, 5, "Controlword");

  const PdoMapTableRow row = pdoMapTableRowFromTable(&table, 0);
  expectTrue(pdoMapTableRowHasTarget(row), "PDO row has target");
  expectEqual(row.index, "0x6040", "PDO row normalized index");
  expectEqual(row.subIndex, "0x00", "PDO row normalized subindex");
  expectEqual(row.syncManager, "SM2", "PDO row sync manager");
  expectEqual(row.name, "Controlword", "PDO row name");

  expectFalse(pdoMapTableRowHasTarget(pdoMapTableRowFromTable(&table, 3)),
              "missing PDO row has no target");
}

void testFreeRunEntryRowParsing() {
  QTableWidget table;
  table.setColumnCount(15);
  table.setRowCount(1);
  setCell(&table, 0, 0, "3");
  setCell(&table, 0, 1, "2");
  setCell(&table, 0, 2, "Rx");
  setCell(&table, 0, 3, "0x1600");
  setCell(&table, 0, 4, "6040");
  setCell(&table, 0, 5, "0");
  setCell(&table, 0, 6, "16");
  setCell(&table, 0, 7, "12");
  setCell(&table, 0, 8, "0");
  setCell(&table, 0, 9, "Controlword");
  setCell(&table, 0, 10, "0x0006");
  setCell(&table, 0, 11, "Switch on");
  setCell(&table, 0, 12, "Controlword");
  setCell(&table, 0, 13, "Mapped warning: bit mismatch 16/32");
  setCell(&table, 0, 14, "0x1600 32 bit | Name source: PDO map");
  table.item(0, 0)->setData(Qt::UserRole, true);

  const FreeRunEntryTableRow row = freeRunEntryTableRowFromTable(&table, 0);
  expectTrue(freeRunEntryTableRowHasTarget(row), "Free Run row has target");
  expectEqual(row.position, 3, "Free Run row position");
  expectEqual(row.syncManager, "2", "Free Run sync manager");
  expectEqual(row.direction, "Rx", "Free Run direction");
  expectEqual(row.pdo, "0x1600", "Free Run PDO");
  expectEqual(row.index, "0x6040", "Free Run row normalized index");
  expectEqual(row.subIndex, "0x00", "Free Run row normalized subindex");
  expectEqual(row.bits, "16", "Free Run bits");
  expectEqual(row.offset, "12", "Free Run offset");
  expectEqual(row.bit, "0", "Free Run bit offset");
  expectEqual(row.name, "Controlword", "Free Run name");
  expectEqual(row.raw, "0x0006", "Free Run raw value");
  expectEqual(row.decoded, "Switch on", "Free Run decoded value");
  expectEqual(row.meaning, "Controlword", "Free Run meaning");
  expectEqual(row.mapStatus, "Mapped warning: bit mismatch 16/32",
              "Free Run map status");
  expectEqual(row.mapDetail, "0x1600 32 bit | Name source: PDO map",
              "Free Run map detail");
  expectTrue(row.changed, "Free Run changed state");
}

void testIoVariableRowParsing() {
  QTableWidget table;
  table.setColumnCount(19);
  table.setRowCount(3);
  setCell(&table, 0, 0, "3");
  setCell(&table, 0, 1, "Rx");
  setCell(&table, 0, 2, "Drive.Controlword");
  setCell(&table, 0, 3, "6040");
  setCell(&table, 0, 4, "0");
  setCell(&table, 0, 5, "16");
  setCell(&table, 0, 6, "0x1600");
  setCell(&table, 0, 7, "Process | Word");
  setCell(&table, 0, 8, "0x0006");
  setCell(&table, 0, 9, "Switch on");
  setCell(&table, 0, 10, "Controlword");
  setCell(&table, 0, 11, "0x000F");
  setCell(&table, 0, 12, "Watch diff");
  setCell(&table, 0, 13, "Missing in PDO map");
  setCell(&table, 0, 14, "Yes");
  setCell(&table, 0, 15, "Missing Alias");
  setCell(&table, 0, 16, "AxisControl");
  setCell(&table, 0, 17, "axis, output");
  setCell(&table, 0, 18, "commissioning note");

  setCell(&table, 1, 0, "4");
  setCell(&table, 1, 1, "Tx Input");
  setCell(&table, 1, 3, "6060");
  setCell(&table, 1, 4, "0");
  setCell(&table, 1, 5, "8");
  setCell(&table, 1, 7, "PDO | Byte");
  table.hideRow(1);

  setCell(&table, 2, 0, "bad");
  setCell(&table, 2, 3, "6041");
  setCell(&table, 2, 4, "0");

  const IoVariableTableRow row = ioVariableTableRowFromTable(&table, 0);
  expectTrue(ioVariableTableRowHasTarget(row), "I/O row has target");
  expectTrue(ioVariableTableRowHasValue(row), "I/O row has value");
  expectTrue(ioVariableTableRowHasValue(&table, 0),
             "I/O row table value helper has value");
  expectEqual(row.position, 3, "I/O row position");
  expectEqual(row.index, "0x6040", "I/O row normalized index");
  expectEqual(row.subIndex, "0x00", "I/O row normalized subindex");
  expectEqual(ioVariableTableRowPreferredValue(row), "0x0006",
              "I/O preferred value keeps Raw priority");
  expectEqual(ioVariableTableRowStartupValue(row), "0x000F",
              "I/O startup value keeps Watch priority");
  expectEqual(ioVariableTableRowTypeFromBits(row), "uint16",
              "I/O type from bits");
  expectEqual(ioVariableTableRowSdoType(row), "uint16",
              "I/O type from source alias");
  expectEqual(ioVariableTableRowKey(row), "3|0x6040|0x00", "I/O object key");
  expectEqual(ioVariableTableRowKey(&table, 0), "3|0x6040|0x00",
              "I/O object key from table");
  expectEqual(ioVariableTableObjectKey(3, "6040", "0"), "3|0x6040|0x00",
              "I/O object key normalizes address");
  expectTrue(ioVariableTableRowHasProcessSource(row),
             "I/O process source detected");
  expectTrue(ioVariableTableRowHasStartupDiff(row),
             "I/O startup diff detected");
  expectTrue(ioVariableTableRowHasPdoMapIssue(row),
             "I/O PDO map issue detected");
  expectTrue(ioVariableTableRowHasChangedValue(row),
             "I/O changed value detected");
  expectTrue(ioVariableTableRowHasPlcIssue(row, "Ready"),
             "I/O PLC issue detected");
  expectTrue(ioVariableTableRowIsRx(row), "I/O Rx output detected");
  expectTrue(ioVariableTableRowIsCia402(row), "I/O CiA 402 object detected");

  const QVector<int> visible = visibleIoVariableTableRows(&table);
  expectEqual(visible.size(), 2, "visible I/O row count");
  const QVector<int> selected = selectedIoVariableTableRows(&table, true);
  expectEqual(selected.size(), 0, "no selected visible I/O rows");
  expectTrue(ioVariableTableRowsContainValue(&table, visible),
             "visible I/O rows contain a value");
  expectFalse(
      ioVariableTableRowHasTarget(ioVariableTableRowFromTable(&table, 2)),
      "invalid position I/O row has no target");
  expectTrue(ioVariableTableRowIsTx(ioVariableTableRowFromTable(&table, 1)),
             "I/O Tx input detected");
  expectTrue(
      ioVariableTableRowHasPdoSource(ioVariableTableRowFromTable(&table, 1)),
      "I/O PDO source detected");
  expectFalse(ioVariableTableRowHasMissingValue(row),
              "I/O row with Raw/Watch is not missing value");
}

} // namespace

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  testPdoMapRowParsing();
  testFreeRunEntryRowParsing();
  testIoVariableRowParsing();
  return 0;
}
