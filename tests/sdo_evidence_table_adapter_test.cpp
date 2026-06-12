#include "adapters/SdoEvidenceTableAdapter.h"

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

void testCandidateExtraction() {
  QTableWidget dictionary;
  dictionary.setColumnCount(8);
  dictionary.setRowCount(1);
  setCell(&dictionary, 0, 7, "0x0006");

  QTableWidget watch;
  watch.setColumnCount(5);
  watch.setRowCount(1);
  setCell(&watch, 0, 4, "0x0007");

  QTableWidget startup;
  startup.setColumnCount(4);
  startup.setRowCount(1);
  setCell(&startup, 0, 3, "0x0008");

  QTableWidget bookmark;
  bookmark.setColumnCount(9);
  bookmark.setRowCount(1);
  setCell(&bookmark, 0, 8, "0x0009");

  QTableWidget trail;
  trail.setColumnCount(8);
  trail.setRowCount(1);
  setCell(&trail, 0, 6, "0x000A");
  setCell(&trail, 0, 7, "0x000B");

  const SdoEvidenceCandidates candidates =
      sdoEvidenceCandidatesFromTables("0x0005",
                                      {.dictionaryTable = &dictionary,
                                       .watchTable = &watch,
                                       .startupTable = &startup,
                                       .bookmarkTable = &bookmark,
                                       .targetTrailTable = &trail},
                                      {.dictionaryRow = 0,
                                       .watchRow = 0,
                                       .startupRow = 0,
                                       .bookmarkRow = 0,
                                       .targetTrailRow = 0},
                                      {.readValue = "Read",
                                       .watch = "Watch",
                                       .dictionary = "OD",
                                       .startup = "Startup",
                                       .bookmark = "Bookmark",
                                       .targetTrailWrite = "Trail Write",
                                       .targetTrail = "Trail"});

  expectEqual(candidates.size(), 6, "candidate count");
  expectEqual(candidates.at(0).first, "Read", "read source");
  expectEqual(candidates.at(0).second, "0x0005", "read value");
  expectEqual(candidates.at(1).first, "Watch", "watch source");
  expectEqual(candidates.at(1).second, "0x0007", "watch value");
  expectEqual(candidates.at(2).first, "OD", "dictionary source");
  expectEqual(candidates.at(2).second, "0x0006", "dictionary value");
  expectEqual(candidates.at(5).first, "Trail Write", "trail write source");
  expectEqual(candidates.at(5).second, "0x000B", "trail write value");
}

void testTargetRowLookup() {
  QTableWidget dictionary;
  dictionary.setColumnCount(8);
  dictionary.setRowCount(1);
  setCell(&dictionary, 0, 1, "6040");
  setCell(&dictionary, 0, 2, "0");

  QTableWidget watch;
  watch.setColumnCount(5);
  watch.setRowCount(1);
  setCell(&watch, 0, 1, "3");
  setCell(&watch, 0, 2, "0x6040");
  setCell(&watch, 0, 3, "0x00");

  QTableWidget startup;
  startup.setColumnCount(4);
  startup.setRowCount(1);
  setCell(&startup, 0, 0, "3");
  setCell(&startup, 0, 1, "0x6040");
  setCell(&startup, 0, 2, "0x00");

  QTableWidget bookmark;
  bookmark.setColumnCount(9);
  bookmark.setRowCount(1);
  setCell(&bookmark, 0, 0, "3");
  setCell(&bookmark, 0, 2, "0x6040");
  setCell(&bookmark, 0, 3, "0x00");

  QTableWidget trail;
  trail.setColumnCount(8);
  trail.setRowCount(1);
  setCell(&trail, 0, 1, "3");
  setCell(&trail, 0, 2, "0x6040");
  setCell(&trail, 0, 3, "0x00");

  const SdoEvidenceTableRows rows =
      sdoEvidenceTableRowsForTarget({.dictionaryTable = &dictionary,
                                     .watchTable = &watch,
                                     .startupTable = &startup,
                                     .bookmarkTable = &bookmark,
                                     .targetTrailTable = &trail},
                                    {.position = 3,
                                     .index = "0x6040",
                                     .subIndex = "0x00",
                                     .dictionaryLoadedForPosition = true});

  expectEqual(rows.dictionaryRow, 0, "dictionary row lookup");
  expectEqual(rows.watchRow, 0, "watch row lookup");
  expectEqual(rows.startupRow, 0, "startup row lookup");
  expectEqual(rows.bookmarkRow, 0, "bookmark row lookup");
  expectEqual(rows.targetTrailRow, 0, "target trail row lookup");

  const SdoEvidenceTableRows missing = sdoEvidenceTableRowsForTarget(
      {.dictionaryTable = &dictionary}, {.position = 3,
                                         .index = "0x6040",
                                         .subIndex = "0x00",
                                         .dictionaryLoadedForPosition = false});
  expectEqual(missing.dictionaryRow, -1,
              "dictionary row waits for loaded position");
}

