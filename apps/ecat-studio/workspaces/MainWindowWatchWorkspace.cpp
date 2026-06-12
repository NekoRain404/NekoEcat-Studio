#include "MainWindow.h"

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


// — Create the Watch table columns if not yet initialized
void MainWindow::ensureWatchTable() {
  if (!watchTable_) {
    return;
  }
  if (watchTable_->columnCount() != 12) {
    watchTable_->setColumnCount(12);
  }
  watchTable_->setHorizontalHeaderLabels(
      {uiText("Time", "时间"), uiText("Slave", "从站"), uiText("Index", "索引"),
       uiText("Sub", "子项"), uiText("Value", "值"), uiText("Decoded", "解析"),
       uiText("Type", "类型"), uiText("Mode", "模式"),
       uiText("Baseline", "基线"), uiText("Delta", "偏差"),
       uiText("Startup", "启动值"), uiText("Startup Delta", "启动偏差")});
}


// — Decode a raw Watch value using CiA 402 object semantics, if applicable
QString MainWindow::decodeWatchValue(const QString &index,
                                     const QString &subIndex,
                                     const QString &type, const QString &value,
                                     const QString &mode) const {
  Q_UNUSED(subIndex);
  const QString normalizedIndex = index.trimmed().toLower();
  if (!isCia402Object(index, mode)) {
    Q_UNUSED(type);
    return QString();
  }

  const QString decoded = decodeCia402Value(index, value);
  if (decoded == "Actual position") {
    return uiText("Actual position", "实际位置");
  }
  if (decoded == "Actual velocity") {
    return uiText("Actual velocity", "实际速度");
  }
  if (decoded == "Actual torque") {
    return uiText("Actual torque", "实际转矩");
  }
  if (decoded == "Target position") {
    return uiText("Target position", "目标位置");
  }
  if (decoded == "Target velocity") {
    return uiText("Target velocity", "目标速度");
  }
  if (decoded == "Target torque") {
    return uiText("Target torque", "目标转矩");
  }
  Q_UNUSED(type);
  Q_UNUSED(normalizedIndex);
  return decoded;
}


// — Add current sdo to watch
void MainWindow::addCurrentSdoToWatch(bool requestRead) {
  const int position = selectedPosition();
  if (position < 0) {
    return;
  }
  ensureWatchTable();
  const QString index = sdoIndex_ ? sdoIndex_->text().trimmed() : QString();
  const QString subIndex =
      sdoSubIndex_ ? sdoSubIndex_->text().trimmed() : QString();
  if (index.isEmpty() || subIndex.isEmpty()) {
    return;
  }

  for (int row = 0; row < watchTable_->rowCount(); ++row) {
    const bool match =
        (watchTable_->item(row, 1) &&
         watchTable_->item(row, 1)->text().toInt() == position) &&
        (watchTable_->item(row, 2) && watchTable_->item(row, 2)->text().compare(
                                          index, Qt::CaseInsensitive) == 0) &&
        (watchTable_->item(row, 3) && watchTable_->item(row, 3)->text().compare(
                                          subIndex, Qt::CaseInsensitive) == 0);
    if (match) {
      watchTable_->selectRow(row);
      const QString type = sdoType_ ? sdoType_->currentText() : QString();
      if (!type.isEmpty() &&
          (!watchTable_->item(row, 6) ||
           watchTable_->item(row, 6)->text().trimmed().isEmpty())) {
        watchTable_->setItem(row, 6, new QTableWidgetItem(type));
      }
      if (requestRead && client_.isConnected()) {
        const QString type =
            watchTable_->item(row, 6)
                ? watchTable_->item(row, 6)->text().trimmed()
                : (sdoType_ ? sdoType_->currentText() : QString());
        requestSdoRead(position, index, subIndex,
                       uiText("Existing Watch item", "已有监视项"), type);
      }
      updateWatchAutoRefresh();
      return;
    }
  }

  const int row = watchTable_->rowCount();
  watchTable_->insertRow(row);
  watchTable_->setItem(
      row, 0,
      new QTableWidgetItem(QDateTime::currentDateTime().toString("HH:mm:ss")));
  watchTable_->setItem(row, 1, new QTableWidgetItem(QString::number(position)));
  watchTable_->setItem(row, 2, new QTableWidgetItem(index));
  watchTable_->setItem(row, 3, new QTableWidgetItem(subIndex));
  watchTable_->setItem(
      row, 4, new QTableWidgetItem(sdoValue_ ? sdoValue_->text() : QString()));
  watchTable_->setItem(
      row, 5,
      new QTableWidgetItem(decodeWatchValue(
          index, subIndex, sdoType_ ? sdoType_->currentText() : QString(),
          sdoValue_ ? sdoValue_->text() : QString(), "Watch")));
  watchTable_->setItem(
      row, 6,
      new QTableWidgetItem(sdoType_ ? sdoType_->currentText() : QString()));
  watchTable_->setItem(row, 7, new QTableWidgetItem("Watch"));
  watchTable_->setItem(row, 8, new QTableWidgetItem);
  watchTable_->setItem(row, 9, new QTableWidgetItem);
  watchTable_->setItem(row, 10, new QTableWidgetItem);
  watchTable_->setItem(row, 11, new QTableWidgetItem);
  updateWatchStartupDelta(row);
  watchTable_->resizeColumnsToContents(); // auto-fit column widths
  watchTable_->selectRow(row);
  updateDiagnostics(
      "Info", "Watch",
      QString("Added SDO watch #%1 %2:%3").arg(position).arg(index, subIndex));
  if (requestRead && client_.isConnected()) {
    const QString type = sdoType_ ? sdoType_->currentText() : QString();
    requestSdoRead(position, index, subIndex,
                   uiText("Add to Watch", "加入监视"), type);
  }
  updateWatchAutoRefresh();
}


