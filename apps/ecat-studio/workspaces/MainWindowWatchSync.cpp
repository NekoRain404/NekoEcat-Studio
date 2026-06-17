// Watch list, add-to-watch, and baseline comparison.

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



// — Insert a Startup SDO row from the selected Watch row
void MainWindow::addStartupSdoFromWatchRow(int row) {
  if (!watch_->watchTable || row < 0 || row >= watch_->watchTable->rowCount()) {
    return;
  }

  auto textAt = [this, row](int column) {
    const auto *item = watch_->watchTable->item(row, column);
    return item ? item->text().trimmed() : QString();
  };

  bool positionOk = false;
  const int position = textAt(1).toInt(&positionOk);
  const QString index = textAt(2);
  const QString subIndex = textAt(3);
  const QString value = textAt(4);
  const QString type = textAt(6);
  if (!positionOk || position < 0 || index.isEmpty() || subIndex.isEmpty() ||
      value.isEmpty()) {
    updateDiagnostics(
        "Error", "Watch",
        QString("Cannot create Startup SDO from watch row %1: missing value")
            .arg(row + 1));
    return;
  }

  setSelectedSlave(position);
  const QSignalBlocker indexBlocker(sdoInspector_->sdoIndex); // prevent recursive signal updates
  const QSignalBlocker subIndexBlocker(sdoInspector_->sdoSubIndex); // prevent recursive signal updates
  const QSignalBlocker typeBlocker(sdoInspector_->sdoType); // prevent recursive signal updates
  sdoInspector_->sdoIndex->setText(index);
  sdoInspector_->sdoSubIndex->setText(subIndex);
  if (sdoInspector_->sdoValue) {
    sdoInspector_->sdoValue->setText(value);
  }
  if (sdoInspector_->sdoType) {
    sdoInspector_->sdoType->setCurrentText(type);
  }
  selectedSdoWritable_ = true;
  if (sdoInspector_->sdoWriteValue) {
    sdoInspector_->sdoWriteValue->setEnabled(true);
    sdoInspector_->sdoWriteValue->setText(value);
    sdoInspector_->sdoWriteValue->setPlaceholderText(
        uiText("Value from Watch", "来自 Watch 的值"));
  }
  updateSdoInspector(uiText("Watch", "监视"),
                     uiText("Creating Startup SDO from Watch row %1",
                            "从 Watch 第 %1 行创建 Startup SDO")
                         .arg(row + 1));
  addStartupSdo();
  updateDiagnostics(
      "Info", "Watch",
      QString("Created Startup SDO from watch row %1").arg(row + 1));
}


// — Add startup sdo from selected watch rows
void MainWindow::addStartupSdoFromSelectedWatchRows() {
  if (!watch_->watchTable || !watch_->watchTable->selectionModel()) {
    return;
  }

  const QVector<int> rows = selectedTableRows(watch_->watchTable);

  const int previousPosition = selectedPosition();
  const QString previousIndex = sdoInspector_->sdoIndex ? sdoInspector_->sdoIndex->text() : QString();
  const QString previousSubIndex =
      sdoInspector_->sdoSubIndex ? sdoInspector_->sdoSubIndex->text() : QString();
  const QString previousType = sdoInspector_->sdoType ? sdoInspector_->sdoType->currentText() : QString();
  const QString previousReadValue = sdoInspector_->sdoValue ? sdoInspector_->sdoValue->text() : QString();
  const QString previousWriteValue =
      sdoInspector_->sdoWriteValue ? sdoInspector_->sdoWriteValue->text() : QString();
  const bool previousWritable = selectedSdoWritable_;

  int created = 0;
  int skipped = 0;
  for (const int row : rows) {
    if (row < 0 || row >= watch_->watchTable->rowCount() ||
        watch_->watchTable->isRowHidden(row)) {
      continue;
    }
    const QString value = watch_->watchTable->item(row, 4)
                              ? watch_->watchTable->item(row, 4)->text().trimmed()
                              : QString();
    if (value.isEmpty()) {
      ++skipped;
      continue;
    }
    const int before = startupSdoTable_ ? startupSdoTable_->rowCount() : 0;
    addStartupSdoFromWatchRow(row);
    const int after = startupSdoTable_ ? startupSdoTable_->rowCount() : before;
    if (after > before) {
      ++created;
    } else {
      ++skipped;
    }
  }

  if (previousPosition >= 0) {
    setSelectedSlave(previousPosition);
  }
  {
    const QSignalBlocker indexBlocker(sdoInspector_->sdoIndex); // prevent recursive signal updates
    const QSignalBlocker subIndexBlocker(sdoInspector_->sdoSubIndex); // prevent recursive signal updates
    const QSignalBlocker typeBlocker(sdoInspector_->sdoType); // prevent recursive signal updates
    if (sdoInspector_->sdoIndex) {
      sdoInspector_->sdoIndex->setText(previousIndex);
    }
    if (sdoInspector_->sdoSubIndex) {
      sdoInspector_->sdoSubIndex->setText(previousSubIndex);
    }
    if (sdoInspector_->sdoType) {
      sdoInspector_->sdoType->setCurrentText(previousType);
    }
  }
  if (sdoInspector_->sdoValue) {
    sdoInspector_->sdoValue->setText(previousReadValue);
  }
  if (sdoInspector_->sdoWriteValue) {
    sdoInspector_->sdoWriteValue->setText(previousWriteValue);
  }
  selectedSdoWritable_ = previousWritable;
  updateSdoInspector(uiText("Restored selection", "已恢复选择"));
  updateStartupSdoControls();
  updateActionAvailability();
  updateDiagnostics(
      "Info", "Watch",
      QString("Created %1 Startup SDO row(s) from selected Watch values%2")
          .arg(created)
          .arg(skipped > 0 ? QString(", skipped %1").arg(skipped) : QString()));
  if (created > 0) {
    activateWorkspaceTab(startupSdoTabIndex_);
  }
}