void testEmptyRowsAndTrailFallback() {
  QTableWidget trail;
  trail.setColumnCount(8);
  trail.setRowCount(1);
  setCell(&trail, 0, 6, "0x0010");
  setCell(&trail, 0, 7, " ");

  const SdoEvidenceCandidates candidates =
      sdoEvidenceCandidatesFromTables(QString(), {.targetTrailTable = &trail},
                                      {.dictionaryRow = -1,
                                       .watchRow = -1,
                                       .startupRow = -1,
                                       .bookmarkRow = -1,
                                       .targetTrailRow = 0},
                                      {.readValue = "Read",
                                       .watch = "Watch",
                                       .dictionary = "OD",
                                       .startup = "Startup",
                                       .bookmark = "Bookmark",
                                       .targetTrailWrite = "Trail Write",
                                       .targetTrail = "Trail"});

  expectEqual(candidates.size(), 1, "trail fallback count");
  expectEqual(candidates.at(0).first, "Trail", "trail fallback source");
  expectEqual(candidates.at(0).second, "0x0010", "trail fallback value");
}

void testLocalEvidenceExtraction() {
  QTableWidget watch;
  watch.setColumnCount(5);
  watch.setRowCount(2);
  setCell(&watch, 0, 1, "3");
  setCell(&watch, 0, 2, "0x6040");
  setCell(&watch, 0, 3, "0x00");
  setCell(&watch, 0, 4, "0x0006");
  setCell(&watch, 1, 1, "4");
  setCell(&watch, 1, 2, "0x6040");
  setCell(&watch, 1, 3, "0x00");
  setCell(&watch, 1, 4, "0x9999");

  QTableWidget dictionary;
  dictionary.setColumnCount(8);
  dictionary.setRowCount(1);
  setCell(&dictionary, 0, 1, "6040");
  setCell(&dictionary, 0, 2, "0");
  setCell(&dictionary, 0, 7, "0x0007");

  QTableWidget startup;
  startup.setColumnCount(4);
  startup.setRowCount(1);
  setCell(&startup, 0, 0, "3");
  setCell(&startup, 0, 1, "0x6040");
  setCell(&startup, 0, 2, "0x00");
  setCell(&startup, 0, 3, "0x0008");

  const QVector<SdoEvidenceItem> items = sdoLocalEvidenceItemsFromTables(
      3, "0x6040", "0x00", "0x0005", "0x1111", true, true,
      {.dictionaryTable = &dictionary,
       .watchTable = &watch,
       .startupTable = &startup},
      {.read = "Read",
       .watchPrefix = "Watch",
       .dictionary = "OD",
       .startupPrefix = "Startup",
       .bookmarkPrefix = "Bookmark"});

  expectEqual(items.size(), 4, "local evidence count");
  expectEqual(items.at(0).source, "Read", "local read source");
  expectEqual(items.at(1).source, "Watch #1", "local watch source");
  expectEqual(items.at(2).source, "OD", "local OD source");
  expectEqual(items.at(3).source, "Startup #1", "local startup source");
}

