// Consistency checks and commissioning workflow management.

#include "MainWindow.h"

#include "models/Cia402DriveModel.h"
#include "models/CommissioningWorkflowModel.h"
#include "detail/CommissioningWorkflowStepDetail.h"
#include "adapters/CommissioningWorkflowTableAdapter.h"
#include "detail/CommissioningWorkflowDetail.h"
#include "detail/ConsistencyDetail.h"
#include "models/ConsistencyEvidenceRouteModel.h"
#include "models/ConsistencyGateModel.h"
#include "adapters/ConsistencyTableAdapter.h"
#include "detail/DiagnosticsEventDetail.h"
#include "models/EvidenceStatusModel.h"
#include "detail/FreeRunEntryDetail.h"
#include "detail/HostHealthDetail.h"
#include "models/IoVariableBulkNamingModel.h"
#include "detail/IoVariableDetail.h"
#include "models/IoVariableFilterModel.h"
#include "models/IoVariableHandoffModel.h"
#include "models/NextBestActionModel.h"
#include "detail/NextBestActionDetail.h"
#include "detail/ObjectBookmarkDetail.h"
#include "detail/PdoMapDetail.h"
#include "models/ProcessDataRowModel.h"
#include "adapters/ProcessDataTableAdapter.h"
#include "adapters/SdoDictionaryTableAdapter.h"
#include "models/SdoEvidenceModel.h"
#include "adapters/SdoEvidenceTableAdapter.h"
#include "detail/SdoHistoryRowDetail.h"
#include "models/SdoTargetPanelRouteModel.h"
#include "detail/SdoTargetTrailDetail.h"
#include "detail/SelectedDriveSummaryDetail.h"
#include "detail/SelectedSlaveEvidenceSummaryDetail.h"
#include "models/SessionBriefModel.h"
#include "adapters/SessionBriefTableAdapter.h"
#include "detail/SessionBriefDetail.h"
#include "models/SlaveEvidenceModel.h"
#include "adapters/SlaveEvidenceTableAdapter.h"
#include "detail/SlaveEvidenceDetail.h"
#include "detail/StartupSdoRowDetail.h"
#include "detail/StateMachineRowDetail.h"
#include "adapters/StateMachineTableAdapter.h"
#include "models/StateRecommendationModel.h"
#include "utils/Documentation.h"
#include "utils/TableHelpers.h"
#include "utils/TextHelpers.h"
#include "utils/UiHelpers.h"
#include "models/TopologyBaselineModel.h"
#include "models/TopologyChangeModel.h"
#include "detail/WatchRowDetail.h"
#include "models/WatchStartupModel.h"
#include "adapters/WatchStartupTableAdapter.h"
#include "detail/WatchStartupDetail.h"
#include "detail/WorkspaceBoundaryDetail.h"
#include "adapters/WorkspaceTabBadgeTableAdapter.h"
#include "detail/WorkspaceTabBadgeDetail.h"
#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <QAbstractItemView>
#include <QAction>
#include <QApplication>
#include <QBrush>
#include <QCheckBox>
#include <QClipboard>
#include <QColor>
#include <QComboBox>
#include <QCoreApplication>
#include <QDateTime>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDir>
#include <QDockWidget>
#include <QEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHash>
#include <QHeaderView>
#include <QItemSelectionModel>
    // Serialize/deserialize JSON data
#include <QJsonArray>
#include <QJsonDocument>
    // Serialize/deserialize JSON data
#include <QJsonObject>
#include <QKeyEvent>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QScrollBar>
#include <QSettings>
#include <QShortcut>
#include <QSignalBlocker>
#include <QSize>
#include <QSizePolicy>
#include <QSplitter>
#include <QStatusBar>
#include <QStyle>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextBrowser>
#include <QTextStream>
    // Schedule deferred or periodic execution
#include <QTimer>
#include <QToolBar>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QXmlStreamReader>



