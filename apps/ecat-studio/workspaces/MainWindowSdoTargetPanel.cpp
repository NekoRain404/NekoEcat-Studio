// SDO Target Panel: inspector, evidence digest, and cross-workspace navigation.
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


// — Build a human-readable impact summary for a proposed SDO write
bool MainWindow::openSdoTargetPanelRow(int row) {
// Open the SDO target panel for a specific dictionary row and populate all fields.
  if (!sdoInspector_->sdoTargetTable || row < 0 || row >= sdoInspector_->sdoTargetTable->rowCount()) {
    return false;
  }

  const QString key = tableText(sdoInspector_->sdoTargetTable, row, 0);
  const SdoTargetPanelRouteDecision decision =
      sdoTargetPanelRouteDecision(key, currentSdoWriteDeltaReviewAvailable());

  switch (decision.routeKind) {
  case SdoTargetPanelRouteKind::Watch:
    openCurrentSdoWatchLink();
    break;
  case SdoTargetPanelRouteKind::Startup:
    openCurrentSdoStartupLink();
    break;
  case SdoTargetPanelRouteKind::Bookmark:
    openCurrentSdoBookmarkLink();
    break;
  case SdoTargetPanelRouteKind::TargetTrail:
    openCurrentSdoTargetTrailLink();
    break;
  case SdoTargetPanelRouteKind::EvidenceReview:
    reviewCurrentSdoWriteDelta();
    break;
  case SdoTargetPanelRouteKind::EvidenceDigest:
    copyCurrentSdoEvidenceDigest();
    break;
  case SdoTargetPanelRouteKind::ObjectDictionary: {
    activateObjectDictionaryPaneFor(sdo_->sdoTable);
    const int dictionaryRow = currentSdoDictionaryRow();
    if (dictionaryRow >= 0 && sdo_->sdoTable) {
      if (sdo_->sdoFilter && sdo_->sdoTable->isRowHidden(dictionaryRow)) {
        sdo_->sdoFilter->clear();
        filterSdoTable(QString());
      }
      sdo_->sdoTable->clearSelection();
      selectAndFocusTableRow(sdo_->sdoTable, dictionaryRow, 1);
    } else if (sdoInspector_->sdoIndex) {
      sdoInspector_->sdoIndex->setFocus();
      sdoInspector_->sdoIndex->selectAll();
    }
    updateDiagnostics("Info", "Navigation",
                      uiText("Opened Object Dictionary context for selected "
                             "SDO target",
                             "已打开当前 SDO 目标的对象字典上下文"));
    break;
  }
  case SdoTargetPanelRouteKind::CopyDigest:
    statusBar()->showMessage(
        uiText("This Selected Object row has no local evidence target; copied "
               "the digest instead.",
               "该选中对象行没有本地证据目标，已改为复制证据摘要。"),
        3000);
    copyCurrentSdoEvidenceDigest();
    break;
  }
  return true;
}


// — Copy a summary of the SDO target panel row to clipboard
bool MainWindow::copySdoTargetPanelRowDigest(int row) {
  if (!sdoInspector_->sdoTargetTable || row < 0 || row >= sdoInspector_->sdoTargetTable->rowCount()) {
    statusBar()->showMessage(uiText("Select a Selected Object row to copy.",
                                    "请选择一行选中对象复核内容再复制。"),
                             3000);
    return false;
  }

  const QString key = tableText(sdoInspector_->sdoTargetTable, row, 0);
  const QString value = tableText(sdoInspector_->sdoTargetTable, row, 1);
  const SdoTargetPanelRouteDecision decision =
      sdoTargetPanelRouteDecision(key, currentSdoWriteDeltaReviewAvailable());

  QString action;
// Copy the SDO target panel row digest (index, subindex, value, type) to clipboard.
  switch (decision.copyActionKind) {
  case SdoTargetPanelCopyActionKind::OpenWatch:
    action = uiText("Open the matching Watch evidence row locally",
                    "本地打开匹配的 Watch 证据行");
    break;
  case SdoTargetPanelCopyActionKind::OpenStartup:
    action = uiText("Open the matching Startup SDO evidence row locally",
                    "本地打开匹配的 Startup SDO 证据行");
    break;
  case SdoTargetPanelCopyActionKind::OpenBookmark:
    action = uiText("Open the matching Object Bookmark locally",
                    "本地打开匹配的对象书签");
    break;
  case SdoTargetPanelCopyActionKind::OpenTargetTrail:
    action = uiText("Open the matching SDO Target Trail row locally",
                    "本地打开匹配的 SDO 目标轨迹行");
    break;
  case SdoTargetPanelCopyActionKind::ReviewEvidence:
    action = uiText("Review the local evidence behind this row",
                    "审阅该行背后的本地证据");
    break;
  case SdoTargetPanelCopyActionKind::CopyDigestNoDelta:
    action = uiText("Copy the full evidence digest; no delta evidence "
                    "is currently available",
                    "复制完整证据摘要；当前没有可审阅的差异证据");
    break;
  case SdoTargetPanelCopyActionKind::FocusObjectDictionary:
    action = uiText("Focus the Object Dictionary context locally",
                    "本地聚焦对象字典上下文");
    break;
  case SdoTargetPanelCopyActionKind::FullDigest:
    action = uiText("Copy the full evidence digest for broader context",
                    "复制完整证据摘要以获得更完整上下文");
    break;
  }

  const int position = selectedPosition();
  const QString index = sdoInspector_->sdoIndex ? sdoInspector_->sdoIndex->text().trimmed() : QString();
  const QString subIndex =
      sdoInspector_->sdoSubIndex ? sdoInspector_->sdoSubIndex->text().trimmed() : QString();
  const QString target =
      position >= 0 && !index.isEmpty() && !subIndex.isEmpty()
          ? QString("#%1 %2:%3").arg(position).arg(index, subIndex)
          : uiText("No complete SDO target", "尚无完整 SDO 目标");

  QStringList lines;
  lines << uiText("NekoEcat Studio Selected Object Row Evidence",
                  "NekoEcat Studio 选中对象本行证据");
  lines << QString("%1: %2").arg(uiText("Master", "主站"), activeMasterName());
  lines << QString("%1: %2").arg(uiText("Target", "目标"), target);
  lines << QString("%1: %2").arg(uiText("Row", "行"), key);
  lines << QString("%1: %2").arg(
      uiText("Value", "值"), value.isEmpty() ? uiText("Empty", "空") : value);
  lines << QString("%1: %2").arg(uiText("Local Action", "本地动作"), action);
  lines << uiText("Boundary: clipboard copy only; no bus read, no SDO write, "
                  "no state change, no Free Run change, and no Host Health.",
                  "边界：只复制到剪贴板；不读取总线、不写 SDO、不切换状态、"
                  "不改变 Free Run，也不运行 Host Health。");

  QApplication::clipboard()->setText(lines.join('\n')); // copy to system clipboard
  updateDiagnostics("Info", "SDO",
                    uiText("Copied Selected Object row evidence to clipboard",
                           "已复制选中对象本行证据到剪贴板"));
  return true;
}


