// Unit tests for IoVariableBulkNamingModel.
#include "models/IoVariableModel.h"

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

void expectEqual(int actual, int expected, const QString &message) {
  if (actual != expected) {
    fail(QString("%1: expected %2, got %3")
             .arg(message)
             .arg(expected)
             .arg(actual));
  }
}

void expectTrue(bool condition, const QString &message) {
  if (!condition) {
    fail(message);
  }
}

IoVariableTableRow rowFor(int rowNumber, int position, const QString &index,
                          const QString &alias, const QString &symbol) {
  IoVariableTableRow row;
  row.row = rowNumber;
  row.position = position;
  row.positionValid = true;
  row.direction = "Rx Output";
  row.symbol = symbol;
  row.index = index;
  row.subIndex = "0x00";
  row.bits = "16";
  row.source = "Process | uint16";
  row.meaning = symbol;
  row.alias = alias;
  return row;
}

void testKeepExistingAliasAndMergeTags() {
  const IoVariableTableRow first =
      rowFor(0, 3, "0x6040", "Existing_Alias", "Drive.Controlword");
  const IoVariableTableRow second =
      rowFor(1, 3, "0x6041", QString(), "Drive.Statusword");
  const IoVariableTableRow reserved =
      rowFor(2, 4, "0x6040", "Reserved_Alias", "Other.Controlword");

  QHash<QString, QStringList> metadata;
  metadata.insert("3|0x6040|0x00", {"Existing_Alias", "axis", "keep"});

  IoVariableBulkNamingOptions options;
  options.prefix = "Line 1";
  options.requestedTags = {"plc", "axis"};
  options.includeAddress = true;
  options.keepExistingAliases = true;
  options.addDirectionTags = true;

  const IoVariableBulkNamingResult result = buildIoVariableBulkNamingPlan(
      {first, second, reserved}, {0, 1}, metadata, options);

  expectEqual(result.updated, 2, "both keyed rows receive metadata updates");
  expectEqual(result.skippedExistingAliases, 1,
              "existing alias is counted as skipped");
  expectEqual(result.skippedInvalidRows, 0, "no invalid rows are skipped");

  const QStringList firstUpdate = result.metadataUpdates.value("3|0x6040|0x00");
  expectEqual(firstUpdate.value(0), "Existing_Alias",
              "kept row preserves alias");
  expectEqual(firstUpdate.value(1), "axis, plc, output, uint",
              "kept row merges requested and inferred tags");
  expectEqual(firstUpdate.value(2), "keep", "kept row preserves note");

  const QStringList secondUpdate =
      result.metadataUpdates.value("3|0x6041|0x00");
  expectEqual(secondUpdate.value(0), "Line_1_S3_6041_00_Drive_Statusword",
              "new row gets prefixed generated alias");
  expectEqual(secondUpdate.value(1), "plc, axis, output, uint",
              "new row receives requested and inferred tags");
}

void testReplaceExistingAliasesAndSuffixDuplicates() {
  IoVariableTableRow first =
      rowFor(0, 3, "0x6040", "Axis_Controlword", "Axis Controlword");
  IoVariableTableRow second =
      rowFor(1, 3, "0x6041", QString(), "Axis Controlword");
  first.meaning = "Controlword";
  second.meaning = "Statusword";

  QHash<QString, QStringList> metadata;
  metadata.insert("3|0x6040|0x00", {"Axis_Controlword", "old", QString()});

  IoVariableBulkNamingOptions options;
  options.includeAddress = false;
  options.keepExistingAliases = false;
  options.addDirectionTags = false;

  const IoVariableBulkNamingResult result =
      buildIoVariableBulkNamingPlan({first, second}, {0, 1}, metadata, options);

  expectEqual(result.updated, 2, "replacement updates both rows");
  expectEqual(result.skippedExistingAliases, 0,
              "replacement does not skip existing aliases");
  expectEqual(result.metadataUpdates.value("3|0x6040|0x00").value(0),
              "Axis_Controlword", "first replacement takes base alias");
  expectEqual(result.metadataUpdates.value("3|0x6041|0x00").value(0),
              "Axis_Controlword_2", "second replacement gets unique suffix");
}

void testInvalidRowsAreSkipped() {
  IoVariableTableRow invalid = rowFor(0, 3, "0x6040", QString(), "Signal");
  invalid.positionValid = false;

  const IoVariableBulkNamingResult result = buildIoVariableBulkNamingPlan(
      {invalid}, {0, 99}, {}, IoVariableBulkNamingOptions{});

  expectEqual(result.updated, 0, "invalid rows are not updated");
  expectEqual(result.skippedInvalidRows, 2, "missing and unkeyed rows skipped");
  expectTrue(result.metadataUpdates.isEmpty(), "invalid plan has no updates");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testKeepExistingAliasAndMergeTags();
  testReplaceExistingAliasesAndSuffixDuplicates();
  testInvalidRowsAreSkipped();
  return 0;
}