// — Rebuild the slave evidence overview matrix from all loaded evidence sources
void MainWindow::updateSlaveEvidenceMatrix() {
  if (!slaveEvidence_->slaveEvidenceMatrixTable) {
    return;
  }

  int previousPosition = selectedPosition();
  if (slaveEvidence_->slaveEvidenceMatrixTable->currentRow() >= 0) {
    bool ok = false;
    const int rowPosition = tableText(slaveEvidence_->slaveEvidenceMatrixTable,
                                      slaveEvidence_->slaveEvidenceMatrixTable->currentRow(),
                                      kSlaveEvidenceMatrixPositionColumn)
                                .toInt(&ok);
    if (ok) {
      previousPosition = rowPosition;
    }
  }

  const SlaveEvidenceUiTexts uiTexts = slaveEvidenceUiTexts();

  QVector<SlaveEvidenceInput> evidenceInputs;
  evidenceInputs.reserve(slaves_.size());
  const QStringList topologyIssues = topologyBaselineIssues();
  const SlaveEvidenceLoadedPositions loadedPositions = {
      loadedSlaveInfoPosition_, identityTable_ ? identityTable_->rowCount() : 0,
      loadedSdoPosition_,       sdo_->sdoTable ? sdo_->sdoTable->rowCount() : 0,
      loadedPdoPosition_,       sdo_->pdoTable ? sdo_->pdoTable->rowCount() : 0,
  };
  const SlaveEvidenceLoadedTables loadedTables = {
      watch_->watchTable,
      startupSdoTable_,
      freeRunWidgets_->freeRunEntryTable,
  };

  for (const auto &slave : slaves_) {
    SlaveEvidenceInput input;
    input.position = slave.position;
    input.name = slave.name.trimmed();
    input.state = slave.state.trimmed();
    applyLoadedSlaveEvidence(&input, loadedPositions, loadedTables);
    input.topologyIssue = !topologyIssues.isEmpty();
    evidenceInputs.append(input);
  }

  const SlaveEvidenceMatrix evidenceMatrix =
      buildSlaveEvidenceMatrix(evidenceInputs);

  QList<QStringList> rows;
  for (const auto &row : evidenceMatrix.rows) {
    rows.append(slaveEvidenceUiRow(row, uiTexts).cells);
  }

  setTableRows(slaveEvidence_->slaveEvidenceMatrixTable, slaveEvidenceMatrixHeaders(uiTexts),
               rows);

  const QColor okColor("#22c55e");
  const QColor actionColor("#f59e0b");
  const QColor warningColor("#ef4444");
  const QColor infoColor("#60a5fa");
  int restoreRow = -1;
  for (int rowIndex = 0; rowIndex < slaveEvidence_->slaveEvidenceMatrixTable->rowCount();
       ++rowIndex) {
    bool ok = false;
    const int rowPosition = tableText(slaveEvidence_->slaveEvidenceMatrixTable, rowIndex,
                                      kSlaveEvidenceMatrixPositionColumn)
                                .toInt(&ok);
    if (ok && rowPosition == previousPosition) {
      restoreRow = rowIndex;
    }
    const auto evidence = evidenceMatrix.rows.value(rowIndex);
    const SlaveEvidenceUiRow uiRow = slaveEvidenceUiRow(evidence, uiTexts);
    // Route to the appropriate evidence workspace
    setSlaveEvidenceMatrixRouteTarget(slaveEvidence_->slaveEvidenceMatrixTable, rowIndex,
                                      // Route to the appropriate evidence workspace
                                      slaveEvidenceRouteTarget(evidence));
    const QColor readinessColor =
        evidence.risks.isEmpty() && evidence.readiness >= evidence.maxReadiness
            ? okColor
            : (!evidence.risks.isEmpty() ? warningColor : actionColor);
    const QString tooltip = uiRow.detailLines.join('\n');
    for (int column = 0; column < slaveEvidence_->slaveEvidenceMatrixTable->columnCount();
         ++column) {
      if (auto *item = slaveEvidence_->slaveEvidenceMatrixTable->item(rowIndex, column)) {
        item->setToolTip(tooltip);
      }
    }
    if (auto *item = slaveEvidence_->slaveEvidenceMatrixTable->item(
            rowIndex, kSlaveEvidenceMatrixPriorityColumn)) {
      const int priorityRank = slaveEvidencePriorityRank(evidence.priority);
      item->setForeground(priorityRank <= 1   ? warningColor
                          : priorityRank == 2 ? actionColor
                                              : okColor);
    }
    if (auto *item = slaveEvidence_->slaveEvidenceMatrixTable->item(
            rowIndex, kSlaveEvidenceMatrixStateColumn)) {
      const QString current = item->text().toUpper();
      item->setForeground(
          (current == "OP" || current.startsWith("OP ")) ? okColor
          : (current.contains("SAFEOP") || current.contains("PREOP"))
              ? actionColor
              : infoColor);
    }
    if (auto *item = slaveEvidence_->slaveEvidenceMatrixTable->item(
            rowIndex, kSlaveEvidenceMatrixReadinessColumn)) {
      item->setForeground(readinessColor);
    }
    for (int column : {5, 6, 7, 9}) {
      if (auto *item = slaveEvidence_->slaveEvidenceMatrixTable->item(rowIndex, column)) {
        const QString text = item->text().toLower();
        item->setForeground((text.contains("missing") || text.contains("缺失"))
                                ? warningColor
                                : okColor);
      }
    }
    if (auto *item = slaveEvidence_->slaveEvidenceMatrixTable->item(
            rowIndex, kSlaveEvidenceMatrixStartupColumn)) {
      item->setForeground(evidence.startupDiffs > 0 ? warningColor : okColor);
    }
    if (auto *item = slaveEvidence_->slaveEvidenceMatrixTable->item(
            rowIndex, kSlaveEvidenceMatrixRiskColumn)) {
      item->setForeground(evidence.risks.isEmpty() ? okColor : warningColor);
    }
    if (auto *item = slaveEvidence_->slaveEvidenceMatrixTable->item(
            rowIndex, kSlaveEvidenceMatrixNextColumn)) {
      item->setForeground(evidence.nextAction == SlaveEvidenceNextAction::Ready
                              ? okColor
                              : actionColor);
    }
  }

  if (restoreRow >= 0) {
    slaveEvidence_->slaveEvidenceMatrixTable->setCurrentCell(
        restoreRow, kSlaveEvidenceMatrixPositionColumn);
  }
  fitTableColumnsToViewport(slaveEvidence_->slaveEvidenceMatrixTable,
                            kSlaveEvidenceMatrixNextColumn);

  if (slaveEvidence_->slaveEvidenceMatrixSummaryLabel) {
    const QString summary =
        slaves_.isEmpty()
            ? uiText("No slaves in current scan", "当前扫描没有从站")
            : uiText("Priority queue | slaves %1 | ready %2 | action %3 | "
                     "risk %4 | evidence gaps %5",
                     "优先级队列 | 从站 %1 | 就绪 %2 | 待执行 %3 | 风险 %4 | "
                     "证据缺口 %5")
                  .arg(slaves_.size())
                  .arg(evidenceMatrix.readyRows)
                  .arg(evidenceMatrix.actionRows)
                  .arg(evidenceMatrix.riskRows)
                  .arg(evidenceMatrix.evidenceGaps);
    slaveEvidence_->slaveEvidenceMatrixSummaryLabel->setText(summary);
    slaveEvidence_->slaveEvidenceMatrixSummaryLabel->setToolTip(uiText(
        "This matrix is read-only and uses already loaded UI evidence only. "
        "Double-click or Alt+Enter selects the slave locally and routes to the "
        "best loaded evidence table without loading OD/PDO/ESI data from the "
        "bus.",
        "该矩阵只读且只使用已加载界面证据。双击或 Alt+Enter 会本地选择从站并"
        "路由到最相关的已加载证据表，不会从总线加载 OD/PDO/ESI 数据。"));
    slaveEvidence_->slaveEvidenceMatrixSummaryLabel->setProperty(
        "severity", evidenceMatrix.riskRows > 0
                        ? "warning"
                        : (evidenceMatrix.actionRows > 0 ? "action" : "ok"));
    repolish(slaveEvidence_->slaveEvidenceMatrixSummaryLabel); // force QSS re-evaluation after property change
  }
  filterSlaveEvidenceMatrix();
}