// — Copy a summary of the current SDO evidence to clipboard
void MainWindow::copyCurrentSdoEvidenceDigest() {
  const int position = selectedPosition();
  const QString index = sdoInspector_->sdoIndex ? sdoInspector_->sdoIndex->text().trimmed() : QString();
  const QString subIndex =
      sdoInspector_->sdoSubIndex ? sdoInspector_->sdoSubIndex->text().trimmed() : QString();
  if (position < 0 || index.isEmpty() || subIndex.isEmpty()) {
    updateDiagnostics(
        "Info", "SDO",
        uiText("No complete SDO target to copy", "没有可复制的完整 SDO 目标"));
    return;
  }

  QStringList lines;
  lines << uiText("NekoEcat Studio SDO Evidence Digest",
                  "NekoEcat Studio SDO 证据摘要");
  lines << QString("%1: %2").arg(uiText("Master", "主站"), activeMasterName());
  lines << QString("%1: #%2 %3:%4")
               .arg(uiText("Target", "目标"))
               .arg(position)
               .arg(index, subIndex);

  const QString type = sdoInspector_->sdoType ? sdoInspector_->sdoType->currentText().trimmed() : QString();
  const QString readValue = sdoInspector_->sdoValue ? sdoInspector_->sdoValue->text().trimmed() : QString();
  const QString writeValue =
      sdoInspector_->sdoWriteValue ? sdoInspector_->sdoWriteValue->text().trimmed() : QString();
  if (!type.isEmpty()) {
    lines << QString("%1: %2").arg(uiText("Type", "类型"), type);
  }
  if (!readValue.isEmpty()) {
    lines << QString("%1: %2").arg(uiText("Read Value", "读回值"), readValue);
  }
  if (!writeValue.isEmpty()) {
// Copy a comprehensive evidence digest for the current SDO target to clipboard.
    lines << QString("%1: %2").arg(uiText("Write Value", "写入值"), writeValue);
  }

  if (sdoInspector_->sdoTargetTable && sdoInspector_->sdoTargetTable->rowCount() > 0) {
    lines << QString();
    lines << uiText("Selected Object Review", "选中对象复核");
    for (int row = 0; row < sdoInspector_->sdoTargetTable->rowCount(); ++row) {
      const QString key = tableText(sdoInspector_->sdoTargetTable, row, 0);
      const QString value = tableText(sdoInspector_->sdoTargetTable, row, 1);
      if (!key.isEmpty() && !value.isEmpty()) {
        lines << QString("- %1: %2").arg(key, value);
      }
    }
  }

  const auto candidates = currentSdoEvidenceCandidates();
  lines << QString();
  lines << uiText("Local Evidence Candidates", "本地证据候选");
  if (candidates.isEmpty()) {
    lines << QString("- %1").arg(uiText("None", "无"));
  } else {
    for (const auto &candidate : candidates) {
      lines << QString("- %1 = %2").arg(candidate.first, candidate.second);
    }
  }

  lines << QString();
  lines << uiText("Local Evidence Links", "本地证据链接");
  const int watchRow = currentSdoWatchRow();
  const int startupRow = currentSdoStartupRow();
  const int bookmarkRow = currentSdoBookmarkRow();
  const int trailRow = currentSdoTargetTrailRow();
  lines << QString("- Watch: %1")
               .arg(watchRow >= 0
                        ? uiText("row %1", "第 %1 行").arg(watchRow + 1)
                        : uiText("none", "无"));
  lines << QString("- Startup SDO: %1")
               .arg(startupRow >= 0
                        ? uiText("row %1", "第 %1 行").arg(startupRow + 1)
                        : uiText("none", "无"));
  lines << QString("- Object Bookmark: %1")
               .arg(bookmarkRow >= 0
                        ? uiText("row %1", "第 %1 行").arg(bookmarkRow + 1)
                        : uiText("none", "无"));
  lines << QString("- SDO Target Trail: %1")
               .arg(trailRow >= 0
                        ? uiText("row %1", "第 %1 行").arg(trailRow + 1)
                        : uiText("none", "无"));

  lines << QString();
  lines << uiText("Boundary: clipboard copy only; no bus read, no SDO write, "
                  "no state change, no Free Run change, and no Host Health.",
                  "边界：只复制到剪贴板；不读取总线、不写 SDO、不切换状态、"
                  "不改变 Free Run，也不运行 Host Health。");

  QApplication::clipboard()->setText(lines.join('\n')); // copy to system clipboard
  updateDiagnostics("Info", "SDO",
                    uiText("Copied current SDO evidence digest to clipboard",
                           "已复制当前 SDO 证据摘要到剪贴板"));
}


