// Startup SDO management and delta comparison.

#include "MainWindow.h"

#include "models/Cia402DriveModel.h"
#include "models/CommissioningWorkflowModel.h"
#include "ui_state/CommissioningWorkflowStepDetailUiState.h"
#include "adapters/CommissioningWorkflowTableAdapter.h"
#include "ui_state/CommissioningWorkflowUiState.h"
#include "ui_state/ConsistencyDetailUiState.h"
#include "models/ConsistencyEvidenceRouteModel.h"
#include "models/ConsistencyGateModel.h"
#include "adapters/ConsistencyTableAdapter.h"
#include "ui_state/DiagnosticsEventUiState.h"
#include "models/EvidenceStatusModel.h"
#include "ui_state/FreeRunEntryDetailUiState.h"
#include "ui_state/HostHealthUiState.h"
#include "models/IoVariableBulkNamingModel.h"
#include "ui_state/IoVariableDetailUiState.h"
#include "models/IoVariableFilterModel.h"
#include "models/IoVariableHandoffModel.h"
#include "models/NextBestActionModel.h"
#include "ui_state/NextBestActionUiState.h"
#include "ui_state/ObjectBookmarkDetailUiState.h"
#include "ui_state/PdoMapDetailUiState.h"
#include "models/ProcessDataRowModel.h"
#include "adapters/ProcessDataTableAdapter.h"
#include "adapters/SdoDictionaryTableAdapter.h"
#include "models/SdoEvidenceModel.h"
#include "adapters/SdoEvidenceTableAdapter.h"
#include "ui_state/SdoHistoryRowDetailUiState.h"
#include "models/SdoTargetPanelRouteModel.h"
#include "ui_state/SdoTargetTrailDetailUiState.h"
#include "ui_state/SelectedDriveSummaryUiState.h"
#include "ui_state/SelectedSlaveEvidenceSummaryUiState.h"
#include "models/SessionBriefModel.h"
#include "adapters/SessionBriefTableAdapter.h"
#include "ui_state/SessionBriefUiState.h"
#include "models/SlaveEvidenceModel.h"
#include "adapters/SlaveEvidenceTableAdapter.h"
#include "ui_state/SlaveEvidenceUiState.h"
#include "ui_state/StartupSdoRowDetailUiState.h"
#include "ui_state/StateMachineRowDetailUiState.h"
#include "adapters/StateMachineTableAdapter.h"
#include "models/StateRecommendationModel.h"
#include "helpers/StudioDocumentation.h"
#include "helpers/StudioTableHelpers.h"
#include "helpers/StudioTextHelpers.h"
#include "helpers/StudioUiHelpers.h"
#include "models/TopologyBaselineModel.h"
#include "models/TopologyChangeModel.h"
#include "ui_state/WatchRowDetailUiState.h"
#include "models/WatchStartupModel.h"
#include "adapters/WatchStartupTableAdapter.h"
#include "ui_state/WatchStartupUiState.h"
#include "ui_state/WorkspaceBoundaryUiState.h"
#include "adapters/WorkspaceTabBadgeTableAdapter.h"
#include "ui_state/WorkspaceTabBadgeUiState.h"
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
#include <QJsonArray>
#include <QJsonDocument>
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
#include <QTimer>
#include <QToolBar>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QXmlStreamReader>


// — Create the Startup SDO table columns if not yet initialized
void MainWindow::ensureStartupSdoTable() {
  if (!startupSdoTable_) {
    return;
  }
  if (startupSdoTable_->columnCount() != 9) {
    startupSdoTable_->setColumnCount(9);
  }
  startupSdoTable_->setHorizontalHeaderLabels(
      {uiText("Slave", "从站"), uiText("Index", "索引"), uiText("Sub", "子项"),
       uiText("Value", "值"), uiText("Type", "类型"), uiText("Status", "状态"),
       uiText("Detail", "详情"), uiText("Watch Value", "Watch 值"),
       uiText("Watch Delta", "Watch 偏差")});
  for (int row = 0; row < startupSdoTable_->rowCount(); ++row) {
    for (int column = 0; column < startupSdoTable_->columnCount(); ++column) {
      if (!startupSdoTable_->item(row, column)) {
        startupSdoTable_->setItem(row, column, new QTableWidgetItem);
      }
    }
  }
}


