// Populates and queries the slave evidence matrix QTableWidget.
#include "SlaveEvidenceTableAdapter.h"

#include "models/EvidenceStatusModel.h"
#include "utils/TableHelpers.h"
#include "utils/TextHelpers.h"

#include <QStringList>
#include <QTableWidget>

namespace {

// Safely parses a position string, returning -1 on invalid input.
int parsedPosition(const QString &text) {
  bool ok = false;
  const int position = text.toInt(&ok);
  return ok ? position : -1;
}

// Scans the watch table for rows matching this slave's position and extracts drive-specific status fields.
void applyWatchEvidence(SlaveEvidenceInput *input, QTableWidget *table) {
  if (!input || !table) {
    return;
  }

    // Iterate over collection
  for (int row = 0; row < table->rowCount(); ++row) {
    if (parsedPosition(
            tableText(table, row, kSlaveEvidenceWatchPositionColumn)) !=
        input->position) {
      continue;
    }

    ++input->watchRows;
    const QString value = tableText(table, row, kSlaveEvidenceWatchValueColumn);
    const QString decoded =
        tableText(table, row, kSlaveEvidenceWatchDecodedColumn);
    if (!value.isEmpty()) {
      ++input->watchValueRows;
    }

    const QString index = normalizeHexText(
        tableText(table, row, kSlaveEvidenceWatchIndexColumn), 4);
    if (index == "0x6041" && !decoded.isEmpty()) {
      input->driveStatusword = decoded;
    } else if (index == "0x6061" && !decoded.isEmpty()) {
      input->driveModeDisplay = decoded;
    } else if (index == "0x603f" && !decoded.isEmpty() && value != "0" &&
               value.toLower() != "0x0000") {
      input->driveErrorCode = decoded;
    }
  }

  QStringList driveFacts;
  if (!input->driveStatusword.isEmpty()) {
    driveFacts << input->driveStatusword;
  }
  if (!input->driveModeDisplay.isEmpty()) {
    driveFacts << QStringLiteral("mode %1").arg(input->driveModeDisplay);
  }
  if (!input->driveErrorCode.isEmpty()) {
    driveFacts << input->driveErrorCode;
  }
  input->driveEvidence = driveFacts.join(QStringLiteral(" | "));
  input->driveFault = hasDriveFaultEvidence(input->driveEvidence);
}

// Counts startup SDO rows and diffs for this slave's position to assess startup readiness.
void applyStartupEvidence(SlaveEvidenceInput *input, QTableWidget *table) {
  if (!input || !table) {
    return;
  }

    // Iterate over collection
  for (int row = 0; row < table->rowCount(); ++row) {
    if (parsedPosition(
            tableText(table, row, kSlaveEvidenceStartupPositionColumn)) !=
        input->position) {
      continue;
    }
    ++input->startupRows;
    if (hasStartupDiffEvidence(
            tableText(table, row, kSlaveEvidenceStartupDeltaColumn))) {
      ++input->startupDiffs;
    }
  }
}

// Counts process data rows and PDO map issues for this slave's position.
void applyProcessEvidence(SlaveEvidenceInput *input, QTableWidget *table) {
  if (!input || !table) {
    return;
  }

    // Iterate over collection
  for (int row = 0; row < table->rowCount(); ++row) {
    if (parsedPosition(
            tableText(table, row, kSlaveEvidenceProcessPositionColumn)) !=
        input->position) {
      continue;
    }
    ++input->processRows;
    if (hasPdoMapIssueEvidence(
            tableText(table, row, kSlaveEvidenceProcessMapColumn))) {
      ++input->mapIssues;
    }
  }
}

// Checks for localized "missing" keywords in evidence/risk text.
bool isMissingText(const QString &text) {
  const QString normalized = text.toLower();
  return normalized.contains("missing") || normalized.contains("缺失");
}

constexpr int kSlaveEvidenceMatrixRouteRole = Qt::UserRole + 24;

} // namespace