// — Open current sdo watch link
void MainWindow::openCurrentSdoWatchLink() {
  const int row = currentSdoWatchRow();
  if (row < 0 || !watch_->watchTable) {
    updateDiagnostics("Info", "Navigation",
                      uiText("No matching Watch row for current SDO target",
                             "当前 SDO 目标没有匹配的 Watch 行"));
    return;
  }
  activateWorkspaceTab(watchTabIndex_);
  selectAndFocusTableRow(watch_->watchTable, row, 1);
  updateDiagnostics("Info", "Navigation",
                    uiText("Opened matching Watch row for current SDO target",
                           "已打开当前 SDO 目标匹配的 Watch 行"));
}


// — Open current sdo startup link
void MainWindow::openCurrentSdoStartupLink() {
  const int row = currentSdoStartupRow();
  if (row < 0 || !startupSdoTable_) {
    updateDiagnostics("Info", "Navigation",
                      uiText("No matching Startup SDO row for current target",
                             "当前目标没有匹配的 Startup SDO 行"));
    return;
  }
  activateWorkspaceTab(startupSdoTabIndex_);
  if (watch_->startupWatchDiffsOnly) {
    watch_->startupWatchDiffsOnly->setChecked(false);
  }
  filterStartupSdoTable();
  selectAndFocusTableRow(startupSdoTable_, row, 0);
  updateStartupSdoControls();
  updateDiagnostics("Info", "Navigation",
                    uiText("Opened matching Startup SDO row for current target",
                           "已打开当前目标匹配的 Startup SDO 行"));
}

// Navigate from SDO target to the corresponding Watch row.

// — Open current sdo bookmark link
void MainWindow::openCurrentSdoBookmarkLink() {
  const int row = currentSdoBookmarkRow();
  if (row < 0 || !bookmark_->objectBookmarkTable) {
    updateDiagnostics("Info", "Navigation",
                      uiText("No matching Object Bookmark for current target",
                             "当前目标没有匹配的对象书签"));
    return;
  }
  activateObjectDictionaryPaneFor(bookmark_->objectBookmarkTable);
  selectAndFocusTableRow(bookmark_->objectBookmarkTable, row, 0);
  updateDiagnostics("Info", "Navigation",
                    uiText("Opened matching Object Bookmark for current target",
                           "已打开当前目标匹配的对象书签"));
}


// — Open current sdo target trail link
void MainWindow::openCurrentSdoTargetTrailLink() {
// Navigate from SDO target to the corresponding Startup SDO row.
  const int row = currentSdoTargetTrailRow();
  if (row < 0 || !sdoTargetTrailTable_) {
    updateDiagnostics(
        "Info", "Navigation",
        uiText("No matching SDO Target Trail row for current target",
               "当前目标没有匹配的 SDO 目标轨迹行"));
    return;
  }
  activateObjectDictionaryPaneFor(sdoTargetTrailTable_);
  selectAndFocusTableRow(sdoTargetTrailTable_, row, 1);
  updateDiagnostics(
      "Info", "Navigation",
      uiText("Opened matching SDO Target Trail row for current target",
             "已打开当前目标匹配的 SDO 目标轨迹行"));
}


// — Return the sdo object category
QString MainWindow::sdoObjectCategory(const QString &index, const QString &name,
                                      const QString &object,
// Navigate from SDO target to the corresponding Object Bookmark row.
                                      const QString &detail) const {
  const QString key =
      QString("%1 %2 %3 %4").arg(index, name, object, detail).toLower();
  if (key.contains("0x6040") || key.contains("0x6041") ||
      key.contains("0x6060") || key.contains("0x6061") ||
      key.contains("0x603f") || key.contains("controlword") ||
      key.contains("statusword") || key.contains("mode") ||
      key.contains("fault")) {
    return uiText("CiA 402 drive", "CiA 402 驱动");
  }
  if (key.contains("0x1000") || key.contains("0x1008") ||
      key.contains("0x1009") || key.contains("0x100a") ||
      key.contains("0x1018") || key.contains("identity") ||
      key.contains("vendor") || key.contains("product") ||
      key.contains("revision") || key.contains("serial")) {
    return uiText("Identity", "身份");
  }
  if (key.contains("0x160") || key.contains("0x1a0") || key.contains("0x1c1") ||
      key.contains("0x1c2") || key.contains("pdo") || key.contains("mapping") ||
      key.contains("assignment")) {
// Navigate from SDO target to the corresponding Target Trail row.
    return uiText("PDO mapping", "PDO 映射");
  }
  if (key.contains("0x1001") || key.contains("0x1002") ||
      key.contains("0x1003") || key.contains("error") ||
      key.contains("emergency") || key.contains("diagnostic")) {
    return uiText("Error/diagnostic", "错误/诊断");
  }
  return uiText("Application object", "应用对象");
}