// — Sync watch rows to startup sdo
void MainWindow::syncWatchRowsToStartupSdo(const QVector<int> &rows) {
  if (!watch_->watchTable || rows.isEmpty()) {
    return;
  }

  ensureWatchTable();
  ensureStartupSdoTable();

  auto watchTextAt = [this](int row, int column) {
    const auto *item = watch_->watchTable->item(row, column);
    return item ? item->text().trimmed() : QString();
  };
  auto startupTextAt = [this](int row, int column) {
    const auto *item = startupSdoTable_->item(row, column);
    return item ? item->text().trimmed() : QString();
  };
  auto ensureStartupCell = [this](int row, int column) {
    auto *item = startupSdoTable_->item(row, column);
    if (!item) {
      item = new QTableWidgetItem;
      startupSdoTable_->setItem(row, column, item);
    }
    return item;
  };
  struct Candidate {
    int watchRow = -1;
    int position = -1;
    QString index;
    QString subIndex;
    QString value;
    QString type;
    QVector<int> startupRows;
  };

  QVector<int> uniqueRows = rows;
  std::sort(uniqueRows.begin(), uniqueRows.end());
  uniqueRows.erase(std::unique(uniqueRows.begin(), uniqueRows.end()),
                   uniqueRows.end());

  QVector<Candidate> candidates;
  QSet<QString> processedKeys;
  int skipped = 0;
  int duplicateSkipped = 0;

  for (const int row : uniqueRows) {
    if (row < 0 || row >= watch_->watchTable->rowCount() ||
        watch_->watchTable->isRowHidden(row)) {
      continue;
    }

    bool positionOk = false;
    const int position = watchTextAt(row, 1).toInt(&positionOk);
    const QString rawIndex = watchTextAt(row, 2);
    const QString rawSubIndex = watchTextAt(row, 3);
    const QString value = watchTextAt(row, 4);
    const QString type = watchTextAt(row, 6);
    if (!positionOk || position < 0 || rawIndex.isEmpty() ||
        rawSubIndex.isEmpty() || value.isEmpty()) {
      ++skipped;
      continue;
    }

    // Normalize hex address for consistent comparison
    const QString index = normalizeHexText(rawIndex, 4);
    // Normalize hex address for consistent comparison
    const QString subIndex = normalizeHexText(rawSubIndex, 2);
    const QString key = sdoEvidenceKey(position, index, subIndex);
    if (processedKeys.contains(key)) {
      ++duplicateSkipped;
      continue;
    }
    processedKeys.insert(key);

    Candidate candidate;
    candidate.watchRow = row;
    candidate.position = position;
    candidate.index = index;
    candidate.subIndex = subIndex;
    candidate.value = value;
    candidate.type = type;

    for (int startupRow = 0; startupRow < startupSdoTable_->rowCount();
         ++startupRow) {
      bool startupPositionOk = false;
      const int startupPosition =
          startupTextAt(startupRow, 0).toInt(&startupPositionOk);
      const QString startupIndex =
          // Normalize hex address for consistent comparison
          normalizeHexText(startupTextAt(startupRow, 1), 4);
      const QString startupSubIndex =
          // Normalize hex address for consistent comparison
          normalizeHexText(startupTextAt(startupRow, 2), 2);
      if (startupPositionOk && startupPosition == position &&
          startupIndex == index && startupSubIndex == subIndex) {
        candidate.startupRows.append(startupRow);
      }
    }

    candidates.append(candidate);
  }

  if (candidates.isEmpty()) {
    updateDiagnostics(
        "Warning", "Startup SDO",
        QString("Startup sync skipped: no selected Watch rows with current "
                "values%1")
            .arg(skipped > 0 ? QString(", skipped %1 row(s)").arg(skipped)
                             : QString()));
    return;
  }

  int existingRowsAffected = 0;
  int newRowsPlanned = 0;
  for (const auto &candidate : candidates) {
    if (candidate.startupRows.isEmpty()) {
      ++newRowsPlanned;
    } else {
      existingRowsAffected += candidate.startupRows.size();
    }
  }

  QStringList details = {
      uiText("Master: %1", "主站：%1").arg(activeMasterName()),
      uiText("Accepted Watch values: %1", "可同步 Watch 值：%1")
          .arg(candidates.size()),
      uiText("Existing Startup SDO rows to update: %1",
             "将更新的已有 Startup SDO 行：%1")
          .arg(existingRowsAffected),
      uiText("Missing Startup SDO rows to create: %1",
             "将创建的缺失 Startup SDO 行：%1")
          .arg(newRowsPlanned),
      uiText(
          "This changes only the Startup SDO table and does not write to the "
          "bus.",
          "此操作只修改 Startup SDO 表，不会向总线写入。"),
  };

  const int previewRows = std::min(static_cast<int>(candidates.size()), 6);
  for (int i = 0; i < previewRows; ++i) {
    const auto &candidate = candidates.at(i);
    QString target =
        candidate.startupRows.isEmpty()
            ? uiText("create", "创建")
            : uiText("update startup row(s) %1", "更新启动行 %1")
                  .arg([&candidate] {
                    QStringList rowNumbers;
                    for (const int startupRow : candidate.startupRows) {
                      rowNumbers << QString::number(startupRow + 1);
                    }
                    return rowNumbers.join(", ");
                  }());
    details << QString("#%1  %2:%3 = %4  [%5]")
                   .arg(candidate.position)
                   .arg(candidate.index, candidate.subIndex, candidate.value,
                        target);
  }
  if (candidates.size() > previewRows) {
    details << uiText("...and %1 more Watch value(s)", "...另有 %1 个 Watch 值")
                   .arg(candidates.size() - previewRows);
  }
  if (skipped > 0) {
    details << uiText("Skipped rows without address or current value: %1",
                      "已跳过缺少地址或当前值的行：%1")
                   .arg(skipped);
  }
  if (duplicateSkipped > 0) {
    details << uiText("Skipped duplicate selected Watch address(es): %1",
                      "已跳过重复选中 Watch 地址：%1")
                   .arg(duplicateSkipped);
  }

  // Safety gate: require explicit confirmation before bus write
  if (!confirmDangerousOperation(
          uiText("Confirm Startup Sync", "确认同步 Startup SDO"),
          uiText("Synchronize selected Watch values into Startup SDO.",
                 "将选中 Watch 当前值同步到 Startup SDO。"),
          details, uiText("Sync Startup", "同步启动"))) {
    return;
  }

  const QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");
  int updated = 0;
  int unchanged = 0;
  int created = 0;
  int lastTouchedRow = -1;

  for (const auto &candidate : candidates) {
    if (candidate.startupRows.isEmpty()) {
      const int startupRow = startupSdoTable_->rowCount();
      startupSdoTable_->insertRow(startupRow);
      startupSdoTable_->setItem(
          startupRow, 0,
          new QTableWidgetItem(QString::number(candidate.position)));
      startupSdoTable_->setItem(startupRow, 1,
                                new QTableWidgetItem(candidate.index));
      startupSdoTable_->setItem(startupRow, 2,
                                new QTableWidgetItem(candidate.subIndex));
      startupSdoTable_->setItem(startupRow, 3,
                                new QTableWidgetItem(candidate.value));
      startupSdoTable_->setItem(startupRow, 4,
                                new QTableWidgetItem(candidate.type));
      startupSdoTable_->setItem(
          startupRow, 5, new QTableWidgetItem(uiText("Synced", "已同步")));
      startupSdoTable_->setItem(
          startupRow, 6,
          new QTableWidgetItem(uiText("Created from Watch row %1 at %2",
                                      "由 Watch 第 %1 行在 %2 创建")
                                   .arg(candidate.watchRow + 1)
                                   .arg(timestamp)));
      ++created;
      lastTouchedRow = startupRow;
      continue;
    }

    for (const int startupRow : candidate.startupRows) {
      if (startupRow < 0 || startupRow >= startupSdoTable_->rowCount()) {
        continue;
      }
      const QString previousValue = startupTextAt(startupRow, 3);
      ensureStartupCell(startupRow, 3)->setText(candidate.value);
      if (!candidate.type.isEmpty()) {
        ensureStartupCell(startupRow, 4)->setText(candidate.type);
      }
      ensureStartupCell(startupRow, 5)->setText(uiText("Synced", "已同步"));
      ensureStartupCell(startupRow, 6)
          ->setText(
              previousValue.compare(candidate.value, Qt::CaseInsensitive) == 0
                  ? uiText("Confirmed from Watch row %1 at %2",
                           "由 Watch 第 %1 行在 %2 确认")
                        .arg(candidate.watchRow + 1)
                        .arg(timestamp)
                  : uiText("Synced from Watch row %1 at %2; previous "
                           "value: %3",
                           "由 Watch 第 %1 行在 %2 同步；原值：%3")
                        .arg(candidate.watchRow + 1)
                        .arg(timestamp, previousValue));
      if (previousValue.compare(candidate.value, Qt::CaseInsensitive) == 0) {
        ++unchanged;
      } else {
        ++updated;
      }
      lastTouchedRow = startupRow;
    }
  }

  if (lastTouchedRow >= 0) {
    startupSdoTable_->selectRow(lastTouchedRow);
  }
  startupSdoTable_->resizeColumnsToContents(); // auto-fit column widths
  updateWatchStartupDeltas();
  updateStartupSdoControls();
  updateWatchAutoRefresh();
  updateDiagnostics(
      "Info", "Startup SDO",
      QString("Synced Startup SDO from Watch: %1 updated, %2 unchanged, %3 "
              "created%4%5")
          .arg(updated)
          .arg(unchanged)
          .arg(created)
          .arg(skipped > 0 ? QString(", skipped %1").arg(skipped) : QString())
          .arg(duplicateSkipped > 0
                   ? QString(", duplicate selections %1").arg(duplicateSkipped)
                   : QString()));
}