// Aggregates evidence from identity, OD, PDO, watch, startup, and process tables into the slave input model.
void applyLoadedSlaveEvidence(SlaveEvidenceInput *input,
                              const SlaveEvidenceLoadedPositions &positions,
                              const SlaveEvidenceLoadedTables &tables) {
  if (!input) {
    return;
  }

  if (positions.identityPosition == input->position) {
    input->identityRows = positions.identityRows;
  }
  if (positions.odPosition == input->position) {
    input->odRows = positions.odRows;
  }
  if (positions.pdoPosition == input->position) {
    input->pdoRows = positions.pdoRows;
  }

  applyWatchEvidence(input, tables.watchTable);
  applyStartupEvidence(input, tables.startupTable);
  applyProcessEvidence(input, tables.processTable);
}

// Derives boolean flags for priority, readiness, missing evidence, and risk from a matrix row.
SlaveEvidenceMatrixRowState slaveEvidenceMatrixRowState(QTableWidget *table,
                                                        int row) {
  SlaveEvidenceMatrixRowState state;
  if (!table || row < 0 || row >= table->rowCount()) {
    return state;
  }

  const QString priority =
      tableText(table, row, kSlaveEvidenceMatrixPriorityColumn).toLower();
  const QString readiness =
      tableText(table, row, kSlaveEvidenceMatrixReadinessColumn).toLower();
  const QString od =
      tableText(table, row, kSlaveEvidenceMatrixOdColumn).toLower();
  const QString pdo =
      tableText(table, row, kSlaveEvidenceMatrixPdoColumn).toLower();
  const QString watch =
      tableText(table, row, kSlaveEvidenceMatrixWatchColumn).toLower();
  const QString startup =
      tableText(table, row, kSlaveEvidenceMatrixStartupColumn).toLower();
  const QString process =
      tableText(table, row, kSlaveEvidenceMatrixProcessColumn).toLower();
  const QString risk =
      tableText(table, row, kSlaveEvidenceMatrixRiskColumn).trimmed().toLower();
  const QString next =
      tableText(table, row, kSlaveEvidenceMatrixNextColumn).trimmed().toLower();
  const bool nextReady = next == "ready" || next == "就绪";

  state.hasRisk = !risk.isEmpty() && risk != "none" && risk != "无";
  state.isReady = nextReady && !state.hasRisk && readiness.startsWith("100");
  state.isAction = !state.isReady && !state.hasRisk;
  state.reviewIssue = state.hasRisk || !nextReady;
  state.priorityP0 = priority.startsWith("p0");
  state.priorityP1 = priority.startsWith("p1");
  state.priorityP2 = priority.startsWith("p2");
  state.priorityP3 = priority.startsWith("p3");
  state.missingOd = isMissingText(od);
  state.missingPdo = isMissingText(pdo);
  state.missingWatch = isMissingText(watch);
  state.startupDiff = startup.contains("diff") || startup.contains("偏差") ||
                      risk.contains("startup") || risk.contains("启动");
  state.processMissing = isMissingText(process) ||
                         risk.contains("process evidence") ||
                         risk.contains("过程证据");
  return state;
}

// Tests whether a matrix row passes the active filter scope (priority, risk, missing evidence, etc.).
bool slaveEvidenceMatrixScopeMatches(const SlaveEvidenceMatrixRowState &state,
                                     const QString &scope) {
  if (scope == QString::fromLatin1(kSlaveEvidenceScopeRisk)) {
    return state.hasRisk;
  }
  if (scope == QString::fromLatin1(kSlaveEvidenceScopePriorityP0)) {
    return state.priorityP0;
  }
  if (scope == QString::fromLatin1(kSlaveEvidenceScopePriorityP1)) {
    return state.priorityP1;
  }
  if (scope == QString::fromLatin1(kSlaveEvidenceScopePriorityP2)) {
    return state.priorityP2;
  }
  if (scope == QString::fromLatin1(kSlaveEvidenceScopePriorityP3)) {
    return state.priorityP3;
  }
  if (scope == QString::fromLatin1(kSlaveEvidenceScopeAction)) {
    return state.isAction;
  }
  if (scope == QString::fromLatin1(kSlaveEvidenceScopeReady)) {
    return state.isReady;
  }
  if (scope == QString::fromLatin1(kSlaveEvidenceScopeMissingOd)) {
    return state.missingOd;
  }
  if (scope == QString::fromLatin1(kSlaveEvidenceScopeMissingPdo)) {
    return state.missingPdo;
  }
  if (scope == QString::fromLatin1(kSlaveEvidenceScopeMissingWatch)) {
    return state.missingWatch;
  }
  if (scope == QString::fromLatin1(kSlaveEvidenceScopeStartupDiff)) {
    return state.startupDiff;
  }
  if (scope == QString::fromLatin1(kSlaveEvidenceScopeProcessMissing)) {
    return state.processMissing;
  }
  return true;
}