// — Cross-reference Startup SDO rows with current Watch values
void MainWindow::updateStartupSdoWatchEvidence() {
  if (!startupSdoTable_) {
    return;
  }
  const QSignalBlocker tableBlocker(startupSdoTable_); // prevent recursive signal updates
  ensureStartupSdoTable();

  if (watch_->watchTable) {
    ensureWatchTable();
  }
  const QVector<WatchStartupStartupDelta> deltas =
      evaluateStartupWatchDeltas(watchStartupStartupRows(startupSdoTable_),
                                 watchStartupWatchRows(watch_->watchTable));
  const WatchStartupSummary summary = summarizeStartupWatchDeltas(deltas);

  for (int row = 0; row < startupSdoTable_->rowCount(); ++row) {
    auto *watchValueItem =
        startupSdoTable_->item(row, kWatchStartupStartupWatchValueColumn);
    if (!watchValueItem) {
      watchValueItem = new QTableWidgetItem;
      startupSdoTable_->setItem(row, kWatchStartupStartupWatchValueColumn,
                                watchValueItem);
    }
    auto *deltaItem =
        startupSdoTable_->item(row, kWatchStartupStartupWatchDeltaColumn);
    if (!deltaItem) {
      deltaItem = new QTableWidgetItem;
      startupSdoTable_->setItem(row, kWatchStartupStartupWatchDeltaColumn,
                                deltaItem);
    }
    clearWatchStartupDeltaCells(watchValueItem, deltaItem);

    const WatchStartupStartupDelta delta =
        row < deltas.size() ? deltas.at(row) : WatchStartupStartupDelta{};
    if (delta.state == WatchStartupDeltaState::NoTarget) {
      continue;
    }

    applyWatchStartupDeltaCells(watchValueItem, deltaItem, delta.state,
                                delta.watchValue, watchStartupDeltaTexts(),
                                settings_.theme == "Light");
  }

  if (watch_->startupWatchSummaryLabel) {
    const int rows = startupSdoTable_->rowCount();
    watch_->startupWatchSummaryLabel->setText(
        rows <= 0
            ? uiText("No Startup SDO rows", "暂无 Startup SDO 行")
            : uiText("Startup rows: %1 | Watch match: %2 | diff: %3 | "
                     "pending: %4 | no watch: %5 | Apply Diffs will write: %6",
                     "启动行：%1 | Watch 匹配：%2 | 不一致：%3 | 待比较：%4 | "
                     "无监视：%5 | 应用偏差将写入：%6")
                  .arg(rows)
                  .arg(summary.matched)
                  .arg(summary.diff)
                  .arg(summary.pending)
                  .arg(summary.missingWatch)
                  .arg(summary.diff));
  }
  filterStartupSdoTable();
  updateStartupSdoControls();
  updateNextBestAction();
  updateIoVariableTable();
  updateStateMachineView();
}


// — Apply text and diffs-only filtering to the Startup SDO table
void MainWindow::filterStartupSdoTable() {
  if (!startupSdoTable_) {
    return;
  }

  const bool diffsOnly =
      watch_->startupWatchDiffsOnly && watch_->startupWatchDiffsOnly->isChecked();
  QSet<int> diffRows;
  if (diffsOnly) {
    const QVector<int> rows = startupSdoRowsWithWatchDiffs();
    for (const int row : rows) {
      diffRows.insert(row);
    }
  }

  for (int row = 0; row < startupSdoTable_->rowCount(); ++row) {
    startupSdoTable_->setRowHidden(row, diffsOnly && !diffRows.contains(row)); // show/hide based on filter match
  }
  updateStartupSdoRowDetail();
}


// — Refresh the Startup SDO detail strip for the focused row
void MainWindow::updateStartupSdoRowDetail() {
  if (!watch_->startupSdoDetailLabel) {
    return;
  }
  const StartupSdoRowDetailTexts texts = startupSdoRowDetailTexts();
  // Lambda to push UI state changes to the label widget
  auto applyState = [this](const StartupSdoRowDetailUiState &state) {
    watch_->startupSdoDetailLabel->setText(state.text);
    watch_->startupSdoDetailLabel->setProperty("severity", state.severityKey);
    watch_->startupSdoDetailLabel->setToolTip(state.tooltip);
    repolish(watch_->startupSdoDetailLabel); // force QSS re-evaluation after property change
  };

  if (!startupSdoTable_) {
    applyState(startupSdoRowDetailUnavailableState(texts));
    return;
  }

  const int row = startupSdoTable_->currentRow();
  if (row < 0 || row >= startupSdoTable_->rowCount() ||
      startupSdoTable_->isRowHidden(row)) {
    applyState(startupSdoRowDetailNoSelectionState(texts));
    return;
  }

  applyState(buildStartupSdoRowDetailUiState(
      watchStartupStartupRow(startupSdoTable_, row), texts));
}


// — Focus startup sdo watch diffs
void MainWindow::focusStartupSdoWatchDiffs() {
  ensureStartupSdoTable();
  updateStartupSdoWatchEvidence();
  const QVector<int> rows = startupSdoRowsWithWatchDiffs();
  if (rows.isEmpty()) {
    if (watch_->startupWatchDiffsOnly) {
      watch_->startupWatchDiffsOnly->setChecked(false);
    }
    filterStartupSdoTable();
    updateDiagnostics(
        "Info", "Startup SDO",
        uiText("No Startup SDO rows currently differ from Watch values",
               "当前没有和 Watch 值不一致的 Startup SDO 行"));
    return;
  }

  if (tabs_) {
    const int startupIndex = tabs_->indexOf(startupSdoTable_->parentWidget());
    if (startupIndex >= 0) {
      tabs_->setCurrentIndex(startupIndex);
    }
  }
  if (watch_->startupWatchDiffsOnly) {
    watch_->startupWatchDiffsOnly->setChecked(true);
  } else {
    filterStartupSdoTable();
  }
  const int firstRow = rows.first();
  startupSdoTable_->selectRow(firstRow);
  startupSdoTable_->scrollToItem(startupSdoTable_->item(firstRow, 8),
                                 QAbstractItemView::PositionAtCenter);
  startupSdoTable_->setFocus();
  updateStartupSdoControls();
  updateDiagnostics("Info", "Startup SDO",
                    uiText("Focused %1 Startup SDO Watch diff row(s)",
                           "已聚焦 %1 条 Startup SDO Watch 偏差行")
                        .arg(rows.size()));
}


