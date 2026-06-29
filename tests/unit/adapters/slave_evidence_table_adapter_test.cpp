// SlaveEvidenceTableAdapterTest — Tests for Slave Evidence Table Adapter
//
// Test coverage:
//   - Loaded table evidence application (watch, startup, process data)
//   - Drive status word, mode display, error code, and fault detection
//   - Evidence row lookup helpers
//   - Matrix filtering by scope (missing watch, ready, search)
//   - Matrix priority counting (P0-P3)
//   - Matrix route target storage and retrieval
#include "adapters/SlaveEvidenceTableAdapter.h"

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

void expectRoute(SlaveEvidenceRouteTarget actual,
                 SlaveEvidenceRouteTarget expected, const QString &message) {
  if (actual != expected) {
    fail(message);
  }
}

void setCell(QTableWidget *table, int row, int column, const QString &text) {
  table->setItem(row, column, new QTableWidgetItem(text));
}

void initTable(QTableWidget *table, int rows, int columns) {
  table->setColumnCount(columns);
  table->setRowCount(rows);
}

// Apply loaded watch/startup/process table evidence for a slave position
// Test applying loaded table evidence into SlaveEvidenceInput
void testAppliesLoadedTableEvidence() {
  QTableWidget watch;
  initTable(&watch, 4, 6);
  setCell(&watch, 0, kSlaveEvidenceWatchPositionColumn, "2");
  setCell(&watch, 0, kSlaveEvidenceWatchIndexColumn, "6041");
  setCell(&watch, 0, kSlaveEvidenceWatchValueColumn, "0x0027");
  setCell(&watch, 0, kSlaveEvidenceWatchDecodedColumn, "operation enabled");
  setCell(&watch, 1, kSlaveEvidenceWatchPositionColumn, "2");
  setCell(&watch, 1, kSlaveEvidenceWatchIndexColumn, "6061");
  setCell(&watch, 1, kSlaveEvidenceWatchValueColumn, "8");
  setCell(&watch, 1, kSlaveEvidenceWatchDecodedColumn, "Cyclic sync position");
  setCell(&watch, 2, kSlaveEvidenceWatchPositionColumn, "2");
  setCell(&watch, 2, kSlaveEvidenceWatchIndexColumn, "603f");
  setCell(&watch, 2, kSlaveEvidenceWatchValueColumn, "0x2310");
  setCell(&watch, 2, kSlaveEvidenceWatchDecodedColumn, "Error code 0x2310");
  setCell(&watch, 3, kSlaveEvidenceWatchPositionColumn, "4");
  setCell(&watch, 3, kSlaveEvidenceWatchValueColumn, "ignored");

  QTableWidget startup;
  initTable(&startup, 3, 9);
  setCell(&startup, 0, kSlaveEvidenceStartupPositionColumn, "2");
  setCell(&startup, 0, kSlaveEvidenceStartupDeltaColumn, "diff");
  setCell(&startup, 1, kSlaveEvidenceStartupPositionColumn, "2");
  setCell(&startup, 1, kSlaveEvidenceStartupDeltaColumn, "match");
  setCell(&startup, 2, kSlaveEvidenceStartupPositionColumn, "4");
  setCell(&startup, 2, kSlaveEvidenceStartupDeltaColumn, "diff");

  QTableWidget process;
  initTable(&process, 2, 14);
  setCell(&process, 0, kSlaveEvidenceProcessPositionColumn, "2");
  setCell(&process, 0, kSlaveEvidenceProcessMapColumn, "Missing process map");
  setCell(&process, 1, kSlaveEvidenceProcessPositionColumn, "3");
  setCell(&process, 1, kSlaveEvidenceProcessMapColumn, "Missing process map");

  SlaveEvidenceInput input;
  input.position = 2;
  const SlaveEvidenceLoadedPositions positions = {2, 5, 2, 10, 2, 8};
  const SlaveEvidenceLoadedTables tables = {&watch, &startup, &process};
  applyLoadedSlaveEvidence(&input, positions, tables);

  expectEqual(input.identityRows, 5, "identity rows");
  expectEqual(input.odRows, 10, "OD rows");
  expectEqual(input.pdoRows, 8, "PDO rows");
  expectEqual(input.watchRows, 3, "watch row count");
  expectEqual(input.watchValueRows, 3, "watch value count");
  expectEqual(input.startupRows, 2, "startup row count");
  expectEqual(input.startupDiffs, 1, "startup diff count");
  expectEqual(input.processRows, 1, "process row count");
  expectEqual(input.mapIssues, 1, "PDO map issue count");
  expectEqual(input.driveStatusword, "operation enabled", "statusword");
  expectEqual(input.driveModeDisplay, "Cyclic sync position", "mode display");
  expectEqual(input.driveErrorCode, "Error code 0x2310", "error code");
  expectTrue(input.driveFault, "drive fault evidence is detected");

  expectEqual(firstSlaveEvidenceDriveWatchRow(&watch, 2), 0,
              "first CiA 402 Watch row");
  expectEqual(firstSlaveEvidenceRowForPosition(
                  &watch, 4, kSlaveEvidenceWatchPositionColumn),
              3, "generic Watch position lookup");
  expectEqual(firstSlaveEvidenceStartupDiffRow(&startup, 2), 0,
              "first Startup diff row");
  expectEqual(firstSlaveEvidenceProcessIssueRow(&process, 2), 0,
              "first process issue row");
}