// — Add selected history rows to watch
void MainWindow::addSelectedHistoryRowsToWatch() {
  if (!sdoHistoryTable_) {
    return;
  }
  const QVector<int> rows = selectedSdoHistoryRows();
  if (rows.isEmpty()) {
    return;
  }

  ensureWatchTable();
  int addedOrReused = 0;
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
    if (!positionOk || position < 0 || index.isEmpty() || subIndex.isEmpty()) {
      ++skipped;
      continue;
    }

    int watchRow = -1;
    for (int existingRow = 0; existingRow < watchTable_->rowCount();
         ++existingRow) {
      const bool match =
          (watchTable_->item(existingRow, 1) &&
           watchTable_->item(existingRow, 1)->text().toInt() == position) &&
          (watchTable_->item(existingRow, 2) &&
           watchTable_->item(existingRow, 2)
                   ->text()
                   .compare(index, Qt::CaseInsensitive) == 0) &&
          (watchTable_->item(existingRow, 3) &&
           watchTable_->item(existingRow, 3)
                   ->text()
                   .compare(subIndex, Qt::CaseInsensitive) == 0);
      if (match) {
        watchRow = existingRow;
        break;
      }
    }

    if (watchRow < 0) {
      watchRow = watchTable_->rowCount();
      watchTable_->insertRow(watchRow);
      watchTable_->setItem(
          watchRow, 0,
          new QTableWidgetItem(
              QDateTime::currentDateTime().toString("HH:mm:ss")));
      watchTable_->setItem(watchRow, 1,
                           new QTableWidgetItem(QString::number(position)));
      watchTable_->setItem(watchRow, 2, new QTableWidgetItem(index));
      watchTable_->setItem(watchRow, 3, new QTableWidgetItem(subIndex));
      watchTable_->setItem(watchRow, 8, new QTableWidgetItem);
      watchTable_->setItem(watchRow, 9, new QTableWidgetItem);
      watchTable_->setItem(watchRow, 10, new QTableWidgetItem);
      watchTable_->setItem(watchRow, 11, new QTableWidgetItem);
    } else {
      auto *timeItem = watchTable_->item(watchRow, 0);
      if (!timeItem) {
        timeItem = new QTableWidgetItem;
        watchTable_->setItem(watchRow, 0, timeItem);
      }
      timeItem->setText(QDateTime::currentDateTime().toString("HH:mm:ss"));
    }

    if (!value.isEmpty()) {
      watchTable_->setItem(watchRow, 4, new QTableWidgetItem(value));
      const QString key =
          QString("%1|%2|%3").arg(position).arg(index, subIndex);
      watchValues_[key] = value;
    } else if (!watchTable_->item(watchRow, 4)) {
      watchTable_->setItem(watchRow, 4, new QTableWidgetItem);
    }

    const QString effectiveValue =
        value.isEmpty() && watchTable_->item(watchRow, 4)
            ? watchTable_->item(watchRow, 4)->text().trimmed()
            : value;
    const QString decoded =
        decodeWatchValue(index, subIndex, type, effectiveValue,
                         uiText("SDO History", "SDO 历史"));
    watchTable_->setItem(watchRow, 5, new QTableWidgetItem(decoded));
    watchTable_->setItem(watchRow, 6, new QTableWidgetItem(type));
    watchTable_->setItem(watchRow, 7,
                         new QTableWidgetItem(uiText("History", "历史")));
    if (!status.isEmpty() && watchTable_->item(watchRow, 7)) {
      watchTable_->item(watchRow, 7)
          ->setToolTip(detail.isEmpty()
                           ? status
                           : QString("%1 - %2").arg(status, detail));
    }
    updateWatchBaselineDelta(watchRow);
    updateWatchStartupDelta(watchRow);
    watchTable_->selectRow(watchRow);
    ++addedOrReused;
  }

  watchTable_->resizeColumnsToContents(); // auto-fit column widths
  filterWatchTable();
  updateSelectedDriveSummary();
  updateActionAvailability();
  updateDiagnostics(
      "Info", "Watch",
      QString("Added/reused %1 SDO history row(s) in Watch%2; use Refresh "
              "Watch to read current values")
          .arg(addedOrReused)
          .arg(skipped > 0 ? QString(", skipped %1").arg(skipped) : QString()));
  if (addedOrReused > 0) {
    activateWorkspaceTab(watchTabIndex_);
  }
}