// — Add startup sdo
void MainWindow::addStartupSdo() {
  ensureStartupSdoTable();
  const int row = startupSdoTable_->rowCount();
  startupSdoTable_->insertRow(row);
  startupSdoTable_->setItem(
      row, 0,
      new QTableWidgetItem(
          QString::number(selectedPosition() >= 0 ? selectedPosition() : 0)));
  startupSdoTable_->setItem(row, 1, new QTableWidgetItem(sdoInspector_->sdoIndex->text()));
  startupSdoTable_->setItem(row, 2, new QTableWidgetItem(sdoInspector_->sdoSubIndex->text()));
  startupSdoTable_->setItem(row, 3,
                            new QTableWidgetItem(sdoInspector_->sdoWriteValue
                                                     ? sdoInspector_->sdoWriteValue->text()
                                                     : QString()));
  startupSdoTable_->setItem(
      row, 4,
      new QTableWidgetItem(sdoInspector_->sdoType ? sdoInspector_->sdoType->currentText() : QString()));
  startupSdoTable_->setItem(row, 5,
                            new QTableWidgetItem(uiText("Pending", "待应用")));
  startupSdoTable_->setItem(row, 6, new QTableWidgetItem);
  startupSdoTable_->resizeColumnsToContents(); // auto-fit column widths
  startupSdoTable_->selectRow(row);
  updateWatchStartupDeltas();
  filterStartupSdoTable();
  updateStartupSdoControls();
  updateDiagnostics("Info", "Startup SDO", "Added startup SDO row");
}


// — Remove startup sdo
void MainWindow::removeStartupSdo() {
  ensureStartupSdoTable();
  if (!startupSdoTable_) {
    return;
  }
  QVector<int> rows = selectedStartupSdoRows();
  if (rows.isEmpty()) {
    return;
  }
  std::sort(rows.begin(), rows.end(), std::greater<int>());
  for (const int row : rows) {
    if (row < 0 || row >= startupSdoTable_->rowCount()) {
      continue;
    }
    startupSdoTable_->removeRow(row);
  }
  updateDiagnostics(
      "Info", "Startup SDO",
      QString("Removed %1 selected startup SDO row(s)").arg(rows.size()));
  updateWatchStartupDeltas();
  filterStartupSdoTable();
  updateStartupSdoControls();
}


// — Move startup sdo row
void MainWindow::moveStartupSdoRow(int delta) {
  ensureStartupSdoTable();
  if (!startupSdoTable_ || delta == 0) {
    return;
  }
  const int row = startupSdoTable_->currentRow();
  const int target = row + delta;
  if (row < 0 || target < 0 || target >= startupSdoTable_->rowCount()) {
    return;
  }

  QStringList current;
  QStringList next;
  for (int column = 0; column < startupSdoTable_->columnCount(); ++column) {
    current << (startupSdoTable_->item(row, column)
                    ? startupSdoTable_->item(row, column)->text()
                    : QString());
    next << (startupSdoTable_->item(target, column)
                 ? startupSdoTable_->item(target, column)->text()
                 : QString());
  }
  for (int column = 0; column < startupSdoTable_->columnCount(); ++column) {
    startupSdoTable_->setItem(row, column,
                              new QTableWidgetItem(next.value(column)));
    startupSdoTable_->setItem(target, column,
                              new QTableWidgetItem(current.value(column)));
  }
  startupSdoTable_->selectRow(target);
  startupSdoTable_->resizeColumnsToContents(); // auto-fit column widths
  updateWatchStartupDeltas();
  filterStartupSdoTable();
  updateStartupSdoControls();
  updateDiagnostics("Info", "Startup SDO",
                    QString("Moved startup SDO row from %1 to %2")
                        .arg(row + 1)
                        .arg(target + 1));
}


// — Verify startup sdo list
void MainWindow::verifyStartupSdoList() {
  ensureStartupSdoTable();
  if (!client_.isConnected() || !startupSdoTable_ ||
      startupSdoTable_->rowCount() <= 0) {
    return;
  }

  int requested = 0;
  pendingStartupSdoChecks_.clear();
  for (int row = 0; row < startupSdoTable_->rowCount(); ++row) {
    const int position = startupSdoTable_->item(row, 0)
                             ? startupSdoTable_->item(row, 0)->text().toInt()
                             : -1;
    const QString index = startupSdoTable_->item(row, 1)
                              ? startupSdoTable_->item(row, 1)->text().trimmed()
                              : QString();
    const QString subIndex =
        startupSdoTable_->item(row, 2)
            ? startupSdoTable_->item(row, 2)->text().trimmed()
            : QString();
    const QString type = startupSdoTable_->item(row, 4)
                             ? startupSdoTable_->item(row, 4)->text().trimmed()
                             : QString();
    auto *status = startupSdoTable_->item(row, 5);
    if (!status) {
      status = new QTableWidgetItem;
      startupSdoTable_->setItem(row, 5, status);
    }
    auto *detail = startupSdoTable_->item(row, 6);
    if (!detail) {
      detail = new QTableWidgetItem;
      startupSdoTable_->setItem(row, 6, detail);
    }
    if (position < 0 || index.isEmpty() || subIndex.isEmpty()) {
      status->setText(uiText("Failed", "失败"));
      status->setForeground(QColor("#ef4444"));
      detail->setText(
          uiText("Missing slave, index, or subindex", "缺少从站、索引或子项"));
      continue;
    }
    status->setText(uiText("Verifying", "校验中"));
    status->setForeground(QColor("#f59e0b"));
    status->setBackground(QBrush());
    detail->setText(QString());
    const QString key = sdoEvidenceKey(position, index, subIndex);
    pendingStartupSdoChecks_[key].append(row);
    requestSdoRead(position, index, subIndex,
                   uiText("Startup SDO verify", "Startup SDO 校验"), type);
    ++requested;
  }
  startupSdoTable_->resizeColumnsToContents(); // auto-fit column widths
  updateWatchStartupDeltas();
  updateStartupSdoControls();
  updateDiagnostics(
      "Info", "Startup SDO",
      QString("Verification requested for %1 startup entries").arg(requested));
}