// Filter matrix by scope and count priorities per level
// Test matrix filtering by scope and priority counting
void testMatrixFilteringAndPriorityCounts() {
  QTableWidget matrix;
  initTable(&matrix, 4, 12);
  setCell(&matrix, 0, kSlaveEvidenceMatrixPriorityColumn, "P0 Fault");
  setCell(&matrix, 0, kSlaveEvidenceMatrixPositionColumn, "1");
  setCell(&matrix, 0, kSlaveEvidenceMatrixNameColumn, "Drive Fault");
  setCell(&matrix, 0, kSlaveEvidenceMatrixReadinessColumn, "80% (5/6)");
  setCell(&matrix, 0, kSlaveEvidenceMatrixOdColumn, "10");
  setCell(&matrix, 0, kSlaveEvidenceMatrixPdoColumn, "8");
  setCell(&matrix, 0, kSlaveEvidenceMatrixWatchColumn, "2/2 values");
  setCell(&matrix, 0, kSlaveEvidenceMatrixStartupColumn, "1 row(s), 1 diff(s)");
  setCell(&matrix, 0, kSlaveEvidenceMatrixProcessColumn,
          "3 row(s), 0 issue(s)");
  setCell(&matrix, 0, kSlaveEvidenceMatrixRiskColumn, "Startup diff 1");
  setCell(&matrix, 0, kSlaveEvidenceMatrixNextColumn, "Review Startup");

  setCell(&matrix, 1, kSlaveEvidenceMatrixPriorityColumn, "P1 Risk");
  setCell(&matrix, 1, kSlaveEvidenceMatrixPositionColumn, "2");
  setCell(&matrix, 1, kSlaveEvidenceMatrixNameColumn, "Missing Watch");
  setCell(&matrix, 1, kSlaveEvidenceMatrixReadinessColumn, "60% (4/6)");
  setCell(&matrix, 1, kSlaveEvidenceMatrixOdColumn, "10");
  setCell(&matrix, 1, kSlaveEvidenceMatrixPdoColumn, "8");
  setCell(&matrix, 1, kSlaveEvidenceMatrixWatchColumn, "Missing");
  setCell(&matrix, 1, kSlaveEvidenceMatrixStartupColumn, "No rows");
  setCell(&matrix, 1, kSlaveEvidenceMatrixProcessColumn, "Missing");
  setCell(&matrix, 1, kSlaveEvidenceMatrixRiskColumn, "Watch missing");
  setCell(&matrix, 1, kSlaveEvidenceMatrixNextColumn, "Add Watch");

  setCell(&matrix, 2, kSlaveEvidenceMatrixPriorityColumn, "P2 Action");
  setCell(&matrix, 2, kSlaveEvidenceMatrixPositionColumn, "3");
  setCell(&matrix, 2, kSlaveEvidenceMatrixNameColumn, "Load PDO");
  setCell(&matrix, 2, kSlaveEvidenceMatrixReadinessColumn, "50% (3/6)");
  setCell(&matrix, 2, kSlaveEvidenceMatrixOdColumn, "10");
  setCell(&matrix, 2, kSlaveEvidenceMatrixPdoColumn, "Missing");
  setCell(&matrix, 2, kSlaveEvidenceMatrixWatchColumn, "Missing");
  setCell(&matrix, 2, kSlaveEvidenceMatrixStartupColumn, "No rows");
  setCell(&matrix, 2, kSlaveEvidenceMatrixProcessColumn, "Missing");
  setCell(&matrix, 2, kSlaveEvidenceMatrixRiskColumn, "None");
  setCell(&matrix, 2, kSlaveEvidenceMatrixNextColumn, "Load PDO");

  setCell(&matrix, 3, kSlaveEvidenceMatrixPriorityColumn, "P3 Ready");
  setCell(&matrix, 3, kSlaveEvidenceMatrixPositionColumn, "4");
  setCell(&matrix, 3, kSlaveEvidenceMatrixNameColumn, "Ready Axis");
  setCell(&matrix, 3, kSlaveEvidenceMatrixReadinessColumn, "100% (6/6)");
  setCell(&matrix, 3, kSlaveEvidenceMatrixOdColumn, "10");
  setCell(&matrix, 3, kSlaveEvidenceMatrixPdoColumn, "8");
  setCell(&matrix, 3, kSlaveEvidenceMatrixWatchColumn, "2/2 values");
  setCell(&matrix, 3, kSlaveEvidenceMatrixStartupColumn, "No rows");
  setCell(&matrix, 3, kSlaveEvidenceMatrixProcessColumn,
          "3 row(s), 0 issue(s)");
  setCell(&matrix, 3, kSlaveEvidenceMatrixRiskColumn, "None");
  setCell(&matrix, 3, kSlaveEvidenceMatrixNextColumn, "Ready");

  int p0 = 0;
  int p1 = 0;
  int p2 = 0;
  int p3 = 0;
  countSlaveEvidenceMatrixPriorities(&matrix, &p0, &p1, &p2, &p3);
  expectEqual(p0, 1, "P0 count");
  expectEqual(p1, 1, "P1 count");
  expectEqual(p2, 1, "P2 count");
  expectEqual(p3, 1, "P3 count");
  const SlaveEvidenceMatrixPriorityCounts priorityCounts =
      slaveEvidenceMatrixPriorityCounts(&matrix);
  expectEqual(priorityCounts.p0, 1, "structured P0 count");
  expectEqual(priorityCounts.p1, 1, "structured P1 count");
  expectEqual(priorityCounts.p2, 1, "structured P2 count");
  expectEqual(priorityCounts.p3, 1, "structured P3 count");

  const SlaveEvidenceMatrixFilterStats missingWatch =
      filterSlaveEvidenceMatrixTable(
          &matrix, QString::fromLatin1(kSlaveEvidenceScopeMissingWatch),
          QString());
  expectEqual(missingWatch.visible, 2, "missing Watch visible rows");
  expectTrue(!matrix.isRowHidden(1), "P1 missing Watch row visible");
  expectTrue(!matrix.isRowHidden(2), "P2 missing Watch row visible");
  expectTrue(matrix.isRowHidden(0), "P0 row hidden by missing Watch filter");

  const SlaveEvidenceMatrixFilterStats ready = filterSlaveEvidenceMatrixTable(
      &matrix, QString::fromLatin1(kSlaveEvidenceScopeReady), QString());
  expectEqual(ready.visible, 1, "ready visible rows");
  expectTrue(!ready.hasVisibleIssue, "ready filter has no visible issue");

  const SlaveEvidenceMatrixFilterStats search = filterSlaveEvidenceMatrixTable(
      &matrix, QString::fromLatin1(kSlaveEvidenceScopeAll), "Axis");
  expectEqual(search.visible, 1, "search visible rows");
  expectTrue(!matrix.isRowHidden(3), "search keeps matching row visible");
}