// — Filter slave evidence matrix
void MainWindow::filterSlaveEvidenceMatrix() {
  if (!slaveEvidence_->slaveEvidenceMatrixTable) {
    return;
  }

  const QString needle = slaveEvidence_->slaveEvidenceMatrixFilter
                             ? slaveEvidence_->slaveEvidenceMatrixFilter->text().trimmed()
                             : QString();
  const QString scope =
      slaveEvidence_->slaveEvidenceMatrixScopeFilter
          ? slaveEvidence_->slaveEvidenceMatrixScopeFilter->currentData().toString()
          : QString::fromLatin1(kSlaveEvidenceScopeAll);
  const SlaveEvidenceMatrixFilterStats stats =
      filterSlaveEvidenceMatrixTable(slaveEvidence_->slaveEvidenceMatrixTable, scope, needle);

  if (slaveEvidence_->slaveEvidenceMatrixReviewButton) {
    slaveEvidence_->slaveEvidenceMatrixReviewButton->setEnabled(stats.hasVisibleIssue);
  }
  if (slaveEvidence_->slaveEvidenceMatrixReviewNextButton) {
    slaveEvidence_->slaveEvidenceMatrixReviewNextButton->setEnabled(stats.hasVisibleIssue);
  }
  if (slaveEvidence_->slaveEvidenceMatrixCopyButton) {
    const int current = slaveEvidence_->slaveEvidenceMatrixTable->currentRow();
    slaveEvidence_->slaveEvidenceMatrixCopyButton->setEnabled(
        current >= 0 && current < slaveEvidence_->slaveEvidenceMatrixTable->rowCount() &&
        !slaveEvidence_->slaveEvidenceMatrixTable->isRowHidden(current));
  }

  if (slaveEvidence_->slaveEvidenceMatrixSummaryLabel) {
    const QString scopeLabel =
        slaveEvidence_->slaveEvidenceMatrixScopeFilter
            ? slaveEvidence_->slaveEvidenceMatrixScopeFilter->currentText()
            : uiText("All", "全部");
    slaveEvidence_->slaveEvidenceMatrixSummaryLabel->setToolTip(
        uiText("Visible matrix rows: %1/%2\nScope: %3\nPriority: P0 %4 | P1 "
               "%5 | P2 %6 | P3 %7\nRisk: %8 | Action: %9 | Ready: "
               "%10\nFiltering is local only and does not read the bus.",
               "可见矩阵行：%1/%2\n范围：%3\n优先级：P0 %4 | P1 %5 | P2 %6 | "
               "P3 %7\n风险：%8 | 待执行：%9 | 就绪：%10\n过滤仅在本地完成，不"
               "读取总线。")
            .arg(stats.visible)
            .arg(slaveEvidence_->slaveEvidenceMatrixTable->rowCount())
            .arg(scopeLabel)
            .arg(stats.p0)
            .arg(stats.p1)
            .arg(stats.p2)
            .arg(stats.p3)
            .arg(stats.risk)
            .arg(stats.action)
            .arg(stats.ready));
  }
  updateSlaveEvidenceMatrixTriageButtons();
  updateTabBadges();
  updateWorkspaceBoundary();
}