// — Verify startup sdo row
void MainWindow::verifyStartupSdoRow(int row) {
  ensureStartupSdoTable();
  if (!client_.isConnected() || !startupSdoTable_ || row < 0 ||
      row >= startupSdoTable_->rowCount()) {
    return;
  }

  auto textAt = [this, row](int column) {
    const auto *item = startupSdoTable_->item(row, column);
    return item ? item->text().trimmed() : QString();
  };

  bool positionOk = false;
  const int position = textAt(0).toInt(&positionOk);
  const QString index = textAt(1);
  QString subIndex = textAt(2);
  const QString type = textAt(4);
  if (!subIndex.isEmpty() && !subIndex.startsWith("0x", Qt::CaseInsensitive)) {
    subIndex = QString("0x%1").arg(subIndex.rightJustified(2, '0'));
  }
  auto *status = startupSdoTable_->item(row, 5);
  if (!status) {
    status = new QTableWidgetItem;
    startupSdoTable_->setItem(row, 5, status);
  }
  auto *detail = startupSdoTable_->item(row, 6);
  if (!detail) {
    detail = new QTableWidgetItem;
    startupSdoTable_->setItem(row, 6, detail);
  }

  if (!positionOk || position < 0 || index.isEmpty() || subIndex.isEmpty()) {
    status->setText(uiText("Failed", "失败"));
    status->setForeground(QColor("#ef4444"));
    status->setBackground(settings_.theme == "Light" ? QColor("#fef2f2")
                                                     : QColor("#3a1218"));
    detail->setText(
        uiText("Missing slave, index, or subindex", "缺少从站、索引或子项"));
    startupSdoTable_->resizeColumnsToContents(); // auto-fit column widths
    updateWatchStartupDeltas();
    updateDiagnostics(
        "Error", "Startup SDO",
        QString("Cannot verify startup SDO row %1: missing address")
            .arg(row + 1));
    return;
  }

  status->setText(uiText("Verifying", "校验中"));
  status->setForeground(QColor("#f59e0b"));
  status->setBackground(QBrush());
  detail->setText(QString());
  const QString key = sdoEvidenceKey(position, index, subIndex);
  pendingStartupSdoChecks_[key].append(row);
  requestSdoRead(position, index, subIndex,
                 uiText("Startup SDO row verify", "Startup SDO 行校验"), type);
  startupSdoTable_->resizeColumnsToContents(); // auto-fit column widths
  updateWatchStartupDeltas();
  updateStartupSdoControls();
  updateDiagnostics(
      "Info", "Startup SDO",
      QString("Verification requested for startup row %1").arg(row + 1));
}


// — Verify selected startup sdo rows
void MainWindow::verifySelectedStartupSdoRows() {
  ensureStartupSdoTable();
  if (!client_.isConnected() || !startupSdoTable_) {
    return;
  }
  const QVector<int> rows = selectedStartupSdoRows();
  if (rows.isEmpty()) {
    return;
  }

  int requested = 0;
  for (const int row : rows) {
    verifyStartupSdoRow(row);
    ++requested;
  }
  updateDiagnostics(
      "Info", "Startup SDO",
      QString("Verification requested for %1 selected startup row(s)")
          .arg(requested));
}


// — Add startup sdo row to watch
void MainWindow::addStartupSdoRowToWatch(int row) {
  ensureStartupSdoTable();
  if (!startupSdoTable_ || row < 0 || row >= startupSdoTable_->rowCount()) {
    return;
  }

  auto textAt = [this, row](int column) {
    const auto *item = startupSdoTable_->item(row, column);
    return item ? item->text().trimmed() : QString();
  };

  bool positionOk = false;
  const int position = textAt(0).toInt(&positionOk);
  const QString index = textAt(1);
  QString subIndex = textAt(2);
  if (!subIndex.isEmpty() && !subIndex.startsWith("0x", Qt::CaseInsensitive)) {
    subIndex = QString("0x%1").arg(subIndex.rightJustified(2, '0'));
  }
  if (!positionOk || position < 0 || index.isEmpty() || subIndex.isEmpty()) {
    updateDiagnostics(
        "Error", "Startup SDO",
        QString("Cannot add startup row %1 to Watch: missing address")
            .arg(row + 1));
    return;
  }

  setSelectedSlave(position);
  const QSignalBlocker indexBlocker(sdoInspector_->sdoIndex); // prevent recursive signal updates
  const QSignalBlocker subIndexBlocker(sdoInspector_->sdoSubIndex); // prevent recursive signal updates
  sdoInspector_->sdoIndex->setText(index);
  sdoInspector_->sdoSubIndex->setText(subIndex);
  if (sdoInspector_->sdoValue) {
    sdoInspector_->sdoValue->setText(textAt(3));
  }
  addCurrentSdoToWatch();
}


