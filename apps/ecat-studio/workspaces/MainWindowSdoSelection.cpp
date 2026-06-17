// SDO inspector, target panel, evidence trail, and history.

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

// — Fill the SDO target panel from the selected OD row
void MainWindow::applySdoSelectionFromDictionary(int row, bool readAfterFill) {
  if (!sdo_->sdoTable || row < 0 || row >= sdo_->sdoTable->rowCount()) {
    return;
  }
  if (selectedPosition() < 0 || loadedSdoPosition_ != selectedPosition()) {
    updateDiagnostics("Warning", "SDO",
                      uiText("Ignored Object Dictionary row because it is not "
                             "loaded for the current slave",
                             "已忽略对象字典行：它不属于当前选中从站"));
    return;
  }

  const SdoDictionaryRow dictionary = sdoDictionaryRowFromTable(sdo_->sdoTable, row);
  if (!sdoDictionaryRowHasTarget(dictionary)) {
    return;
  }

  const QString access = dictionary.access.toLower();
  const bool writable = sdoDictionaryRowIsWritable(dictionary);

  const QSignalBlocker indexBlocker(sdoInspector_->sdoIndex); // prevent recursive signal updates
  const QSignalBlocker subIndexBlocker(sdoInspector_->sdoSubIndex); // prevent recursive signal updates
  const QSignalBlocker typeBlocker(sdoInspector_->sdoType); // prevent recursive signal updates
  const QSignalBlocker valueBlocker(sdoInspector_->sdoValue); // prevent recursive signal updates

  sdoInspector_->sdoIndex->setText(dictionary.index);
  sdoInspector_->sdoSubIndex->setText(dictionary.subIndex);
  if (sdoInspector_->sdoValue) {
    sdoInspector_->sdoValue->setText(dictionary.value);
    sdoInspector_->sdoValue->setPlaceholderText(
        dictionary.value.isEmpty()
            ? uiText("No read-back for selected object", "选中对象暂无读回值")
            : uiText("Last value from Object Dictionary evidence",
                     "来自对象字典证据的最后值"));
  }

  if (!dictionary.type.isEmpty()) {
    const QString normalized = dictionary.type.toLower().replace(' ', "_");
    const int typeIndex = sdoInspector_->sdoType->findText(normalized, Qt::MatchFixedString);
    sdoInspector_->sdoType->setCurrentIndex(typeIndex >= 0 ? typeIndex : 0);
  }

  selectedSdoWritable_ = writable;
  sdoInspector_->sdoWriteValue->setEnabled(writable);
  if (!writable) {
    sdoInspector_->sdoWriteValue->clear();
    sdoInspector_->sdoWriteValue->setPlaceholderText(uiText("Read-only object", "只读对象"));
  } else {
    sdoInspector_->sdoWriteValue->setPlaceholderText(uiText("Value to write", "写入值"));
  }
  updateSdoInspector(uiText("Object Dictionary", "对象字典"),
                     QString("%1 %2 %3 bit %4")
                         .arg(access.toUpper(), dictionary.name,
                              dictionary.bits,
                              writable ? uiText("writable", "可写")
                                       : uiText("read-only", "只读")));
  rememberCurrentSdoTarget(uiText("Object Dictionary", "对象字典"),
                           QString("%1 %2 %3 bit %4")
                               .arg(access.toUpper(), dictionary.name,
                                    dictionary.bits,
                                    writable ? uiText("writable", "可写")
                                             : uiText("read-only", "只读")));

  updateDiagnostics(
      "Info", "SDO",
      QString("Selected object %1:%2 %3 %4 bit %5%6")
          .arg(dictionary.index, sdoInspector_->sdoSubIndex->text(), access.toUpper(),
               dictionary.bits, dictionary.name,
               writable ? QString()
                        : uiText(" (write disabled)", "（已禁用写入）")));

  if (readAfterFill && client_.isConnected() && selectedPosition() >= 0) {
    requestSdoRead(selectedPosition(), sdoInspector_->sdoIndex->text(), sdoInspector_->sdoSubIndex->text(),
                   uiText("Object Dictionary", "对象字典"));
  }
  updateActionAvailability();
}



void MainWindow::addSelectedDictionaryRowsToWatch() {
  if (selectedPosition() < 0 || !sdo_->sdoTable ||
      loadedSdoPosition_ != selectedPosition()) {
    return;
  }
  const QVector<int> rows = selectedDictionaryRows();
  if (rows.isEmpty()) {
    return;
  }
  addDictionaryRowsToWatch(
      rows, uiText("selected object dictionary row(s)", "选中对象字典行"));
}


// — Add visible dictionary rows to watch
void MainWindow::addVisibleDictionaryRowsToWatch() {
  if (selectedPosition() < 0 || !sdo_->sdoTable ||
      loadedSdoPosition_ != selectedPosition()) {
    return;
  }
  const QVector<int> rows = visibleSdoDictionaryRows(sdo_->sdoTable);
  if (rows.isEmpty()) {
    return;
  }
  addDictionaryRowsToWatch(
      rows, uiText("visible object dictionary row(s)", "可见对象字典行"));
}