// — Update slave evidence matrix triage buttons
void MainWindow::updateSlaveEvidenceMatrixTriageButtons() {
  if (slaveEvidence_->slaveEvidenceMatrixTriageButtons.isEmpty()) {
    return;
  }
  const SlaveEvidenceMatrixPriorityCounts priorityCounts =
      slaveEvidenceMatrixPriorityCounts(slaveEvidence_->slaveEvidenceMatrixTable);
  const QHash<QString, int> counts = {
      {QString::fromLatin1(kSlaveEvidenceScopePriorityP0), priorityCounts.p0},
      {QString::fromLatin1(kSlaveEvidenceScopePriorityP1), priorityCounts.p1},
      {QString::fromLatin1(kSlaveEvidenceScopePriorityP2), priorityCounts.p2},
      {QString::fromLatin1(kSlaveEvidenceScopePriorityP3), priorityCounts.p3},
  };
  const QHash<QString, QString> labels = {
      {QString::fromLatin1(kSlaveEvidenceScopePriorityP0), uiText("P0", "P0")},
      {QString::fromLatin1(kSlaveEvidenceScopePriorityP1), uiText("P1", "P1")},
      {QString::fromLatin1(kSlaveEvidenceScopePriorityP2), uiText("P2", "P2")},
      {QString::fromLatin1(kSlaveEvidenceScopePriorityP3), uiText("P3", "P3")},
  };
  const QString currentScope =
      slaveEvidence_->slaveEvidenceMatrixScopeFilter
          ? slaveEvidence_->slaveEvidenceMatrixScopeFilter->currentData().toString()
          : QString();
  for (auto *button : slaveEvidence_->slaveEvidenceMatrixTriageButtons) {
    if (!button) {
      continue;
    }
    const QString scope = button->property("scope").toString();
    const int count = counts.value(scope, 0);
    button->setText(
        QString("%1 %2").arg(labels.value(scope, scope)).arg(count));
    button->setCheckable(true);
    button->setChecked(scope == currentScope);
    button->setEnabled(slaveEvidence_->slaveEvidenceMatrixTable &&
                       slaveEvidence_->slaveEvidenceMatrixTable->rowCount() > 0);
    button->setToolTip(
        uiText("Show %1 matrix rows. Count: %2. This is local UI filtering "
               "only and does not read the bus.",
               "显示 %1 矩阵行。数量：%2。这只是本地界面过滤，不读取总线。")
            .arg(labels.value(scope, scope))
            .arg(count));
  }
}