// — Add selected pdo entries to watch
void MainWindow::addSelectedPdoEntriesToWatch() {
  const int position = selectedPosition();
  if (position < 0 || !pdoTable_ || loadedPdoPosition_ != position) {
    return;
  }
  ensureWatchTable();

  const QVector<int> rows = selectedTableRows(pdoTable_);

  const QString previousIndex = sdoIndex_ ? sdoIndex_->text() : QString();
  const QString previousSubIndex =
      sdoSubIndex_ ? sdoSubIndex_->text() : QString();
  const QString previousType = sdoType_ ? sdoType_->currentText() : QString();
  const QString previousReadValue = sdoValue_ ? sdoValue_->text() : QString();
  const QString previousWriteValue =
      sdoWriteValue_ ? sdoWriteValue_->text() : QString();
  const bool previousWritable = selectedSdoWritable_;

  int addedOrReused = 0;
  int skipped = 0;
  for (const int row : rows) {
    if (row < 0 || row >= pdoTable_->rowCount() ||
        pdoTable_->isRowHidden(row)) {
      continue;
    }
    auto textAt = [this, row](int column) {
      const auto *item = pdoTable_->item(row, column);
      return item ? item->text().trimmed() : QString();
    };
    const QString index = textAt(2);
    const QString subIndex = textAt(3);
    if (index.isEmpty() || subIndex.isEmpty()) {
      ++skipped;
      continue;
    }

    {
      const QSignalBlocker indexBlocker(sdoIndex_); // prevent recursive signal updates
      const QSignalBlocker subIndexBlocker(sdoSubIndex_); // prevent recursive signal updates
      const QSignalBlocker typeBlocker(sdoType_); // prevent recursive signal updates
      if (sdoIndex_) {
        sdoIndex_->setText(index);
      }
      if (sdoSubIndex_) {
        sdoSubIndex_->setText(
            QString("0x%1").arg(subIndex.rightJustified(2, '0')));
      }
      if (sdoType_) {
        sdoType_->setCurrentIndex(0);
      }
      if (sdoValue_) {
        sdoValue_->clear();
      }
    }
    selectedSdoWritable_ = true;
    addCurrentSdoToWatch(false);
    const int watchRow = watchTable_ ? watchTable_->currentRow() : -1;
    if (watchRow >= 0) {
      const QString mode =
          textAt(0).contains("Rx", Qt::CaseInsensitive) ||
                  textAt(1).contains("RxPDO", Qt::CaseInsensitive)
              ? "RxPDO"
              : (textAt(0).contains("Tx", Qt::CaseInsensitive) ||
                         textAt(1).contains("TxPDO", Qt::CaseInsensitive)
                     ? "TxPDO"
                     : "PDO");
      watchTable_->setItem(watchRow, 7, new QTableWidgetItem(mode));
    }
    ++addedOrReused;
  }

  {
    const QSignalBlocker indexBlocker(sdoIndex_); // prevent recursive signal updates
    const QSignalBlocker subIndexBlocker(sdoSubIndex_); // prevent recursive signal updates
    const QSignalBlocker typeBlocker(sdoType_); // prevent recursive signal updates
    if (sdoIndex_) {
      sdoIndex_->setText(previousIndex);
    }
    if (sdoSubIndex_) {
      sdoSubIndex_->setText(previousSubIndex);
    }
    if (sdoType_) {
      sdoType_->setCurrentText(previousType);
    }
  }
  if (sdoValue_) {
    sdoValue_->setText(previousReadValue);
  }
  if (sdoWriteValue_) {
    sdoWriteValue_->setText(previousWriteValue);
  }
  selectedSdoWritable_ = previousWritable;
  updateSdoInspector(uiText("Restored selection", "已恢复选择"));
  if (watchTable_) {
    watchTable_->resizeColumnsToContents(); // auto-fit column widths
  }
  filterWatchTable();
  updateSelectedDriveSummary();
  updateActionAvailability();
  updateDiagnostics(
      "Info", "Watch",
      QString("Added/reused %1 PDO map item(s) in Watch for slave #%2%3; use "
              "Refresh Watch to read values")
          .arg(addedOrReused)
          .arg(position)
          .arg(skipped > 0 ? QString(", skipped %1").arg(skipped) : QString()));
  activateWorkspaceTab(watchTabIndex_);
}