// — Add dictionary rows to watch
void MainWindow::addDictionaryRowsToWatch(const QVector<int> &rows,
                                          const QString &sourceLabel) {
  if (selectedPosition() < 0 || !sdo_->sdoTable ||
      loadedSdoPosition_ != selectedPosition() || rows.isEmpty()) {
    return;
  }

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
    if (row < 0 || row >= sdo_->sdoTable->rowCount() ||
        sdo_->sdoTable->isRowHidden(row)) {
      continue;
    }
    const SdoDictionaryRow dictionary =
        sdoDictionaryRowFromTable(sdo_->sdoTable, row);
    if (!sdoDictionaryRowHasTarget(dictionary)) {
      ++skipped;
      continue;
    }
    {
      const QSignalBlocker indexBlocker(sdoInspector_->sdoIndex); // prevent recursive signal updates
      const QSignalBlocker subIndexBlocker(sdoInspector_->sdoSubIndex); // prevent recursive signal updates
      const QSignalBlocker typeBlocker(sdoInspector_->sdoType); // prevent recursive signal updates
      if (sdoInspector_->sdoIndex) {
        sdoInspector_->sdoIndex->setText(dictionary.index);
      }
      if (sdoInspector_->sdoSubIndex) {
        sdoInspector_->sdoSubIndex->setText(dictionary.subIndex);
      }
      if (sdoInspector_->sdoType) {
        const QString normalized = dictionary.type.toLower().replace(' ', "_");
        const int typeIndex =
            sdoInspector_->sdoType->findText(normalized, Qt::MatchFixedString);
        sdoInspector_->sdoType->setCurrentIndex(typeIndex >= 0 ? typeIndex : 0);
      }
      if (sdoInspector_->sdoValue) {
        sdoInspector_->sdoValue->clear();
      }
    }
    selectedSdoWritable_ = true;
    addCurrentSdoToWatch(false);
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
  filterWatchTable();
  updateActionAvailability();
  updateDiagnostics(
      "Info", "Watch",
      QString("Added/reused %1 %2 in Watch%3; use Refresh Watch to read values")
          .arg(addedOrReused)
          .arg(sourceLabel)
          .arg(skipped > 0 ? QString(", skipped %1").arg(skipped) : QString()));
  if (addedOrReused > 0) {
    activateWorkspaceTab(watchTabIndex_);
  }
}


// — Fill the SDO target panel from the selected PDO map row
void MainWindow::applySdoSelectionFromPdoMap(int row, bool readAfterFill) {
  if (!sdo_->pdoTable || row < 0 || row >= sdo_->pdoTable->rowCount()) {
    return;
  }
  if (selectedPosition() < 0 || loadedPdoPosition_ != selectedPosition()) {
    updateDiagnostics("Warning", "PDO",
                      uiText("Ignored PDO row because it is not loaded for the "
                             "current slave",
                             "已忽略 PDO 行：它不属于当前选中从站"));
    return;
  }

  const int position = selectedPosition();
  const PdoMapTableRow pdoRow = pdoMapTableRowFromTable(sdo_->pdoTable, row);
  if (position < 0 || !pdoMapTableRowHasTarget(pdoRow)) {
    return;
  }

  const QSignalBlocker indexBlocker(sdoInspector_->sdoIndex); // prevent recursive signal updates
  const QSignalBlocker subIndexBlocker(sdoInspector_->sdoSubIndex); // prevent recursive signal updates
  sdoInspector_->sdoIndex->setText(pdoRow.index);
  sdoInspector_->sdoSubIndex->setText(pdoRow.subIndex);
  selectedSdoWritable_ = true;
  if (sdoInspector_->sdoWriteValue) {
    sdoInspector_->sdoWriteValue->setEnabled(true);
    sdoInspector_->sdoWriteValue->setPlaceholderText(uiText("Value to write", "写入值"));
  }
  updateSdoInspector(
      uiText("PDO Map", "PDO 映射"),
      QString("%1 %2 %3 bit %4")
          .arg(pdoRow.syncManager, pdoRow.pdo, pdoRow.bits, pdoRow.name));
  rememberCurrentSdoTarget(
      uiText("PDO Map", "PDO 映射"),
      QString("%1 %2 %3 bit %4")
          .arg(pdoRow.syncManager, pdoRow.pdo, pdoRow.bits, pdoRow.name));

  updateDiagnostics("Info", "PDO",
                    QString("Selected PDO entry #%1 %2 %3 %4:%5 %6 bit %7")
                        .arg(position)
                        .arg(pdoRow.syncManager, pdoRow.pdo, pdoRow.index,
                             sdoInspector_->sdoSubIndex->text(), pdoRow.bits, pdoRow.name));

  if (readAfterFill && client_.isConnected()) {
    requestSdoRead(position, sdoInspector_->sdoIndex->text(), sdoInspector_->sdoSubIndex->text(),
                   uiText("PDO Map", "PDO 映射"));
  }
  updateActionAvailability();
}


// — Fill the SDO target panel from the selected Free Run row
void MainWindow::applySdoSelectionFromFreeRunEntry(int row,
                                                   bool readAfterFill) {
  if (!freeRunWidgets_->freeRunEntryTable || row < 0 || row >= freeRunWidgets_->freeRunEntryTable->rowCount()) {
    return;
  }

  const FreeRunEntryTableRow entry =
      freeRunEntryTableRowFromTable(freeRunWidgets_->freeRunEntryTable, row);
  if (!freeRunEntryTableRowHasTarget(entry)) {
    return;
  }

  if (readAfterFill) {
    setSelectedSlave(entry.position);
  } else if (!selectSlaveForLocalEvidence(entry.position)) {
    return;
  }

  const QSignalBlocker indexBlocker(sdoInspector_->sdoIndex); // prevent recursive signal updates
  const QSignalBlocker subIndexBlocker(sdoInspector_->sdoSubIndex); // prevent recursive signal updates
  sdoInspector_->sdoIndex->setText(entry.index);
  sdoInspector_->sdoSubIndex->setText(entry.subIndex);
  sdoInspector_->sdoValue->setText(entry.raw);
  selectedSdoWritable_ = true;
  if (sdoInspector_->sdoWriteValue) {
    sdoInspector_->sdoWriteValue->setEnabled(true);
    sdoInspector_->sdoWriteValue->setPlaceholderText(uiText("Value to write", "写入值"));
  }
  updateSdoInspector(uiText("Free Run", "自由运行"),
                     QString("%1 %2").arg(entry.direction, entry.name));
  rememberCurrentSdoTarget(uiText("Free Run", "自由运行"),
                           QString("%1 %2").arg(entry.direction, entry.name));

  updateDiagnostics(
      "Info", "Free Run",
      QString("Selected process entry #%1 %2 %3:%4 %5")
          .arg(entry.position)
          .arg(entry.direction, entry.index, entry.subIndex, entry.name));

  if (readAfterFill && client_.isConnected()) {
    requestSdoRead(entry.position, entry.index, entry.subIndex,
                   uiText("Free Run", "自由运行"));
  }
  updateActionAvailability();
}