// — Refresh the SDO target panel table from the current SDO history
void MainWindow::updateSdoTargetPanel(const QString &source,
                                      const QString &detail,
                                      const QString &status,
                                      const QStringList &problems) {
  if (!sdoInspector_->sdoTargetTable) {
    return;
  }

// Categorize an SDO object by index range and name (CoE standard, CiA 402, vendor, etc.).
  const int position = selectedPosition();
  const QString index = sdoInspector_->sdoIndex ? sdoInspector_->sdoIndex->text().trimmed() : QString();
  const QString subIndex =
      sdoInspector_->sdoSubIndex ? sdoInspector_->sdoSubIndex->text().trimmed() : QString();
  const QString type = sdoInspector_->sdoType ? sdoInspector_->sdoType->currentText().trimmed() : QString();
  const QString readValue = sdoInspector_->sdoValue ? sdoInspector_->sdoValue->text().trimmed() : QString();
  const QString writeValue =
      sdoInspector_->sdoWriteValue ? sdoInspector_->sdoWriteValue->text().trimmed() : QString();
  // Normalize hex address for consistent comparison
  const QString normalizedIndex = normalizeHexText(index, 4);
  // Normalize hex address for consistent comparison
  const QString normalizedSubIndex = normalizeHexText(subIndex, 2);
  const bool hasCompleteTarget = position >= 0 && !normalizedIndex.isEmpty() &&
                                 !normalizedSubIndex.isEmpty();

  int dictionaryRow = -1;
  SdoDictionaryRow dictionary;
  if (hasCompleteTarget && sdo_->sdoTable && loadedSdoPosition_ == position) {
    dictionaryRow = tableRowForObjectIndex(sdo_->sdoTable, normalizedIndex,
                                           normalizedSubIndex, 1, 2);
    dictionary = sdoDictionaryRowFromTable(sdo_->sdoTable, dictionaryRow);
  }

  const QString object = dictionary.object;
  const QString access = dictionary.access;
  const QString bits = dictionary.bits;
  const QString name = dictionary.name;
  const QString lastValue = dictionary.value;
  const QString lastStatus = dictionary.status;
  const QString address =
      position >= 0 && !index.isEmpty() && !subIndex.isEmpty()
          ? QString("#%1 %2:%3").arg(position).arg(index, subIndex)
          : uiText("No complete SDO target", "尚无完整 SDO 目标");
  const QString category = sdoObjectCategory(index, name, object, detail);
  const QString writable = selectedSdoWritable_ ? uiText("Writable", "可写")
                                                : uiText("Read only", "只读");

  int watchRow = -1;
  QString watchValue;
  QString watchDecoded;
// Rebuild the SDO target panel from the given source, position, index, subindex, and value.
  if (hasCompleteTarget && watch_->watchTable) {
    watchRow = currentSdoWatchRow();
    if (watchRow >= 0) {
      watchValue = tableText(watch_->watchTable, watchRow, 4);
      watchDecoded = tableText(watch_->watchTable, watchRow, 5);
    }
  }

  int startupRows = 0;
  QString startupValue;
  QString startupDelta;
  if (hasCompleteTarget && startupSdoTable_) {
    ensureStartupSdoTable();
    const int primaryStartupRow = currentSdoStartupRow();
    for (int row = 0; row < startupSdoTable_->rowCount(); ++row) {
      if (!tableObjectAddressMatches(startupSdoTable_, row, position,
                                     normalizedIndex, normalizedSubIndex, 0, 1,
                                     2)) {
        continue;
      }
      ++startupRows;
      if (startupValue.isEmpty() || row == primaryStartupRow) {
        startupValue = tableText(startupSdoTable_, row, 3);
      }
      const QString rowDelta = tableText(startupSdoTable_, row, 8);
      if (startupDelta.isEmpty() ||
          rowDelta.compare(uiText("diff", "不一致"), Qt::CaseInsensitive) ==
              0 ||
          rowDelta.contains(uiText("diff", "不一致"), Qt::CaseInsensitive)) {
        startupDelta = rowDelta;
      }
    }
  }

  int bookmarkRows = 0;
  QString bookmarkValue;
  if (hasCompleteTarget && bookmark_->objectBookmarkTable) {
    ensureObjectBookmarkTable();
    const int primaryBookmarkRow = currentSdoBookmarkRow();
    for (int row = 0; row < bookmark_->objectBookmarkTable->rowCount(); ++row) {
      if (!tableObjectAddressMatches(bookmark_->objectBookmarkTable, row, position,
                                     normalizedIndex, normalizedSubIndex, 0, 2,
                                     3)) {
        continue;
      }
      ++bookmarkRows;
      const SdoObjectBookmarkRow bookmark =
          sdoObjectBookmarkRowFromTable(bookmark_->objectBookmarkTable, row);
      if (bookmarkValue.isEmpty() || row == primaryBookmarkRow) {
        bookmarkValue = bookmark.lastValue;
      }
    }
  }

  int targetTrailRow = -1;
  QString targetTrailValue;
  QString targetTrailWriteValue;
  QString targetTrailSource;
  if (hasCompleteTarget && sdoTargetTrailTable_) {
    targetTrailRow = currentSdoTargetTrailRow();
    if (targetTrailRow >= 0) {
      const SdoTargetTrailRow trail =
          sdoTargetTrailRowFromTable(sdoTargetTrailTable_, targetTrailRow);
      targetTrailValue = trail.value;
      targetTrailWriteValue = trail.writeValue;
      targetTrailSource = trail.source;
    }
  }

  const QVector<SdoEvidenceItem> writeEvidence =
      sdoWriteEvidenceItemsFromValues(readValue, lastValue, watchValue,
                                      startupValue, bookmarkValue,
                                      targetTrailValue, targetTrailWriteValue,
                                      {.read = uiText("Read", "读回"),
                                       .dictionary = uiText("OD", "OD"),
                                       .watch = uiText("Watch", "Watch"),
                                       .startup = uiText("Startup", "Startup"),
                                       .bookmark = uiText("Bookmark", "书签"),
                                       .targetTrail = uiText("Trail", "轨迹")});

  QString evidenceSetState = QStringLiteral("none");
  QString evidenceSetSummary = uiText("No local evidence", "无本地证据");
  if (!writeEvidence.isEmpty()) {
    const auto evidenceGroups = groupSdoEvidence(writeEvidence);

    QStringList facts;
    for (const auto &group : evidenceGroups) {
      facts << QString("%1=%2").arg(group.sources.join("/"), group.value);
    }
    evidenceSetState = evidenceGroups.size() > 1 ? QStringLiteral("conflict")
                                                 : QStringLiteral("match");
    evidenceSetSummary =
        evidenceGroups.size() > 1
            ? uiText("Conflict: %1", "冲突：%1").arg(facts.join("; "))
            : uiText("Consistent: %1", "一致：%1").arg(facts.join("; "));
  }

  QString writeDeltaState = QStringLiteral("idle");
  bool writeDeltaHasDiff = false;
  bool writeDeltaHasConflict = false;
  bool writeDeltaMatchesEvidence = false;
  QString writeDeltaSummary;
  if (!hasCompleteTarget) {
    writeDeltaSummary = uiText("No complete target", "尚无完整目标");
  } else if (writeValue.isEmpty()) {
    writeDeltaSummary = uiText("No write value", "未填写写入值");
  } else if (writeEvidence.isEmpty()) {
    writeDeltaState = QStringLiteral("none");
    writeDeltaSummary =
        uiText("No comparable local evidence", "无可比较的本地证据");
  } else {
    const SdoWriteDeltaReview review =
        reviewSdoWriteDelta(writeEvidence, writeValue);
    writeDeltaHasDiff = review.hasDiff;
    writeDeltaHasConflict = review.hasConflict;
    writeDeltaMatchesEvidence = review.matchesEvidence;
    writeDeltaState = review.state;
    QStringList diffFacts;
    for (const auto &group : review.differingGroups) {
      diffFacts << uiText("%1 %2 -> %3", "%1 %2 -> %3")
                       .arg(group.sources.join("/"), group.value, writeValue);
    }
    if (writeDeltaHasConflict && writeDeltaHasDiff) {
      writeDeltaSummary =
          uiText("Evidence conflict: %1; differs from %2",
                 "证据冲突：%1；不同于 %2")
              .arg(review.conflictFacts.join("; "), diffFacts.join("; "));
    } else if (writeDeltaHasConflict) {
      writeDeltaSummary = uiText("Evidence conflict: %1", "证据冲突：%1")
                              .arg(review.conflictFacts.join("; "));
    } else if (writeDeltaHasDiff) {
      writeDeltaSummary =
          uiText("Differs from %1", "不同于 %1").arg(diffFacts.join("; "));
    } else {
      writeDeltaSummary =
          uiText("Matches %1", "匹配 %1").arg(review.matchingSources.join("/"));
    }
  }

  const bool hasValueEvidence =
      !readValue.isEmpty() || !lastValue.isEmpty() || !watchValue.isEmpty() ||
      !startupValue.isEmpty() || !bookmarkValue.isEmpty() ||
      !targetTrailValue.isEmpty() || !targetTrailWriteValue.isEmpty();
  QStringList nextActions;
  if (!hasCompleteTarget) {
    nextActions << uiText("select a slave and object first",
                          "先选择从站和对象");
  } else {
    if (watchRow < 0) {
      nextActions << uiText("add to Watch for repeat observation",
                            "加入 Watch 便于反复观察");
    } else if (watchValue.isEmpty()) {
      nextActions << uiText("refresh Watch or read target for current value",
                            "刷新 Watch 或读取目标获取当前值");
    }
    if (selectedSdoWritable_) {
      if (writeValue.isEmpty() && hasValueEvidence) {
        nextActions << uiText("use evidence or copy a value into Write Value "
                              "before tuning",
                              "调参前可使用证据或复制数值到写入框");
      } else if (!writeValue.isEmpty()) {
        if (writeDeltaHasConflict) {
          nextActions << uiText(
              "review conflicting local evidence before writing",
              "写入前先审阅冲突的本地证据");
        } else if (writeDeltaHasDiff) {
          nextActions << uiText(
              "review Write Delta before write or Startup changes",
              "写入或修改 Startup 前先审阅写入差异");
        } else if (writeDeltaMatchesEvidence) {
          nextActions << uiText(
              "write value already matches local evidence; skip if unchanged",
              "写入值已匹配本地证据；无变更时可跳过");
        } else {
          nextActions << uiText("write uses validation, confirmation, and "
                                "read-back verification",
                                "写入会走校验、确认和读回校验");
        }
      } else {
        nextActions << uiText("fill Write Value before write or Startup",
                              "填写写入值后再写入或创建 Startup");
      }
    } else {
      nextActions << uiText("read-only target: use Read or Watch, not Write",
                            "只读目标：使用读取或 Watch，不执行写入");
    }
    if (startupRows <= 0 && selectedSdoWritable_ &&
        (!writeValue.isEmpty() || hasValueEvidence)) {
      nextActions << uiText("create Startup when this parameter must persist",
                            "需要持久化时创建 Startup");
    } else if (startupRows > 0 &&
               (startupDelta.contains("diff", Qt::CaseInsensitive) ||
                startupDelta.contains("不一致"))) {
      nextActions << uiText("review Startup diff before applying",
                            "应用前审阅 Startup 偏差");
    }
    if (bookmarkRows <= 0) {
      nextActions << uiText("bookmark if this object is reused",
                            "常用对象可加入书签");
    }
    if (targetTrailRow >= 0 && watchRow < 0) {
      nextActions << uiText("reuse Target Trail to add Watch without a read",
                            "可复用目标轨迹加入 Watch 且不立即读取");
    }
  }
  const QString nextAction =
      nextActions.isEmpty()
          ? uiText("target context is ready", "目标上下文就绪")
          : nextActions.join("; ");

  QStringList safetyNotes;
  safetyNotes << uiText("Panel updates are local context only",
                        "面板更新只整理本地上下文");
  safetyNotes << uiText("Read/Watch/Write/Startup buttons stay explicit",
                        "读取、监视、写入和 Startup 仍需显式点击");
  safetyNotes << uiText("Host Health remains in Diagnostics",
                        "Host Health 仍只在诊断页");

  const QString evidenceSetLabel = uiText("Evidence Set", "证据集");
  const QString writeDeltaLabel = uiText("Write Delta", "写入差异");
  QList<QPair<QString, QString>> rows = {
      {uiText("Target", "目标"), address},
      {uiText("Source", "来源"), source.trimmed().isEmpty()
                                     ? uiText("Manual fields", "手动字段")
                                     : source.trimmed()},
      {uiText("Category", "类别"), category},
      {uiText("Name", "名称"),
       !name.isEmpty() ? name
                       : (!object.isEmpty() ? object : detail.trimmed())},
      {uiText("Access", "权限"),
       access.isEmpty() ? writable : QString("%1 (%2)").arg(access, writable)},
      {uiText("Type / Bits", "类型 / 位宽"),
       bits.isEmpty() ? type : QString("%1 / %2 bit").arg(type, bits)},
      {uiText("Read Value", "读回值"),
       !readValue.isEmpty()
           ? readValue
           : (!lastValue.isEmpty()
                  ? lastValue
                  : uiText("Not read in this context", "当前上下文未读取"))},
      {uiText("OD Evidence", "OD 证据"),
       lastStatus.isEmpty()
           ? uiText("No table evidence", "表格暂无证据")
           : QString("%1%2").arg(lastStatus, lastValue.isEmpty()
                                                 ? QString()
                                                 : " = " + lastValue)},
      {uiText("Watch Link", "Watch 关联"),
       watchRow < 0
           ? uiText("Not in Watch", "未加入 Watch")
           : QString("#%1%2%3")
                 .arg(watchRow + 1)
                 .arg(watchValue.isEmpty() ? QString()
                                           : QString(" = %1").arg(watchValue))
                 .arg(watchDecoded.isEmpty()
                          ? QString()
                          : QString(" (%1)").arg(watchDecoded))},
      {uiText("Startup Link", "Startup 关联"),
       startupRows <= 0 ? uiText("No Startup SDO row", "无 Startup SDO 行")
                        : uiText("%1 row(s)%2%3", "%1 行%2%3")
                              .arg(startupRows)
                              .arg(startupValue.isEmpty()
                                       ? QString()
                                       : QString(" = %1").arg(startupValue))
                              .arg(startupDelta.isEmpty()
                                       ? QString()
                                       : QString(" [%1]").arg(startupDelta))},
      {uiText("Bookmark", "书签"),
       bookmarkRows <= 0 ? uiText("Not bookmarked", "未收藏")
                         : uiText("%1 bookmark(s)%2", "%1 个书签%2")
                               .arg(bookmarkRows)
                               .arg(bookmarkValue.isEmpty()
                                        ? QString()
                                        : QString(" = %1").arg(bookmarkValue))},
      {uiText("Target Trail", "目标轨迹"),
       targetTrailRow < 0
           ? uiText("No matching target trail row", "无匹配目标轨迹行")
           : QString("#%1%2%3%4")
                 .arg(targetTrailRow + 1)
                 .arg(targetTrailSource.isEmpty()
                          ? QString()
                          : QString(" [%1]").arg(targetTrailSource))
                 .arg(targetTrailValue.isEmpty()
                          ? QString()
                          : QString(" value=%1").arg(targetTrailValue))
                 .arg(targetTrailWriteValue.isEmpty()
                          ? QString()
                          : QString(" write=%1").arg(targetTrailWriteValue))},
      {evidenceSetLabel, evidenceSetSummary},
      {uiText("Write Value", "写入值"),
       writeValue.isEmpty() ? uiText("Empty", "空") : writeValue},
      {writeDeltaLabel, writeDeltaSummary},
      {uiText("Next Action", "下一步"), nextAction},
      {uiText("Safety", "安全边界"), safetyNotes.join("; ")},
      {uiText("State", "状态"), status},
  };
  if (!problems.isEmpty()) {
    rows.append({uiText("Check", "检查"), problems.join("; ")});
  }

  const QSignalBlocker blocker(sdoInspector_->sdoTargetTable); // prevent recursive signal updates
  sdoInspector_->sdoTargetTable->clearContents();
  sdoInspector_->sdoTargetTable->setRowCount(rows.size());
  auto rowActionText = [this, evidenceSetLabel, writeDeltaLabel, watchRow,
                        startupRows, bookmarkRows,
                        targetTrailRow](const QString &key) {
    if (key == uiText("Watch Link", "Watch 关联")) {
      return watchRow >= 0 ? uiText("Open Watch", "打开 Watch")
                           : uiText("Copy row", "复制本行");
    }
    if (key == uiText("Startup Link", "Startup 关联")) {
      return startupRows > 0 ? uiText("Open Startup", "打开 Startup")
                             : uiText("Copy row", "复制本行");
    }
    if (key == uiText("Bookmark", "书签")) {
      return bookmarkRows > 0 ? uiText("Open Bookmark", "打开书签")
                              : uiText("Copy row", "复制本行");
    }
    if (key == uiText("Target Trail", "目标轨迹")) {
      return targetTrailRow >= 0 ? uiText("Open Trail", "打开轨迹")
                                 : uiText("Copy row", "复制本行");
    }
    if (key == evidenceSetLabel) {
      return currentSdoWriteDeltaReviewAvailable()
                 ? uiText("Review evidence", "审阅证据")
                 : uiText("Copy digest", "复制摘要");
    }
    if (key == writeDeltaLabel) {
      return currentSdoWriteDeltaReviewAvailable()
                 ? uiText("Review delta", "审阅差异")
                 : uiText("Copy digest", "复制摘要");
    }
    if (key == uiText("Target", "目标") ||
        key == uiText("Read Value", "读回值") ||
        key == uiText("OD Evidence", "OD 证据")) {
      return uiText("Focus OD", "聚焦 OD");
    }
    if (key == uiText("Next Action", "下一步") ||
        key == uiText("Safety", "安全边界") || key == uiText("State", "状态") ||
        key == uiText("Check", "检查")) {
      return uiText("Copy digest", "复制摘要");
    }
    return uiText("Copy row", "复制本行");
  };
  for (int row = 0; row < rows.size(); ++row) {
    auto *keyItem = new QTableWidgetItem(rows.at(row).first);
    auto *valueItem = new QTableWidgetItem(rows.at(row).second);
    const QString actionText = rowActionText(rows.at(row).first);
    auto *actionItem = new QTableWidgetItem(actionText);
    keyItem->setToolTip(rows.at(row).first);
    valueItem->setToolTip(rows.at(row).second);
    actionItem->setToolTip(uiText(
        "Double-click, press Alt+Enter, right-click, or use the Command "
        "Palette to run this local row action. It does not access the bus.",
        "双击、按 "
        "Alt+Enter、右键或使用命令面板可执行该本地行动作；不访问总线。"));
    actionItem->setForeground(settings_.theme == "Light" ? QColor("#2563eb")
                                                         : QColor("#93c5fd"));
    if (rows.at(row).first == uiText("State", "状态")) {
      const QColor color =
          status == uiText("Blocked", "受阻") ||
                  status == uiText("Read only", "只读")
              ? QColor("#ef4444")
              : (status == uiText("Warning", "警告") ||
                         status == uiText("Incomplete", "未完整")
                     ? QColor("#f59e0b")
                     : QColor("#22c55e"));
      valueItem->setForeground(color);
    }
    if (rows.at(row).first == uiText("Check", "检查")) {
      valueItem->setForeground(QColor("#f59e0b"));
    }
    if (rows.at(row).first == uiText("Next Action", "下一步")) {
      valueItem->setForeground(settings_.theme == "Light" ? QColor("#1d4ed8")
                                                          : QColor("#93c5fd"));
    }
    if (rows.at(row).first == uiText("Safety", "安全边界")) {
      valueItem->setForeground(settings_.theme == "Light" ? QColor("#475569")
                                                          : QColor("#b9c6d6"));
    }
    if (rows.at(row).first == evidenceSetLabel) {
      if (evidenceSetState == QStringLiteral("match")) {
// Update the SDO target row action button based on current state.
        valueItem->setForeground(
            settings_.theme == "Light" ? QColor("#166534") : QColor("#86efac"));
      } else if (evidenceSetState == QStringLiteral("conflict")) {
        valueItem->setForeground(QColor("#ef4444"));
      } else {
        valueItem->setForeground(
            settings_.theme == "Light" ? QColor("#475569") : QColor("#b9c6d6"));
      }
    }
    if (rows.at(row).first == writeDeltaLabel) {
      if (writeDeltaState == QStringLiteral("match")) {
        valueItem->setForeground(
            settings_.theme == "Light" ? QColor("#166534") : QColor("#86efac"));
      } else if (writeDeltaState == QStringLiteral("diff")) {
        valueItem->setForeground(QColor("#f59e0b"));
      } else if (writeDeltaState == QStringLiteral("conflict")) {
        valueItem->setForeground(QColor("#ef4444"));
      } else {
        valueItem->setForeground(
            settings_.theme == "Light" ? QColor("#475569") : QColor("#b9c6d6"));
      }
    }
    if (rows.at(row).first == uiText("Watch Link", "Watch 关联") &&
        watchRow >= 0) {
      valueItem->setForeground(settings_.theme == "Light" ? QColor("#166534")
                                                          : QColor("#86efac"));
    }
    if (rows.at(row).first == uiText("Startup Link", "Startup 关联") &&
        startupRows > 0) {
      const bool diff = startupDelta.contains("diff", Qt::CaseInsensitive) ||
                        startupDelta.contains("不一致");
      valueItem->setForeground(diff ? QColor("#f59e0b")
                                    : (settings_.theme == "Light"
                                           ? QColor("#166534")
                                           : QColor("#86efac")));
    }
    if (rows.at(row).first == uiText("Target Trail", "目标轨迹") &&
        targetTrailRow >= 0) {
      valueItem->setForeground(settings_.theme == "Light" ? QColor("#1d4ed8")
                                                          : QColor("#93c5fd"));
// Update the SDO target row copy button based on available data.
    }
    sdoInspector_->sdoTargetTable->setItem(row, 0, keyItem);
    sdoInspector_->sdoTargetTable->setItem(row, 1, valueItem);
    sdoInspector_->sdoTargetTable->setItem(row, 2, actionItem);
  }
  sdoInspector_->sdoTargetTable->resizeRowsToContents();
  updateSdoTargetRowActionButton();
  updateSdoTargetRowCopyButton();
}