// — Validate all Startup SDO rows before applying; mark errors inline
bool MainWindow::preflightStartupSdoList(bool showSuccess) {
  ensureStartupSdoTable();
  if (!startupSdoTable_ || startupSdoTable_->rowCount() <= 0) {
    return false;
  }

  QSet<int> onlinePositions;
  for (const auto &slave : slaves_) {
    onlinePositions.insert(slave.position);
  }

  QHash<QString, int> firstOccurrence;
  QHash<QString, QString> firstOccurrenceValue;
  QSet<int> firstConflictRows;
  QStringList globalIssues = topologyBaselineIssues();
  int errors = 0;
  int warnings = globalIssues.isEmpty() ? 0 : globalIssues.size();

  for (int row = 0; row < startupSdoTable_->rowCount(); ++row) {
    auto textAt = [this, row](int column) {
      return startupSdoTable_->item(row, column)
                 ? startupSdoTable_->item(row, column)->text().trimmed()
                 : QString();
    };
    bool positionOk = false;
    const int position = textAt(0).toInt(&positionOk);
    const QString index = textAt(1);
    const QString subIndex = textAt(2);
    const QString value = textAt(3);
    const QString type = textAt(4);

    QStringList rowErrors;
    QStringList rowWarnings;
    if (!positionOk || position < 0) {
      rowErrors << uiText("invalid slave position", "从站位置无效");
    } else if (!onlinePositions.isEmpty() &&
               !onlinePositions.contains(position)) {
      rowErrors << uiText("slave is not present in current topology",
                          "当前拓扑中不存在该从站");
    }
    validateSdoAddressAndValue(index, subIndex, value, type, &rowErrors,
                               &rowWarnings);

    const QString key = QString("%1|%2|%3")
                            .arg(position)
                            .arg(index.toLower(), subIndex.toLower());
    if (!index.isEmpty() && !subIndex.isEmpty()) {
      if (firstOccurrence.contains(key)) {
        const int firstRow = firstOccurrence.value(key) + 1;
        const QString firstValue = firstOccurrenceValue.value(key);
        if (firstValue.trimmed().compare(value.trimmed(),
                                         Qt::CaseInsensitive) != 0) {
          const int firstRowIndex = firstOccurrence.value(key);
          if (firstRowIndex >= 0 &&
              firstRowIndex < startupSdoTable_->rowCount()) {
            firstConflictRows.insert(firstRowIndex);
            auto *firstStatus = startupSdoTable_->item(firstRowIndex, 5);
            if (!firstStatus) {
              firstStatus = new QTableWidgetItem;
              startupSdoTable_->setItem(firstRowIndex, 5, firstStatus);
            }
            auto *firstDetail = startupSdoTable_->item(firstRowIndex, 6);
            if (!firstDetail) {
              firstDetail = new QTableWidgetItem;
              startupSdoTable_->setItem(firstRowIndex, 6, firstDetail);
            }
            firstStatus->setText(uiText("Preflight Error", "预检查错误"));
            firstStatus->setForeground(QColor("#ef4444"));
            firstStatus->setBackground(settings_.theme == "Light"
                                           ? QColor("#fef2f2")
                                           : QColor("#3a1218"));
            firstDetail->setText(
                uiText("conflicting duplicate object also appears on row %1",
                       "对象和第 %1 行重复且写入值冲突")
                    .arg(row + 1));
          }
          rowErrors
              << uiText("conflicting duplicate object on row %1 has value %2",
                        "与第 %1 行对象重复且写入值冲突：%2")
                     .arg(firstRow)
                     .arg(firstValue);
        } else {
          rowWarnings << uiText("duplicate object also appears on row %1",
                                "对象和第 %1 行重复")
                             .arg(firstRow);
        }
      } else {
        firstOccurrence.insert(key, row);
        firstOccurrenceValue.insert(key, value);
      }
    }

    auto *status = startupSdoTable_->item(row, 5);
    if (!status) {
      status = new QTableWidgetItem;
      startupSdoTable_->setItem(row, 5, status);
    }
    auto *detail = startupSdoTable_->item(row, 6);
    if (!detail) {
      detail = new QTableWidgetItem;
      startupSdoTable_->setItem(row, 6, detail);
    }

    if (!rowErrors.isEmpty()) {
      ++errors;
      status->setText(uiText("Preflight Error", "预检查错误"));
      status->setForeground(QColor("#ef4444"));
      status->setBackground(settings_.theme == "Light" ? QColor("#fef2f2")
                                                       : QColor("#3a1218"));
      detail->setText(rowErrors.join("; "));
    } else if (!rowWarnings.isEmpty()) {
      ++warnings;
      status->setText(uiText("Preflight Warning", "预检查警告"));
      status->setForeground(QColor("#f59e0b"));
      status->setBackground(settings_.theme == "Light" ? QColor("#fff7ed")
                                                       : QColor("#2b1d10"));
      detail->setText(rowWarnings.join("; "));
    } else {
      status->setText(uiText("Preflight OK", "预检查通过"));
      status->setForeground(QColor("#22c55e"));
      status->setBackground(QBrush());
      detail->setText(QString());
    }
  }

  errors += firstConflictRows.size();
  startupSdoTable_->resizeColumnsToContents(); // auto-fit column widths
  updateWatchStartupDeltas();
  if (!globalIssues.isEmpty()) {
    updateDiagnostics(
        "Warning", "Startup SDO",
        uiText("Topology baseline issue(s): %1", "拓扑基线问题：%1")
            .arg(globalIssues.join(" | ")));
  }

  const QString summary =
      uiText("Startup SDO preflight: %1 error(s), %2 warning(s)",
             "Startup SDO 预检查：%1 个错误，%2 个警告")
          .arg(errors)
          .arg(warnings);
  updateDiagnostics(errors > 0 ? "Error" : (warnings > 0 ? "Warning" : "Info"),
                    "Startup SDO", summary);
  if (showSuccess && errors == 0) {
    QMessageBox::information(
        this, uiText("Startup SDO Preflight", "Startup SDO 预检查"),
        warnings > 0 ? summary
                     : uiText("Startup SDO preflight passed.",
                              "Startup SDO 预检查通过。"));
  }
  return errors == 0;
}