// — Review first slave evidence matrix issue
void MainWindow::reviewFirstSlaveEvidenceMatrixIssue() {
  if (!slaveEvidence_->slaveEvidenceMatrixTable) {
    return;
  }
  filterSlaveEvidenceMatrix();
  int fallbackRow = -1;
  for (int row = 0; row < slaveEvidence_->slaveEvidenceMatrixTable->rowCount(); ++row) {
    if (slaveEvidence_->slaveEvidenceMatrixTable->isRowHidden(row)) {
      continue;
    }
    const SlaveEvidenceMatrixRowState state =
        slaveEvidenceMatrixRowState(slaveEvidence_->slaveEvidenceMatrixTable, row);
    if (!state.reviewIssue) {
      continue;
    }
    if (state.hasRisk) {
      openSlaveEvidenceMatrixRow(row);
      return;
    }
    if (fallbackRow < 0) {
      fallbackRow = row;
    }
  }
  if (fallbackRow >= 0) {
    openSlaveEvidenceMatrixRow(fallbackRow);
    return;
  }
  statusBar()->showMessage(uiText("No visible matrix issue to review.",
                                  "当前没有可见的矩阵问题可审阅。"),
                           3000);
}


// — Review next slave evidence matrix issue
void MainWindow::reviewNextSlaveEvidenceMatrixIssue() {
  if (!slaveEvidence_->slaveEvidenceMatrixTable) {
    return;
  }
  filterSlaveEvidenceMatrix();
  const int rowCount = slaveEvidence_->slaveEvidenceMatrixTable->rowCount();
  if (rowCount <= 0) {
    statusBar()->showMessage(
        uiText("No matrix issue to review.", "当前没有矩阵问题可审阅。"), 3000);
    return;
  }

  auto isIssueRow = [this](int row) {
    if (row < 0 || row >= slaveEvidence_->slaveEvidenceMatrixTable->rowCount() ||
        slaveEvidence_->slaveEvidenceMatrixTable->isRowHidden(row)) {
      return false;
    }
    return slaveEvidenceMatrixRowState(slaveEvidence_->slaveEvidenceMatrixTable, row)
        .reviewIssue;
  };

  const int current = slaveEvidence_->slaveEvidenceMatrixTable->currentRow();
  for (int offset = 1; offset <= rowCount; ++offset) {
    const int row = (qMax(0, current) + offset) % rowCount;
    if (isIssueRow(row)) {
      openSlaveEvidenceMatrixRow(row);
      return;
    }
  }

  statusBar()->showMessage(uiText("No visible matrix issue to review.",
                                  "当前没有可见的矩阵问题可审阅。"),
                           3000);
}