// — Update sdo target row action button
void MainWindow::updateSdoTargetRowActionButton() {
  auto *button = findChild<QPushButton *>("runSdoTargetRowAction");
  if (!button) {
    return;
  }

  QString action;
  QString field;
  if (sdoInspector_->sdoTargetTable) {
    const int row = sdoInspector_->sdoTargetTable->currentRow();
    if (row >= 0 && row < sdoInspector_->sdoTargetTable->rowCount()) {
      field = tableText(sdoInspector_->sdoTargetTable, row, 0);
      action = tableText(sdoInspector_->sdoTargetTable, row, 2);
    }
  }

  if (action.isEmpty()) {
    button->setText(uiText("Run Row Action", "执行本行动作"));
    button->setToolTip(uiText(
        "Select a Selected Object row, then run its local Action column entry. "
        "This does not read the bus, write SDOs, change state, toggle Free "
        "Run, or run Host Health.",
        "选择一行选中对象后，执行其 Action/动作列中的本地动作；不读取总线、"
        "不写 SDO、不切换状态、不改变 Free Run，也不运行 Host Health。"));
    return;
  }

  button->setText(uiText("Run: %1", "执行：%1").arg(action));
// Update the SDO inspector with the given source and target data.
  button->setToolTip(
      uiText("Run the selected row action \"%1\" for \"%2\" using local "
             "evidence/navigation only. No bus read, no SDO write, no state "
             "change, no Free Run change, and no Host Health.",
             "对“%2”执行所选行本地动作“%1”；只使用本地证据/导航，不读取总线、"
             "不写 SDO、不切换状态、不改变 Free Run，也不运行 Host Health。")
          .arg(action, field.isEmpty()
                           ? uiText("Selected Object row", "选中对象行")
                           : field));
}