// — Add cia 402 watch preset
void MainWindow::addCia402WatchPreset() {
  const int position = selectedPosition();
  if (position < 0) {
    return;
  }
  ensureWatchTable();

  struct PresetObject {
    QString index;
    QString subIndex;
    QString type;
    QString note;
  };
  const QVector<PresetObject> objects = {
      {"0x6040", "0x00", "uint16", "Controlword"},
      {"0x6041", "0x00", "uint16", "Statusword"},
      {"0x6060", "0x00", "int8", "Mode of operation"},
      {"0x6061", "0x00", "int8", "Mode display"},
      {"0x603f", "0x00", "uint16", "Error code"},
      {"0x6064", "0x00", "int32", "Position actual value"},
      {"0x606c", "0x00", "int32", "Velocity actual value"},
      {"0x6077", "0x00", "int16", "Torque actual value"},
      {"0x607a", "0x00", "int32", "Target position"},
      {"0x60ff", "0x00", "int32", "Target velocity"},
      {"0x6071", "0x00", "int16", "Target torque"},
  };

  const QString previousIndex = sdoIndex_ ? sdoIndex_->text() : QString();
  const QString previousSubIndex =
      sdoSubIndex_ ? sdoSubIndex_->text() : QString();
  const QString previousType = sdoType_ ? sdoType_->currentText() : QString();
  const QString previousReadValue = sdoValue_ ? sdoValue_->text() : QString();
  const QString previousWriteValue =
      sdoWriteValue_ ? sdoWriteValue_->text() : QString();
  const bool previousWritable = selectedSdoWritable_;
  const int before = watchTable_ ? watchTable_->rowCount() : 0;

  for (const auto &object : objects) {
    {
      const QSignalBlocker indexBlocker(sdoIndex_); // prevent recursive signal updates
      const QSignalBlocker subIndexBlocker(sdoSubIndex_); // prevent recursive signal updates
      const QSignalBlocker typeBlocker(sdoType_); // prevent recursive signal updates
      if (sdoIndex_) {
        sdoIndex_->setText(object.index);
      }
      if (sdoSubIndex_) {
        sdoSubIndex_->setText(object.subIndex);
      }
      if (sdoType_) {
        sdoType_->setCurrentText(object.type);
      }
      if (sdoValue_) {
        sdoValue_->clear();
      }
    }
    selectedSdoWritable_ = true;
    addCurrentSdoToWatch();
    const int row = watchTable_ ? watchTable_->currentRow() : -1;
    if (row >= 0) {
      watchTable_->setItem(row, 5, new QTableWidgetItem(object.note));
      watchTable_->setItem(row, 7, new QTableWidgetItem("CiA 402"));
    }
  }

  {
    const QSignalBlocker indexBlocker(sdoIndex_); // prevent recursive signal updates
    const QSignalBlocker subIndexBlocker(sdoSubIndex_); // prevent recursive signal updates
    const QSignalBlocker typeBlocker(sdoType_); // prevent recursive signal updates
    if (sdoIndex_) {
      sdoIndex_->setText(previousIndex);
    }
    if (sdoSubIndex_) {
      sdoSubIndex_->setText(previousSubIndex);
    }
    if (sdoType_) {
      sdoType_->setCurrentText(previousType);
    }
  }
  if (sdoValue_) {
    sdoValue_->setText(previousReadValue);
  }
  if (sdoWriteValue_) {
    sdoWriteValue_->setText(previousWriteValue);
  }
  selectedSdoWritable_ = previousWritable;
  updateSdoInspector(uiText("Restored selection", "已恢复选择"));
  updateActionAvailability();

  const int after = watchTable_ ? watchTable_->rowCount() : before;
  updateDiagnostics(
      "Info", "Watch",
      QString("CiA 402 watch preset added for slave #%1: %2 new item(s)")
          .arg(position)
          .arg(qMax(0, after - before)));
  activateWorkspaceTab(watchTabIndex_);
}


