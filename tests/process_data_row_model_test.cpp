#include "ProcessDataRowModel.h"

#include <QCoreApplication>

#include <cstdlib>
#include <iostream>

namespace {

void fail(const QString &message) {
  std::cerr << message.toStdString() << '\n';
  std::exit(1);
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

IoVariableTableRow baseIoRow() {
  IoVariableTableRow row;
  row.position = 2;
  row.positionValid = true;
  row.direction = "Rx Output";
  row.index = "0x6040";
  row.subIndex = "0x00";
  row.bits = "16";
  row.source = "Process | Word";
  row.raw = "0x0006";
  row.watch = "0x000F";
  row.startup = "Diff";
  row.map = "Missing in PDO map";
  row.changed = "Yes";
  row.plcQuality = "Missing Alias";
  row.meaning = "Controlword";
  return row;
}

void testTypeRules() {
  expectEqual(processDataTypeFromBits("1"), "bool", "1-bit type");
  expectEqual(processDataTypeFromBits("8"), "uint8", "8-bit type");
  expectEqual(processDataTypeFromBits("16"), "uint16", "16-bit type");
  expectEqual(processDataTypeFromBits("32"), "uint32", "32-bit type");
  expectEqual(processDataTypeFromBits("64"), "uint64", "64-bit type");
  expectEqual(processDataTypeFromBits("bad"), QString(), "bad bit text");

  expectEqual(processDataNormalizedKnownType(" Word "), "uint16",
              "IEC alias normalized to SDO type");
  expectEqual(processDataNormalizedKnownType("Real64"), "double",
              "real64 normalized to double");
  expectEqual(processDataIecTypeFromNormalizedType("uint16"), "UINT",
              "uint16 maps to IEC UINT");
  expectEqual(processDataIecTypeFromNormalizedType("double"), "LREAL",
              "double maps to IEC LREAL");
}

void testIoRowRules() {
  const IoVariableTableRow row = baseIoRow();
  expectTrue(ioVariableTableRowHasTarget(row), "I/O row has target");
  expectTrue(ioVariableTableRowHasValue(row), "I/O row has value");
  expectEqual(ioVariableTableRowPreferredValue(row), "0x0006",
              "preferred value keeps Raw priority");
  expectEqual(ioVariableTableRowStartupValue(row), "0x000F",
              "startup value keeps Watch priority");
  expectEqual(ioVariableTableRowTypeFromBits(row), "uint16",
              "type inferred from bits");
  expectEqual(ioVariableTableRowSdoType(row), "uint16",
              "type inferred from source alias");
  expectEqual(ioVariableTableRowIecType(row), "UINT", "IEC type inferred");
  expectEqual(ioVariableTableRowKey(row), "2|0x6040|0x00",
              "I/O row key generated");
  expectTrue(ioVariableTableRowHasProcessSource(row),
             "process source detected");
  expectTrue(ioVariableTableRowHasStartupDiff(row), "startup diff detected");
  expectTrue(ioVariableTableRowHasPdoMapIssue(row), "PDO map issue detected");
  expectTrue(ioVariableTableRowHasChangedValue(row), "changed row detected");
  expectTrue(ioVariableTableRowHasPlcIssue(row, "Ready"), "PLC issue detected");
  expectTrue(ioVariableTableRowIsRx(row), "Rx output detected");
  expectTrue(ioVariableTableRowIsCia402(row), "CiA 402 object detected");
  expectFalse(ioVariableTableRowHasMissingValue(row),
              "row with raw/watch is not missing value");
}

void testFallbackRules() {
  IoVariableTableRow row;
  row.position = 4;
  row.positionValid = true;
  row.direction = "Tx Input";
  row.index = "0x6061";
  row.subIndex = "0x00";
  row.bits = "8";
  row.source = "PDO";

  expectEqual(ioVariableTableRowSdoType(row), "uint8",
              "source without type falls back to bits");
  expectEqual(ioVariableTableRowIecType(row), "USINT",
              "IEC type falls back to bit width");
  expectTrue(ioVariableTableRowHasPdoSource(row), "PDO source detected");
  expectTrue(ioVariableTableRowIsTx(row), "Tx input detected");
  expectTrue(ioVariableTableRowHasMissingValue(row),
             "row without raw/watch is missing value");

  row.positionValid = false;
  expectFalse(ioVariableTableRowHasTarget(row),
              "invalid position has no target");
  expectEqual(ioVariableTableRowKey(row), QString(),
              "invalid target has no key");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testTypeRules();
  testIoRowRules();
  testFallbackRules();
  return 0;
}
