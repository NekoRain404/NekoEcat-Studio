// Watch list, add-to-watch, and baseline comparison.

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


// — Create the Watch table columns if not yet initialized
void MainWindow::ensureWatchTable() {
  if (!watch_->watchTable) {
    return;
  }
  if (watch_->watchTable->columnCount() != 12) {
    watch_->watchTable->setColumnCount(12);
  }
    // Define column headers for the table
  watch_->watchTable->setHorizontalHeaderLabels(
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
  const QString index = sdoInspector_->sdoIndex ? sdoInspector_->sdoIndex->text().trimmed() : QString();
  const QString subIndex =
      sdoInspector_->sdoSubIndex ? sdoInspector_->sdoSubIndex->text().trimmed() : QString();
  if (index.isEmpty() || subIndex.isEmpty()) {
    return;
  }

  for (int row = 0; row < watch_->watchTable->rowCount(); ++row) {
    const bool match =
        (watch_->watchTable->item(row, 1) &&
         watch_->watchTable->item(row, 1)->text().toInt() == position) &&
        (watch_->watchTable->item(row, 2) && watch_->watchTable->item(row, 2)->text().compare(
                                          index, Qt::CaseInsensitive) == 0) &&
        (watch_->watchTable->item(row, 3) && watch_->watchTable->item(row, 3)->text().compare(
                                          subIndex, Qt::CaseInsensitive) == 0);
    if (match) {
      watch_->watchTable->selectRow(row);
      const QString type = sdoInspector_->sdoType ? sdoInspector_->sdoType->currentText() : QString();
      if (!type.isEmpty() &&
          (!watch_->watchTable->item(row, 6) ||
           watch_->watchTable->item(row, 6)->text().trimmed().isEmpty())) {
    // Create table cell
        watch_->watchTable->setItem(row, 6, new QTableWidgetItem(type));
      }
      if (requestRead && client_.isConnected()) {
        const QString type =
            watch_->watchTable->item(row, 6)
                ? watch_->watchTable->item(row, 6)->text().trimmed()
                : (sdoInspector_->sdoType ? sdoInspector_->sdoType->currentText() : QString());
        requestSdoRead(position, index, subIndex,
                       uiText("Existing Watch item", "已有监视项"), type);
      }
      updateWatchAutoRefresh();
      return;
    }
  }

  const int row = watch_->watchTable->rowCount();
  watch_->watchTable->insertRow(row);
  watch_->watchTable->setItem(
      row, 0,
    // Create table cell
      new QTableWidgetItem(QDateTime::currentDateTime().toString("HH:mm:ss")));
    // Create table cell
  watch_->watchTable->setItem(row, 1, new QTableWidgetItem(QString::number(position)));
    // Create table cell
  watch_->watchTable->setItem(row, 2, new QTableWidgetItem(index));
    // Create table cell
  watch_->watchTable->setItem(row, 3, new QTableWidgetItem(subIndex));
  watch_->watchTable->setItem(
    // Create table cell
      row, 4, new QTableWidgetItem(sdoInspector_->sdoValue ? sdoInspector_->sdoValue->text() : QString()));
  watch_->watchTable->setItem(
      row, 5,
    // Create table cell
      new QTableWidgetItem(decodeWatchValue(
          index, subIndex, sdoInspector_->sdoType ? sdoInspector_->sdoType->currentText() : QString(),
          sdoInspector_->sdoValue ? sdoInspector_->sdoValue->text() : QString(), "Watch")));
  watch_->watchTable->setItem(
      row, 6,
    // Create table cell
      new QTableWidgetItem(sdoInspector_->sdoType ? sdoInspector_->sdoType->currentText() : QString()));
    // Create table cell
  watch_->watchTable->setItem(row, 7, new QTableWidgetItem("Watch"));
    // Create table cell
  watch_->watchTable->setItem(row, 8, new QTableWidgetItem);
    // Create table cell
  watch_->watchTable->setItem(row, 9, new QTableWidgetItem);
    // Create table cell
  watch_->watchTable->setItem(row, 10, new QTableWidgetItem);
    // Create table cell
  watch_->watchTable->setItem(row, 11, new QTableWidgetItem);
  updateWatchStartupDelta(row);
  watch_->watchTable->resizeColumnsToContents(); // auto-fit column widths
  watch_->watchTable->selectRow(row);
  updateDiagnostics(
      "Info", "Watch",
      QString("Added SDO watch #%1 %2:%3").arg(position).arg(index, subIndex));
  if (requestRead && client_.isConnected()) {
    const QString type = sdoInspector_->sdoType ? sdoInspector_->sdoType->currentText() : QString();
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
    for (int existingRow = 0; existingRow < watch_->watchTable->rowCount();
         ++existingRow) {
      const bool match =
          (watch_->watchTable->item(existingRow, 1) &&
           watch_->watchTable->item(existingRow, 1)->text().toInt() == position) &&
          (watch_->watchTable->item(existingRow, 2) &&
           watch_->watchTable->item(existingRow, 2)
                   ->text()
                   .compare(index, Qt::CaseInsensitive) == 0) &&
          (watch_->watchTable->item(existingRow, 3) &&
           watch_->watchTable->item(existingRow, 3)
                   ->text()
                   .compare(subIndex, Qt::CaseInsensitive) == 0);
      if (match) {
        watchRow = existingRow;
        break;
      }
    }

    if (watchRow < 0) {
      watchRow = watch_->watchTable->rowCount();
      watch_->watchTable->insertRow(watchRow);
      watch_->watchTable->setItem(
          watchRow, 0,
    // Create table cell
          new QTableWidgetItem(
              QDateTime::currentDateTime().toString("HH:mm:ss")));
      watch_->watchTable->setItem(watchRow, 1,
    // Create table cell
                           new QTableWidgetItem(QString::number(position)));
    // Create table cell
      watch_->watchTable->setItem(watchRow, 2, new QTableWidgetItem(index));
    // Create table cell
      watch_->watchTable->setItem(watchRow, 3, new QTableWidgetItem(subIndex));
    // Create table cell
      watch_->watchTable->setItem(watchRow, 8, new QTableWidgetItem);
    // Create table cell
      watch_->watchTable->setItem(watchRow, 9, new QTableWidgetItem);
    // Create table cell
      watch_->watchTable->setItem(watchRow, 10, new QTableWidgetItem);
    // Create table cell
      watch_->watchTable->setItem(watchRow, 11, new QTableWidgetItem);
    } else {
      auto *timeItem = watch_->watchTable->item(watchRow, 0);
      if (!timeItem) {
    // Create table cell
        timeItem = new QTableWidgetItem;
        watch_->watchTable->setItem(watchRow, 0, timeItem);
      }
      timeItem->setText(QDateTime::currentDateTime().toString("HH:mm:ss"));
    }

    if (!value.isEmpty()) {
    // Create table cell
      watch_->watchTable->setItem(watchRow, 4, new QTableWidgetItem(value));
      const QString key =
          QString("%1|%2|%3").arg(position).arg(index, subIndex);
      watchValues_[key] = value;
    } else if (!watch_->watchTable->item(watchRow, 4)) {
    // Create table cell
      watch_->watchTable->setItem(watchRow, 4, new QTableWidgetItem);
    }

    const QString effectiveValue =
        value.isEmpty() && watch_->watchTable->item(watchRow, 4)
            ? watch_->watchTable->item(watchRow, 4)->text().trimmed()
            : value;
    const QString decoded =
        decodeWatchValue(index, subIndex, type, effectiveValue,
                         uiText("SDO History", "SDO 历史"));
    // Create table cell
    watch_->watchTable->setItem(watchRow, 5, new QTableWidgetItem(decoded));
    // Create table cell
    watch_->watchTable->setItem(watchRow, 6, new QTableWidgetItem(type));
    watch_->watchTable->setItem(watchRow, 7,
    // Create table cell
                         new QTableWidgetItem(uiText("History", "历史")));
    if (!status.isEmpty() && watch_->watchTable->item(watchRow, 7)) {
      watch_->watchTable->item(watchRow, 7)
          ->setToolTip(detail.isEmpty()
                           ? status
                           : QString("%1 - %2").arg(status, detail));
    }
    updateWatchBaselineDelta(watchRow);
    updateWatchStartupDelta(watchRow);
    watch_->watchTable->selectRow(watchRow);
    ++addedOrReused;
  }

  watch_->watchTable->resizeColumnsToContents(); // auto-fit column widths
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
  if (position < 0 || !sdo_->pdoTable || loadedPdoPosition_ != position) {
    return;
  }
  ensureWatchTable();

  const QVector<int> rows = selectedTableRows(sdo_->pdoTable);

  const QString previousIndex = sdoInspector_->sdoIndex ? sdoInspector_->sdoIndex->text() : QString();
  const QString previousSubIndex =
      sdoInspector_->sdoSubIndex ? sdoInspector_->sdoSubIndex->text() : QString();
  const QString previousType = sdoInspector_->sdoType ? sdoInspector_->sdoType->currentText() : QString();
  const QString previousReadValue = sdoInspector_->sdoValue ? sdoInspector_->sdoValue->text() : QString();
  const QString previousWriteValue =
      sdoInspector_->sdoWriteValue ? sdoInspector_->sdoWriteValue->text() : QString();
  const bool previousWritable = selectedSdoWritable_;

  int addedOrReused = 0;
  int skipped = 0;
  for (const int row : rows) {
    if (row < 0 || row >= sdo_->pdoTable->rowCount() ||
        sdo_->pdoTable->isRowHidden(row)) {
      continue;
    }
    auto textAt = [this, row](int column) {
      const auto *item = sdo_->pdoTable->item(row, column);
      return item ? item->text().trimmed() : QString();
    };
    const QString index = textAt(2);
    const QString subIndex = textAt(3);
    if (index.isEmpty() || subIndex.isEmpty()) {
      ++skipped;
      continue;
    }

    {
      const QSignalBlocker indexBlocker(sdoInspector_->sdoIndex); // prevent recursive signal updates
      const QSignalBlocker subIndexBlocker(sdoInspector_->sdoSubIndex); // prevent recursive signal updates
      const QSignalBlocker typeBlocker(sdoInspector_->sdoType); // prevent recursive signal updates
      if (sdoInspector_->sdoIndex) {
        sdoInspector_->sdoIndex->setText(index);
      }
      if (sdoInspector_->sdoSubIndex) {
        sdoInspector_->sdoSubIndex->setText(
            QString("0x%1").arg(subIndex.rightJustified(2, '0')));
      }
      if (sdoInspector_->sdoType) {
        sdoInspector_->sdoType->setCurrentIndex(0);
      }
      if (sdoInspector_->sdoValue) {
        sdoInspector_->sdoValue->clear();
      }
    }
    selectedSdoWritable_ = true;
    addCurrentSdoToWatch(false);
    const int watchRow = watch_->watchTable ? watch_->watchTable->currentRow() : -1;
    if (watchRow >= 0) {
      const QString mode =
          textAt(0).contains("Rx", Qt::CaseInsensitive) ||
                  textAt(1).contains("RxPDO", Qt::CaseInsensitive)
              ? "RxPDO"
              : (textAt(0).contains("Tx", Qt::CaseInsensitive) ||
                         textAt(1).contains("TxPDO", Qt::CaseInsensitive)
                     ? "TxPDO"
                     : "PDO");
    // Create table cell
      watch_->watchTable->setItem(watchRow, 7, new QTableWidgetItem(mode));
    }
    ++addedOrReused;
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
  if (watch_->watchTable) {
    watch_->watchTable->resizeColumnsToContents(); // auto-fit column widths
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

  const QString previousIndex = sdoInspector_->sdoIndex ? sdoInspector_->sdoIndex->text() : QString();
  const QString previousSubIndex =
      sdoInspector_->sdoSubIndex ? sdoInspector_->sdoSubIndex->text() : QString();
  const QString previousType = sdoInspector_->sdoType ? sdoInspector_->sdoType->currentText() : QString();
  const QString previousReadValue = sdoInspector_->sdoValue ? sdoInspector_->sdoValue->text() : QString();
  const QString previousWriteValue =
      sdoInspector_->sdoWriteValue ? sdoInspector_->sdoWriteValue->text() : QString();
  const bool previousWritable = selectedSdoWritable_;
  const int before = watch_->watchTable ? watch_->watchTable->rowCount() : 0;

  for (const auto &object : objects) {
    {
      const QSignalBlocker indexBlocker(sdoInspector_->sdoIndex); // prevent recursive signal updates
      const QSignalBlocker subIndexBlocker(sdoInspector_->sdoSubIndex); // prevent recursive signal updates
      const QSignalBlocker typeBlocker(sdoInspector_->sdoType); // prevent recursive signal updates
      if (sdoInspector_->sdoIndex) {
        sdoInspector_->sdoIndex->setText(object.index);
      }
      if (sdoInspector_->sdoSubIndex) {
        sdoInspector_->sdoSubIndex->setText(object.subIndex);
      }
      if (sdoInspector_->sdoType) {
        sdoInspector_->sdoType->setCurrentText(object.type);
      }
      if (sdoInspector_->sdoValue) {
        sdoInspector_->sdoValue->clear();
      }
    }
    selectedSdoWritable_ = true;
    addCurrentSdoToWatch();
    const int row = watch_->watchTable ? watch_->watchTable->currentRow() : -1;
    if (row >= 0) {
    // Create table cell
      watch_->watchTable->setItem(row, 5, new QTableWidgetItem(object.note));
    // Create table cell
      watch_->watchTable->setItem(row, 7, new QTableWidgetItem("CiA 402"));
    }
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
  updateActionAvailability();

  const int after = watch_->watchTable ? watch_->watchTable->rowCount() : before;
  updateDiagnostics(
      "Info", "Watch",
      QString("CiA 402 watch preset added for slave #%1: %2 new item(s)")
          .arg(position)
          .arg(qMax(0, after - before)));
  activateWorkspaceTab(watchTabIndex_);
}



// — Refresh watch list
void MainWindow::refreshWatchList(bool quiet) {
  if (!client_.isConnected() || !watch_->watchTable) {
    updateWatchAutoRefresh();
    return;
  }
  ensureWatchTable();
  int requested = 0;
  for (int row = 0; row < watch_->watchTable->rowCount(); ++row) {
    const int position = watch_->watchTable->item(row, 1)
                             ? watch_->watchTable->item(row, 1)->text().toInt()
                             : -1;
    const QString index = watch_->watchTable->item(row, 2)
                              ? watch_->watchTable->item(row, 2)->text().trimmed()
                              : QString();
    const QString subIndex = watch_->watchTable->item(row, 3)
                                 ? watch_->watchTable->item(row, 3)->text().trimmed()
                                 : QString();
    const QString type = watch_->watchTable->item(row, 6)
                             ? watch_->watchTable->item(row, 6)->text().trimmed()
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
  if (!watch_->watchTable) {
    return;
  }
  ensureWatchTable();
  int captured = 0;
  for (int row = 0; row < watch_->watchTable->rowCount(); ++row) {
    const QString value = watch_->watchTable->item(row, 4)
                              ? watch_->watchTable->item(row, 4)->text().trimmed()
                              : QString();
    if (value.isEmpty()) {
      continue;
    }
    // Create table cell
    watch_->watchTable->setItem(row, 8, new QTableWidgetItem(value));
    ++captured;
    updateWatchBaselineDelta(row);
  }
  watch_->watchTable->resizeColumnsToContents(); // auto-fit column widths
  filterWatchTable();
  updateActionAvailability();
  updateDiagnostics(
      "Info", "Watch",
      QString("Captured Watch baseline for %1 row(s)").arg(captured));
}


// — Clear watch baseline
void MainWindow::clearWatchBaseline() {
  if (!watch_->watchTable) {
    return;
  }
  ensureWatchTable();
  int cleared = 0;
  for (int row = 0; row < watch_->watchTable->rowCount(); ++row) {
    if (watch_->watchTable->item(row, 8) &&
        !watch_->watchTable->item(row, 8)->text().trimmed().isEmpty()) {
      ++cleared;
    }
    // Create table cell
    watch_->watchTable->setItem(row, 8, new QTableWidgetItem);
    // Create table cell
    watch_->watchTable->setItem(row, 9, new QTableWidgetItem);
  }
  watch_->watchTable->resizeColumnsToContents(); // auto-fit column widths
  filterWatchTable();
  updateActionAvailability();
  updateDiagnostics(
      "Info", "Watch",
      QString("Cleared Watch baseline for %1 row(s)").arg(cleared));
}


// — Compute and color the baseline delta cell for a single Watch row
void MainWindow::updateWatchBaselineDelta(int row) {
  if (!watch_->watchTable || row < 0 || row >= watch_->watchTable->rowCount()) {
    return;
  }
  ensureWatchTable();
  const QString value = watch_->watchTable->item(row, 4)
                            ? watch_->watchTable->item(row, 4)->text().trimmed()
                            : QString();
  const QString baseline = watch_->watchTable->item(row, 8)
                               ? watch_->watchTable->item(row, 8)->text().trimmed()
                               : QString();
  auto *deltaItem = watch_->watchTable->item(row, 9);
  if (!deltaItem) {
    // Create table cell
    deltaItem = new QTableWidgetItem;
    watch_->watchTable->setItem(row, 9, deltaItem);
  }
  if (value.isEmpty() || baseline.isEmpty()) {
    deltaItem->setText(QString());
    // Define color for visual feedback
    deltaItem->setBackground(QBrush());
    // Define color for visual feedback
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
    // Define color for visual feedback
    deltaItem->setBackground(settings_.theme == "Light" ? QColor("#fff7cc")
    // Define color for visual feedback
                                                        : QColor("#3a2f16"));
    // Define color for visual feedback
    deltaItem->setForeground(settings_.theme == "Light" ? QColor("#854d0e")
    // Define color for visual feedback
                                                        : QColor("#fde68a"));
  } else {
    // Define color for visual feedback
    deltaItem->setBackground(settings_.theme == "Light" ? QColor("#dcfce7")
    // Define color for visual feedback
                                                        : QColor("#12351f"));
    // Define color for visual feedback
    deltaItem->setForeground(settings_.theme == "Light" ? QColor("#166534")
    // Define color for visual feedback
                                                        : QColor("#86efac"));
  }
}


// — Recompute baseline deltas for every Watch row
void MainWindow::updateWatchBaselineDeltas() {
  if (!watch_->watchTable) {
    return;
  }
  ensureWatchTable();
  for (int row = 0; row < watch_->watchTable->rowCount(); ++row) {
    updateWatchBaselineDelta(row);
  }
}


// — Compute and color the Startup SDO delta cell for a single Watch row
void MainWindow::updateWatchStartupDelta(int row) {
  if (!watch_->watchTable || row < 0 || row >= watch_->watchTable->rowCount()) {
    return;
  }
  ensureWatchTable();

  const WatchStartupWatchRow watchRow = watchStartupWatchRow(watch_->watchTable, row);

  auto *expectedItem = ensureWatchStartupTableItem(
      watch_->watchTable, row, kWatchStartupWatchExpectedColumn);
  auto *deltaItem = ensureWatchStartupTableItem(watch_->watchTable, row,
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
  if (!watch_->watchTable) {
    return;
  }
  ensureWatchTable();
  for (int row = 0; row < watch_->watchTable->rowCount(); ++row) {
    updateWatchStartupDelta(row);
  }
  updateStartupSdoWatchEvidence();
  updateIoVariableTable();
  updateStateMachineView();
}


// — Start or stop the Watch auto-refresh timer based on user settings
void MainWindow::updateWatchAutoRefresh() {
  const bool hasItems = watch_->watchTable && watch_->watchTable->rowCount() > 0;
  const bool autoEnabled = watch_->watchAutoRefresh && watch_->watchAutoRefresh->isChecked();
  const int interval = watch_->watchRefreshInterval
                           ? watch_->watchRefreshInterval->currentData().toInt()
                           : 1000;

  if (watchRefreshTimer_) {
    // Only tick when connected, items exist, and user opted in
    if (client_.isConnected() && hasItems && autoEnabled) {
      watchRefreshTimer_->start(interval > 0 ? interval : 1000);
    } else {
      watchRefreshTimer_->stop();
    }
  }
  if (watch_->watchRefreshInterval) {
    watch_->watchRefreshInterval->setEnabled(autoEnabled);
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
  if (!watch_->watchTable) {
    return;
  }
  watch_->watchTable->clearContents();
  watch_->watchTable->setRowCount(0);
  watchValues_.clear();
  watchChangedKeys_.clear();
  ensureWatchTable();
  updateWatchAutoRefresh();
  updateSelectedDriveSummary();
  updateIoVariableTable();
  updateStateMachineView();
}