void testWriteEvidenceExtraction() {
  QVector<SdoEvidenceItem> items = sdoWriteEvidenceItemsFromValues(
      QString(), "0x0006", "0x0007", QString(), "0x0008", "0x0009", QString(),
      {.read = "Read",
       .dictionary = "OD",
       .watch = "Watch",
       .startup = "Startup",
       .bookmark = "Bookmark",
       .targetTrail = "Trail"});

  expectEqual(items.size(), 4, "write evidence fallback count");
  expectEqual(items.at(0).source, "OD", "write evidence dictionary fallback");
  expectEqual(items.at(3).source, "Trail", "write evidence trail source");

  items = sdoWriteEvidenceItemsFromValues(
      "0x0005", "0x0006", QString(), QString(), QString(), "0x0009", "0x000A",
      {.read = "Read",
       .dictionary = "OD",
       .watch = "Watch",
       .startup = "Startup",
       .bookmark = "Bookmark",
       .targetTrail = "Trail"});

  expectEqual(items.size(), 2, "write evidence read priority count");
  expectEqual(items.at(0).source, "Read", "write evidence read source");
  expectEqual(items.at(1).value, "0x000A", "write evidence trail write value");
}

void testWriteDeltaAvailability() {
  QTableWidget dictionary;
  dictionary.setColumnCount(9);
  dictionary.setRowCount(1);
  setCell(&dictionary, 0, 7, QString());
  setCell(&dictionary, 0, 8, "Complete");

  QTableWidget trail;
  trail.setColumnCount(8);
  trail.setRowCount(1);
  setCell(&trail, 0, 6, "0x0009");
  setCell(&trail, 0, 7, QString());

  expectEqual(sdoWriteDeltaReviewEvidenceAvailable("0x0005", {},
                                                   {.dictionaryRow = -1,
                                                    .watchRow = -1,
                                                    .startupRow = -1,
                                                    .bookmarkRow = -1,
                                                    .targetTrailRow = -1}),
              true, "read value makes review available");
  expectEqual(sdoWriteDeltaReviewEvidenceAvailable(
                  QString(), {.dictionaryTable = &dictionary},
                  {.dictionaryRow = 0,
                   .watchRow = -1,
                   .startupRow = -1,
                   .bookmarkRow = -1,
                   .targetTrailRow = -1}),
              true, "dictionary status makes review available");
  expectEqual(sdoWriteDeltaReviewEvidenceAvailable(QString(),
                                                   {.targetTrailTable = &trail},
                                                   {.dictionaryRow = -1,
                                                    .watchRow = -1,
                                                    .startupRow = -1,
                                                    .bookmarkRow = -1,
                                                    .targetTrailRow = 0}),
              true, "target trail value makes review available");
  expectEqual(sdoWriteDeltaReviewEvidenceAvailable(QString(), {},
                                                   {.dictionaryRow = -1,
                                                    .watchRow = -1,
                                                    .startupRow = -1,
                                                    .bookmarkRow = -1,
                                                    .targetTrailRow = -1}),
              false, "empty evidence is unavailable");
}