// — Fill the SDO target panel from the selected I/O variable row
void MainWindow::applySdoSelectionFromIoVariable(int row, bool readAfterFill) {
  if (!ioVar_->ioVariableTable || row < 0 || row >= ioVar_->ioVariableTable->rowCount()) {
    return;
  }

  const IoVariableTableRow variable =
      ioVariableTableRowFromTable(ioVar_->ioVariableTable, row);
  if (!ioVariableTableRowHasTarget(variable)) {
    return;
  }

  if (readAfterFill) {
    setSelectedSlave(variable.position);
  } else if (!selectSlaveForLocalEvidence(variable.position)) {
    return;
  }
  const QSignalBlocker indexBlocker(sdoInspector_->sdoIndex); // prevent recursive signal updates
  const QSignalBlocker subIndexBlocker(sdoInspector_->sdoSubIndex); // prevent recursive signal updates
  const QSignalBlocker typeBlocker(sdoInspector_->sdoType); // prevent recursive signal updates
  if (sdoInspector_->sdoIndex) {
    sdoInspector_->sdoIndex->setText(variable.index);
  }
  if (sdoInspector_->sdoSubIndex) {
    sdoInspector_->sdoSubIndex->setText(variable.subIndex);
  }
  if (sdoInspector_->sdoType) {
    const QString type = ioVariableTableRowTypeFromBits(variable);
    if (!type.isEmpty()) {
      sdoInspector_->sdoType->setCurrentText(type);
    }
  }
  if (sdoInspector_->sdoValue) {
    sdoInspector_->sdoValue->setText(ioVariableTableRowPreferredValue(variable));
  }
  selectedSdoWritable_ = true;
  if (sdoInspector_->sdoWriteValue) {
    sdoInspector_->sdoWriteValue->setEnabled(true);
    sdoInspector_->sdoWriteValue->setPlaceholderText(uiText("Value to write", "写入值"));
  }
  updateSdoInspector(
      uiText("I/O Variables", "I/O 变量"),
      QString("%1 | %2 | %3")
          .arg(variable.direction, variable.source, variable.symbol));
  rememberCurrentSdoTarget(
      uiText("I/O Variables", "I/O 变量"),
      QString("%1 | %2 | %3")
          .arg(variable.direction, variable.source, variable.symbol));
  updateDiagnostics("Info", "I/O Variables",
                    QString("Selected I/O variable #%1 %2 %3:%4 %5")
                        .arg(variable.position)
                        .arg(variable.direction, variable.index,
                             variable.subIndex, variable.symbol));

  if (readAfterFill && client_.isConnected()) {
    requestSdoRead(variable.position, variable.index, variable.subIndex,
                   uiText("I/O Variables", "I/O 变量"),
                   sdoInspector_->sdoType ? sdoInspector_->sdoType->currentText() : QString());
  }
  updateActionAvailability();
}


// — Add io variable rows to watch
void MainWindow::addIoVariableRowsToWatch(const QVector<int> &rows,
                                          const QString &sourceLabel) {
  if (!ioVar_->ioVariableTable || rows.isEmpty()) {
    return;
  }
  ensureWatchTable();

  int added = 0;
  int reused = 0;
  int skipped = 0;
  for (const int row : rows) {
    if (row < 0 || row >= ioVar_->ioVariableTable->rowCount() ||
        ioVar_->ioVariableTable->isRowHidden(row)) {
      ++skipped;
      continue;
    }
    const IoVariableTableRow variable =
        ioVariableTableRowFromTable(ioVar_->ioVariableTable, row);
    if (!ioVariableTableRowHasTarget(variable)) {
      ++skipped;
      continue;
    }
    const QString type = ioVariableTableRowTypeFromBits(variable);

    int existing = -1;
    for (int watchRow = 0; watchRow < watch_->watchTable->rowCount(); ++watchRow) {
      const bool match =
          tableText(watch_->watchTable, watchRow, 1).toInt() == variable.position &&
          tableText(watch_->watchTable, watchRow, 2)
                  .compare(variable.index, Qt::CaseInsensitive) == 0 &&
          tableText(watch_->watchTable, watchRow, 3)
                  .compare(variable.subIndex, Qt::CaseInsensitive) == 0;
      if (match) {
        existing = watchRow;
        break;
      }
    }

    if (existing >= 0) {
      if (!type.isEmpty() && tableText(watch_->watchTable, existing, 6).isEmpty()) {
        watch_->watchTable->setItem(existing, 6, new QTableWidgetItem(type));
      }
      watch_->watchTable->selectRow(existing);
      ++reused;
      continue;
    }

    const int watchRow = watch_->watchTable->rowCount();
    watch_->watchTable->insertRow(watchRow);
    watch_->watchTable->setItem(
        watchRow, 0,
        new QTableWidgetItem(
            QDateTime::currentDateTime().toString("HH:mm:ss")));
    watch_->watchTable->setItem(
        watchRow, 1, new QTableWidgetItem(QString::number(variable.position)));
    watch_->watchTable->setItem(watchRow, 2, new QTableWidgetItem(variable.index));
    watch_->watchTable->setItem(watchRow, 3, new QTableWidgetItem(variable.subIndex));
    watch_->watchTable->setItem(watchRow, 4, new QTableWidgetItem(variable.raw));
    watch_->watchTable->setItem(
        watchRow, 5,
        new QTableWidgetItem(
            variable.decoded.isEmpty()
                ? decodeWatchValue(variable.index, variable.subIndex, type,
                                   variable.raw, "I/O Variables")
                : variable.decoded));
    watch_->watchTable->setItem(watchRow, 6, new QTableWidgetItem(type));
    watch_->watchTable->setItem(watchRow, 7,
                         new QTableWidgetItem(variable.meaning.isEmpty()
                                                  ? "I/O Variables"
                                                  : variable.meaning));
    watch_->watchTable->setItem(watchRow, 8, new QTableWidgetItem);
    watch_->watchTable->setItem(watchRow, 9, new QTableWidgetItem);
    watch_->watchTable->setItem(watchRow, 10, new QTableWidgetItem);
    watch_->watchTable->setItem(watchRow, 11, new QTableWidgetItem);
    updateWatchStartupDelta(watchRow);
    watch_->watchTable->selectRow(watchRow);
    ++added;
  }

  watch_->watchTable->resizeColumnsToContents(); // auto-fit column widths
  updateWatchAutoRefresh();
  updateIoVariableTable();
  updateDiagnostics("Info", "I/O Variables",
                    uiText("Watch %1: added %2, reused %3, skipped %4",
                           "监视 %1：新增 %2，复用 %3，跳过 %4")
                        .arg(sourceLabel)
                        .arg(added)
                        .arg(reused)
                        .arg(skipped));
  if (added > 0 || reused > 0) {
    activateWorkspaceTab(watchTabIndex_);
  }
}