// — Update sdo target row copy button
void MainWindow::updateSdoTargetRowCopyButton() {
  auto *button = findChild<QPushButton *>("copySdoTargetRowEvidence");
  if (!button) {
    return;
  }

  QString field;
  if (sdoInspector_->sdoTargetTable) {
    const int row = sdoInspector_->sdoTargetTable->currentRow();
    if (row >= 0 && row < sdoInspector_->sdoTargetTable->rowCount()) {
      field = tableText(sdoInspector_->sdoTargetTable, row, 0);
    }
  }

  if (field.isEmpty()) {
    button->setText(uiText("Copy Row", "复制本行"));
    button->setToolTip(uiText(
        "Select a Selected Object row to copy that row's target, value, local "
        "action, and safety boundary. This does not access the bus.",
        "选择一行选中对象后，复制该行目标、值、本地动作和安全边界；不访问总线"
        "。"));
    return;
  }

  button->setText(uiText("Copy: %1", "复制：%1").arg(field));
  button->setToolTip(
      uiText("Copy the Selected Object row \"%1\" with target, value, local "
             "action, and safety boundary. Clipboard only; no bus access.",
             "复制选中对象行“%1”的目标、值、本地动作和安全边界；只写剪贴板，"
             "不访问总线。")
          .arg(field));
}