// — Sync selected watch rows to startup sdo
void MainWindow::syncSelectedWatchRowsToStartupSdo() {
  if (!watch_->watchTable) {
    return;
  }

  syncWatchRowsToStartupSdo(selectedTableRows(watch_->watchTable));
}


// — Add selected history rows to startup sdo
void MainWindow::addSelectedHistoryRowsToStartupSdo() {
  if (!sdoHistoryTable_) {
    return;
  }
  const QVector<int> rows = selectedSdoHistoryRows();
  if (rows.isEmpty()) {
    return;
  }

  ensureStartupSdoTable();
  int created = 0;
  int skipped = 0;
  for (const int row : rows) {
    if (row < 0 || row >= sdoHistoryTable_->rowCount() ||
        sdoHistoryTable_->isRowHidden(row)) {
      continue;
    }

    auto textAt = [this, row](int column) {
      const auto *item = sdoHistoryTable_->item(row, column);
      return item ? item->text().trimmed() : QString();
    };

    bool positionOk = false;
    const int position = textAt(2).toInt(&positionOk);
    const QString index = textAt(3);
    QString subIndex = textAt(4);
    const QString type = textAt(5);
    const QString value = textAt(6);
    const QString status = textAt(7);
    const QString detail = textAt(8);
    if (!subIndex.isEmpty() &&
        !subIndex.startsWith("0x", Qt::CaseInsensitive)) {
      subIndex = QString("0x%1").arg(subIndex.rightJustified(2, '0'));
    }
    if (!positionOk || position < 0 || index.isEmpty() || subIndex.isEmpty() ||
        !isSdoHistoryStartupSource(status, value)) {
      ++skipped;
      continue;
    }

    const int startupRow = startupSdoTable_->rowCount();
    startupSdoTable_->insertRow(startupRow);
    startupSdoTable_->setItem(startupRow, 0,
                              new QTableWidgetItem(QString::number(position)));
    startupSdoTable_->setItem(startupRow, 1, new QTableWidgetItem(index));
    startupSdoTable_->setItem(startupRow, 2, new QTableWidgetItem(subIndex));
    startupSdoTable_->setItem(startupRow, 3, new QTableWidgetItem(value));
    startupSdoTable_->setItem(startupRow, 4, new QTableWidgetItem(type));
    startupSdoTable_->setItem(
        startupRow, 5, new QTableWidgetItem(uiText("Pending", "待应用")));
    startupSdoTable_->setItem(
        startupRow, 6,
        new QTableWidgetItem(
            QString("%1 %2%3")
                .arg(uiText("From SDO history row", "来自 SDO 历史行"))
                .arg(row + 1)
                .arg(detail.isEmpty() ? QString()
                                      : QString(": %1").arg(detail))));
    startupSdoTable_->selectRow(startupRow);
    ++created;
  }

  startupSdoTable_->resizeColumnsToContents(); // auto-fit column widths
  updateWatchStartupDeltas();
  updateStartupSdoControls();
  updateActionAvailability();
  updateDiagnostics(
      "Info", "Startup SDO",
      QString("Created %1 Startup SDO row(s) from selected SDO history rows%2")
          .arg(created)
          .arg(skipped > 0 ? QString(", skipped %1").arg(skipped) : QString()));
  if (created > 0) {
    activateWorkspaceTab(startupSdoTabIndex_);
  }
}