// Case-insensitive full-row search across all matrix columns.
bool slaveEvidenceMatrixSearchMatches(QTableWidget *table, int row,
                                      const QString &needle) {
  if (needle.trimmed().isEmpty()) {
    return true;
  }
  if (!table || row < 0 || row >= table->rowCount()) {
    return false;
  }

    // Iterate over collection
  for (int column = 0; column < table->columnCount(); ++column) {
    if (tableText(table, row, column).contains(needle, Qt::CaseInsensitive)) {
      return true;
    }
  }
  return false;
}

// Applies scope + text filters to the matrix, toggles visibility, and collects aggregate stats.
SlaveEvidenceMatrixFilterStats
filterSlaveEvidenceMatrixTable(QTableWidget *table, const QString &scope,
                               const QString &needle) {
  SlaveEvidenceMatrixFilterStats stats;
  if (!table) {
    return stats;
  }

    // Iterate over collection
  for (int row = 0; row < table->rowCount(); ++row) {
    const SlaveEvidenceMatrixRowState state =
        slaveEvidenceMatrixRowState(table, row);
    if (state.priorityP0) {
      ++stats.p0;
    } else if (state.priorityP1) {
      ++stats.p1;
    } else if (state.priorityP2) {
      ++stats.p2;
    } else if (state.priorityP3) {
      ++stats.p3;
    }
    if (state.hasRisk) {
      ++stats.risk;
    } else if (state.isReady) {
      ++stats.ready;
    } else {
      ++stats.action;
    }

    const bool show = slaveEvidenceMatrixScopeMatches(state, scope) &&
                      slaveEvidenceMatrixSearchMatches(table, row, needle);
    table->setRowHidden(row, !show);
    if (show) {
      ++stats.visible;
      if (!state.isReady) {
        stats.hasVisibleIssue = true;
      }
    }
  }
  return stats;
}

// Outputs per-priority counts via out-parameters for badge and summary display.
void countSlaveEvidenceMatrixPriorities(QTableWidget *table, int *p0, int *p1,
                                        int *p2, int *p3) {
  if (p0) {
    *p0 = 0;
  }
  if (p1) {
    *p1 = 0;
  }
  if (p2) {
    *p2 = 0;
  }
  if (p3) {
    *p3 = 0;
  }
  if (!table) {
    return;
  }

  const SlaveEvidenceMatrixPriorityCounts counts =
      slaveEvidenceMatrixPriorityCounts(table);
  if (p0) {
    *p0 = counts.p0;
  }
  if (p1) {
    *p1 = counts.p1;
  }
  if (p2) {
    *p2 = counts.p2;
  }
  if (p3) {
    *p3 = counts.p3;
  }
}

// Counts rows in each priority tier (P0-P3) across the entire matrix.
SlaveEvidenceMatrixPriorityCounts
slaveEvidenceMatrixPriorityCounts(QTableWidget *table) {
  SlaveEvidenceMatrixPriorityCounts counts;
  if (!table) {
    return counts;
  }

    // Iterate over collection
  for (int row = 0; row < table->rowCount(); ++row) {
    const SlaveEvidenceMatrixRowState state =
        slaveEvidenceMatrixRowState(table, row);
    if (state.priorityP0) {
      ++counts.p0;
    } else if (state.priorityP1) {
      ++counts.p1;
    } else if (state.priorityP2) {
      ++counts.p2;
    } else if (state.priorityP3) {
      ++counts.p3;
    }
  }
  return counts;
}

// Stores the navigation route target as custom data on the position cell for click handling.
void setSlaveEvidenceMatrixRouteTarget(QTableWidget *table, int row,
                                       SlaveEvidenceRouteTarget target) {
  if (!table || row < 0 || row >= table->rowCount()) {
    return;
  }

  if (auto *item = table->item(row, kSlaveEvidenceMatrixPositionColumn)) {
    item->setData(kSlaveEvidenceMatrixRouteRole, static_cast<int>(target));
  }
}