// — Enable or disable Verify and Apply buttons based on table state
void MainWindow::updateStartupSdoControls() {
  if (!startupSdoTable_) {
    return;
  }
  const bool connected = client_.isConnected();
  const int rows = startupSdoTable_->rowCount();
  const int row = startupSdoTable_->currentRow();
  const bool hasCurrentVisibleRow =
      row >= 0 && row < rows && !startupSdoTable_->isRowHidden(row);
  const bool hasSelectedRows = !selectedStartupSdoRows().isEmpty();
  const bool hasWatchDiffs = !startupSdoRowsWithWatchDiffs().isEmpty();
  auto setEnabled = [this](const char *name, bool enabled) {
    if (auto *button = findChild<QPushButton *>(name)) {
      button->setEnabled(enabled);
    }
  };
  setEnabled("removeStartupSdo", hasSelectedRows);
  setEnabled("moveStartupSdoUp", hasCurrentVisibleRow && row > 0);
  setEnabled("moveStartupSdoDown", hasCurrentVisibleRow && row < rows - 1);
  setEnabled("preflightStartupSdo", rows > 0);
  setEnabled("verifyStartupSdo", connected && rows > 0);
  setEnabled("verifySelectedStartupSdo", connected && hasSelectedRows);
  setEnabled("focusStartupSdoWatchDiffs", hasWatchDiffs);
  setEnabled("applyStartupSdoWatchDiffs", connected && hasWatchDiffs);
  setEnabled("applyStartupSdo", connected && rows > 0);
  setEnabled("applySelectedStartupSdo", connected && hasSelectedRows);
  if (watch_->startupWatchDiffsOnly) {
    watch_->startupWatchDiffsOnly->setEnabled(rows > 0);
  }
}


// — Apply startup sdo row
void MainWindow::applyStartupSdoRow(int row) {
  ensureStartupSdoTable();
  if (!client_.isConnected() || !startupSdoTable_ || row < 0 ||
      row >= startupSdoTable_->rowCount()) {
    return;
  }

  auto textAt = [this, row](int column) {
    const auto *item = startupSdoTable_->item(row, column);
    return item ? item->text().trimmed() : QString();
  };

  bool positionOk = false;
  const int position = textAt(0).toInt(&positionOk);
  const QString index = textAt(1);
  QString subIndex = textAt(2);
  if (!subIndex.isEmpty() && !subIndex.startsWith("0x", Qt::CaseInsensitive)) {
    subIndex = QString("0x%1").arg(subIndex.rightJustified(2, '0'));
  }
  const QString value = textAt(3);
  const QString type = textAt(4);
  QStringList validationErrors;
  QStringList validationWarnings;
  if (!positionOk || position < 0) {
    validationErrors << uiText("invalid slave position", "从站位置无效");
  }
  validateSdoAddressAndValue(index, subIndex, value, type, &validationErrors,
                             &validationWarnings);
  if (!validationErrors.isEmpty()) {
    auto *status = startupSdoTable_->item(row, 5);
    if (!status) {
      status = new QTableWidgetItem;
      startupSdoTable_->setItem(row, 5, status);
    }
    auto *detail = startupSdoTable_->item(row, 6);
    if (!detail) {
      detail = new QTableWidgetItem;
      startupSdoTable_->setItem(row, 6, detail);
    }
    status->setText(uiText("Validation Error", "校验错误"));
    status->setForeground(QColor("#ef4444"));
    status->setBackground(settings_.theme == "Light" ? QColor("#fef2f2")
                                                     : QColor("#3a1218"));
    detail->setText(validationErrors.join("; "));
    startupSdoTable_->resizeColumnsToContents(); // auto-fit column widths
    updateWatchStartupDeltas();
    QMessageBox::warning(
        this, uiText("Startup SDO Validation Failed", "Startup SDO 校验失败"),
        validationErrors.join("\n"));
    return;
  }

  QStringList details = {
      uiText("Master: %1", "主站：%1").arg(activeMasterName()),
      uiText("Row: %1", "行：%1").arg(row + 1),
      QString("#%1  %2:%3 = %4").arg(position).arg(index, subIndex, value),
      uiText("Type: %1", "类型：%1").arg(type.isEmpty() ? "default" : type),
  };
  updateStartupSdoWatchEvidence();
  details << sdoWriteImpactDetails(position, index, subIndex, value, type);
  if (!validationWarnings.isEmpty()) {
    details << uiText("Validation warning: %1", "校验警告：%1")
                   .arg(validationWarnings.join("; "));
  }
  // Safety gate: require explicit confirmation before bus write
  if (!confirmDangerousOperation(
          uiText("Confirm Startup SDO Row Apply", "确认应用 Startup SDO 行"),
          uiText("This operation writes one Startup SDO value to the bus.",
                 "此操作会向总线写入一条 Startup SDO 参数。"),
          details, uiText("Apply Row", "应用此行"))) {
    return;
  }

  auto *status = startupSdoTable_->item(row, 5);
  if (!status) {
    status = new QTableWidgetItem;
    startupSdoTable_->setItem(row, 5, status);
  }
  auto *detail = startupSdoTable_->item(row, 6);
  if (!detail) {
    detail = new QTableWidgetItem;
    startupSdoTable_->setItem(row, 6, detail);
  }
  status->setText(uiText("Applying", "应用中"));
  status->setForeground(QColor("#f59e0b"));
  status->setBackground(QBrush());
  detail->setText(QString());

  const QString key = sdoEvidenceKey(position, index, subIndex);
  pendingSdoWrites_.insert(
      key, {QString::number(position), index, subIndex, type, value});
  const QString verifyKey = sdoEvidenceKey(position, index, subIndex);
  pendingStartupSdoChecks_[verifyKey].append(row);
  appendSdoHistory(uiText("Write", "写入"), position, index, subIndex, type,
                   value, uiText("Requested", "已请求"),
                   uiText("Startup SDO row apply", "Startup SDO 行应用"));
  client_.download(position, index, subIndex, value, type);
  updateDiagnostics("Info", "Startup SDO",
                    QString("Apply requested for startup row %1").arg(row + 1));
  startupSdoTable_->resizeColumnsToContents(); // auto-fit column widths
  updateWatchStartupDeltas();
  updateStartupSdoControls();
}