// — Check whether copy slave evidence matrix row digest
bool MainWindow::copySlaveEvidenceMatrixRowDigest(int row) {
  if (!slaveEvidence_->slaveEvidenceMatrixTable || row < 0 ||
      row >= slaveEvidence_->slaveEvidenceMatrixTable->rowCount() ||
      slaveEvidence_->slaveEvidenceMatrixTable->isRowHidden(row)) {
    statusBar()->showMessage(
        uiText("Select a visible slave evidence matrix row to copy.",
               "请选择一条可见的从站证据矩阵行再复制。"),
        3000);
    return false;
  }

  QStringList lines;
  lines << uiText("NekoEcat Studio Slave Evidence Matrix Row",
                  "NekoEcat Studio 从站证据矩阵本行");
  lines << QString("%1: %2").arg(uiText("Master", "主站"), activeMasterName());
  if (slaveEvidence_->slaveEvidenceMatrixScopeFilter) {
    lines << QString("%1: %2").arg(
        uiText("Matrix Scope", "矩阵范围"),
        slaveEvidence_->slaveEvidenceMatrixScopeFilter->currentText());
  }
  if (slaveEvidence_->slaveEvidenceMatrixFilter &&
      !slaveEvidence_->slaveEvidenceMatrixFilter->text().trimmed().isEmpty()) {
    lines << QString("%1: %2").arg(
        uiText("Search", "搜索"), slaveEvidence_->slaveEvidenceMatrixFilter->text().trimmed());
  }
  if (slaveEvidence_->slaveEvidenceMatrixSummaryLabel &&
      !slaveEvidence_->slaveEvidenceMatrixSummaryLabel->text().trimmed().isEmpty()) {
    lines << QString("%1: %2").arg(
        uiText("Matrix Summary", "矩阵摘要"),
        slaveEvidence_->slaveEvidenceMatrixSummaryLabel->text().trimmed());
  }

  lines << QString();
  lines << uiText("Row Evidence", "本行证据");
  for (int column = 0; column < slaveEvidence_->slaveEvidenceMatrixTable->columnCount();
       ++column) {
    const auto *header =
        slaveEvidence_->slaveEvidenceMatrixTable->horizontalHeaderItem(column);
    const QString field =
        header ? header->text().trimmed() : QString::number(column + 1);
    const QString value = tableText(slaveEvidence_->slaveEvidenceMatrixTable, row, column);
    lines << QString("- %1: %2")
                 .arg(field, value.isEmpty() ? uiText("Empty", "空") : value);
  }

  QString details;
  for (int column = 0; column < slaveEvidence_->slaveEvidenceMatrixTable->columnCount();
       ++column) {
    if (auto *item = slaveEvidence_->slaveEvidenceMatrixTable->item(row, column)) {
      details = item->toolTip().trimmed();
      if (!details.isEmpty()) {
        break;
      }
    }
  }
  if (!details.isEmpty()) {
    lines << QString();
    lines << uiText("Detailed Evidence", "详细证据");
    const QStringList detailLines = details.split('\n', Qt::SkipEmptyParts);
    for (const QString &detail : detailLines) {
      lines << QString("- %1").arg(detail.trimmed());
    }
  }

  lines << QString();
  lines << uiText("Local Action: Open Matrix Evidence routes this slave to the "
                  "best already loaded evidence table; Review First/Next walks "
                  "visible matrix issues.",
                  "本地动作：打开矩阵证据会把该从站路由到最相关的已加载证据表；"
                  "审阅首个/下个问题会遍历可见矩阵问题。");
  lines << uiText("Boundary: clipboard copy only; no bus read, no OD/PDO/ESI "
                  "load, no SDO write, no state change, no Free Run change, "
                  "and no Host Health.",
                  "边界：只复制到剪贴板；不读取总线、不加载 OD/PDO/ESI、不写 "
                  "SDO、不切换状态、不改变 Free Run，也不运行 Host Health。");

  QApplication::clipboard()->setText(lines.join('\n')); // copy to system clipboard
  updateDiagnostics("Info", "Slave Evidence Matrix",
                    uiText("Copied slave evidence matrix row #%1 to clipboard",
                           "已复制从站证据矩阵第 %1 行到剪贴板")
                        .arg(row + 1));
  statusBar()->showMessage(
      uiText("Copied slave evidence matrix row. No bus access was requested.",
             "已复制从站证据矩阵行；未请求总线访问。"),
      3000);
  return true;
}