// Retrieves the stored route target, falling back to Overview for missing or invalid data.
SlaveEvidenceRouteTarget
slaveEvidenceMatrixRouteTargetForRow(QTableWidget *table, int row) {
  if (!table || row < 0 || row >= table->rowCount()) {
    return SlaveEvidenceRouteTarget::Overview;
  }

  const auto *item = table->item(row, kSlaveEvidenceMatrixPositionColumn);
  if (!item) {
    return SlaveEvidenceRouteTarget::Overview;
  }
  bool ok = false;
  const int raw = item->data(kSlaveEvidenceMatrixRouteRole).toInt(&ok);
  if (!ok) {
    return SlaveEvidenceRouteTarget::Overview;
  }

  switch (static_cast<SlaveEvidenceRouteTarget>(raw)) {
  case SlaveEvidenceRouteTarget::Overview:
  case SlaveEvidenceRouteTarget::ObjectDictionary:
  case SlaveEvidenceRouteTarget::PdoMap:
  case SlaveEvidenceRouteTarget::Watch:
  case SlaveEvidenceRouteTarget::Startup:
  case SlaveEvidenceRouteTarget::Process:
  case SlaveEvidenceRouteTarget::StateMachine:
    return static_cast<SlaveEvidenceRouteTarget>(raw);
  }
  return SlaveEvidenceRouteTarget::Overview;
}

// Finds the first row matching a slave position in a given column, for jump-to navigation.
int firstSlaveEvidenceRowForPosition(QTableWidget *table, int position,
                                     int positionColumn) {
  if (!table || position < 0 || positionColumn < 0 ||
      positionColumn >= table->columnCount()) {
    return -1;
  }

    // Iterate over collection
  for (int row = 0; row < table->rowCount(); ++row) {
    if (parsedPosition(tableText(table, row, positionColumn)) == position) {
      return row;
    }
  }
  return -1;
}

// Locates the first startup diff row for a specific slave, for auto-scroll to the issue.
int firstSlaveEvidenceStartupDiffRow(QTableWidget *startupTable, int position) {
  if (!startupTable || position < 0) {
    return -1;
  }

    // Iterate over collection
  for (int row = 0; row < startupTable->rowCount(); ++row) {
    if (parsedPosition(tableText(startupTable, row,
                                 kSlaveEvidenceStartupPositionColumn)) !=
        position) {
      continue;
    }
    if (hasStartupDiffEvidence(
            tableText(startupTable, row, kSlaveEvidenceStartupDeltaColumn))) {
      return row;
    }
  }
  return -1;
}

// Locates the first process data issue row for a specific slave.
int firstSlaveEvidenceProcessIssueRow(QTableWidget *processTable,
                                      int position) {
  if (!processTable || position < 0) {
    return -1;
  }

    // Iterate over collection
  for (int row = 0; row < processTable->rowCount(); ++row) {
    if (parsedPosition(tableText(processTable, row,
                                 kSlaveEvidenceProcessPositionColumn)) !=
        position) {
      continue;
    }
    if (hasPdoMapIssueEvidence(
            tableText(processTable, row, kSlaveEvidenceProcessMapColumn))) {
      return row;
    }
  }
  return -1;
}

// Finds the first drive-related watch row (statusword, mode, error) for a specific slave.
int firstSlaveEvidenceDriveWatchRow(QTableWidget *watchTable, int position) {
  if (!watchTable || position < 0) {
    return -1;
  }

    // Iterate over collection
  for (int row = 0; row < watchTable->rowCount(); ++row) {
    if (parsedPosition(tableText(
            watchTable, row, kSlaveEvidenceWatchPositionColumn)) != position) {
      continue;
    }
    const QString index = normalizeHexText(
        tableText(watchTable, row, kSlaveEvidenceWatchIndexColumn), 4);
    if (index == "0x6041" || index == "0x603f" || index == "0x6061" ||
        index == "0x6040" || index == "0x6060") {
      return row;
    }
  }
  return -1;
}