// — Insert a Startup SDO row from the selected Watch row
void MainWindow::addStartupSdoFromWatchRow(int row) {
  if (!watchTable_ || row < 0 || row >= watchTable_->rowCount()) {
    return;
  }

  auto textAt = [this, row](int column) {
    const auto *item = watchTable_->item(row, column);
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
  const QSignalBlocker indexBlocker(sdoIndex_); // prevent recursive signal updates
  const QSignalBlocker subIndexBlocker(sdoSubIndex_); // prevent recursive signal updates
  const QSignalBlocker typeBlocker(sdoType_); // prevent recursive signal updates
  sdoIndex_->setText(index);
  sdoSubIndex_->setText(subIndex);
  if (sdoValue_) {
    sdoValue_->setText(value);
  }
  if (sdoType_) {
    sdoType_->setCurrentText(type);
  }
  selectedSdoWritable_ = true;
  if (sdoWriteValue_) {
    sdoWriteValue_->setEnabled(true);
    sdoWriteValue_->setText(value);
    sdoWriteValue_->setPlaceholderText(
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
  if (!watchTable_ || !watchTable_->selectionModel()) {
    return;
  }

  const QVector<int> rows = selectedTableRows(watchTable_);

  const int previousPosition = selectedPosition();
  const QString previousIndex = sdoIndex_ ? sdoIndex_->text() : QString();
  const QString previousSubIndex =
      sdoSubIndex_ ? sdoSubIndex_->text() : QString();
  const QString previousType = sdoType_ ? sdoType_->currentText() : QString();
  const QString previousReadValue = sdoValue_ ? sdoValue_->text() : QString();
  const QString previousWriteValue =
      sdoWriteValue_ ? sdoWriteValue_->text() : QString();
  const bool previousWritable = selectedSdoWritable_;

  int created = 0;
  int skipped = 0;
  for (const int row : rows) {
    if (row < 0 || row >= watchTable_->rowCount() ||
        watchTable_->isRowHidden(row)) {
      continue;
    }
    const QString value = watchTable_->item(row, 4)
                              ? watchTable_->item(row, 4)->text().trimmed()
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
    const QSignalBlocker indexBlocker(sdoIndex_); // prevent recursive signal updates
    const QSignalBlocker subIndexBlocker(sdoSubIndex_); // prevent recursive signal updates
    const QSignalBlocker typeBlocker(sdoType_); // prevent recursive signal updates
    if (sdoIndex_) {
      sdoIndex_->setText(previousIndex);
    }
    if (sdoSubIndex_) {
      sdoSubIndex_->setText(previousSubIndex);
    }
    if (sdoType_) {
      sdoType_->setCurrentText(previousType);
    }
  }
  if (sdoValue_) {
    sdoValue_->setText(previousReadValue);
  }
  if (sdoWriteValue_) {
    sdoWriteValue_->setText(previousWriteValue);
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
  if (!watchTable_ || rows.isEmpty()) {
    return;
  }

  ensureWatchTable();
  ensureStartupSdoTable();

  auto watchTextAt = [this](int row, int column) {
    const auto *item = watchTable_->item(row, column);
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
    if (row < 0 || row >= watchTable_->rowCount() ||
        watchTable_->isRowHidden(row)) {
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
  if (!watchTable_) {
    return;
  }

  syncWatchRowsToStartupSdo(selectedTableRows(watchTable_));
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
  if (selectedPosition() < 0 || !sdoTable_ ||
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
    if (dictionaryRow < 0 || dictionaryRow >= sdoTable_->rowCount() ||
        sdoTable_->isRowHidden(dictionaryRow)) {
      ++skipped;
      continue;
    }
    const SdoDictionaryRow dictionary =
        sdoDictionaryRowFromTable(sdoTable_, dictionaryRow);
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


// — Refresh watch list
void MainWindow::refreshWatchList(bool quiet) {
  if (!client_.isConnected() || !watchTable_) {
    updateWatchAutoRefresh();
    return;
  }
  ensureWatchTable();
  int requested = 0;
  for (int row = 0; row < watchTable_->rowCount(); ++row) {
    const int position = watchTable_->item(row, 1)
                             ? watchTable_->item(row, 1)->text().toInt()
                             : -1;
    const QString index = watchTable_->item(row, 2)
                              ? watchTable_->item(row, 2)->text().trimmed()
                              : QString();
    const QString subIndex = watchTable_->item(row, 3)
                                 ? watchTable_->item(row, 3)->text().trimmed()
                                 : QString();
    const QString type = watchTable_->item(row, 6)
                             ? watchTable_->item(row, 6)->text().trimmed()
                             : QString();
    if (position >= 0 && !index.isEmpty() && !subIndex.isEmpty()) {
      requestSdoRead(position, index, subIndex,
                     quiet ? uiText("Watch auto refresh", "监视自动刷新")
                           : uiText("Watch manual refresh", "监视手动刷新"),
                     type);
      ++requested;
    }
  }
  if (!quiet) {
    updateDiagnostics(
        "Info", "Watch",
        QString("Refresh requested for %1 SDO watch item(s)").arg(requested));
  }
  updateWatchAutoRefresh();
}


// — Capture watch baseline
void MainWindow::captureWatchBaseline() {
  if (!watchTable_) {
    return;
  }
  ensureWatchTable();
  int captured = 0;
  for (int row = 0; row < watchTable_->rowCount(); ++row) {
    const QString value = watchTable_->item(row, 4)
                              ? watchTable_->item(row, 4)->text().trimmed()
                              : QString();
    if (value.isEmpty()) {
      continue;
    }
    watchTable_->setItem(row, 8, new QTableWidgetItem(value));
    ++captured;
    updateWatchBaselineDelta(row);
  }
  watchTable_->resizeColumnsToContents(); // auto-fit column widths
  filterWatchTable();
  updateActionAvailability();
  updateDiagnostics(
      "Info", "Watch",
      QString("Captured Watch baseline for %1 row(s)").arg(captured));
}


// — Clear watch baseline
void MainWindow::clearWatchBaseline() {
  if (!watchTable_) {
    return;
  }
  ensureWatchTable();
  int cleared = 0;
  for (int row = 0; row < watchTable_->rowCount(); ++row) {
    if (watchTable_->item(row, 8) &&
        !watchTable_->item(row, 8)->text().trimmed().isEmpty()) {
      ++cleared;
    }
    watchTable_->setItem(row, 8, new QTableWidgetItem);
    watchTable_->setItem(row, 9, new QTableWidgetItem);
  }
  watchTable_->resizeColumnsToContents(); // auto-fit column widths
  filterWatchTable();
  updateActionAvailability();
  updateDiagnostics(
      "Info", "Watch",
      QString("Cleared Watch baseline for %1 row(s)").arg(cleared));
}


// — Compute and color the baseline delta cell for a single Watch row
void MainWindow::updateWatchBaselineDelta(int row) {
  if (!watchTable_ || row < 0 || row >= watchTable_->rowCount()) {
    return;
  }
  ensureWatchTable();
  const QString value = watchTable_->item(row, 4)
                            ? watchTable_->item(row, 4)->text().trimmed()
                            : QString();
  const QString baseline = watchTable_->item(row, 8)
                               ? watchTable_->item(row, 8)->text().trimmed()
                               : QString();
  auto *deltaItem = watchTable_->item(row, 9);
  if (!deltaItem) {
    deltaItem = new QTableWidgetItem;
    watchTable_->setItem(row, 9, deltaItem);
  }
  if (value.isEmpty() || baseline.isEmpty()) {
    deltaItem->setText(QString());
    deltaItem->setBackground(QBrush());
    deltaItem->setForeground(QBrush());
    return;
  }

  // Parse a numeric string to double for delta comparison
  auto parseNumber = [](QString text, double *out) {
    text = text.trimmed();
    bool ok = false;
    if (text.startsWith("0x", Qt::CaseInsensitive)) {
      const QString digits = text.mid(2);
      const qint64 parsed = digits.toLongLong(&ok, 16);
      if (ok) {
        *out = static_cast<double>(parsed);
        return true;
      }
      const quint64 unsignedParsed = digits.toULongLong(&ok, 16);
      if (ok) {
        *out = static_cast<double>(unsignedParsed);
        return true;
      }
      return false;
    }
    *out = text.toDouble(&ok);
    return ok;
  };

  double numericValue = 0.0;
  double numericBaseline = 0.0;
  const bool numeric = parseNumber(value, &numericValue) &&
                       parseNumber(baseline, &numericBaseline);
  const bool same = value.compare(baseline, Qt::CaseInsensitive) == 0;
  if (numeric) {
    const double delta = numericValue - numericBaseline;
    const bool effectivelyZero = std::abs(delta) < 1e-12;
    deltaItem->setText(effectivelyZero ? QString("0")
                                       : QString::number(delta, 'g', 12));
  } else {
    deltaItem->setText(same ? uiText("match", "匹配")
                            : uiText("changed", "已变化"));
  }

  const bool changed =
      numeric ? std::abs(numericValue - numericBaseline) >= 1e-12 : !same;
  if (changed) {
    deltaItem->setBackground(settings_.theme == "Light" ? QColor("#fff7cc")
                                                        : QColor("#3a2f16"));
    deltaItem->setForeground(settings_.theme == "Light" ? QColor("#854d0e")
                                                        : QColor("#fde68a"));
  } else {
    deltaItem->setBackground(settings_.theme == "Light" ? QColor("#dcfce7")
                                                        : QColor("#12351f"));
    deltaItem->setForeground(settings_.theme == "Light" ? QColor("#166534")
                                                        : QColor("#86efac"));
  }
}


// — Recompute baseline deltas for every Watch row
void MainWindow::updateWatchBaselineDeltas() {
  if (!watchTable_) {
    return;
  }
  ensureWatchTable();
  for (int row = 0; row < watchTable_->rowCount(); ++row) {
    updateWatchBaselineDelta(row);
  }
}


// — Compute and color the Startup SDO delta cell for a single Watch row
void MainWindow::updateWatchStartupDelta(int row) {
  if (!watchTable_ || row < 0 || row >= watchTable_->rowCount()) {
    return;
  }
  ensureWatchTable();

  const WatchStartupWatchRow watchRow = watchStartupWatchRow(watchTable_, row);

  auto *expectedItem = ensureWatchStartupTableItem(
      watchTable_, row, kWatchStartupWatchExpectedColumn);
  auto *deltaItem = ensureWatchStartupTableItem(watchTable_, row,
                                                kWatchStartupWatchDeltaColumn);
  clearWatchStartupDeltaCells(expectedItem, deltaItem);

  if (!startupSdoTable_ ||
      !watchStartupHasTarget(watchRow.position, watchRow.index,
                             watchRow.subIndex)) {
    return;
  }

  const WatchStartupWatchMatch match = watchStartupMatchForWatchRow(
      watchStartupStartupRows(startupSdoTable_), watchRow);
  if (match.state == WatchStartupDeltaState::NoStartup ||
      match.state == WatchStartupDeltaState::NoTarget) {
    return;
  }

  expectedItem->setText(
      match.matchingStartupRows > 1
          ? uiText("%1 (%2 startup rows)", "%1（%2 个启动项）")
                .arg(match.expectedValue)
                .arg(match.matchingStartupRows)
          : match.expectedValue);
  if (!match.expectedType.isEmpty()) {
    expectedItem->setToolTip(
        uiText("Startup type: %1", "启动类型：%1").arg(match.expectedType));
  }

  applyWatchStartupDeltaCells(nullptr, deltaItem, match.state, QString(),
                              watchStartupDeltaTexts(),
                              settings_.theme == "Light");
}


// — Recompute Startup SDO deltas for every Watch row
void MainWindow::updateWatchStartupDeltas() {
  if (!watchTable_) {
    return;
  }
  ensureWatchTable();
  for (int row = 0; row < watchTable_->rowCount(); ++row) {
    updateWatchStartupDelta(row);
  }
  updateStartupSdoWatchEvidence();
  updateIoVariableTable();
  updateStateMachineView();
}


// — Start or stop the Watch auto-refresh timer based on user settings
void MainWindow::updateWatchAutoRefresh() {
  const bool hasItems = watchTable_ && watchTable_->rowCount() > 0;
  const bool autoEnabled = watchAutoRefresh_ && watchAutoRefresh_->isChecked();
  const int interval = watchRefreshInterval_
                           ? watchRefreshInterval_->currentData().toInt()
                           : 1000;

  if (watchRefreshTimer_) {
    // Only tick when connected, items exist, and user opted in
    if (client_.isConnected() && hasItems && autoEnabled) {
      watchRefreshTimer_->start(interval > 0 ? interval : 1000);
    } else {
      watchRefreshTimer_->stop();
    }
  }
  if (watchRefreshInterval_) {
    watchRefreshInterval_->setEnabled(autoEnabled);
  }
  filterWatchTable();
  updateStartupSdoWatchEvidence();
  updateSelectedDriveSummary();
  updateActionAvailability();
  updateIoVariableTable();
  updateStateMachineView();
}


// — Remove all Watch entries and reset tracking state
void MainWindow::clearWatchList() {
  if (!watchTable_) {
    return;
  }
  watchTable_->clearContents();
  watchTable_->setRowCount(0);
  watchValues_.clear();
  watchChangedKeys_.clear();
  ensureWatchTable();
  updateWatchAutoRefresh();
  updateSelectedDriveSummary();
  updateIoVariableTable();
  updateStateMachineView();
}