// — Add selected io variables to watch
void MainWindow::addSelectedIoVariablesToWatch() {
  if (!ioVar_->ioVariableTable) {
    return;
  }
  addIoVariableRowsToWatch(selectedIoVariableRows(false),
                           uiText("selected I/O variables", "所选 I/O 变量"));
}


// — Add visible io variables to watch
void MainWindow::addVisibleIoVariablesToWatch() {
  QVector<int> rows;
  if (!ioVar_->ioVariableTable) {
    return;
  }
  // Iterate all rows and apply active filter predicates
  for (int row = 0; row < ioVar_->ioVariableTable->rowCount(); ++row) {
    if (!ioVar_->ioVariableTable->isRowHidden(row)) {
      rows.append(row);
    }
  }
  addIoVariableRowsToWatch(rows,
                           uiText("visible I/O variables", "可见 I/O 变量"));
}


// — Add selected io variables to startup sdo
void MainWindow::addSelectedIoVariablesToStartupSdo() {
  const QVector<int> rows = selectedIoVariableRows(true);
  if (rows.isEmpty()) {
    return;
  }
  addIoVariableRowsToStartupSdo(
      rows, uiText("selected I/O variables", "所选 I/O 变量"));
}


// — Add visible io variables to startup sdo
void MainWindow::addVisibleIoVariablesToStartupSdo() {
  const QVector<int> rows = visibleIoVariableRows();
  if (rows.isEmpty()) {
    return;
  }
  addIoVariableRowsToStartupSdo(
      rows, uiText("visible I/O variables", "可见 I/O 变量"));
}