// — Apply selected startup sdo rows
void MainWindow::applySelectedStartupSdoRows() {
  ensureStartupSdoTable();
  if (!client_.isConnected() || !startupSdoTable_) {
    return;
  }
  const QVector<int> rows = selectedStartupSdoRows();
  if (rows.isEmpty()) {
    return;
  }

  applyStartupSdoRows(rows, uiText("Selected Startup SDO", "所选 Startup SDO"),
                      uiText("This operation writes selected SDO values to the "
                             "bus.",
                             "此操作会向总线写入选中的 SDO 参数。"),
                      uiText("Apply Selected", "应用所选"));
}


// — Send the given Startup SDO rows to ecatd for sequential write
void MainWindow::applyStartupSdoRows(const QVector<int> &rows,
                                     const QString &operationLabel,
                                     const QString &summary,
                                     const QString &confirmText) {
  ensureStartupSdoTable();
  if (!client_.isConnected() || !startupSdoTable_ || rows.isEmpty()) {
    return;
  }

  QJsonArray items;
  QVector<int> validRows;
  QStringList validationErrors;
  QStringList validationWarnings;
  for (const int row : rows) {
    if (row < 0 || row >= startupSdoTable_->rowCount()) {
      continue;
    }
    auto textAt = [this, row](int column) {
      const auto *item = startupSdoTable_->item(row, column);
      return item ? item->text().trimmed() : QString();
    };

    bool positionOk = false;
    const int position = textAt(0).toInt(&positionOk);
    const QString index = textAt(1);
    const QString subIndex = textAt(2);
    const QString value = textAt(3);
    const QString type = textAt(4);
    QStringList rowErrors;
    QStringList rowWarnings;
    if (!positionOk || position < 0) {
      rowErrors << uiText("invalid slave position", "从站位置无效");
    }
    validateSdoAddressAndValue(index, subIndex, value, type, &rowErrors,
                               &rowWarnings);

    if (!rowErrors.isEmpty()) {
      auto *status = startupSdoTable_->item(row, 5);
      if (!status) {
        status = new QTableWidgetItem;
        startupSdoTable_->setItem(row, 5, status);
      }
      auto *detail = startupSdoTable_->item(row, 6);
      if (!detail) {
        detail = new QTableWidgetItem;
        startupSdoTable_->setItem(row, 6, detail);
      }
      status->setText(uiText("Validation Error", "校验错误"));
      status->setForeground(QColor("#ef4444"));
      status->setBackground(settings_.theme == "Light" ? QColor("#fef2f2")
                                                       : QColor("#3a1218"));
      detail->setText(rowErrors.join("; "));
      validationErrors << uiText("Row %1: %2", "第 %1 行：%2")
                              .arg(row + 1)
                              .arg(rowErrors.join("; "));
      continue;
    }

    if (!rowWarnings.isEmpty()) {
      validationWarnings << uiText("Row %1: %2", "第 %1 行：%2")
                                .arg(row + 1)
                                .arg(rowWarnings.join("; "));
    }
    // Build the SDO write request payload
    items.append(QJsonObject{
        {"row", row},
        {"position", position},
        {"index", index},
        {"subIndex", subIndex},
        {"value", value},
        {"type", type},
    });
    validRows.append(row);
  }

  if (!validationErrors.isEmpty()) {
    startupSdoTable_->resizeColumnsToContents(); // auto-fit column widths
    updateStartupSdoControls();
    updateDiagnostics("Error", "Startup SDO",
                      uiText("%1 apply blocked: %2", "%1 应用已阻止：%2")
                          .arg(operationLabel)
                          .arg(validationErrors.join("; ")));
    QMessageBox::warning(
        this, uiText("Startup SDO Validation Failed", "Startup SDO 校验失败"),
        validationErrors.join("\n"));
    return;
  }
  if (items.isEmpty()) {
    return;
  }

  QStringList details = {
      uiText("Master: %1", "主站：%1").arg(activeMasterName()),
      uiText("%1 entries: %2", "%1 数量：%2")
          .arg(operationLabel)
          .arg(items.size()),
      summary,
  };
  updateStartupSdoWatchEvidence();
  details << startupSdoBatchImpactDetails(validRows, 5);
  if (!validationWarnings.isEmpty()) {
    details << uiText("Validation warning: %1", "校验警告：%1")
                   .arg(validationWarnings.join("; "));
  }
  // Safety gate: require explicit confirmation before bus write
  if (!confirmDangerousOperation(
          uiText("Confirm %1 Apply", "确认应用 %1").arg(operationLabel),
          summary, details, confirmText)) {
    return;
  }

  for (const auto &itemValue : items) {
    const int row = itemValue.toObject().value("row").toInt(-1);
    if (row < 0 || row >= startupSdoTable_->rowCount()) {
      continue;
    }
    auto *status = startupSdoTable_->item(row, 5);
    if (!status) {
      status = new QTableWidgetItem;
      startupSdoTable_->setItem(row, 5, status);
    }
    auto *detail = startupSdoTable_->item(row, 6);
    if (!detail) {
      detail = new QTableWidgetItem;
      startupSdoTable_->setItem(row, 6, detail);
    }
    status->setText(uiText("Applying", "应用中"));
    status->setForeground(QBrush());
    status->setBackground(QBrush());
    detail->setText(QString());
  }
  client_.applyStartupSdos(items);
  updateDiagnostics("Info", "Startup SDO",
                    QString("Apply requested for %1 %2 row(s)")
                        .arg(items.size())
                        .arg(operationLabel));
  startupSdoTable_->resizeColumnsToContents(); // auto-fit column widths
  updateStartupSdoControls();
}