void testTargetTrailRowParsing() {
  QTableWidget trail;
  trail.setColumnCount(9);
  trail.setRowCount(2);
  setCell(&trail, 0, 0, "12:00:00");
  setCell(&trail, 0, 1, "3");
  setCell(&trail, 0, 2, "6040");
  setCell(&trail, 0, 3, "0");
  setCell(&trail, 0, 4, "uint16");
  setCell(&trail, 0, 5, "Object Bookmark");
  setCell(&trail, 0, 6, "0x0006");
  setCell(&trail, 0, 7, "0x000F");
  setCell(&trail, 0, 8, "Controlword evidence with detail text");
  setCell(&trail, 1, 1, "4");
  setCell(&trail, 1, 2, "0x6060");
  setCell(&trail, 1, 3, "0x00");
  setCell(&trail, 1, 4, "int8");
  setCell(&trail, 1, 5, "Manual fields");
  setCell(&trail, 1, 6, "8");
  setCell(&trail, 1, 7, " ");
  setCell(&trail, 1, 8, "Mode");

  const SdoTargetTrailRow row = sdoTargetTrailRowFromTable(&trail, 0);
  expectTrue(sdoTargetTrailRowHasTarget(row), "trail row has target");
  expectEqual(row.position, 3, "trail row position");
  expectEqual(row.index, "0x6040", "trail row normalized index");
  expectEqual(row.subIndex, "0x00", "trail row normalized subindex");
  expectEqual(sdoTargetTrailRowStartupValue(row), "0x000F",
              "trail write value has startup priority");
  expectEqual(sdoTargetTrailStartupValueFromTable(&trail, 1), "8",
              "trail startup fallback uses read value");
  expectEqual(sdoTargetTrailRowKeyFromTable(&trail, 0),
              sdoTargetTrailRowKey(3, "0x6040", "0x00", "uint16",
                                   "Object Bookmark",
                                   "Controlword evidence with detail text"),
              "trail row key matches value constructor");
  expectEqual(sdoTargetTrailKeysFromTable(&trail).size(), 2,
              "trail key set size");

  const SdoTargetTrailRow missing = sdoTargetTrailRowFromTable(&trail, 9);
  expectFalse(sdoTargetTrailRowHasTarget(missing),
              "missing trail row has no target");
}

void testObjectBookmarkRowParsing() {
  QTableWidget bookmark;
  bookmark.setColumnCount(10);
  bookmark.setRowCount(2);
  setCell(&bookmark, 0, 0, "3");
  setCell(&bookmark, 0, 1, "Drive A");
  setCell(&bookmark, 0, 2, "6041");
  setCell(&bookmark, 0, 3, "0");
  setCell(&bookmark, 0, 4, "ro");
  setCell(&bookmark, 0, 5, "uint16");
  setCell(&bookmark, 0, 6, "16");
  setCell(&bookmark, 0, 7, "Statusword");
  setCell(&bookmark, 0, 8, "0x1234");
  setCell(&bookmark, 0, 9, "Object Dictionary");
  setCell(&bookmark, 1, 0, "4");
  setCell(&bookmark, 1, 2, "0x6040");
  setCell(&bookmark, 1, 3, "0x00");
  setCell(&bookmark, 1, 4, "读写");

  const SdoObjectBookmarkRow row = sdoObjectBookmarkRowFromTable(&bookmark, 0);
  expectTrue(sdoObjectBookmarkRowHasTarget(row), "bookmark row has target");
  expectEqual(row.positionText, "3", "bookmark position text");
  expectEqual(row.position, 3, "bookmark position");
  expectEqual(row.index, "0x6041", "bookmark normalized index");
  expectEqual(row.subIndex, "0x00", "bookmark normalized subindex");
  expectEqual(row.lastValue, "0x1234", "bookmark last value");
  expectTrue(sdoObjectAccessIsReadOnly(row.access, "只读"),
             "english ro access is read-only");
  expectFalse(sdoObjectAccessIsReadOnly(
                  sdoObjectBookmarkRowFromTable(&bookmark, 1).access, "只读"),
              "rw bookmark access is writable");
  expectTrue(sdoObjectAccessIsReadOnly("只读", "只读"),
             "localized read-only access is read-only");

  const SdoObjectBookmarkRow missing =
      sdoObjectBookmarkRowFromTable(&bookmark, -1);
  expectFalse(sdoObjectBookmarkRowHasTarget(missing),
              "missing bookmark row has no target");
}

} // namespace

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  testCandidateExtraction();
  testTargetRowLookup();
  testEmptyRowsAndTrailFallback();
  testLocalEvidenceExtraction();
  testWriteEvidenceExtraction();
  testWriteDeltaAvailability();
  testTargetTrailRowParsing();
  testObjectBookmarkRowParsing();
  return 0;
}
