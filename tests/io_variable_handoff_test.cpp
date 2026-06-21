// IoVariableHandoffTest — Tests for IoVariableHandoffModel
//
// Test coverage:
//   - Suggested alias generation from symbol and meaning
//   - Handoff issues and duplicate detection
//   - Comment generation with quality labels
//   - Declaration blocks and CSV export

#include "models/IoVariableModel.h"

#include <QCoreApplication>
#include <QSet>

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

void expectEqual(qsizetype actual, qsizetype expected, const QString &message) {
  if (actual != expected) {
    fail(QString("%1: expected %2, got %3")
             .arg(message)
             .arg(static_cast<qlonglong>(expected))
             .arg(static_cast<qlonglong>(actual)));
  }
}

void expectTrue(bool condition, const QString &message) {
  if (!condition) {
    fail(message);
  }
}

IoVariableTableRow baseRow() {
  IoVariableTableRow row;
  row.row = 0;
  row.position = 3;
  row.positionValid = true;
  row.direction = "Rx Output";
  row.symbol = "Drive.Controlword";
  row.index = "0x6040";
  row.subIndex = "0x00";
  row.bits = "16";
  row.pdo = "0x1600";
  row.source = "Process | uint16";
  row.meaning = "Controlword";
  row.tags = "axis, output";
  return row;
}

// Test alias generation from symbol with prefix and address options
void testSuggestedAlias() {
  IoVariableTableRow row = baseRow();
  expectEqual(suggestedIoVariableAlias(row, QString(), true),
              "S3_6040_00_Drive_Controlword",
              "address-prefixed alias from symbol");
  expectEqual(suggestedIoVariableAlias(row, "Line 1", true),
              "Line_1_S3_6040_00_Drive_Controlword",
              "prefixed alias from symbol");

  row.symbol.clear();
  row.meaning = "Control word & enable";
  expectEqual(suggestedIoVariableAlias(row, QString(), false),
              "Control_word_and_enable", "alias from meaning");
}

// Test handoff issues: missing alias, auto-name, duplicate detection
void testIssuesAndDuplicates() {
  IoVariableTableRow ready = baseRow();
  ready.alias = "AxisX_Controlword";
  const QVector<IoVariableHandoffIssue> readyIssues =
      ioVariableHandoffIssues(ready, nullptr);
  expectTrue(readyIssues.isEmpty(), "ready row has no handoff issue");
  expectEqual(ioVariableHandoffName(ready).symbol, "AxisX_Controlword",
              "handoff symbol uses alias");
  expectEqual(ioVariableHandoffPlcDirection(ready), "Output",
              "handoff PLC direction");
  expectEqual(ioVariableHandoffPlcType(ready), "UINT", "handoff PLC type");

  IoVariableTableRow missing = baseRow();
  const QVector<IoVariableHandoffIssue> missingIssues =
      ioVariableHandoffIssues(missing, nullptr);
  expectTrue(ioVariableHandoffHasIssue(missingIssues,
                                       IoVariableHandoffIssue::MissingAlias),
             "missing alias issue detected");
  expectTrue(ioVariableHandoffIssueKeys(missingIssues).contains("missingAlias"),
             "missing alias key emitted");

  IoVariableTableRow generated = baseRow();
  generated.alias = suggestedIoVariableAlias(generated, QString(), true);
  const QVector<IoVariableHandoffIssue> generatedIssues =
      ioVariableHandoffIssues(generated, nullptr);
  expectTrue(ioVariableHandoffHasIssue(generatedIssues,
                                       IoVariableHandoffIssue::AutoName),
             "auto-name issue detected");

  QVector<IoVariableTableRow> rows = {ready, ready};
  const QSet<QString> duplicates = duplicateIoVariableHandoffSymbols(rows);
  expectTrue(duplicates.contains("axisx_controlword"),
             "duplicate symbol detected");
  const QVector<IoVariableHandoffIssue> duplicateIssues =
      ioVariableHandoffIssues(ready, &duplicates);
  expectTrue(ioVariableHandoffHasIssue(duplicateIssues,
                                       IoVariableHandoffIssue::DuplicateSymbol),
             "duplicate symbol issue detected");
}