// Store and retrieve route targets for matrix rows
// Test matrix route target storage and retrieval per row
void testMatrixRouteTargetStorage() {
  QTableWidget matrix;
  initTable(&matrix, 2, 12);
  setCell(&matrix, 0, kSlaveEvidenceMatrixPositionColumn, "1");
  setCell(&matrix, 1, kSlaveEvidenceMatrixPositionColumn, "2");

  setSlaveEvidenceMatrixRouteTarget(&matrix, 0,
                                    SlaveEvidenceRouteTarget::ObjectDictionary);
  setSlaveEvidenceMatrixRouteTarget(&matrix, 1,
                                    SlaveEvidenceRouteTarget::Process);

  expectRoute(slaveEvidenceMatrixRouteTargetForRow(&matrix, 0),
              SlaveEvidenceRouteTarget::ObjectDictionary,
              "first matrix row stores OD route");
  expectRoute(slaveEvidenceMatrixRouteTargetForRow(&matrix, 1),
              SlaveEvidenceRouteTarget::Process,
              "second matrix row stores process route");
  expectRoute(slaveEvidenceMatrixRouteTargetForRow(&matrix, -1),
              SlaveEvidenceRouteTarget::Overview,
              "invalid matrix row falls back to Overview");
}

} // namespace

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  testAppliesLoadedTableEvidence();
  testMatrixFilteringAndPriorityCounts();
  testMatrixRouteTargetStorage();
  return 0;
}