// — Add selected dictionary evidence to startup sdo
void MainWindow::addSelectedDictionaryEvidenceToStartupSdo() {
  const QVector<int> rows = selectedDictionaryRows();
  if (rows.isEmpty()) {
    return;
  }
  addDictionaryEvidenceRowsToStartupSdo(rows);
}


// — Add dictionary evidence rows to startup sdo
void MainWindow::addDictionaryEvidenceRowsToStartupSdo(
    const QVector<int> &rows) {
  if (selectedPosition() < 0 || !sdo_->sdoTable ||
      loadedSdoPosition_ != selectedPosition() || rows.isEmpty()) {
    return;
  }
  ensureStartupSdoTable();
  if (!startupSdoTable_) {
    return;
  }

  auto ensureStartupCell = [this](int row, int column) {
    auto *item = startupSdoTable_->item(row, column);
    if (!item) {
      item = new QTableWidgetItem;
      startupSdoTable_->setItem(row, column, item);
    }
    return item;
  };

  struct Candidate {
    int dictionaryRow = -1;
    int position = -1;
    QString index;
    QString subIndex;
    QString value;
    QString type;
    QString access;
    QString bits;
    QString name;
    QVector<int> startupRows;
  };

  QVector<int> uniqueRows = rows;
  std::sort(uniqueRows.begin(), uniqueRows.end());
  uniqueRows.erase(std::unique(uniqueRows.begin(), uniqueRows.end()),
                   uniqueRows.end());

  QVector<Candidate> candidates;
  QSet<QString> processedKeys;
  int skipped = 0;
  int duplicateSkipped = 0;
  const int position = selectedPosition();

  for (const int dictionaryRow : uniqueRows) {
    if (dictionaryRow < 0 || dictionaryRow >= sdo_->sdoTable->rowCount() ||
        sdo_->sdoTable->isRowHidden(dictionaryRow)) {
      ++skipped;
      continue;
    }
    const SdoDictionaryRow dictionary =
        sdoDictionaryRowFromTable(sdo_->sdoTable, dictionaryRow);
    if (position < 0 || !sdoDictionaryRowHasTarget(dictionary) ||
        !sdoDictionaryRowHasValue(dictionary)) {
      ++skipped;
      continue;
    }

    const QString key = QString("%1|%2|%3")
                            .arg(position)
                            .arg(dictionary.index, dictionary.subIndex);
    if (processedKeys.contains(key)) {
      ++duplicateSkipped;
      continue;
    }
    processedKeys.insert(key);

    Candidate candidate;
    candidate.dictionaryRow = dictionaryRow;
    candidate.position = position;
    candidate.index = dictionary.index;
    candidate.subIndex = dictionary.subIndex;
    candidate.value = dictionary.value;
    candidate.type = dictionary.type;
    candidate.access = dictionary.access;
    candidate.bits = dictionary.bits;
    candidate.name = dictionary.name;

    for (int startupRow = 0; startupRow < startupSdoTable_->rowCount();
         ++startupRow) {
      if (tableObjectAddressMatches(startupSdoTable_, startupRow, position,
                                    dictionary.index, dictionary.subIndex, 0, 1,
                                    2)) {
        candidate.startupRows.append(startupRow);
      }
    }

    candidates.append(candidate);
  }

  if (candidates.isEmpty()) {
    updateDiagnostics(
        "Warning", "Startup SDO",
        uiText("Startup creation skipped: selected Object Dictionary rows have "
               "no Last Value evidence%1",
               "创建启动项已跳过：所选对象字典行没有 Last Value 证据%1")
            .arg(
                skipped > 0
                    ? uiText(", skipped %1 row(s)", "，跳过 %1 行").arg(skipped)
                    : QString()));
    return;
  }

  int existingRowsAffected = 0;
  int newRowsPlanned = 0;
  int readOnlyRows = 0;
  for (const auto &candidate : candidates) {
    if (candidate.startupRows.isEmpty()) {
      ++newRowsPlanned;
    } else {
      existingRowsAffected += candidate.startupRows.size();
    }
    const QString access = candidate.access.toLower();
    if (access.contains("ro") || access.contains("const") ||
        candidate.access.contains(uiText("只读", "只读"))) {
      ++readOnlyRows;
    }
  }

  QStringList details = {
      uiText("Master: %1", "主站：%1").arg(activeMasterName()),
      uiText("Slave: #%1", "从站：#%1").arg(position),
      uiText("Accepted OD evidence values: %1", "可使用 OD 证据值：%1")
          .arg(candidates.size()),
      uiText("Existing Startup SDO rows to update: %1",
             "将更新的已有 Startup SDO 行：%1")
          .arg(existingRowsAffected),
      uiText("Missing Startup SDO rows to create: %1",
             "将创建的缺失 Startup SDO 行：%1")
          .arg(newRowsPlanned),
      uiText(
          "This changes only the Startup SDO table and does not read or write "
          "the bus.",
          "此操作只修改 Startup SDO 表，不会读取或写入总线。"),
  };
  if (readOnlyRows > 0) {
    details << uiText("Review access: %1 selected evidence row(s) look "
                      "read-only or constant.",
                      "请复核权限：%1 条所选证据行看起来是只读或常量。")
                   .arg(readOnlyRows);
  }
  const int previewRows = std::min(static_cast<int>(candidates.size()), 6);
  for (int i = 0; i < previewRows; ++i) {
    const auto &candidate = candidates.at(i);
    QString target =
        candidate.startupRows.isEmpty()
            ? uiText("create", "创建")
            : uiText("update startup row(s) %1", "更新启动行 %1")
                  .arg([&candidate] {
                    QStringList rowNumbers;
                    for (const int startupRow : candidate.startupRows) {
                      rowNumbers << QString::number(startupRow + 1);
                    }
                    return rowNumbers.join(", ");
                  }());
    details << QString("#%1  %2:%3 = %4  [%5]%6")
                   .arg(candidate.position)
                   .arg(candidate.index, candidate.subIndex, candidate.value,
                        target,
                        candidate.name.isEmpty()
                            ? QString()
                            : QString("  %1").arg(candidate.name));
  }
  if (candidates.size() > previewRows) {
    details << uiText("...and %1 more OD evidence value(s)",
                      "...另有 %1 个 OD 证据值")
                   .arg(candidates.size() - previewRows);
  }
  if (skipped > 0) {
    details << uiText("Skipped rows without address or Last Value: %1",
                      "已跳过缺少地址或 Last Value 的行：%1")
                   .arg(skipped);
  }
  if (duplicateSkipped > 0) {
    details << uiText("Skipped duplicate selected OD address(es): %1",
                      "已跳过重复选中 OD 地址：%1")
                   .arg(duplicateSkipped);
  }

  // Safety gate: require explicit confirmation before bus write
  if (!confirmDangerousOperation(
          uiText("Confirm Startup from OD Evidence",
                 "确认从 OD 证据创建 Startup SDO"),
          uiText("Create or update Startup SDO rows from selected Object "
                 "Dictionary evidence.",
                 "从所选对象字典证据创建或更新 Startup SDO 行。"),
          details, uiText("Create Startup", "创建启动项"))) {
    return;
  }

  const QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");
  int updated = 0;
  int unchanged = 0;
  int created = 0;
  int lastTouchedRow = -1;

  for (const auto &candidate : candidates) {
    if (candidate.startupRows.isEmpty()) {
      const int startupRow = startupSdoTable_->rowCount();
      startupSdoTable_->insertRow(startupRow);
      startupSdoTable_->setItem(
          startupRow, 0,
          new QTableWidgetItem(QString::number(candidate.position)));
      startupSdoTable_->setItem(startupRow, 1,
                                new QTableWidgetItem(candidate.index));
      startupSdoTable_->setItem(startupRow, 2,
                                new QTableWidgetItem(candidate.subIndex));
      startupSdoTable_->setItem(startupRow, 3,
                                new QTableWidgetItem(candidate.value));
      startupSdoTable_->setItem(startupRow, 4,
                                new QTableWidgetItem(candidate.type));
      startupSdoTable_->setItem(
          startupRow, 5, new QTableWidgetItem(uiText("From OD", "来自 OD")));
      startupSdoTable_->setItem(
          startupRow, 6,
          new QTableWidgetItem(
              uiText("Created from Object Dictionary row %1 at %2%3",
                     "由对象字典第 %1 行在 %2 创建%3")
                  .arg(candidate.dictionaryRow + 1)
                  .arg(timestamp,
                       candidate.access.isEmpty()
                           ? QString()
                           : QString(" (%1)").arg(candidate.access))));
      for (int column = 7; column < startupSdoTable_->columnCount(); ++column) {
        startupSdoTable_->setItem(startupRow, column, new QTableWidgetItem);
      }
      ++created;
      lastTouchedRow = startupRow;
      continue;
    }

    for (const int startupRow : candidate.startupRows) {
      if (startupRow < 0 || startupRow >= startupSdoTable_->rowCount()) {
        continue;
      }
      const QString previousValue = tableText(startupSdoTable_, startupRow, 3);
      ensureStartupCell(startupRow, 3)->setText(candidate.value);
      if (!candidate.type.isEmpty()) {
        ensureStartupCell(startupRow, 4)->setText(candidate.type);
      }
      ensureStartupCell(startupRow, 5)->setText(uiText("From OD", "来自 OD"));
      ensureStartupCell(startupRow, 6)
          ->setText(
              previousValue.compare(candidate.value, Qt::CaseInsensitive) == 0
                  ? uiText("Confirmed from Object Dictionary row %1 at %2",
                           "由对象字典第 %1 行在 %2 确认")
                        .arg(candidate.dictionaryRow + 1)
                        .arg(timestamp)
                  : uiText("Updated from Object Dictionary row %1 at %2; "
                           "previous value: %3",
                           "由对象字典第 %1 行在 %2 更新；原值：%3")
                        .arg(candidate.dictionaryRow + 1)
                        .arg(timestamp, previousValue));
      if (previousValue.compare(candidate.value, Qt::CaseInsensitive) == 0) {
        ++unchanged;
      } else {
        ++updated;
      }
      lastTouchedRow = startupRow;
    }
  }

  if (lastTouchedRow >= 0) {
    startupSdoTable_->selectRow(lastTouchedRow);
  }
  startupSdoTable_->resizeColumnsToContents(); // auto-fit column widths
  updateWatchStartupDeltas();
  filterStartupSdoTable();
  updateStartupSdoControls();
  updateActionAvailability();
  updateDiagnostics(
      "Info", "Startup SDO",
      uiText("Startup from OD evidence: %1 updated, %2 unchanged, %3 "
             "created%4%5",
             "从 OD 证据生成 Startup：更新 %1，未变 %2，新建 %3%4%5")
          .arg(updated)
          .arg(unchanged)
          .arg(created)
          .arg(skipped > 0 ? uiText(", skipped %1", "，跳过 %1").arg(skipped)
                           : QString())
          .arg(duplicateSkipped > 0
                   ? uiText(", duplicate selections %1", "，重复选择 %1")
                         .arg(duplicateSkipped)
                   : QString()));
  if (created > 0 || updated > 0 || unchanged > 0) {
    activateWorkspaceTab(startupSdoTabIndex_);
  }
}


