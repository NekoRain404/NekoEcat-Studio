// Context menus for topology tree, tables, and SDO target panel.

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


// — Build and display the right-click menu for the topology tree
void MainWindow::showTopologyContextMenu(const QPoint &position) {
  if (!topologyTree_) {
    return;
  }
  auto *item = topologyTree_->itemAt(position);
  if (item) {
    topologyTree_->setCurrentItem(item);
  }

  const int slave = selectedPosition();
  const bool hasSlave = slave >= 0;
  const bool connected = client_.isConnected();

  QMenu menu(this);
  auto *refresh =
      menu.addAction(style()->standardIcon(QStyle::SP_BrowserReload),
                     uiText("Refresh Online Data", "刷新在线数据"));
  refresh->setEnabled(connected);
  auto *rescan =
      menu.addAction(style()->standardIcon(QStyle::SP_FileDialogDetailedView),
                     uiText("Rescan Bus", "重新扫描总线"));
  rescan->setEnabled(connected);
  menu.addSeparator();

  auto *readInfo =
      menu.addAction(style()->standardIcon(QStyle::SP_FileDialogInfoView),
                     uiText("Read Selected Slave", "读取选中从站"));
  readInfo->setEnabled(connected && hasSlave);
  auto *addStartup = menu.addAction(
      style()->standardIcon(QStyle::SP_FileDialogNewFolder),
      uiText("Add Startup SDO from Current Fields", "用当前字段添加启动 SDO"));
  addStartup->setEnabled(connected && hasSlave);
  menu.addSeparator();

  auto *init = menu.addAction("INIT");
  auto *preop = menu.addAction("PREOP");
  auto *safeop = menu.addAction("SAFEOP");
  auto *op = menu.addAction("OP");
  for (auto *action : {init, preop, safeop, op}) {
    action->setEnabled(connected && hasSlave);
  }
  menu.addSeparator();
  auto *allInit = menu.addAction(uiText("All Slaves: INIT", "全部从站：INIT"));
  auto *allPreop =
      menu.addAction(uiText("All Slaves: PREOP", "全部从站：PREOP"));
  auto *allSafeop =
      menu.addAction(uiText("All Slaves: SAFEOP", "全部从站：SAFEOP"));
  auto *allOp = menu.addAction(uiText("All Slaves: OP", "全部从站：OP"));
  for (auto *action : {allInit, allPreop, allSafeop, allOp}) {
    action->setEnabled(connected && !slaves_.isEmpty());
  }
  menu.addSeparator();

  auto *copyDevice =
      menu.addAction(uiText("Copy Device Label", "复制设备标签"));
  copyDevice->setEnabled(item != nullptr);
  auto *copyTopology = menu.addAction(uiText("Copy Topology", "复制拓扑"));
  copyTopology->setEnabled(topologyTree_->topLevelItemCount() > 0);

  const QAction *chosen =
      menu.exec(topologyTree_->viewport()->mapToGlobal(position));
  if (!chosen) {
    return;
  }
  if (chosen == refresh) {
    requestRefresh();
  } else if (chosen == rescan) {
    client_.rescan();
  } else if (chosen == readInfo && hasSlave) {
    setSelectedSlave(slave);
  } else if (chosen == addStartup && hasSlave) {
    addStartupSdo();
  } else if (chosen == init && hasSlave) {
    requestSlaveStateWithConfirmation(slave, "INIT");
  } else if (chosen == preop && hasSlave) {
    requestSlaveStateWithConfirmation(slave, "PREOP");
  } else if (chosen == safeop && hasSlave) {
    requestSlaveStateWithConfirmation(slave, "SAFEOP");
  } else if (chosen == op && hasSlave) {
    requestSlaveStateWithConfirmation(slave, "OP");
  } else if (chosen == allInit) {
    requestAllSlaveState("INIT");
  } else if (chosen == allPreop) {
    requestAllSlaveState("PREOP");
  } else if (chosen == allSafeop) {
    requestAllSlaveState("SAFEOP");
  } else if (chosen == allOp) {
    requestAllSlaveState("OP");
  } else if (chosen == copyDevice && item) {
    QApplication::clipboard()->setText(
        QString("%1\t%2").arg(item->text(0), item->text(1)));
    log("Copied topology item to clipboard");
  } else if (chosen == copyTopology) {
    QStringList rows;
    for (int top = 0; top < topologyTree_->topLevelItemCount(); ++top) {
      auto *master = topologyTree_->topLevelItem(top);
      rows << QString("%1\t%2").arg(master->text(0), master->text(1));
      for (int child = 0; child < master->childCount(); ++child) {
        auto *slaveItem = master->child(child);
        rows << QString("  %1\t%2").arg(slaveItem->text(0), slaveItem->text(1));
      }
    }
    QApplication::clipboard()->setText(rows.join('\n')); // copy to system clipboard
    log("Copied topology to clipboard");
  }
}