// — Update sdo inspector
void MainWindow::updateSdoInspector(const QString &source,
                                    const QString &detail) {
  if (!sdoInspector_->sdoInspectorLabel) {
    return;
  }

  const int position = selectedPosition();
  const QString index = sdoInspector_->sdoIndex ? sdoInspector_->sdoIndex->text().trimmed() : QString();
  const QString subIndex =
      sdoInspector_->sdoSubIndex ? sdoInspector_->sdoSubIndex->text().trimmed() : QString();
  const QString type = sdoInspector_->sdoType ? sdoInspector_->sdoType->currentText().trimmed() : QString();
  const QString readValue = sdoInspector_->sdoValue ? sdoInspector_->sdoValue->text().trimmed() : QString();
  const QString writeValue =
      sdoInspector_->sdoWriteValue ? sdoInspector_->sdoWriteValue->text().trimmed() : QString();

  QString state = QStringLiteral("ready");
  QString status = uiText("Ready", "就绪");
  QStringList facts;
  facts << uiText("Master %1", "主站 %1").arg(activeMasterName());
  facts << (position >= 0 ? uiText("Slave #%1", "从站 #%1").arg(position)
                          : uiText("No slave selected", "尚未选择从站"));
  facts << (!index.isEmpty() && !subIndex.isEmpty()
                ? uiText("Object %1:%2", "对象 %1:%2").arg(index, subIndex)
                : uiText("No object selected", "尚未选择对象"));
  if (!type.isEmpty()) {
    facts << uiText("Type %1", "类型 %1").arg(type);
  }
  if (!readValue.isEmpty()) {
    facts << uiText("Read %1", "读回 %1").arg(readValue);
  }
  if (!writeValue.isEmpty()) {
    facts << uiText("Write %1", "写入 %1").arg(writeValue);
  }

  QStringList problems;
  if (position < 0) {
    problems << uiText("select a slave first", "先选择从站");
  }
  if (index.isEmpty() || subIndex.isEmpty()) {
    problems << uiText("fill Index/Sub", "填写 Index/Sub");
  }
  if (!selectedSdoWritable_) {
    problems << uiText("read-only object", "只读对象");
  }

  if (!writeValue.isEmpty()) {
    QStringList validationErrors;
    QStringList validationWarnings;
    validateSdoAddressAndValue(index, subIndex, writeValue, type,
                               &validationErrors, &validationWarnings);
    if (!validationErrors.isEmpty()) {
      state = QStringLiteral("blocked");
      status = uiText("Blocked", "受阻");
      problems << validationErrors;
    } else if (!validationWarnings.isEmpty()) {
      state = QStringLiteral("warning");
      status = uiText("Warning", "警告");
      problems << validationWarnings;
    }
  } else if (!problems.isEmpty()) {
    state = QStringLiteral("warning");
    status = uiText("Incomplete", "未完整");
  }

  if (!selectedSdoWritable_) {
    state = QStringLiteral("blocked");
    status = uiText("Read only", "只读");
  }

  QStringList parts;
  parts << status;
  if (!source.trimmed().isEmpty()) {
    parts << uiText("Source: %1", "来源：%1").arg(source.trimmed());
  }
  parts << facts.join(" | ");
  if (!detail.trimmed().isEmpty()) {
    parts << detail.trimmed();
  }
  if (!problems.isEmpty()) {
    parts << uiText("Check: %1", "检查：%1").arg(problems.join("; "));
  }

  sdoInspector_->sdoInspectorLabel->setText(parts.join("  -  "));
  sdoInspector_->sdoInspectorLabel->setToolTip(parts.join("\n"));
  sdoInspector_->sdoInspectorLabel->setProperty("state", state);
  repolish(sdoInspector_->sdoInspectorLabel); // force QSS re-evaluation after property change
  updateSdoTargetPanel(source, detail, status, problems);
}


// — Create the SDO target trail table columns if not yet initialized