// — Return indices of Startup SDO rows that differ from Watch values
QVector<int> MainWindow::startupSdoRowsWithWatchDiffs() const {
  if (!startupSdoTable_ || !watch_->watchTable) {
    return {};
  }
  return ::startupRowsWithWatchDiffs(
      evaluateStartupWatchDeltas(watchStartupStartupRows(startupSdoTable_),
                                 watchStartupWatchRows(watch_->watchTable)));
}


// — Apply only Startup SDO rows that differ from current Watch values
void MainWindow::applyStartupSdoWatchDiffRows() {
  const QVector<int> rows = startupSdoRowsWithWatchDiffs();
  if (rows.isEmpty()) {
    updateDiagnostics(
        "Info", "Startup SDO",
        uiText("No Startup SDO rows currently differ from Watch values",
               "当前没有和 Watch 值不一致的 Startup SDO 行"));
    return;
  }
  applyStartupSdoRows(
      rows, uiText("Watch-diff Startup SDO", "Watch 偏差 Startup SDO"),
      uiText(
          "This operation writes only Startup SDO rows whose expected values "
          "differ from current Watch values.",
          "此操作只写入和当前 Watch 值不一致的 Startup SDO 行。"),
      uiText("Apply Diffs", "应用偏差"));
}


// — Validate and apply all Startup SDO rows after confirmation
void MainWindow::applyStartupSdoList() {
  ensureStartupSdoTable();
  if (!client_.isConnected() || startupSdoTable_->rowCount() <= 0) {
    return;
  }
  if (!preflightStartupSdoList(false)) {
    QMessageBox::warning(
        this, uiText("Startup SDO Preflight Failed", "Startup SDO 预检查失败"),
        uiText(
            "Fix rows marked with Preflight Error before applying Startup SDO.",
            "请先修复标记为预检查错误的行，再应用 Startup SDO。"));
    return;
  }
  QStringList details = {
      uiText("Master: %1", "主站：%1").arg(activeMasterName()),
      uiText("Startup entries: %1", "启动项数量：%1")
          .arg(startupSdoTable_->rowCount()),
      uiText("This applies the listed SDO writes in sequence.",
             "此操作会按顺序应用表格中的 SDO 写入。"),
  };
  QVector<int> rows;
  rows.reserve(startupSdoTable_->rowCount());
  for (int row = 0; row < startupSdoTable_->rowCount(); ++row) {
    rows.append(row);
  }
  updateStartupSdoWatchEvidence();
  details << startupSdoBatchImpactDetails(rows, 5);
  // Safety gate: require explicit confirmation before bus write
  if (!confirmDangerousOperation(
          uiText("Confirm Startup SDO Apply", "确认应用 Startup SDO"),
          uiText("This operation writes multiple SDO values to the bus.",
                 "此操作会向总线写入多条 SDO 参数。"),
          details, uiText("Apply Startup SDO", "应用 Startup SDO"))) {
    return;
  }
  QJsonArray items;
  for (int row = 0; row < startupSdoTable_->rowCount(); ++row) {
    auto *status = startupSdoTable_->item(row, 5);
    if (!status) {
      status = new QTableWidgetItem;
      startupSdoTable_->setItem(row, 5, status);
    }
    status->setText(uiText("Applying", "应用中"));
    status->setForeground(QBrush());
    status->setBackground(QBrush());
    if (!startupSdoTable_->item(row, 6)) {
      startupSdoTable_->setItem(row, 6, new QTableWidgetItem);
    } else {
      startupSdoTable_->item(row, 6)->setText(QString());
    }
    // Build the SDO write request payload
    items.append(QJsonObject{
        {"row", row},
        {"position", startupSdoTable_->item(row, 0)
                         ? startupSdoTable_->item(row, 0)->text().toInt()
                         : 0},
        {"index", startupSdoTable_->item(row, 1)
                      ? startupSdoTable_->item(row, 1)->text()
                      : QString()},
        {"subIndex", startupSdoTable_->item(row, 2)
                         ? startupSdoTable_->item(row, 2)->text()
                         : QString()},
        {"value", startupSdoTable_->item(row, 3)
                      ? startupSdoTable_->item(row, 3)->text()
                      : QString()},
        {"type", startupSdoTable_->item(row, 4)
                     ? startupSdoTable_->item(row, 4)->text()
                     : QString()},
    });
  }
  client_.applyStartupSdos(items);
  updateDiagnostics(
      "Info", "Startup SDO",
      QString("Apply requested for %1 startup entries").arg(items.size()));
}

