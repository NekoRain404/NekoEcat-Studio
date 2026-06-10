#include "SlaveEvidenceTableAdapter.h"

#include "EvidenceStatusModel.h"
#include "StudioTableHelpers.h"
#include "StudioTextHelpers.h"

#include <QStringList>
#include <QTableWidget>

namespace {

int parsedPosition(const QString &text) {
  bool ok = false;
  const int position = text.toInt(&ok);
  return ok ? position : -1;
}

void applyWatchEvidence(SlaveEvidenceInput *input, QTableWidget *table) {
  if (!input || !table) {
    return;
  }

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

void applyStartupEvidence(SlaveEvidenceInput *input, QTableWidget *table) {
  if (!input || !table) {
    return;
  }

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

void applyProcessEvidence(SlaveEvidenceInput *input, QTableWidget *table) {
  if (!input || !table) {
    return;
  }

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

bool isMissingText(const QString &text) {
  const QString normalized = text.toLower();
  return normalized.contains("missing") || normalized.contains("缺失");
}

constexpr int kSlaveEvidenceMatrixRouteRole = Qt::UserRole + 24;

} // namespace

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

bool slaveEvidenceMatrixSearchMatches(QTableWidget *table, int row,
                                      const QString &needle) {
  if (needle.trimmed().isEmpty()) {
    return true;
  }
  if (!table || row < 0 || row >= table->rowCount()) {
    return false;
  }

  for (int column = 0; column < table->columnCount(); ++column) {
    if (tableText(table, row, column).contains(needle, Qt::CaseInsensitive)) {
      return true;
    }
  }
  return false;
}

SlaveEvidenceMatrixFilterStats
filterSlaveEvidenceMatrixTable(QTableWidget *table, const QString &scope,
                               const QString &needle) {
  SlaveEvidenceMatrixFilterStats stats;
  if (!table) {
    return stats;
  }

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

SlaveEvidenceMatrixPriorityCounts
slaveEvidenceMatrixPriorityCounts(QTableWidget *table) {
  SlaveEvidenceMatrixPriorityCounts counts;
  if (!table) {
    return counts;
  }

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

void setSlaveEvidenceMatrixRouteTarget(QTableWidget *table, int row,
                                       SlaveEvidenceRouteTarget target) {
  if (!table || row < 0 || row >= table->rowCount()) {
    return;
  }

  if (auto *item = table->item(row, kSlaveEvidenceMatrixPositionColumn)) {
    item->setData(kSlaveEvidenceMatrixRouteRole, static_cast<int>(target));
  }
}

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

int firstSlaveEvidenceRowForPosition(QTableWidget *table, int position,
                                     int positionColumn) {
  if (!table || position < 0 || positionColumn < 0 ||
      positionColumn >= table->columnCount()) {
    return -1;
  }

  for (int row = 0; row < table->rowCount(); ++row) {
    if (parsedPosition(tableText(table, row, positionColumn)) == position) {
      return row;
    }
  }
  return -1;
}

int firstSlaveEvidenceStartupDiffRow(QTableWidget *startupTable, int position) {
  if (!startupTable || position < 0) {
    return -1;
  }

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

int firstSlaveEvidenceProcessIssueRow(QTableWidget *processTable,
                                      int position) {
  if (!processTable || position < 0) {
    return -1;
  }

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

int firstSlaveEvidenceDriveWatchRow(QTableWidget *watchTable, int position) {
  if (!watchTable || position < 0) {
    return -1;
  }

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