// — Add io variable rows to startup sdo
void MainWindow::addIoVariableRowsToStartupSdo(const QVector<int> &rows,
                                               const QString &sourceLabel) {
  if (!ioVar_->ioVariableTable || rows.isEmpty()) {
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
    int ioRow = -1;
    int position = -1;
    QString index;
    QString subIndex;
    QString value;
    QString type;
    QString symbol;
    QString source;
    QString valueSource;
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

  for (const int ioRow : uniqueRows) {
    if (ioRow < 0 || ioRow >= ioVar_->ioVariableTable->rowCount() ||
        ioVar_->ioVariableTable->isRowHidden(ioRow)) {
      ++skipped;
      continue;
    }

    const IoVariableTableRow variable =
        ioVariableTableRowFromTable(ioVar_->ioVariableTable, ioRow);
    const QString value = ioVariableTableRowStartupValue(variable);
    const QString valueSource = !variable.watch.isEmpty()
                                    ? uiText("Watch", "Watch")
                                    : uiText("Raw", "Raw");
    if (!ioVariableTableRowHasTarget(variable) || value.isEmpty()) {
      ++skipped;
      continue;
    }

    const QString key = QString("%1|%2|%3")
                            .arg(variable.position)
                            .arg(variable.index, variable.subIndex);
    if (processedKeys.contains(key)) {
      ++duplicateSkipped;
      continue;
    }
    processedKeys.insert(key);

    Candidate candidate;
    candidate.ioRow = ioRow;
    candidate.position = variable.position;
    candidate.index = variable.index;
    candidate.subIndex = variable.subIndex;
    candidate.value = value;
    candidate.type = ioVariableTableRowSdoType(variable);
    candidate.symbol = variable.symbol;
    candidate.source = variable.source;
    candidate.valueSource = valueSource;

    for (int startupRow = 0; startupRow < startupSdoTable_->rowCount();
         ++startupRow) {
      if (tableObjectAddressMatches(startupSdoTable_, startupRow,
                                    variable.position, variable.index,
                                    variable.subIndex, 0, 1, 2)) {
        candidate.startupRows.append(startupRow);
      }
    }

    candidates.append(candidate);
  }

  if (candidates.isEmpty()) {
    updateDiagnostics(
        "Warning", "Startup SDO",
        uiText("Startup creation skipped: %1 have no Watch or Raw value%2",
               "创建启动项已跳过：%1 没有 Watch 或 Raw 值%2")
            .arg(
                sourceLabel,
                skipped > 0
                    ? uiText(", skipped %1 row(s)", "，跳过 %1 行").arg(skipped)
                    : QString()));
    return;
  }

  int existingRowsAffected = 0;
  int newRowsPlanned = 0;
  int watchValueRows = 0;
  int rawFallbackRows = 0;
  for (const auto &candidate : candidates) {
    if (candidate.startupRows.isEmpty()) {
      ++newRowsPlanned;
    } else {
      existingRowsAffected += candidate.startupRows.size();
    }
    if (candidate.valueSource == uiText("Watch", "Watch")) {
      ++watchValueRows;
    } else {
      ++rawFallbackRows;
    }
  }

  QStringList details = {
      uiText("Master: %1", "主站：%1").arg(activeMasterName()),
      uiText("Source: %1", "来源：%1").arg(sourceLabel),
      uiText("Accepted I/O values: %1", "可使用 I/O 值：%1")
          .arg(candidates.size()),
      uiText("Value sources: Watch %1, Raw fallback %2",
             "值来源：Watch %1，Raw 兜底 %2")
          .arg(watchValueRows)
          .arg(rawFallbackRows),
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
    details << QString("#%1  %2:%3 = %4  [%5, %6]%7")
                   .arg(candidate.position)
                   .arg(candidate.index, candidate.subIndex, candidate.value,
                        target, candidate.valueSource,
                        candidate.symbol.isEmpty()
                            ? QString()
                            : QString("  %1").arg(candidate.symbol));
  }
  if (candidates.size() > previewRows) {
    details << uiText("...and %1 more I/O value(s)", "...另有 %1 个 I/O 值")
                   .arg(candidates.size() - previewRows);
  }
  if (skipped > 0) {
    details << uiText("Skipped rows without address or usable value: %1",
                      "已跳过缺少地址或可用值的行：%1")
                   .arg(skipped);
  }
  if (duplicateSkipped > 0) {
    details << uiText("Skipped duplicate selected I/O address(es): %1",
                      "已跳过重复选中 I/O 地址：%1")
                   .arg(duplicateSkipped);
  }

  // Safety gate: require explicit confirmation before bus write
  if (!confirmDangerousOperation(
          uiText("Confirm Startup from I/O Variables",
                 "确认从 I/O 变量创建 Startup SDO"),
          uiText("Create or update Startup SDO rows from I/O variable "
                 "evidence.",
                 "从 I/O 变量证据创建或更新 Startup SDO 行。"),
          details, uiText("Create Startup", "创建启动项"))) {
    return;
  }

  const QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");
  int updated = 0;
  int unchanged = 0;
  int created = 0;
  int lastTouchedRow = -1;

  for (const auto &candidate : candidates) {
    const QString detailSource =
        candidate.source.isEmpty()
            ? candidate.valueSource
            : QString("%1 | %2").arg(candidate.valueSource, candidate.source);
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
          startupRow, 5, new QTableWidgetItem(uiText("From I/O", "来自 I/O")));
      startupSdoTable_->setItem(
          startupRow, 6,
          new QTableWidgetItem(
              uiText("Created from I/O Variable row %1 at %2 (%3)",
                     "由 I/O 变量第 %1 行在 %2 创建（%3）")
                  .arg(candidate.ioRow + 1)
                  .arg(timestamp, detailSource)));
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
      ensureStartupCell(startupRow, 5)->setText(uiText("From I/O", "来自 I/O"));
      ensureStartupCell(startupRow, 6)
          ->setText(
              previousValue.compare(candidate.value, Qt::CaseInsensitive) == 0
                  ? uiText("Confirmed from I/O Variable row %1 at %2 (%3)",
                           "由 I/O 变量第 %1 行在 %2 确认（%3）")
                        .arg(candidate.ioRow + 1)
                        .arg(timestamp, detailSource)
                  : uiText("Updated from I/O Variable row %1 at %2 (%3); "
                           "previous value: %4",
                           "由 I/O 变量第 %1 行在 %2 更新（%3）；原值：%4")
                        .arg(candidate.ioRow + 1)
                        .arg(timestamp, detailSource, previousValue));
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
      uiText("Startup from I/O variables: %1 updated, %2 unchanged, %3 "
             "created%4%5",
             "从 I/O 变量生成 Startup：更新 %1，未变 %2，新建 %3%4%5")
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


// — Fill the SDO target panel from the selected Watch row
void MainWindow::applySdoSelectionFromWatch(int row, bool readAfterFill) {
  if (!watch_->watchTable || row < 0 || row >= watch_->watchTable->rowCount()) {
    return;
  }

  auto textAt = [this, row](int column) {
    const auto *item = watch_->watchTable->item(row, column);
    return item ? item->text().trimmed() : QString();
  };

  const int position = textAt(1).toInt();
  const QString index = textAt(2);
  const QString subIndex = textAt(3);
  const QString value = textAt(4);
  const QString type = textAt(6);
  if (position < 0 || index.isEmpty() || subIndex.isEmpty()) {
    return;
  }

  if (readAfterFill) {
    setSelectedSlave(position);
  } else if (!selectSlaveForLocalEvidence(position)) {
    return;
  }

  const QSignalBlocker indexBlocker(sdoInspector_->sdoIndex); // prevent recursive signal updates
  const QSignalBlocker subIndexBlocker(sdoInspector_->sdoSubIndex); // prevent recursive signal updates
  const QSignalBlocker typeBlocker(sdoInspector_->sdoType); // prevent recursive signal updates
  sdoInspector_->sdoIndex->setText(index);
  sdoInspector_->sdoSubIndex->setText(subIndex);
  sdoInspector_->sdoValue->setText(value);
  if (!type.isEmpty() && sdoInspector_->sdoType) {
    sdoInspector_->sdoType->setCurrentText(type);
  }
  selectedSdoWritable_ = true;
  if (sdoInspector_->sdoWriteValue) {
    sdoInspector_->sdoWriteValue->setEnabled(true);
    sdoInspector_->sdoWriteValue->setPlaceholderText(uiText("Value to write", "写入值"));
  }
  updateSdoInspector(uiText("Watch", "监视"), textAt(5));
  rememberCurrentSdoTarget(uiText("Watch", "监视"), textAt(5));

  updateDiagnostics("Info", "Watch",
                    QString("Selected watch item #%1 %2:%3")
                        .arg(position)
                        .arg(index, subIndex));

  if (readAfterFill && client_.isConnected()) {
    requestSdoRead(position, index, subIndex, uiText("Watch", "监视"), type);
  }
  updateActionAvailability();
}


// — Fill the SDO target panel from the selected SDO history row
void MainWindow::applySdoSelectionFromHistory(int row, bool readAfterFill) {
  if (!sdoHistoryTable_ || row < 0 || row >= sdoHistoryTable_->rowCount()) {
    return;
  }

  auto textAt = [this, row](int column) {
    const auto *item = sdoHistoryTable_->item(row, column);
    return item ? item->text().trimmed() : QString();
  };

  bool ok = false;
  const int position = textAt(2).toInt(&ok);
  const QString index = textAt(3);
  const QString subIndex = textAt(4);
  const QString type = textAt(5);
  const QString value = textAt(6);
  const QString action = textAt(1);
  const QString status = textAt(7);
  if (!ok || position < 0 || index.isEmpty() || subIndex.isEmpty()) {
    return;
  }

  if (readAfterFill) {
    setSelectedSlave(position);
  } else if (!selectSlaveForLocalEvidence(position)) {
    return;
  }

  const QSignalBlocker indexBlocker(sdoInspector_->sdoIndex); // prevent recursive signal updates
  const QSignalBlocker subIndexBlocker(sdoInspector_->sdoSubIndex); // prevent recursive signal updates
  const QSignalBlocker typeBlocker(sdoInspector_->sdoType); // prevent recursive signal updates
  sdoInspector_->sdoIndex->setText(index);
  sdoInspector_->sdoSubIndex->setText(subIndex);
  if (sdoInspector_->sdoValue) {
    sdoInspector_->sdoValue->setText(value);
  }
  if (!type.isEmpty() && sdoInspector_->sdoType) {
    const QString normalized = type.toLower().replace(' ', "_");
    const int typeIndex = sdoInspector_->sdoType->findText(normalized, Qt::MatchFixedString);
    sdoInspector_->sdoType->setCurrentIndex(typeIndex >= 0 ? typeIndex : 0);
  }
  selectedSdoWritable_ = true;
  if (sdoInspector_->sdoWriteValue) {
    sdoInspector_->sdoWriteValue->setEnabled(true);
    sdoInspector_->sdoWriteValue->setText(value);
    sdoInspector_->sdoWriteValue->setPlaceholderText(
        uiText("Value from SDO history", "来自 SDO 历史的值"));
  }
  updateSdoInspector(uiText("SDO History", "SDO 历史"),
                     QString("%1 %2").arg(action, status));
  rememberCurrentSdoTarget(uiText("SDO History", "SDO 历史"),
                           QString("%1 %2").arg(action, status));

  updateDiagnostics("Info", "SDO History",
                    QString("Selected history row #%1 %2:%3 %4 %5")
                        .arg(position)
                        .arg(index, subIndex, action, status));

  if (readAfterFill && client_.isConnected()) {
    requestSdoRead(position, index, subIndex, uiText("SDO History", "SDO 历史"),
                   type);
  }
  updateActionAvailability();
}


// — Fill the SDO target panel from the selected Startup SDO row
void MainWindow::applySdoSelectionFromStartup(int row, bool readAfterFill) {
  ensureStartupSdoTable();
  if (!startupSdoTable_ || row < 0 || row >= startupSdoTable_->rowCount()) {
    return;
  }

  auto textAt = [this, row](int column) {
    const auto *item = startupSdoTable_->item(row, column);
    return item ? item->text().trimmed() : QString();
  };

  bool ok = false;
  const int position = textAt(0).toInt(&ok);
  const QString index = textAt(1);
  const QString subIndex = textAt(2);
  const QString value = textAt(3);
  const QString type = textAt(4);
  const QString status = textAt(5);
  if (!ok || position < 0 || index.isEmpty() || subIndex.isEmpty()) {
    return;
  }

  if (readAfterFill) {
    setSelectedSlave(position);
  } else if (!selectSlaveForLocalEvidence(position)) {
    return;
  }

  const QSignalBlocker indexBlocker(sdoInspector_->sdoIndex); // prevent recursive signal updates
  const QSignalBlocker subIndexBlocker(sdoInspector_->sdoSubIndex); // prevent recursive signal updates
  const QSignalBlocker typeBlocker(sdoInspector_->sdoType); // prevent recursive signal updates
  sdoInspector_->sdoIndex->setText(index);
  sdoInspector_->sdoSubIndex->setText(subIndex);
  if (sdoInspector_->sdoValue) {
    sdoInspector_->sdoValue->setText(value);
  }
  if (!type.isEmpty() && sdoInspector_->sdoType) {
    const QString normalized = type.toLower().replace(' ', "_");
    const int typeIndex = sdoInspector_->sdoType->findText(normalized, Qt::MatchFixedString);
    sdoInspector_->sdoType->setCurrentIndex(typeIndex >= 0 ? typeIndex : 0);
  }
  selectedSdoWritable_ = true;
  if (sdoInspector_->sdoWriteValue) {
    sdoInspector_->sdoWriteValue->setEnabled(true);
    sdoInspector_->sdoWriteValue->setText(value);
    sdoInspector_->sdoWriteValue->setPlaceholderText(
        uiText("Startup SDO value", "Startup SDO 的值"));
  }
  updateSdoInspector(
      uiText("Startup SDO", "Startup SDO"),
      status.isEmpty() ? uiText("Startup row %1", "启动项第 %1 行").arg(row + 1)
                       : status);
  rememberCurrentSdoTarget(
      uiText("Startup SDO", "Startup SDO"),
      status.isEmpty() ? uiText("Startup row %1", "启动项第 %1 行").arg(row + 1)
                       : status);

  updateDiagnostics("Info", "Startup SDO",
                    QString("Selected startup row %1 #%2 %3:%4 %5")
                        .arg(row + 1)
                        .arg(position)
                        .arg(index, subIndex, status));

  if (readAfterFill && client_.isConnected()) {
    requestSdoRead(position, index, subIndex,
                   uiText("Startup SDO", "Startup SDO"), type);
  }
  updateActionAvailability();
}


// — Apply a preset filter string to the OD table and activate the pane// — Read selected dictionary rows
void MainWindow::readSelectedDictionaryRows() {
  if (!client_.isConnected() || selectedPosition() < 0 || !sdo_->sdoTable ||
      loadedSdoPosition_ != selectedPosition()) {
    return;
  }
  const QVector<int> rows = selectedDictionaryRows();
  if (rows.isEmpty()) {
    return;
  }
  readDictionaryRows(
      rows, uiText("selected object dictionary row(s)", "选中对象字典行"),
      false);
}


// — Read visible dictionary rows
void MainWindow::readVisibleDictionaryRows() {
  if (!client_.isConnected() || selectedPosition() < 0 || !sdo_->sdoTable ||
      loadedSdoPosition_ != selectedPosition()) {
    return;
  }
  const QVector<int> rows = visibleSdoDictionaryRows(sdo_->sdoTable);
  if (rows.isEmpty()) {
    return;
  }
  readDictionaryRows(
      rows, uiText("visible object dictionary row(s)", "可见对象字典行"), true);
}


// — Read failed dictionary rows
void MainWindow::readFailedDictionaryRows() {
  if (!client_.isConnected() || selectedPosition() < 0 || !sdo_->sdoTable ||
      loadedSdoPosition_ != selectedPosition()) {
    return;
  }

  const QVector<int> rows = failedSdoDictionaryRows(sdo_->sdoTable);
  if (rows.isEmpty()) {
    updateDiagnostics("Info", "SDO",
                      uiText("No failed Object Dictionary rows to retry",
                             "没有需要重试的对象字典失败行"));
    return;
  }

  setSdoFilterPreset("tag:failed");
  if (!rows.isEmpty()) {
    sdo_->sdoTable->clearSelection();
    sdo_->sdoTable->selectRow(rows.first());
    if (auto *item = sdo_->sdoTable->item(rows.first(), 0)) {
      sdo_->sdoTable->scrollToItem(item, QAbstractItemView::PositionAtCenter);
    }
  }
  readDictionaryRows(
      rows, uiText("failed object dictionary row(s)", "失败对象字典行"), true);
}


// — Read dictionary rows
void MainWindow::readDictionaryRows(const QVector<int> &rows,
                                    const QString &sourceLabel,
                                    bool confirmLargeBatch) {
  if (!client_.isConnected() || selectedPosition() < 0 || !sdo_->sdoTable ||
      loadedSdoPosition_ != selectedPosition() || rows.isEmpty()) {
    return;
  }

  int skipped = 0;
  const QVector<SdoDictionaryReadObject> objects =
      sdoDictionaryReadObjectsFromRows(sdo_->sdoTable, rows, &skipped);
  if (objects.isEmpty()) {
    return;
  }

  if (confirmLargeBatch && objects.size() > 24) {
    QStringList details = {
        uiText("Master: %1", "主站：%1").arg(activeMasterName()),
        uiText("Slave: #%1", "从站：#%1").arg(selectedPosition()),
        uiText("Objects to read: %1 %2", "将读取对象：%1 %2")
            .arg(objects.size())
            .arg(sourceLabel),
        uiText("This request is limited to %1.", "本次请求范围限定为%1。")
            .arg(sourceLabel),
        uiText("This sends one mailbox SDO upload per object.",
               "此操作会对每个对象发送一次 mailbox SDO 上传。"),
    };
    const int preview = std::min(static_cast<int>(objects.size()), 8);
    for (int i = 0; i < preview; ++i) {
      const auto &object = objects.at(i);
      details << (object.type.isEmpty()
                      ? QString("%1:%2").arg(object.index, object.subIndex)
                      : QString("%1:%2 %3")
                            .arg(object.index, object.subIndex, object.type));
    }
    if (objects.size() > preview) {
      details << uiText("...and %1 more object(s)", "...另有 %1 个对象")
                     .arg(objects.size() - preview);
    }
    // Safety gate: require explicit confirmation before bus write
    if (!confirmDangerousOperation(
            uiText("Confirm Visible OD Read", "确认读取可见 OD"),
            uiText("Read the currently visible Object Dictionary rows.",
                   "读取当前过滤后可见的对象字典行。"),
            details, uiText("Read Visible", "读取可见项"))) {
      return;
    }
  }

  for (const auto &object : objects) {
    requestSdoRead(
        selectedPosition(), object.index, object.subIndex,
        uiText("Object Dictionary %1", "对象字典%1").arg(sourceLabel),
        object.type);
  }
  updateDiagnostics(
      "Info", "SDO",
      QString("Read requested for %1 %2%3")
          .arg(objects.size())
          .arg(sourceLabel)
          .arg(skipped > 0 ? QString(", skipped %1").arg(skipped) : QString()));
}



// — Apply a preset filter string to the OD table and activate the pane
void MainWindow::setSdoFilterPreset(const QString &query) {
  if (!sdo_->sdoFilter) {
    return;
  }
  sdo_->sdoFilter->setText(query);
  filterSdoTable(query);
  activateObjectDictionaryPaneFor(sdo_->sdoTable);
}


// — Check whether any OD evidence rows have a Failed status
bool MainWindow::hasFailedSdoEvidence() const {
  return firstFailedSdoEvidenceRow() >= 0;
}


// — Return the first OD row with a Failed status, or -1
int MainWindow::firstFailedSdoEvidenceRow() const {
  if (!sdo_->sdoTable || selectedPosition() < 0 ||
      loadedSdoPosition_ != selectedPosition()) {
    return -1;
  }
  const QVector<int> rows = failedSdoDictionaryRows(sdo_->sdoTable);
  return rows.isEmpty() ? -1 : rows.first();
}


// — Scroll to and select the first failed OD evidence row
void MainWindow::focusFailedSdoEvidence() {
  if (!sdo_->sdoTable) {
    return;
  }
  const int row = firstFailedSdoEvidenceRow();
  if (row < 0) {
    updateDiagnostics("Info", "SDO",
                      uiText("No failed Object Dictionary evidence to review",
                             "没有需要审阅的对象字典失败证据"));
    return;
  }

  setSdoFilterPreset("tag:failed");
  activateObjectDictionaryPaneFor(sdo_->sdoTable);
  sdo_->sdoTable->clearSelection();
  sdo_->sdoTable->setCurrentCell(row, 0);
  sdo_->sdoTable->selectRow(row);
  if (auto *item = sdo_->sdoTable->item(row, 0)) {
    sdo_->sdoTable->scrollToItem(item, QAbstractItemView::PositionAtCenter);
  }
  applySdoSelectionFromDictionary(row, false);

  const SdoDictionaryRow dictionary = sdoDictionaryRowFromTable(sdo_->sdoTable, row);
  updateDiagnostics("Info", "SDO",
                    uiText("Focused failed Object Dictionary evidence %1:%2",
                           "已聚焦失败对象字典证据 %1:%2")
                        .arg(dictionary.index, dictionary.subIndex));
}


// — Apply text or tag-based filtering to the Object Dictionary table
void MainWindow::filterSdoTable(const QString &text) {
  const QString needle = text.trimmed();
  const QString normalizedNeedle = needle.toLower();
  // Support tag: prefix for semantic filtering
  const bool tagMode = normalizedNeedle.startsWith("tag:");
  const QString tag = tagMode ? normalizedNeedle.mid(4).trimmed() : QString();

  auto textAt = [this](int row, int column) {
    const auto *item = sdo_->sdoTable->item(row, column);
    return item ? item->text().trimmed() : QString();
  };
  auto rowHasIndexPrefix = [&textAt](int row, const QStringList &prefixes) {
    const QString index = textAt(row, 1).toLower();
    for (const QString &prefix : prefixes) {
      if (index.startsWith(prefix)) {
        return true;
      }
    }
    return false;
  };

  int visible = 0;
  int evidenceRows = 0;
  int failedRows = 0;
  int writableRows = 0;
  for (int row = 0; row < sdo_->sdoTable->rowCount(); ++row) {
    bool match = needle.isEmpty();
    const QString lastValue = textAt(row, 7);
    const QString lastStatus = textAt(row, 8);
    const QString accessForSummary = textAt(row, 3).toLower();
    const bool hasEvidence = !lastValue.isEmpty() || !lastStatus.isEmpty();
    const bool failedEvidence =
        lastStatus.contains(uiText("Failed", "失败"), Qt::CaseInsensitive) ||
        lastStatus.contains("failed", Qt::CaseInsensitive);
    if (accessForSummary.contains('w')) {
      ++writableRows;
    }
    if (hasEvidence) {
      ++evidenceRows;
    }
    if (failedEvidence) {
      ++failedRows;
    }
    if (tagMode) {
      // Semantic tag filtering: tag:writable, tag:cia402, tag:identity, etc.
      const QString access = textAt(row, 3).toLower();
      const QString objectText = textAt(row, 0).toLower();
      const QString nameText = textAt(row, 6).toLower();
      if (tag == "writable") {
        match = access.contains('w');
      } else if (tag == "readable") {
        match = access.contains('r');
      } else if (tag == "cia402") {
        match = rowHasIndexPrefix(row, {"0x603f", "0x6040", "0x6041", "0x6060",
                                        "0x6061", "0x6064", "0x606c", "0x6071",
                                        "0x6077", "0x607a", "0x60ff"}) ||
                objectText.contains("cia") ||
                nameText.contains("controlword") ||
                nameText.contains("statusword") || nameText.contains("mode") ||
                nameText.contains("position") ||
                nameText.contains("velocity") || nameText.contains("torque");
      } else if (tag == "identity") {
        match = rowHasIndexPrefix(
                    row, {"0x1000", "0x1008", "0x1009", "0x100a", "0x1018"}) ||
                objectText.contains("identity") ||
                objectText.contains("device") || nameText.contains("vendor") ||
                nameText.contains("product") || nameText.contains("revision") ||
                nameText.contains("serial") || nameText.contains("name");
      } else if (tag == "pdo") {
        match = rowHasIndexPrefix(
                    row, {"0x160", "0x1a0", "0x1c1", "0x1c2", "0x1c3"}) ||
                objectText.contains("pdo") || nameText.contains("pdo") ||
                nameText.contains("mapping") || nameText.contains("assignment");
      } else if (tag == "error") {
        match =
            rowHasIndexPrefix(row, {"0x1001", "0x1002", "0x1003", "0x1010",
                                    "0x1011", "0x1029", "0x10f3", "0x603f"}) ||
            objectText.contains("error") || objectText.contains("emergency") ||
            objectText.contains("diagnostic") || nameText.contains("error") ||
            nameText.contains("fault") || nameText.contains("emergency") ||
            nameText.contains("diagnostic");
      } else if (tag == "evidence") {
        match = hasEvidence;
      } else if (tag == "failed") {
        match = failedEvidence;
      } else {
        match = true;
      }
    } else {
      // Free-text search: match against any column
      for (int column = 0; column < sdo_->sdoTable->columnCount() && !match;
           ++column) {
        const auto *item = sdo_->sdoTable->item(row, column);
        match = item && item->text().contains(needle, Qt::CaseInsensitive);
      }
    }
    if (match) {
      ++visible;
    }
    sdo_->sdoTable->setRowHidden(row, !match);
  }
  if (tagMode) {
    updateDiagnostics("Info", "SDO",
                      QString("Object Dictionary filter '%1': %2/%3 visible, "
                              "%4 with evidence, %5 failed")
                          .arg(needle)
                          .arg(visible)
                          .arg(sdo_->sdoTable->rowCount())
                          .arg(evidenceRows)
                          .arg(failedRows));
  }
  if (sdo_->sdoSummaryLabel) {
    sdo_->sdoSummaryLabel->setText(uiText("%1/%2 | W %3 | E %4 | F %5",
                                     "%1/%2 | 可写 %3 | 证据 %4 | 失败 %5")
                                  .arg(visible)
                                  .arg(sdo_->sdoTable->rowCount())
                                  .arg(writableRows)
                                  .arg(evidenceRows)
                                  .arg(failedRows));
    sdo_->sdoSummaryLabel->setToolTip(
        uiText("%1/%2 visible, %3 writable, %4 with evidence, %5 failed",
               "%1/%2 可见，%3 可写，%4 有证据，%5 失败")
            .arg(visible)
            .arg(sdo_->sdoTable->rowCount())
            .arg(writableRows)
            .arg(evidenceRows)
            .arg(failedRows));
  }
  updateActionAvailability();
}