// — Route to the detailed evidence workspace for the selected slave matrix row
void MainWindow::openSlaveEvidenceMatrixRow(int row) {
  if (!slaveEvidence_->slaveEvidenceMatrixTable || row < 0 ||
      row >= slaveEvidence_->slaveEvidenceMatrixTable->rowCount()) {
    statusBar()->showMessage(uiText("Select a slave evidence row first.",
                                    "请先选择一条从站证据矩阵行。"),
                             3000);
    return;
  }

  bool ok = false;
  const int position = tableText(slaveEvidence_->slaveEvidenceMatrixTable, row,
                                 kSlaveEvidenceMatrixPositionColumn)
                           .toInt(&ok);
  if (!ok || position < 0) {
    return;
  }

  if (!selectSlaveForLocalEvidence(position)) {
    statusBar()->showMessage(
        uiText("Slave is not present in the local topology tree.",
               "本地拓扑树中没有该从站。"),
        3000);
    return;
  }

  selectAndFocusTableRow(slaveEvidence_->slaveEvidenceMatrixTable, row,
                         kSlaveEvidenceMatrixPositionColumn);

  QString target = uiText("Overview matrix row", "总览矩阵行");
  bool routed = false;
  switch (
      // Dispatch to the evidence workspace matching the route target
      slaveEvidenceMatrixRouteTargetForRow(slaveEvidence_->slaveEvidenceMatrixTable, row)) {
  // Route to the appropriate evidence workspace
  case SlaveEvidenceRouteTarget::ObjectDictionary:
    activateObjectDictionaryPaneFor(sdo_->sdoTable);
    if (loadedSdoPosition_ == position && sdo_->sdoTable &&
        sdo_->sdoTable->rowCount() > 0) {
      selectAndFocusTableRow(
          sdo_->sdoTable, sdo_->sdoTable->currentRow() >= 0 ? sdo_->sdoTable->currentRow() : 0,
          0);
    }
    target = uiText("Object Dictionary evidence", "对象字典证据");
    routed = true;
    break;
  // Route to the appropriate evidence workspace
  case SlaveEvidenceRouteTarget::PdoMap:
    activateWorkspaceTab(pdoMapTabIndex_);
    if (loadedPdoPosition_ == position && sdo_->pdoTable &&
        sdo_->pdoTable->rowCount() > 0) {
      selectAndFocusTableRow(
          sdo_->pdoTable, sdo_->pdoTable->currentRow() >= 0 ? sdo_->pdoTable->currentRow() : 0,
          0);
    }
    target = uiText("PDO Map evidence", "PDO 映射证据");
    routed = true;
    break;
  // Route to the appropriate evidence workspace
  case SlaveEvidenceRouteTarget::Watch: {
    const int watchRow = firstSlaveEvidenceDriveWatchRow(watch_->watchTable, position);
    activateWorkspaceTab(watchTabIndex_);
    if (watch_->watchScopeFilter) {
      const int scope = watch_->watchScopeFilter->findData("all");
      if (scope >= 0) {
        watch_->watchScopeFilter->setCurrentIndex(scope);
      }
    }
    if (watch_->watchChangedOnly) {
      watch_->watchChangedOnly->setChecked(false);
    }
    filterWatchTable();
    if (watchRow >= 0) {
      selectAndFocusTableRow(watch_->watchTable, watchRow,
                             kSlaveEvidenceWatchPositionColumn);
    } else {
      const int anyWatchRow = firstSlaveEvidenceRowForPosition(
          watch_->watchTable, position, kSlaveEvidenceWatchPositionColumn);
      if (anyWatchRow >= 0) {
        selectAndFocusTableRow(watch_->watchTable, anyWatchRow,
                               kSlaveEvidenceWatchPositionColumn);
      }
    }
    target = uiText("Watch evidence", "Watch 证据");
    routed = true;
    break;
  }
  // Route to the appropriate evidence workspace
  case SlaveEvidenceRouteTarget::Startup: {
    updateStartupSdoWatchEvidence();
    const int startupRow =
        firstSlaveEvidenceStartupDiffRow(startupSdoTable_, position);
    activateWorkspaceTab(startupSdoTabIndex_);
    if (watch_->startupWatchDiffsOnly) {
      watch_->startupWatchDiffsOnly->setChecked(false);
    }
    filterStartupSdoTable();
    if (startupRow >= 0) {
      selectAndFocusTableRow(startupSdoTable_, startupRow,
                             kSlaveEvidenceStartupPositionColumn);
    }
    updateStartupSdoControls();
    target = uiText("Startup SDO evidence", "Startup SDO 证据");
    routed = true;
    break;
  }
  // Route to the appropriate evidence workspace
  case SlaveEvidenceRouteTarget::Process: {
    const int processRow =
        firstSlaveEvidenceProcessIssueRow(freeRunWidgets_->freeRunEntryTable, position);
    activateWorkspaceTab(freeRunTabIndex_);
    filterFreeRunEntryTable();
    if (processRow >= 0) {
      selectAndFocusTableRow(freeRunWidgets_->freeRunEntryTable, processRow,
                             kSlaveEvidenceProcessPositionColumn);
    } else {
      const int anyProcessRow = firstSlaveEvidenceRowForPosition(
          freeRunWidgets_->freeRunEntryTable, position, kSlaveEvidenceProcessPositionColumn);
      if (anyProcessRow >= 0) {
        selectAndFocusTableRow(freeRunWidgets_->freeRunEntryTable, anyProcessRow,
                               kSlaveEvidenceProcessPositionColumn);
      }
    }
    target = uiText("Free Run process evidence", "Free Run 过程证据");
    routed = true;
    break;
  }
  // Route to the appropriate evidence workspace
  case SlaveEvidenceRouteTarget::StateMachine: {
    updateStateMachineView();
    activateWorkspaceTab(stateMachineTabIndex_);
    const int stateRow =
        firstSlaveEvidenceRowForPosition(stateMachine_->stateMachineTable, position, 0);
    if (stateRow >= 0) {
      selectAndFocusTableRow(stateMachine_->stateMachineTable, stateRow, 0);
    }
    target = uiText("State Machine risk evidence", "状态机风险证据");
    routed = true;
    break;
  }
  // Route to the appropriate evidence workspace
  case SlaveEvidenceRouteTarget::Overview:
    break;
  }

  if (!routed) {
    activateWorkspaceTab(overviewTabIndex_);
  }
  updateDiagnostics(
      "Info", "Slave Evidence Matrix",
      uiText("Opened %1 for slave #%2 from the read-only evidence matrix "
             "without bus access.",
             "已从只读证据矩阵为从站 #%2 打开 %1，未访问总线。")
          .arg(target)
          .arg(position));
  statusBar()->showMessage(
      uiText("Local slave evidence opened. No bus access was requested.",
             "已打开本地从站证据；未请求总线访问。"),
      3000);
}


// — Return the matrix table row index matching the given slave position
int MainWindow::slaveEvidenceMatrixRowForPosition(int position) const {
  return firstSlaveEvidenceRowForPosition(slaveEvidence_->slaveEvidenceMatrixTable, position,
                                          kSlaveEvidenceMatrixPositionColumn);
}