// Test comment generation with address, direction, PDO, quality labels
// Verify comment generation includes address, direction, PDO, and quality labels
void testComment() {
  IoVariableTableRow row = baseRow();
  row.meaning = "Control (* unsafe *) word";
  const QString comment =
      ioVariableHandoffComment(row, {"Missing Alias", "No Tags"});
  expectTrue(comment.contains("#3 0x6040:0x00"),
             "comment includes object address");
  expectTrue(comment.contains("Output"), "comment includes direction");
  expectTrue(comment.contains("0x1600"), "comment includes PDO");
  expectTrue(comment.contains("Quality: Missing Alias | No Tags"),
             "comment includes quality labels");
  expectTrue(!comment.contains("(*") && !comment.contains("*)"),
             "comment strips nested ST comment delimiters");
}

// Test declaration block and CSV row generation for PLC export
// Verify declaration block generation and CSV row export with unique symbols
void testDeclarationsAndCsv() {
  IoVariableTableRow first = baseRow();
  first.alias = "Axis_Controlword";
  first.raw = "0x0006";
  first.decoded = "Switch on";
  first.watch = "0x000F";
  first.startup = "Match";
  first.map = "Mapped";
  first.changed = "Yes";
  first.plcQuality = "Ready";
  first.note = "commissioning";

  IoVariableTableRow second = first;
  second.row = 1;
  second.alias = "Axis_Controlword";
  second.index = "0x6041";
  second.meaning = "Statusword";
  second.plcQuality = "Duplicate Symbol";

  QSet<QString> usedSymbols;
  expectEqual(ioVariableHandoffUniqueSymbol(first, &usedSymbols),
              "Axis_Controlword", "first symbol keeps alias");
  expectEqual(ioVariableHandoffUniqueSymbol(second, &usedSymbols),
              "Axis_Controlword_2", "duplicate symbol receives suffix");

  const QString block = ioVariableHandoffDeclarationBlock(
      {first, second}, {{}, {"Duplicate Symbol"}});
  expectTrue(block.startsWith("VAR_GLOBAL"), "declaration block starts");
  expectTrue(block.contains("Axis_Controlword : UINT;"),
             "declaration block contains first symbol");
  expectTrue(block.contains("Axis_Controlword_2 : UINT;"),
             "declaration block contains suffixed duplicate");
  expectTrue(block.contains("Quality: Duplicate Symbol"),
             "declaration block keeps quality label");
  expectTrue(block.endsWith("END_VAR"), "declaration block ends");

  expectEqual(ioVariableHandoffCsvHeaders().size(), 23, "PLC CSV header count");
  usedSymbols.clear();
  const IoVariableHandoffCsvRow csv =
      ioVariableHandoffCsvRow(first, &usedSymbols, "2026-06-09T12:00:00");
  expectEqual(csv.values.size(), 23, "PLC CSV value count");
  expectEqual(csv.values.value(0), "Axis_Controlword", "PLC CSV symbol");
  expectEqual(csv.values.value(1), "Output", "PLC CSV direction");
  expectEqual(csv.values.value(2), "UINT", "PLC CSV type");
  expectEqual(csv.values.value(20), "#3 0x6040:0x00", "PLC CSV address");
  expectEqual(csv.values.value(21), "Drive.Controlword",
              "PLC CSV default name");
  expectEqual(csv.values.value(22), "2026-06-09T12:00:00",
              "PLC CSV exported time");
}

} // namespace

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testSuggestedAlias();
  testIssuesAndDuplicates();
  testComment();
  testDeclarationsAndCsv();
  return 0;
}