// — Build and display the right-click menu for any evidence table
void MainWindow::showTableContextMenu(QTableWidget *table,
                                      const QPoint &position) {
  if (!table) {
    return;
  }
  const int rowAtCursor = table->indexAt(position).row();
  if (rowAtCursor >= 0) {
    const int currentColumn =
        table->currentColumn() >= 0 ? table->currentColumn() : 0;
    if (table->columnCount() > 0) {
      table->setCurrentCell(rowAtCursor,
                            std::min(currentColumn, table->columnCount() - 1),
                            QItemSelectionModel::NoUpdate);
    }
    if (!table->selectionModel() ||
        !table->selectionModel()->isRowSelected(rowAtCursor, QModelIndex())) {
      table->selectRow(rowAtCursor);
    }
  }

  auto tableText = [](QTableWidget *source, int row, int column) {
    if (!source || row < 0 || row >= source->rowCount() || column < 0 ||
        column >= source->columnCount()) {
      return QString();
    }
    const auto *item = source->item(row, column);
    return item ? item->text().trimmed() : QString();
  };
  auto normalizedHex = [](QString value, int minimumDigits) {
    value = value.trimmed();
    if (value.isEmpty()) {
      return value;
    }
    QString digits = value;
    if (digits.startsWith("0x", Qt::CaseInsensitive)) {
      digits = digits.mid(2);
    }
    bool ok = false;
    const quint64 parsed = digits.toULongLong(&ok, 16);
    if (!ok) {
      return value;
    }
    return QString("0x%1")
        .arg(parsed, minimumDigits, 16, QLatin1Char('0'))
        .toLower();
  };
  auto parsedPosition = [](const QString &value) {
    bool ok = false;
    const int parsed = value.trimmed().toInt(&ok);
    return ok ? parsed : -1;
  };

  struct ObjectClipboardPayload {
    int position = -1;
    QString index;
    QString subIndex;
    QString type;
    QString value;

    bool hasAddress() const { return !index.isEmpty() && !subIndex.isEmpty(); }
    QString addressText() const {
      const QString object = QString("%1:%2").arg(index, subIndex);
      return position >= 0 ? QString("#%1 %2").arg(position).arg(object)
                           : object;
    }
    QString valueText() const {
      QString line = addressText();
      if (!type.isEmpty()) {
        line += " " + type;
      }
      if (!value.isEmpty()) {
        line += " = " + value;
      }
      return line;
    }
  };

  ObjectClipboardPayload objectClipboard;
  const int currentRow = table->currentRow();
  if (currentRow >= 0) {
    if (table == sdo_->sdoTable) {
      objectClipboard.position = selectedPosition();
      objectClipboard.index = normalizedHex(tableText(table, currentRow, 1), 4);
      objectClipboard.subIndex =
          normalizedHex(tableText(table, currentRow, 2), 2);
      objectClipboard.type = tableText(table, currentRow, 4);
      objectClipboard.value = tableText(table, currentRow, 7);
    } else if (table == sdo_->pdoTable) {
      objectClipboard.position = selectedPosition();
      objectClipboard.index = normalizedHex(tableText(table, currentRow, 2), 4);
      objectClipboard.subIndex =
          normalizedHex(tableText(table, currentRow, 3), 2);
    } else if (table == watch_->watchTable) {
      objectClipboard.position =
          parsedPosition(tableText(table, currentRow, 1));
      objectClipboard.index = normalizedHex(tableText(table, currentRow, 2), 4);
      objectClipboard.subIndex =
          normalizedHex(tableText(table, currentRow, 3), 2);
      objectClipboard.value = tableText(table, currentRow, 4);
      objectClipboard.type = tableText(table, currentRow, 6);
    } else if (table == freeRunWidgets_->freeRunEntryTable) {
      objectClipboard.position =
          parsedPosition(tableText(table, currentRow, 0));
      objectClipboard.index = normalizedHex(tableText(table, currentRow, 4), 4);
      objectClipboard.subIndex =
          normalizedHex(tableText(table, currentRow, 5), 2);
      objectClipboard.value = tableText(table, currentRow, 11);
      if (objectClipboard.value.isEmpty()) {
        objectClipboard.value = tableText(table, currentRow, 10);
      }
    } else if (table == ioVar_->ioVariableTable) {
      objectClipboard.position =
          parsedPosition(tableText(table, currentRow, 0));
      objectClipboard.index = normalizedHex(tableText(table, currentRow, 3), 4);
      objectClipboard.subIndex =
          normalizedHex(tableText(table, currentRow, 4), 2);
      objectClipboard.value = tableText(table, currentRow, 8);
      if (objectClipboard.value.isEmpty()) {
        objectClipboard.value = tableText(table, currentRow, 11);
      }
    } else if (table == sdoHistoryTable_) {
      objectClipboard.position =
          parsedPosition(tableText(table, currentRow, 2));
      objectClipboard.index = normalizedHex(tableText(table, currentRow, 3), 4);
      objectClipboard.subIndex =
          normalizedHex(tableText(table, currentRow, 4), 2);
      objectClipboard.type = tableText(table, currentRow, 5);
      objectClipboard.value = tableText(table, currentRow, 6);
    } else if (table == startupSdoTable_) {
      objectClipboard.position =
          parsedPosition(tableText(table, currentRow, 0));
      objectClipboard.index = normalizedHex(tableText(table, currentRow, 1), 4);
      objectClipboard.subIndex =
          normalizedHex(tableText(table, currentRow, 2), 2);
      objectClipboard.value = tableText(table, currentRow, 3);
      objectClipboard.type = tableText(table, currentRow, 4);
    } else if (table == bookmark_->objectBookmarkTable) {
      objectClipboard.position =
          parsedPosition(tableText(table, currentRow, 0));
      objectClipboard.index = normalizedHex(tableText(table, currentRow, 2), 4);
      objectClipboard.subIndex =
          normalizedHex(tableText(table, currentRow, 3), 2);
      objectClipboard.value = tableText(table, currentRow, 8);
      objectClipboard.type = tableText(table, currentRow, 5);
    } else if (table == sdoTargetTrailTable_) {
      objectClipboard.position =
          parsedPosition(tableText(table, currentRow, 1));
      objectClipboard.index = normalizedHex(tableText(table, currentRow, 2), 4);
      objectClipboard.subIndex =
          normalizedHex(tableText(table, currentRow, 3), 2);
      objectClipboard.type = tableText(table, currentRow, 4);
      objectClipboard.value = tableText(table, currentRow, 7);
      if (objectClipboard.value.isEmpty()) {
        objectClipboard.value = tableText(table, currentRow, 6);
      }
    }
  }

  QMenu menu(this);
  QAction *fillSdo = nullptr;
  QAction *readSdo = nullptr;
  QAction *readSelectedDictionary = nullptr;
  QAction *readVisibleDictionaryAction = nullptr;
  QAction *readFailedDictionaryAction = nullptr;
  QAction *addWatch = nullptr;
  QAction *addSelectedDictionaryWatch = nullptr;
  QAction *addVisibleDictionaryWatch = nullptr;
  QAction *addSelectedDictionaryStartupEvidence = nullptr;
  QAction *addStartup = nullptr;
  QAction *filterDictionaryEvidence = nullptr;
  QAction *filterDictionaryFailed = nullptr;
  QAction *copyDictionaryEvidence = nullptr;
  QAction *bookmarkDictionaryObject = nullptr;
  QAction *bookmarkSelectedDictionaryObjects = nullptr;
  QAction *fillSdoFromPdo = nullptr;
  QAction *readSdoFromPdo = nullptr;
  QAction *addWatchFromPdo = nullptr;
  QAction *addSelectedPdoWatch = nullptr;
  QAction *fillSdoFromWatch = nullptr;
  QAction *readSdoFromWatch = nullptr;
  QAction *readWatch = nullptr;
  QAction *captureWatchBaselineAction = nullptr;
  QAction *clearWatchBaselineAction = nullptr;
  QAction *addStartupFromWatch = nullptr;
  QAction *addStartupFromSelectedWatch = nullptr;
  QAction *syncStartupFromWatch = nullptr;
  QAction *syncStartupFromSelectedWatch = nullptr;
  QAction *removeWatch = nullptr;
  QAction *fillSdoFromFreeRun = nullptr;
  QAction *readSdoFromFreeRun = nullptr;
  QAction *addWatchFromFreeRun = nullptr;
  QAction *fillSdoFromIoVariable = nullptr;
  QAction *readSdoFromIoVariable = nullptr;
  QAction *addWatchFromIoVariable = nullptr;
  QAction *addVisibleIoVariableWatch = nullptr;
  QAction *addStartupFromIoVariable = nullptr;
  QAction *addVisibleIoVariableStartup = nullptr;
  QAction *editIoVariableMetadataAction = nullptr;
  QAction *bulkNameIoVariableAction = nullptr;
  QAction *reviewPlcHandoffAction = nullptr;
  QAction *copySelectedPlcDeclarationsAction = nullptr;
  QAction *copyVisiblePlcDeclarationsAction = nullptr;
  QAction *exportPlcDeclarationsAction = nullptr;
  QAction *clearIoVariableMetadataAction = nullptr;
  QAction *exportIoVariableCsvAction = nullptr;
  QAction *exportIoVariablePlcCsvAction = nullptr;
  QAction *fillSdoFromHistory = nullptr;
  QAction *readSdoFromHistory = nullptr;
  QAction *addWatchFromHistory = nullptr;
  QAction *addSelectedHistoryWatch = nullptr;
  QAction *addStartupFromHistory = nullptr;
  QAction *addStartupFromSelectedHistory = nullptr;
  QAction *fillSdoFromStartup = nullptr;
  QAction *readSdoFromStartup = nullptr;
  QAction *addWatchFromStartup = nullptr;
  QAction *fillSdoFromBookmark = nullptr;
  QAction *readSdoFromBookmark = nullptr;
  QAction *addWatchFromBookmark = nullptr;
  QAction *addStartupFromBookmark = nullptr;
  QAction *removeBookmark = nullptr;
  QAction *openConsistencyEvidence = nullptr;
  QAction *verifyStartupRow = nullptr;
  QAction *applyStartupRow = nullptr;
  QAction *verifySelectedStartupRows = nullptr;
  QAction *focusStartupWatchDiffRows = nullptr;
  QAction *toggleStartupWatchDiffRows = nullptr;
  QAction *applyStartupWatchDiffRows = nullptr;
  QAction *applySelectedStartupRows = nullptr;
  QAction *moveStartupRowUp = nullptr;
  QAction *moveStartupRowDown = nullptr;
  QAction *removeStartupRow = nullptr;
  QAction *reviewFirstWorkflowIssue = nullptr;
  QAction *reviewNextWorkflowIssue = nullptr;
  QAction *copyWorkflowStep = nullptr;
  QAction *openSessionBriefEvidence = nullptr;
  QAction *copySessionBriefRow = nullptr;
  QAction *openSlaveMatrixEvidence = nullptr;
  QAction *copySlaveMatrixRow = nullptr;
  QAction *reviewFirstMatrixIssue = nullptr;
  QAction *reviewNextMatrixIssue = nullptr;
  QAction *restoreSdoTargetTrail = nullptr;
  QAction *watchSdoTargetTrail = nullptr;
  QAction *bookmarkSdoTargetTrail = nullptr;
  QAction *startupSdoTargetTrail = nullptr;
  QAction *removeSdoTargetTrail = nullptr;
  QAction *clearSdoTargetTrailAction = nullptr;
  QAction *copyObjectAddress = nullptr;
  QAction *copyObjectValue = nullptr;
  // Dispatch Alt+Enter to the correct evidence action for this table type
  if (table == workflow_->workflowTable) {
    const int workflowRow = table->currentRow();
    const bool hasWorkflowRow = workflowRow >= 0 &&
                                workflowRow < table->rowCount() &&
                                !table->isRowHidden(workflowRow);
    reviewFirstWorkflowIssue = menu.addAction(
        uiText("Review First Workflow Issue", "审阅首个工作流问题"));
    reviewFirstWorkflowIssue->setIcon(
        style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    reviewFirstWorkflowIssue->setEnabled(
        workflow_->workflowReviewButton ? workflow_->workflowReviewButton->isEnabled()
                              : table->rowCount() > 0);
    reviewNextWorkflowIssue = menu.addAction(
        uiText("Review Next Workflow Issue", "审阅下个工作流问题"));
    reviewNextWorkflowIssue->setIcon(
        style()->standardIcon(QStyle::SP_ArrowForward));
    reviewNextWorkflowIssue->setEnabled(
        workflow_->workflowReviewNextButton ? workflow_->workflowReviewNextButton->isEnabled()
                                  : table->rowCount() > 0);
    copyWorkflowStep =
        menu.addAction(uiText("Copy Step Evidence", "复制步骤证据"));
    copyWorkflowStep->setIcon(
        style()->standardIcon(QStyle::SP_FileDialogContentsView));
    copyWorkflowStep->setEnabled(hasWorkflowRow);
    menu.addSeparator();
  // Dispatch Alt+Enter to the correct evidence action for this table type
  } else if (table == session_->sessionBriefTable) {
    const bool hasBriefRow = table->currentRow() >= 0;
    openSessionBriefEvidence =
        menu.addAction(uiText("Open Local Evidence", "打开本地证据"));
    openSessionBriefEvidence->setEnabled(hasBriefRow);
    copySessionBriefRow =
        menu.addAction(uiText("Copy Row Evidence", "复制本行证据"));
    copySessionBriefRow->setIcon(
        style()->standardIcon(QStyle::SP_FileDialogContentsView));
    copySessionBriefRow->setEnabled(hasBriefRow);
    menu.addSeparator();
  } else if (table == slaveEvidence_->slaveEvidenceMatrixTable) {
    const bool hasMatrixRow = table->currentRow() >= 0;
    openSlaveMatrixEvidence =
        menu.addAction(uiText("Open Matrix Evidence", "打开矩阵证据"));
    openSlaveMatrixEvidence->setEnabled(hasMatrixRow);
    copySlaveMatrixRow =
        menu.addAction(uiText("Copy Matrix Row Evidence", "复制矩阵本行证据"));
    copySlaveMatrixRow->setEnabled(hasMatrixRow);
    reviewFirstMatrixIssue =
        menu.addAction(uiText("Review First Matrix Issue", "审阅首个矩阵问题"));
    reviewFirstMatrixIssue->setEnabled(
        slaveEvidence_->slaveEvidenceMatrixReviewButton
            ? slaveEvidence_->slaveEvidenceMatrixReviewButton->isEnabled()
            : table->rowCount() > 0);
    reviewNextMatrixIssue =
        menu.addAction(uiText("Review Next Matrix Issue", "审阅下个矩阵问题"));
    reviewNextMatrixIssue->setEnabled(
        slaveEvidence_->slaveEvidenceMatrixReviewNextButton
            ? slaveEvidence_->slaveEvidenceMatrixReviewNextButton->isEnabled()
            : table->rowCount() > 0);
    menu.addSeparator();
  } else if (table == sdo_->sdoTable) {
    const bool hasDictionaryRow = table->currentRow() >= 0;
    const bool dictionaryReady =
        selectedPosition() >= 0 && loadedSdoPosition_ == selectedPosition();
    const bool hasDictionarySelection = !selectedDictionaryRows().isEmpty();
    bool hasVisibleDictionaryRows = false;
    for (int row = 0; row < table->rowCount(); ++row) {
      if (!table->isRowHidden(row)) {
        hasVisibleDictionaryRows = true;
        break;
      }
    }
    fillSdo = menu.addAction(uiText("Fill SDO Fields", "填充 SDO 字段"));
    fillSdo->setEnabled(dictionaryReady && hasDictionaryRow);
    readSdo = menu.addAction(uiText("Fill and Read", "填充并读取"));
    readSdo->setEnabled(dictionaryReady && hasDictionaryRow &&
                        client_.isConnected());
    readSelectedDictionary =
        menu.addAction(uiText("Read Selected Objects", "读取所选对象"));
    readSelectedDictionary->setEnabled(hasDictionarySelection &&
                                       client_.isConnected() &&
                                       selectedPosition() >= 0);
    readVisibleDictionaryAction =
        menu.addAction(uiText("Read Visible Objects", "读取可见对象"));
    readVisibleDictionaryAction->setEnabled(
        dictionaryReady && hasVisibleDictionaryRows && client_.isConnected());
    readFailedDictionaryAction =
        menu.addAction(uiText("Retry Failed Objects", "重试失败对象"));
    readFailedDictionaryAction->setEnabled(
        dictionaryReady && hasFailedSdoEvidence() && client_.isConnected() &&
        selectedPosition() >= 0);
    addWatch = menu.addAction(uiText("Add to Watch", "加入监视"));
    addWatch->setEnabled(dictionaryReady && hasDictionaryRow);
    addSelectedDictionaryWatch =
        menu.addAction(uiText("Add Selected to Watch", "所选加入监视"));
    addSelectedDictionaryWatch->setEnabled(hasDictionarySelection &&
                                           selectedPosition() >= 0);
    addVisibleDictionaryWatch =
        menu.addAction(uiText("Add Visible to Watch", "将可见项加入监视"));
    addVisibleDictionaryWatch->setEnabled(dictionaryReady &&
                                          hasVisibleDictionaryRows);
    bool hasDictionaryValueSelection = false;
    for (const int selectedRow : selectedDictionaryRows()) {
      if (selectedRow >= 0 && selectedRow < table->rowCount() &&
          !table->isRowHidden(selectedRow) &&
          !tableText(table, selectedRow, 7).isEmpty()) {
        hasDictionaryValueSelection = true;
        break;
      }
    }
    addSelectedDictionaryStartupEvidence =
        menu.addAction(uiText("Create Startup SDOs from Selected Evidence",
                              "从所选证据创建 Startup SDO"));
    addSelectedDictionaryStartupEvidence->setEnabled(
        dictionaryReady && hasDictionaryValueSelection);
    addStartup = menu.addAction(uiText("Add Startup SDO", "添加启动 SDO"));
    addStartup->setEnabled(dictionaryReady && hasDictionaryRow &&
                           selectedSdoWritable_);
    bookmarkDictionaryObject =
        menu.addAction(uiText("Bookmark Object", "收藏对象"));
    bookmarkDictionaryObject->setEnabled(dictionaryReady && hasDictionaryRow);
    bookmarkSelectedDictionaryObjects =
        menu.addAction(uiText("Bookmark Selected Objects", "收藏所选对象"));
    bookmarkSelectedDictionaryObjects->setEnabled(dictionaryReady &&
                                                  hasDictionarySelection);
    menu.addSeparator();
    filterDictionaryEvidence =
        menu.addAction(uiText("Show Evidence Rows", "只看有证据行"));
    filterDictionaryEvidence->setEnabled(dictionaryReady &&
                                         table->rowCount() > 0);
    filterDictionaryFailed =
        menu.addAction(uiText("Show Failed Evidence", "只看失败证据"));
    filterDictionaryFailed->setEnabled(dictionaryReady &&
                                       table->rowCount() > 0);
    copyDictionaryEvidence =
        menu.addAction(uiText("Copy Last Evidence", "复制最后证据"));
    copyDictionaryEvidence->setEnabled(
        dictionaryReady && hasDictionaryRow &&
        (!tableText(table, table->currentRow(), 7).isEmpty() ||
         !tableText(table, table->currentRow(), 8).isEmpty()));
    menu.addSeparator();
  } else if (table == sdo_->pdoTable) {
    const bool hasPdoRow = table->currentRow() >= 0;
    const bool pdoReady =
        selectedPosition() >= 0 && loadedPdoPosition_ == selectedPosition();
    const bool hasPdoSelection = !selectedTableRows(table).isEmpty();
    fillSdoFromPdo = menu.addAction(uiText("Fill SDO Fields", "填充 SDO 字段"));
    fillSdoFromPdo->setEnabled(pdoReady && hasPdoRow);
    readSdoFromPdo =
        menu.addAction(uiText("Fill and Read SDO", "填充并读取 SDO"));
    readSdoFromPdo->setEnabled(pdoReady && hasPdoRow && client_.isConnected());
    addWatchFromPdo = menu.addAction(uiText("Add SDO Watch", "加入 SDO 监视"));
    addWatchFromPdo->setEnabled(pdoReady && hasPdoRow);
    addSelectedPdoWatch = menu.addAction(
        uiText("Add Selected PDOs to Watch", "将选中 PDO 加入监视"));
    addSelectedPdoWatch->setEnabled(pdoReady && hasPdoSelection);
    menu.addSeparator();
  } else if (table == watch_->watchTable) {
    const bool hasWatchRow = table->currentRow() >= 0;
    bool hasWatchValueSelection = selectedWatchRowsHaveValue();
    fillSdoFromWatch =
        menu.addAction(uiText("Fill SDO Fields", "填充 SDO 字段"));
    fillSdoFromWatch->setEnabled(hasWatchRow);
    readSdoFromWatch =
        menu.addAction(uiText("Fill and Read SDO", "填充并读取 SDO"));
    readSdoFromWatch->setEnabled(hasWatchRow && client_.isConnected());
    readWatch = menu.addAction(uiText("Read Watch Item", "读取监视项"));
    readWatch->setEnabled(hasWatchRow && client_.isConnected());
    captureWatchBaselineAction =
        menu.addAction(uiText("Capture Watch Baseline", "捕获 Watch 基线"));
    captureWatchBaselineAction->setEnabled(table->rowCount() > 0);
    clearWatchBaselineAction =
        menu.addAction(uiText("Clear Watch Baseline", "清除 Watch 基线"));
    clearWatchBaselineAction->setEnabled(table->rowCount() > 0);
    const QString watchValue =
        hasWatchRow && table->item(table->currentRow(), 4)
            ? table->item(table->currentRow(), 4)->text().trimmed()
            : QString();
    if (!hasWatchValueSelection && hasWatchRow) {
      hasWatchValueSelection = !watchValue.isEmpty();
    }
    addStartupFromWatch = menu.addAction(
        uiText("Add Startup SDO from Watch", "从监视添加启动 SDO"));
    addStartupFromWatch->setEnabled(hasWatchRow && !watchValue.isEmpty());
    addStartupFromSelectedWatch =
        menu.addAction(uiText("Create Startup SDOs from Selected Watch",
                              "从选中监视批量创建启动 SDO"));
    addStartupFromSelectedWatch->setEnabled(hasWatchValueSelection);
    syncStartupFromWatch = menu.addAction(
        uiText("Sync Startup SDO from Watch", "从监视同步 Startup SDO"));
    syncStartupFromWatch->setEnabled(hasWatchRow && !watchValue.isEmpty());
    syncStartupFromSelectedWatch = menu.addAction(uiText(
        "Sync Startup SDOs from Selected Watch", "从选中监视同步 Startup SDO"));
    syncStartupFromSelectedWatch->setEnabled(hasWatchValueSelection);
    removeWatch = menu.addAction(uiText("Remove Watch Item", "移除监视项"));
    removeWatch->setEnabled(hasWatchRow);
    menu.addSeparator();
  } else if (table == freeRunWidgets_->freeRunEntryTable) {
    const bool hasFreeRunRow = table->currentRow() >= 0;
    fillSdoFromFreeRun =
        menu.addAction(uiText("Fill SDO Fields", "填充 SDO 字段"));
    fillSdoFromFreeRun->setEnabled(hasFreeRunRow);
    readSdoFromFreeRun =
        menu.addAction(uiText("Fill and Read SDO", "填充并读取 SDO"));
    readSdoFromFreeRun->setEnabled(hasFreeRunRow && client_.isConnected());
    addWatchFromFreeRun =
        menu.addAction(uiText("Add SDO Watch", "加入 SDO 监视"));
    addWatchFromFreeRun->setEnabled(hasFreeRunRow);
    menu.addSeparator();
  } else if (table == ioVar_->ioVariableTable) {
    const bool hasIoRow = table->currentRow() >= 0;
    const QVector<int> selectedIoRows = selectedIoVariableRows(true);
    const QVector<int> visibleIoRows = visibleIoVariableRows();
    const bool hasVisibleIoRows = !visibleIoRows.isEmpty();
    const bool hasSelectedIoValueRows =
        ioVariableTableRowsContainValue(table, selectedIoRows);
    const bool hasVisibleIoValueRows =
        ioVariableTableRowsContainValue(table, visibleIoRows);
    fillSdoFromIoVariable =
        menu.addAction(uiText("Fill SDO Fields", "填充 SDO 字段"));
    fillSdoFromIoVariable->setEnabled(hasIoRow);
    readSdoFromIoVariable =
        menu.addAction(uiText("Fill and Read SDO", "填充并读取 SDO"));
    readSdoFromIoVariable->setEnabled(hasIoRow && client_.isConnected());
    addWatchFromIoVariable =
        menu.addAction(uiText("Add Selected to Watch", "所选加入监视"));
    addWatchFromIoVariable->setEnabled(hasIoRow);
    addVisibleIoVariableWatch =
        menu.addAction(uiText("Add Visible to Watch", "将可见项加入监视"));
    addVisibleIoVariableWatch->setEnabled(hasVisibleIoRows);
    addStartupFromIoVariable =
        menu.addAction(uiText("Create Startup SDOs from Selected I/O",
                              "从所选 I/O 创建 Startup SDO"));
    addStartupFromIoVariable->setEnabled(hasSelectedIoValueRows);
    addVisibleIoVariableStartup = menu.addAction(uiText(
        "Create Startup SDOs from Visible I/O", "从可见 I/O 创建 Startup SDO"));
    addVisibleIoVariableStartup->setEnabled(hasVisibleIoValueRows);
    editIoVariableMetadataAction =
        menu.addAction(uiText("Set Alias / Tags", "设置别名 / 标签"));
    editIoVariableMetadataAction->setEnabled(hasIoRow);
    bulkNameIoVariableAction = menu.addAction(
        uiText("Bulk Name Visible / Selected", "批量命名可见 / 所选"));
    bulkNameIoVariableAction->setEnabled(hasIoRow || hasVisibleIoRows);
    reviewPlcHandoffAction = menu.addAction(
        uiText("Review PLC Handoff Issues", "审阅 PLC 交接问题"));
    reviewPlcHandoffAction->setEnabled(hasVisibleIoRows);
    copySelectedPlcDeclarationsAction = menu.addAction(
        uiText("Copy Selected PLC Declarations", "复制所选 PLC 声明"));
    copySelectedPlcDeclarationsAction->setEnabled(hasIoRow);
    copyVisiblePlcDeclarationsAction = menu.addAction(
        uiText("Copy Visible PLC Declarations", "复制可见 PLC 声明"));
    copyVisiblePlcDeclarationsAction->setEnabled(hasVisibleIoRows);
    exportPlcDeclarationsAction = menu.addAction(
        uiText("Export PLC Declarations ST", "导出 PLC 声明 ST"));
    exportPlcDeclarationsAction->setEnabled(hasVisibleIoRows);
    clearIoVariableMetadataAction =
        menu.addAction(uiText("Clear Alias / Tags", "清除别名 / 标签"));
    clearIoVariableMetadataAction->setEnabled(hasIoRow);
    exportIoVariableCsvAction =
        menu.addAction(uiText("Export Visible CSV", "导出可见 CSV"));
    exportIoVariableCsvAction->setEnabled(table->rowCount() > 0);
    exportIoVariablePlcCsvAction =
        menu.addAction(uiText("Export PLC Symbols CSV", "导出 PLC 符号 CSV"));
    exportIoVariablePlcCsvAction->setEnabled(table->rowCount() > 0);
    menu.addSeparator();
  } else if (table == sdoHistoryTable_) {
    const int row = table->currentRow();
    const bool hasHistoryRow = row >= 0;
    const QVector<int> selectedRows = selectedSdoHistoryRows();
    const bool hasHistorySelection = !selectedRows.isEmpty();
    bool hasHistoryValueSelection = false;
    for (const int selectedRow : selectedRows) {
      if (selectedRow >= 0 && selectedRow < table->rowCount() &&
          !table->isRowHidden(selectedRow)) {
        const QString selectedStatus = table->item(selectedRow, 7)
                                           ? table->item(selectedRow, 7)->text()
                                           : QString();
        const QString selectedValue = table->item(selectedRow, 6)
                                          ? table->item(selectedRow, 6)->text()
                                          : QString();
        if (!isSdoHistoryStartupSource(selectedStatus, selectedValue)) {
          continue;
        }
        hasHistoryValueSelection = true;
        break;
      }
    }
    const QString value = hasHistoryRow && table->item(row, 6)
                              ? table->item(row, 6)->text().trimmed()
                              : QString();
    fillSdoFromHistory =
        menu.addAction(uiText("Fill SDO Fields", "填充 SDO 字段"));
    fillSdoFromHistory->setEnabled(hasHistoryRow);
    readSdoFromHistory =
        menu.addAction(uiText("Fill and Read SDO", "填充并读取 SDO"));
    readSdoFromHistory->setEnabled(hasHistoryRow && client_.isConnected());
    addWatchFromHistory =
        menu.addAction(uiText("Add SDO Watch", "加入 SDO 监视"));
    addWatchFromHistory->setEnabled(hasHistoryRow);
    addSelectedHistoryWatch = menu.addAction(
        uiText("Add Selected History to Watch", "将所选历史加入监视"));
    addSelectedHistoryWatch->setEnabled(hasHistorySelection);
    addStartupFromHistory = menu.addAction(
        uiText("Add Startup SDO from History", "从历史添加启动 SDO"));
    addStartupFromHistory->setEnabled(hasHistoryRow && !value.isEmpty());
    addStartupFromSelectedHistory = menu.addAction(uiText(
        "Create Startup SDOs from Selected History", "从所选历史创建启动 SDO"));
    addStartupFromSelectedHistory->setEnabled(hasHistoryValueSelection);
    menu.addSeparator();
  } else if (table == startupSdoTable_) {
    const int row = table->currentRow();
    const bool hasStartupRow = row >= 0;
    const bool hasSelectedStartupRows = !selectedStartupSdoRows().isEmpty();
    fillSdoFromStartup =
        menu.addAction(uiText("Fill SDO Fields", "填充 SDO 字段"));
    fillSdoFromStartup->setEnabled(hasStartupRow);
    readSdoFromStartup =
        menu.addAction(uiText("Fill and Read SDO", "填充并读取 SDO"));
    readSdoFromStartup->setEnabled(hasStartupRow && client_.isConnected());
    addWatchFromStartup =
        menu.addAction(uiText("Add SDO Watch", "加入 SDO 监视"));
    addWatchFromStartup->setEnabled(hasStartupRow);
    verifyStartupRow = menu.addAction(uiText("Verify This Row", "校验此行"));
    verifyStartupRow->setEnabled(hasStartupRow && client_.isConnected());
    applyStartupRow = menu.addAction(uiText("Apply This Row", "应用此行"));
    applyStartupRow->setEnabled(hasStartupRow && client_.isConnected());
    verifySelectedStartupRows =
        menu.addAction(uiText("Verify Selected Rows", "校验所选行"));
    verifySelectedStartupRows->setEnabled(hasSelectedStartupRows &&
                                          client_.isConnected());
    const bool hasStartupWatchDiffs = !startupSdoRowsWithWatchDiffs().isEmpty();
    focusStartupWatchDiffRows =
        menu.addAction(uiText("Review Watch Diffs", "审阅 Watch 偏差"));
    focusStartupWatchDiffRows->setEnabled(hasStartupWatchDiffs);
    toggleStartupWatchDiffRows =
        menu.addAction(uiText("Show Watch Diffs Only", "只显示 Watch 偏差"));
    toggleStartupWatchDiffRows->setCheckable(true);
    toggleStartupWatchDiffRows->setChecked(watch_->startupWatchDiffsOnly &&
                                           watch_->startupWatchDiffsOnly->isChecked());
    toggleStartupWatchDiffRows->setEnabled(table->rowCount() > 0);
    applyStartupWatchDiffRows =
        menu.addAction(uiText("Apply Watch Diffs", "应用 Watch 偏差"));
    applyStartupWatchDiffRows->setEnabled(client_.isConnected() &&
                                          hasStartupWatchDiffs);
    applySelectedStartupRows =
        menu.addAction(uiText("Apply Selected Rows", "应用所选行"));
    applySelectedStartupRows->setEnabled(hasSelectedStartupRows &&
                                         client_.isConnected());
    menu.addSeparator();
    moveStartupRowUp = menu.addAction(uiText("Move Row Up", "上移此行"));
    moveStartupRowUp->setEnabled(hasStartupRow && row > 0);
    moveStartupRowDown = menu.addAction(uiText("Move Row Down", "下移此行"));
    moveStartupRowDown->setEnabled(hasStartupRow &&
                                   row < table->rowCount() - 1);
    removeStartupRow =
        menu.addAction(uiText("Remove Selected Rows", "删除所选行"));
    removeStartupRow->setEnabled(hasSelectedStartupRows);
    menu.addSeparator();
  } else if (table == bookmark_->objectBookmarkTable) {
    const int row = table->currentRow();
    const bool hasBookmarkRow = row >= 0;
    const bool hasBookmarkSelection = !selectedObjectBookmarkRows().isEmpty();
    fillSdoFromBookmark =
        menu.addAction(uiText("Fill SDO Fields", "填充 SDO 字段"));
    fillSdoFromBookmark->setEnabled(hasBookmarkRow);
    readSdoFromBookmark =
        menu.addAction(uiText("Fill and Read SDO", "填充并读取 SDO"));
    readSdoFromBookmark->setEnabled(hasBookmarkRow && client_.isConnected());
    addWatchFromBookmark =
        menu.addAction(uiText("Add Bookmark to Watch", "书签加入监视"));
    addWatchFromBookmark->setEnabled(hasBookmarkSelection);
    addStartupFromBookmark = menu.addAction(
        uiText("Create Startup SDO from Bookmark", "从书签创建 Startup SDO"));
    addStartupFromBookmark->setEnabled(hasBookmarkSelection);
    removeBookmark =
        menu.addAction(uiText("Remove Selected Bookmarks", "移除所选书签"));
    removeBookmark->setEnabled(hasBookmarkSelection);
    menu.addSeparator();
  } else if (table == sdoTargetTrailTable_) {
    const bool hasTrailRow = table->currentRow() >= 0;
    const bool trailStartupReady =
        hasTrailRow && sdoTargetTrailRowCanCreateStartup(table->currentRow());
    restoreSdoTargetTrail =
        menu.addAction(uiText("Restore Target", "恢复目标"));
    restoreSdoTargetTrail->setEnabled(hasTrailRow);
    watchSdoTargetTrail =
        menu.addAction(uiText("Add Target to Watch", "目标加入 Watch"));
    watchSdoTargetTrail->setEnabled(hasTrailRow);
    bookmarkSdoTargetTrail =
        menu.addAction(uiText("Bookmark Target", "收藏目标"));
    bookmarkSdoTargetTrail->setEnabled(hasTrailRow);
    startupSdoTargetTrail = menu.addAction(
        uiText("Create Startup SDO from Target", "从目标创建 Startup SDO"));
    startupSdoTargetTrail->setEnabled(trailStartupReady);
    menu.addSeparator();
    removeSdoTargetTrail =
        menu.addAction(uiText("Remove Selected Targets", "移除所选目标"));
    removeSdoTargetTrail->setEnabled(hasTrailRow);
    clearSdoTargetTrailAction =
        menu.addAction(uiText("Clear Target Trail", "清空目标轨迹"));
    clearSdoTargetTrailAction->setEnabled(table->rowCount() > 0);
    menu.addSeparator();
  } else if (table == consistency_->consistencyTable) {
    const bool hasConsistencyRow = table->currentRow() >= 0;
    openConsistencyEvidence =
        menu.addAction(uiText("Open Evidence", "打开证据"));
    openConsistencyEvidence->setEnabled(hasConsistencyRow);
    menu.addSeparator();
  }
  if (objectClipboard.hasAddress()) {
    copyObjectAddress =
        menu.addAction(uiText("Copy Object Address", "复制对象地址"));
    copyObjectValue =
        menu.addAction(uiText("Copy Object and Value", "复制对象和值"));
    copyObjectValue->setEnabled(!objectClipboard.value.isEmpty());
    menu.addSeparator();
  }
  auto *copySelected =
      menu.addAction(uiText("Copy Selected Rows", "复制所选行"));
  copySelected->setEnabled(!table->selectedRanges().isEmpty());
  auto *copyAll = menu.addAction(uiText("Copy Table", "复制整表"));
  copyAll->setEnabled(table->rowCount() > 0 && table->columnCount() > 0);
  menu.addSeparator();
  auto *resizeColumns =
      menu.addAction(uiText("Autosize Columns", "自动调整列宽"));
  auto *clearSelection = menu.addAction(uiText("Clear Selection", "清除选择"));
  clearSelection->setEnabled(!table->selectedRanges().isEmpty());

  const QAction *chosen = menu.exec(table->viewport()->mapToGlobal(position));
  if (!chosen) {
    return;
  }
  if (chosen == reviewFirstWorkflowIssue) {
    reviewFirstCommissioningWorkflowIssue();
  } else if (chosen == reviewNextWorkflowIssue) {
    reviewNextCommissioningWorkflowIssue();
  } else if (chosen == copyWorkflowStep) {
    copyWorkflowStepDigest(table->currentRow());
  } else if (chosen == openSessionBriefEvidence) {
    openSessionBriefRow(table->currentRow());
  } else if (chosen == copySessionBriefRow) {
    copySessionBriefRowDigest(table->currentRow());
  } else if (chosen == openSlaveMatrixEvidence) {
    openSlaveEvidenceMatrixRow(table->currentRow());
  } else if (chosen == copySlaveMatrixRow) {
    copySlaveEvidenceMatrixRowDigest(table->currentRow());
  } else if (chosen == reviewFirstMatrixIssue) {
    reviewFirstSlaveEvidenceMatrixIssue();
  } else if (chosen == reviewNextMatrixIssue) {
    reviewNextSlaveEvidenceMatrixIssue();
  } else if (chosen == restoreSdoTargetTrail) {
    restoreSdoTargetTrailRow(table->currentRow());
  } else if (chosen == watchSdoTargetTrail) {
    addSdoTargetTrailRowToWatch();
  } else if (chosen == bookmarkSdoTargetTrail) {
    bookmarkSdoTargetTrailRow();
  } else if (chosen == startupSdoTargetTrail) {
    addSdoTargetTrailRowToStartup();
  } else if (chosen == removeSdoTargetTrail) {
    removeSelectedSdoTargetTrailRows();
  } else if (chosen == clearSdoTargetTrailAction) {
    clearSdoTargetTrail();
  } else if (chosen == fillSdo) {
    applySdoSelectionFromDictionary(table->currentRow(), false);
  } else if (chosen == readSdo) {
    applySdoSelectionFromDictionary(table->currentRow(), true);
  } else if (chosen == readSelectedDictionary) {
    readSelectedDictionaryRows();
  } else if (chosen == readVisibleDictionaryAction) {
    readVisibleDictionaryRows();
  } else if (chosen == readFailedDictionaryAction) {
    readFailedDictionaryRows();
  } else if (chosen == addWatch) {
    applySdoSelectionFromDictionary(table->currentRow(), false);
    addCurrentSdoToWatch();
  } else if (chosen == addSelectedDictionaryWatch) {
    addSelectedDictionaryRowsToWatch();
  } else if (chosen == addVisibleDictionaryWatch) {
    addVisibleDictionaryRowsToWatch();
  } else if (chosen == addSelectedDictionaryStartupEvidence) {
    addSelectedDictionaryEvidenceToStartupSdo();
  } else if (chosen == addStartup) {
    applySdoSelectionFromDictionary(table->currentRow(), false);
    addStartupSdo();
  } else if (chosen == bookmarkDictionaryObject) {
    addDictionaryRowsToBookmarks({table->currentRow()},
                                 uiText("context row", "右键行"));
  } else if (chosen == bookmarkSelectedDictionaryObjects) {
    addSelectedDictionaryRowsToBookmarks();
  } else if (chosen == filterDictionaryEvidence) {
    setSdoFilterPreset("tag:evidence");
  } else if (chosen == filterDictionaryFailed) {
    setSdoFilterPreset("tag:failed");
  } else if (chosen == copyDictionaryEvidence) {
    const int row = table->currentRow();
    const QString index = normalizedHex(tableText(table, row, 1), 4);
    const QString subIndex = normalizedHex(tableText(table, row, 2), 2);
    const QString type = tableText(table, row, 4);
    const QString lastValue = tableText(table, row, 7);
    const QString lastStatus = tableText(table, row, 8);
    const QString key = sdoEvidenceKey(selectedPosition(), index, subIndex);
    const QStringList evidence = sdoEvidence_.value(key);
    QStringList parts;
    parts << (selectedPosition() >= 0 ? QString("#%1 %2:%3")
                                            .arg(selectedPosition())
                                            .arg(index, subIndex)
                                      : QString("%1:%2").arg(index, subIndex));
    if (!type.isEmpty()) {
      parts << type;
    }
    if (!lastValue.isEmpty()) {
      parts << uiText("Last Value: %1", "最后值：%1").arg(lastValue);
    }
    if (!lastStatus.isEmpty()) {
      parts << uiText("Last Status: %1", "最后状态：%1").arg(lastStatus);
    }
    if (evidence.size() > 2 && !evidence.at(2).trimmed().isEmpty()) {
      parts << uiText("Detail: %1", "详情：%1").arg(evidence.at(2).trimmed());
    }
    if (evidence.size() > 3 && !evidence.at(3).trimmed().isEmpty()) {
      parts << uiText("Time: %1", "时间：%1").arg(evidence.at(3).trimmed());
    }
    QApplication::clipboard()->setText(parts.join(" | ")); // copy to system clipboard
    log(QString("Copied object dictionary evidence %1:%2 to clipboard")
            .arg(index, subIndex));
  } else if (chosen == fillSdoFromPdo) {
    applySdoSelectionFromPdoMap(table->currentRow(), false);
  } else if (chosen == readSdoFromPdo) {
    applySdoSelectionFromPdoMap(table->currentRow(), true);
  } else if (chosen == addWatchFromPdo) {
    applySdoSelectionFromPdoMap(table->currentRow(), false);
    addCurrentSdoToWatch();
  } else if (chosen == addSelectedPdoWatch) {
    addSelectedPdoEntriesToWatch();
  } else if (chosen == fillSdoFromWatch) {
    applySdoSelectionFromWatch(table->currentRow(), false);
  } else if (chosen == readSdoFromWatch) {
    applySdoSelectionFromWatch(table->currentRow(), true);
  } else if (chosen == readWatch) {
    const int row = table->currentRow();
    const int position =
        table->item(row, 1) ? table->item(row, 1)->text().toInt() : -1;
    const QString index =
        table->item(row, 2) ? table->item(row, 2)->text().trimmed() : QString();
    const QString subIndex =
        table->item(row, 3) ? table->item(row, 3)->text().trimmed() : QString();
    if (position >= 0 && !index.isEmpty() && !subIndex.isEmpty()) {
      const QString type = table->item(row, 6)
                               ? table->item(row, 6)->text().trimmed()
                               : QString();
      requestSdoRead(position, index, subIndex,
                     uiText("Watch context menu", "监视右键菜单"), type);
    }
  } else if (chosen == captureWatchBaselineAction) {
    captureWatchBaseline();
  } else if (chosen == clearWatchBaselineAction) {
    clearWatchBaseline();
  } else if (chosen == addStartupFromWatch) {
    addStartupSdoFromWatchRow(table->currentRow());
  } else if (chosen == addStartupFromSelectedWatch) {
    addStartupSdoFromSelectedWatchRows();
  } else if (chosen == syncStartupFromWatch) {
    syncWatchRowsToStartupSdo({table->currentRow()});
  } else if (chosen == syncStartupFromSelectedWatch) {
    syncSelectedWatchRowsToStartupSdo();
  } else if (chosen == removeWatch) {
    const int row = table->currentRow();
    const QString key =
        QString("%1|%2|%3")
            .arg(table->item(row, 1) ? table->item(row, 1)->text().toInt() : -1)
            .arg(table->item(row, 2) ? table->item(row, 2)->text().trimmed()
                                     : QString(),
                 table->item(row, 3) ? table->item(row, 3)->text().trimmed()
                                     : QString());
    watchValues_.remove(key);
    watchChangedKeys_.remove(key);
    table->removeRow(table->currentRow());
    updateWatchAutoRefresh();
  } else if (chosen == fillSdoFromFreeRun) {
    applySdoSelectionFromFreeRunEntry(table->currentRow(), false);
  } else if (chosen == readSdoFromFreeRun) {
    applySdoSelectionFromFreeRunEntry(table->currentRow(), true);
  } else if (chosen == addWatchFromFreeRun) {
    applySdoSelectionFromFreeRunEntry(table->currentRow(), false);
    addCurrentSdoToWatch();
  } else if (chosen == fillSdoFromIoVariable) {
    applySdoSelectionFromIoVariable(table->currentRow(), false);
  } else if (chosen == readSdoFromIoVariable) {
    applySdoSelectionFromIoVariable(table->currentRow(), true);
  } else if (chosen == addWatchFromIoVariable) {
    addSelectedIoVariablesToWatch();
  } else if (chosen == addVisibleIoVariableWatch) {
    addVisibleIoVariablesToWatch();
  } else if (chosen == addStartupFromIoVariable) {
    addSelectedIoVariablesToStartupSdo();
  } else if (chosen == addVisibleIoVariableStartup) {
    addVisibleIoVariablesToStartupSdo();
  } else if (chosen == editIoVariableMetadataAction) {
    editSelectedIoVariableMetadata();
  } else if (chosen == bulkNameIoVariableAction) {
    bulkNameIoVariables();
  } else if (chosen == reviewPlcHandoffAction) {
    reviewPlcHandoffIssues();
  } else if (chosen == copySelectedPlcDeclarationsAction) {
    copyIoVariablePlcDeclarations(true);
  } else if (chosen == copyVisiblePlcDeclarationsAction) {
    copyIoVariablePlcDeclarations(false);
  } else if (chosen == exportPlcDeclarationsAction) {
    exportIoVariablesPlcDeclarationsSt();
  } else if (chosen == clearIoVariableMetadataAction) {
    clearSelectedIoVariableMetadata();
  } else if (chosen == exportIoVariableCsvAction) {
    exportIoVariablesCsv();
  } else if (chosen == exportIoVariablePlcCsvAction) {
    exportIoVariablesPlcCsv();
  } else if (chosen == fillSdoFromHistory) {
    applySdoSelectionFromHistory(table->currentRow(), false);
  } else if (chosen == readSdoFromHistory) {
    applySdoSelectionFromHistory(table->currentRow(), true);
  } else if (chosen == addWatchFromHistory) {
    applySdoSelectionFromHistory(table->currentRow(), false);
    addCurrentSdoToWatch();
  } else if (chosen == addSelectedHistoryWatch) {
    addSelectedHistoryRowsToWatch();
  } else if (chosen == addStartupFromHistory) {
    applySdoSelectionFromHistory(table->currentRow(), false);
    addStartupSdo();
  } else if (chosen == addStartupFromSelectedHistory) {
    addSelectedHistoryRowsToStartupSdo();
  } else if (chosen == fillSdoFromStartup) {
    applySdoSelectionFromStartup(table->currentRow(), false);
  } else if (chosen == readSdoFromStartup) {
    applySdoSelectionFromStartup(table->currentRow(), true);
  } else if (chosen == addWatchFromStartup) {
    addStartupSdoRowToWatch(table->currentRow());
  } else if (chosen == verifyStartupRow) {
    verifyStartupSdoRow(table->currentRow());
  } else if (chosen == applyStartupRow) {
    applyStartupSdoRow(table->currentRow());
  } else if (chosen == verifySelectedStartupRows) {
    verifySelectedStartupSdoRows();
  } else if (chosen == focusStartupWatchDiffRows) {
    focusStartupSdoWatchDiffs();
  } else if (chosen == toggleStartupWatchDiffRows) {
    if (watch_->startupWatchDiffsOnly) {
      watch_->startupWatchDiffsOnly->setChecked(!watch_->startupWatchDiffsOnly->isChecked());
    }
    filterStartupSdoTable();
    updateStartupSdoControls();
  } else if (chosen == applyStartupWatchDiffRows) {
    applyStartupSdoWatchDiffRows();
  } else if (chosen == applySelectedStartupRows) {
    applySelectedStartupSdoRows();
  } else if (chosen == moveStartupRowUp) {
    moveStartupSdoRow(-1);
  } else if (chosen == moveStartupRowDown) {
    moveStartupSdoRow(1);
  } else if (chosen == removeStartupRow) {
    removeStartupSdo();
  } else if (chosen == fillSdoFromBookmark) {
    applySdoSelectionFromBookmark(table->currentRow(), false);
  } else if (chosen == readSdoFromBookmark) {
    applySdoSelectionFromBookmark(table->currentRow(), true);
  } else if (chosen == addWatchFromBookmark) {
    addSelectedObjectBookmarksToWatch();
  } else if (chosen == addStartupFromBookmark) {
    addSelectedObjectBookmarksToStartupSdo();
  } else if (chosen == removeBookmark) {
    removeSelectedObjectBookmarks();
  } else if (chosen == openConsistencyEvidence) {
    focusEvidenceFromConsistency(table->currentRow());
  } else if (chosen == copyObjectAddress && objectClipboard.hasAddress()) {
    QApplication::clipboard()->setText(objectClipboard.addressText()); // copy to system clipboard
    log(QString("Copied object address %1 to clipboard")
            .arg(objectClipboard.addressText()));
  } else if (chosen == copyObjectValue && objectClipboard.hasAddress()) {
    QApplication::clipboard()->setText(objectClipboard.valueText()); // copy to system clipboard
    log(QString("Copied object value %1 to clipboard")
            .arg(objectClipboard.valueText()));
  } else if (chosen == copySelected) {
    copyTableToClipboard(table, true);
  } else if (chosen == copyAll) {
    copyTableToClipboard(table, false);
  } else if (chosen == resizeColumns) {
    table->resizeColumnsToContents(); // auto-fit column widths
    table->horizontalHeader()->setStretchLastSection(true);
  } else if (chosen == clearSelection) {
    table->clearSelection();
  }
}


// — Build and display the right-click menu for the SDO target panel
void MainWindow::showSdoTargetPanelContextMenu(const QPoint &position) {
  if (!sdoInspector_->sdoTargetTable) {
    return;
  }

  const int rowAtCursor = sdoInspector_->sdoTargetTable->indexAt(position).row();
  if (rowAtCursor >= 0) {
    sdoInspector_->sdoTargetTable->setCurrentCell(rowAtCursor, 0,
                                    QItemSelectionModel::NoUpdate);
    sdoInspector_->sdoTargetTable->selectRow(rowAtCursor);
  }
  const int row = sdoInspector_->sdoTargetTable->currentRow();
  const bool hasRow = row >= 0 && row < sdoInspector_->sdoTargetTable->rowCount();

  QMenu menu(this);
  auto *openRow = menu.addAction(uiText("Open Row Evidence", "打开本行证据"));
  openRow->setIcon(style()->standardIcon(QStyle::SP_FileDialogDetailedView));
  openRow->setEnabled(hasRow);
  auto *copyRow = menu.addAction(uiText("Copy Row Evidence", "复制本行证据"));
  copyRow->setIcon(style()->standardIcon(QStyle::SP_FileDialogContentsView));
  copyRow->setEnabled(hasRow);
  menu.addSeparator();
  auto *reviewDelta = menu.addAction(uiText("Review Delta", "审阅差异"));
  reviewDelta->setIcon(style()->standardIcon(QStyle::SP_MessageBoxWarning));
  reviewDelta->setEnabled(currentSdoWriteDeltaReviewAvailable());
  auto *copyDigest =
      menu.addAction(uiText("Copy Full Evidence Digest", "复制完整证据摘要"));
  copyDigest->setIcon(style()->standardIcon(QStyle::SP_FileDialogContentsView));
  copyDigest->setEnabled(selectedPosition() >= 0 && sdoInspector_->sdoIndex && sdoInspector_->sdoSubIndex &&
                         !sdoInspector_->sdoIndex->text().trimmed().isEmpty() &&
                         !sdoInspector_->sdoSubIndex->text().trimmed().isEmpty());
  menu.addSeparator();
  auto *resizeColumns =
      menu.addAction(uiText("Autosize Columns", "自动调整列宽"));

  const QAction *chosen =
      menu.exec(sdoInspector_->sdoTargetTable->viewport()->mapToGlobal(position));
  if (!chosen) {
    return;
  }
  if (chosen == openRow) {
    openSdoTargetPanelRow(row);
  } else if (chosen == copyRow) {
    copySdoTargetPanelRowDigest(row);
  } else if (chosen == reviewDelta) {
    reviewCurrentSdoWriteDelta();
  } else if (chosen == copyDigest) {
    copyCurrentSdoEvidenceDigest();
  } else if (chosen == resizeColumns) {
    sdoInspector_->sdoTargetTable->resizeColumnsToContents(); // auto-fit column widths
    sdoInspector_->sdoTargetTable->horizontalHeader()->setStretchLastSection(false);
    sdoInspector_->sdoTargetTable->horizontalHeader()->setSectionResizeMode(
        0, QHeaderView::ResizeToContents);
    sdoInspector_->sdoTargetTable->horizontalHeader()->setSectionResizeMode(
        1, QHeaderView::Stretch);
    sdoInspector_->sdoTargetTable->horizontalHeader()->setSectionResizeMode(
        2, QHeaderView::ResizeToContents);
  }
}


// — Dispatch Alt+Enter to open local evidence for the focused table row
bool MainWindow::runLocalEvidenceAction(QTableWidget *table) {
  if (!table) {
    return false;
  }

  const int row = table->currentRow();
  if (row < 0 || row >= table->rowCount()) {
    statusBar()->showMessage(uiText("Select a row before using Alt+Enter.",
                                    "使用 Alt+Enter 前请先选择一行。"),
                             3000);
    return true;
  }

  // Dispatch Alt+Enter to the correct evidence action for this table type
  if (table == workflow_->workflowTable) {
    copyWorkflowStepDigest(row);
  // Dispatch Alt+Enter to the correct evidence action for this table type
  } else if (table == session_->sessionBriefTable) {
    openSessionBriefRow(row);
  } else if (table == slaveEvidence_->slaveEvidenceMatrixTable) {
    openSlaveEvidenceMatrixRow(row);
  } else if (table == consistency_->consistencyTable) {
    focusEvidenceFromConsistency(row);
  } else if (table == sdoInspector_->sdoTargetTable) {
    openSdoTargetPanelRow(row);
  } else if (table == sdo_->sdoTable) {
    applySdoSelectionFromDictionary(row, false);
  } else if (table == sdo_->pdoTable) {
    applySdoSelectionFromPdoMap(row, false);
  } else if (table == watch_->watchTable) {
    applySdoSelectionFromWatch(row, false);
  } else if (table == freeRunWidgets_->freeRunEntryTable) {
    applySdoSelectionFromFreeRunEntry(row, false);
  } else if (table == ioVar_->ioVariableTable) {
    applySdoSelectionFromIoVariable(row, false);
  } else if (table == sdoHistoryTable_) {
    applySdoSelectionFromHistory(row, false);
  } else if (table == startupSdoTable_) {
    applySdoSelectionFromStartup(row, false);
  } else if (table == bookmark_->objectBookmarkTable) {
    applySdoSelectionFromBookmark(row, false);
  } else if (table == sdoTargetTrailTable_) {
    restoreSdoTargetTrailRow(row);
  } else {
    statusBar()->showMessage(
        uiText("Alt+Enter is reserved for local evidence actions on object "
               "and evidence tables.",
               "Alt+Enter 仅用于对象和证据表的本地证据动作。"),
        3000);
    return true;
  }

  updateDiagnostics(
      "Info", "Navigator",
      uiText("Alt+Enter opened local evidence or filled the SDO target without "
             "bus access.",
             "Alt+Enter 已打开本地证据或回填 SDO 目标，未访问总线。"));
  statusBar()->showMessage(
      uiText("Local evidence action complete. No bus access was requested.",
             "本地证据动作完成；未请求总线访问。"),
      3000);
  return true;
}


// — Copy table rows as TSV text to the system clipboard
void MainWindow::copyTableToClipboard(QTableWidget *table, bool selectedOnly) {
  if (!table || table->columnCount() <= 0) {
    return;
  }

  QVector<int> rows;
  if (selectedOnly) {
    for (const auto &range : table->selectedRanges()) {
      for (int row = range.topRow(); row <= range.bottomRow(); ++row) {
        rows.append(row);
      }
    }
    std::sort(rows.begin(), rows.end());
    rows.erase(std::unique(rows.begin(), rows.end()), rows.end());
  } else {
    for (int row = 0; row < table->rowCount(); ++row) {
      rows.append(row);
    }
  }
  if (rows.isEmpty()) {
    return;
  }

  // Sanitize cell text for clipboard
  auto cleanCell = [](QString value) {
    value.replace('\n', ' ');
    value.replace('\r', ' ');
    value.replace('\t', ' ');
    return value.trimmed();
  };

  QStringList lines;
  QStringList headers;
  for (int column = 0; column < table->columnCount(); ++column) {
    headers << cleanCell(table->horizontalHeaderItem(column)
                             ? table->horizontalHeaderItem(column)->text()
                             : QString());
  }
  lines << headers.join('\t');

  for (const int row : rows) {
    QStringList values;
    for (int column = 0; column < table->columnCount(); ++column) {
      values << cleanCell(table->item(row, column)
                              ? table->item(row, column)->text()
                              : QString());
    }
    lines << values.join('\t');
  }

  QApplication::clipboard()->setText(lines.join('\n')); // copy to system clipboard
  log(QString("Copied %1 table row(s) to clipboard").arg(rows.size()));
}

