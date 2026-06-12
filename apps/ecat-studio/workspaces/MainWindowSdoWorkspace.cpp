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


// — Build a human-readable impact summary for a proposed SDO write
QStringList MainWindow::sdoWriteImpactDetails(int position,
                                              const QString &index,
                                              const QString &subIndex,
                                              const QString &targetValue,
                                              const QString &type) const {
  QStringList details;
  // Normalize hex address for consistent comparison
  const QString normalizedIndex = normalizeHexText(index, 4);
  // Normalize hex address for consistent comparison
  const QString normalizedSubIndex = normalizeHexText(subIndex, 2);
  const QString target = targetValue.trimmed();

  QString slaveName = uiText("unknown slave", "未知从站");
  QString slaveState = uiText("unknown", "未知");
  for (const auto &slave : slaves_) {
    if (slave.position == position) {
      slaveName =
          slave.name.trimmed().isEmpty() ? slaveName : slave.name.trimmed();
      slaveState =
          slave.state.trimmed().isEmpty() ? slaveState : slave.state.trimmed();
      break;
    }
  }
  details << uiText("Slave context: #%1 %2, state %3",
                    "从站上下文：#%1 %2，状态 %3")
                 .arg(position)
                 .arg(slaveName, slaveState);

  QString objectText;
  QString accessText;
  QString nameText;
  QString bitsText;
  if (sdoTable_ && selectedPosition() == position &&
      loadedSdoPosition_ == position) {
    const SdoDictionaryRow dictionary = sdoDictionaryRowForTarget(
        sdoTable_, normalizedIndex, normalizedSubIndex);
    objectText = dictionary.object;
    accessText = dictionary.access;
    bitsText = dictionary.bits;
    nameText = dictionary.name;
  }

  const QString category =
      sdoObjectCategory(normalizedIndex, nameText, objectText, QString());
  details << uiText("Object class: %1", "对象类别：%1").arg(category);
  if (!nameText.isEmpty() || !accessText.isEmpty() || !bitsText.isEmpty()) {
    QStringList objectFacts;
    if (!nameText.isEmpty()) {
      objectFacts << nameText;
    }
    if (!accessText.isEmpty()) {
      objectFacts << uiText("access %1", "权限 %1").arg(accessText);
    }
    if (!bitsText.isEmpty()) {
      objectFacts << uiText("%1 bit", "%1 位").arg(bitsText);
    }
    details << uiText("Dictionary: %1", "对象字典：%1")
                   .arg(objectFacts.join(" | "));
  }

  const QStringList cachedEvidence =
      sdoEvidence_.value(sdoEvidenceKey(position, index, subIndex));
  const QVector<SdoEvidenceItem> localEvidence =
      sdoLocalEvidenceItemsFromTables(
          position, normalizedIndex, normalizedSubIndex,
          sdoValue_ ? sdoValue_->text() : QString(), cachedEvidence.value(0),
          sdoValue_ &&
              isCurrentSdoTarget(position, normalizedIndex, normalizedSubIndex),
          sdoTable_ && loadedSdoPosition_ == position,
          {.dictionaryTable = sdoTable_,
           .watchTable = watchTable_,
           .startupTable = startupSdoTable_,
           .bookmarkTable = objectBookmarkTable_},
          {.read = uiText("Read", "读回"),
           .watchPrefix = uiText("Watch", "Watch"),
           .dictionary = uiText("OD", "OD"),
           .startupPrefix = uiText("Startup", "Startup"),
           .bookmarkPrefix = uiText("Bookmark", "书签")});

  const auto evidenceGroups = groupSdoEvidence(localEvidence);
  if (evidenceGroups.isEmpty()) {
    details << uiText("Evidence Set: no local comparable evidence",
                      "证据集：无可比较的本地证据");
  } else {
    QStringList groupFacts;
    for (const auto &group : evidenceGroups) {
      groupFacts << QString("%1=%2").arg(group.sources.join("/"), group.value);
    }
    details << (evidenceGroups.size() == 1
                    ? uiText("Evidence Set: consistent %1", "证据集：一致 %1")
                          .arg(groupFacts.join("; "))
                    : uiText("Evidence Set conflict: %1", "证据集冲突：%1")
                          .arg(groupFacts.join("; ")));

    if (!target.isEmpty()) {
      const SdoWriteDeltaReview review =
          reviewSdoWriteDelta(localEvidence, target);
      QStringList differingFacts;
      for (const auto &group : review.differingGroups) {
        differingFacts << QString("%1=%2").arg(group.sources.join("/"),
                                               group.value);
      }
      if (differingFacts.isEmpty()) {
        details << uiText("Write target: matches all local evidence",
                          "写入目标：匹配全部本地证据");
      } else if (!review.matchingSources.isEmpty()) {
        details << uiText("Write target: matches %1, differs from %2",
                          "写入目标：匹配 %1，不同于 %2")
                       .arg(review.matchingSources.join("/"),
                            differingFacts.join("; "));
      } else {
        details << uiText("Write target: differs from local evidence %1",
                          "写入目标：不同于本地证据 %1")
                       .arg(differingFacts.join("; "));
      }
    }
  }

  QString currentValue;
  QString currentSource;
  if (watchTable_) {
    for (int row = 0; row < watchTable_->rowCount(); ++row) {
      const QString value = tableText(watchTable_, row, 4);
      if (tableObjectAddressMatches(watchTable_, row, position, normalizedIndex,
                                    normalizedSubIndex, 1, 2, 3) &&
          !value.isEmpty()) {
        currentValue = value;
        currentSource = uiText("Watch", "Watch");
        break;
      }
    }
  }
  if (currentValue.isEmpty()) {
    const QStringList evidence =
        sdoEvidence_.value(sdoEvidenceKey(position, index, subIndex));
    if (!evidence.value(0).trimmed().isEmpty()) {
      currentValue = evidence.value(0).trimmed();
      currentSource = uiText("Object Dictionary evidence", "对象字典证据");
      const QString status = evidence.value(1).trimmed();
      const QString time = evidence.value(3).trimmed();
      if (!status.isEmpty()) {
        details << uiText("Last OD status: %1%2", "最后 OD 状态：%1%2")
                       .arg(status, time.isEmpty() ? QString() : "  " + time);
      }
    }
  }

  if (!currentValue.isEmpty()) {
    const bool same = normalizeComparableValue(currentValue) ==
                      normalizeComparableValue(target);
    details << (same ? uiText(
                           "Change preview: current %1 already matches target",
                           "变更预览：当前值 %1 已匹配目标值")
                           .arg(currentValue)
                     : uiText("Change preview: %1 -> %2", "变更预览：%1 -> %2")
                           .arg(currentValue, target));
    details << uiText("Current evidence source: %1", "当前证据来源：%1")
                   .arg(currentSource);
  } else {
    details << uiText("Current evidence: no Watch or OD value available",
                      "当前证据：没有可用的 Watch 或 OD 值");
  }

  if (startupSdoTable_) {
    int startupMatches = 0;
    QString startupExpected;
    for (int row = 0; row < startupSdoTable_->rowCount(); ++row) {
      if (!tableObjectAddressMatches(startupSdoTable_, row, position,
                                     normalizedIndex, normalizedSubIndex, 0, 1,
                                     2)) {
        continue;
      }
      ++startupMatches;
      startupExpected = tableText(startupSdoTable_, row, 3);
    }
    if (startupMatches > 0) {
      details << uiText("Startup expectation: %1 (%2 matching startup row(s))",
                        "Startup 期望值：%1（%2 条匹配启动行）")
                     .arg(startupExpected)
                     .arg(startupMatches);
    }
  }

  const QString riskKey = QString("%1 %2 %3 %4")
                              .arg(normalizedIndex, nameText, objectText, type)
                              .toLower();
  QStringList riskHints;
  if (riskKey.contains("0x6040") || riskKey.contains("0x6060") ||
      riskKey.contains("controlword") || riskKey.contains("mode")) {
    riskHints << uiText("drive command/mode object; confirm actuator safety",
                        "驱动命令/模式对象；确认执行机构安全");
  }
  if (riskKey.contains("0x160") || riskKey.contains("0x1a0") ||
      riskKey.contains("0x1c1") || riskKey.contains("0x1c2") ||
      riskKey.contains("pdo") || riskKey.contains("mapping")) {
    riskHints << uiText(
        "PDO mapping object; verify state and process-image impact",
        "PDO 映射对象；确认状态和过程映像影响");
  }
  if (riskKey.contains("0x1010") || riskKey.contains("0x1011") ||
      riskKey.contains("store") || riskKey.contains("save") ||
      riskKey.contains("restore")) {
    riskHints << uiText("persistent storage object; change may survive reboot",
                        "持久化存储对象；变更可能跨重启保留");
  }
  if (!riskHints.isEmpty()) {
    details
        << uiText("Risk flags: %1", "风险标记：%1").arg(riskHints.join("; "));
  }

  const QStringList topologyIssues = topologyBaselineIssues();
  if (!topologyIssues.isEmpty()) {
    details << uiText("Topology baseline: %1 issue(s); review before writing",
                      "拓扑基线：%1 个问题；写入前请复核")
                   .arg(topologyIssues.size());
  }
  return details;
}


// — Return the startup sdo impact line
QString MainWindow::startupSdoImpactLine(int row) const {
  if (!startupSdoTable_ || row < 0 || row >= startupSdoTable_->rowCount()) {
    return QString();
  }
  const QString position = tableText(startupSdoTable_, row, 0);
  const QString index = tableText(startupSdoTable_, row, 1);
  const QString subIndex = tableText(startupSdoTable_, row, 2);
  const QString target = tableText(startupSdoTable_, row, 3);
  const QString type = tableText(startupSdoTable_, row, 4);
  const QString watchValue = tableText(startupSdoTable_, row, 7);
  const QString watchDelta = tableText(startupSdoTable_, row, 8);

  bool positionOk = false;
  const int numericPosition = position.toInt(&positionOk);
  QString current = watchValue;
  QString source = uiText("Watch", "Watch");
  if (current.isEmpty() && positionOk) {
    const QStringList evidence =
        sdoEvidence_.value(sdoEvidenceKey(numericPosition, index, subIndex));
    current = evidence.value(0).trimmed();
    source = uiText("OD evidence", "OD 证据");
  }
  if (current.isEmpty()) {
    current = uiText("unknown current", "当前未知");
    source = uiText("no evidence", "无证据");
  }

  QString line =
      uiText("Row %1: #%2 %3:%4 %5 -> %6", "第 %1 行：#%2 %3:%4 %5 -> %6")
          .arg(row + 1)
          .arg(position, index, subIndex, current, target);
  QStringList suffix;
  suffix << source;
  if (!watchDelta.isEmpty()) {
    suffix << uiText("Watch delta %1", "Watch 偏差 %1").arg(watchDelta);
  }
  if (!type.isEmpty()) {
    suffix << uiText("type %1", "类型 %1").arg(type);
  }
  return QString("%1 (%2)").arg(line, suffix.join(" | "));
}


// — Return a list of startup sdo batch impact details
QStringList MainWindow::startupSdoBatchImpactDetails(const QVector<int> &rows,
                                                     int previewLimit) const {
  QStringList details;
  if (!startupSdoTable_ || rows.isEmpty()) {
    return details;
  }

  int watchMatch = 0;
  int watchDiff = 0;
  int pending = 0;
  int noWatch = 0;
  int driveRisk = 0;
  int pdoRisk = 0;
  int persistentRisk = 0;

  for (const int row : rows) {
    if (row < 0 || row >= startupSdoTable_->rowCount()) {
      continue;
    }
    const QString delta = tableText(startupSdoTable_, row, 8).toLower();
    if (delta == "match" || delta == "匹配") {
      ++watchMatch;
    } else if (delta == "diff" || delta == "不一致") {
      ++watchDiff;
    } else if (delta == "no watch" || delta == "无监视") {
      ++noWatch;
    } else {
      ++pending;
    }

    const QString riskKey =
        QString("%1 %2")
            // Normalize hex address for consistent comparison
            .arg(normalizeHexText(tableText(startupSdoTable_, row, 1), 4),
                 tableText(startupSdoTable_, row, 4))
            .toLower();
    if (riskKey.contains("0x6040") || riskKey.contains("0x6060")) {
      ++driveRisk;
    }
    if (riskKey.contains("0x160") || riskKey.contains("0x1a0") ||
        riskKey.contains("0x1c1") || riskKey.contains("0x1c2")) {
      ++pdoRisk;
    }
    if (riskKey.contains("0x1010") || riskKey.contains("0x1011")) {
      ++persistentRisk;
    }
  }

  details << uiText(
                 "Watch evidence: match %1, diff %2, pending %3, no watch %4",
                 "Watch 证据：匹配 %1，不一致 %2，待比较 %3，无监视 %4")
                 .arg(watchMatch)
                 .arg(watchDiff)
                 .arg(pending)
                 .arg(noWatch);

  QStringList riskFacts;
  if (driveRisk > 0) {
    riskFacts << uiText("drive/mode rows %1", "驱动/模式行 %1").arg(driveRisk);
  }
  if (pdoRisk > 0) {
    riskFacts << uiText("PDO mapping rows %1", "PDO 映射行 %1").arg(pdoRisk);
  }
  if (persistentRisk > 0) {
    riskFacts
        << uiText("persistent rows %1", "持久化行 %1").arg(persistentRisk);
  }
  if (!riskFacts.isEmpty()) {
    details
        << uiText("Risk summary: %1", "风险摘要：%1").arg(riskFacts.join("; "));
  }

  const QStringList topologyIssues = topologyBaselineIssues();
  if (!topologyIssues.isEmpty()) {
    details << uiText("Topology baseline: %1 issue(s); review before applying",
                      "拓扑基线：%1 个问题；应用前请复核")
                   .arg(topologyIssues.size());
  }

  const int limit =
      std::max(0, std::min(previewLimit, static_cast<int>(rows.size())));
  for (int i = 0; i < limit; ++i) {
    const QString line = startupSdoImpactLine(rows.at(i));
    if (!line.isEmpty()) {
      details << line;
    }
  }
  if (rows.size() > limit) {
    details << uiText("...and %1 more item(s)", "...另有 %1 项")
                   .arg(rows.size() - limit);
  }
  return details;
}


// — Restore manual sdo write mode
void MainWindow::restoreManualSdoWriteMode() {
  if (selectedSdoWritable_) {
    updateSdoInspector(uiText("Manual edit", "手动编辑"));
    return;
  }
  selectedSdoWritable_ = true;
  if (sdoWriteValue_) {
    sdoWriteValue_->setEnabled(true);
    sdoWriteValue_->setPlaceholderText(uiText("Value to write", "写入值"));
  }
  updateSdoInspector(uiText("Manual edit", "手动编辑"));
  updateActionAvailability();
}


// — Check whether is current sdo target
bool MainWindow::isCurrentSdoTarget(int position, const QString &index,
                                    const QString &subIndex) const {
  if (position < 0 || selectedPosition() < 0 || !sdoIndex_ || !sdoSubIndex_) {
    return false;
  }
  const QString currentIndex = sdoIndex_->text().trimmed();
  const QString currentSubIndex = sdoSubIndex_->text().trimmed();
  if (currentIndex.isEmpty() || currentSubIndex.isEmpty()) {
    return false;
  }
  return sdoEvidenceKey(position, index, subIndex) ==
         sdoEvidenceKey(selectedPosition(), currentIndex, currentSubIndex);
}


// — Return the current sdo dictionary row
int MainWindow::currentSdoDictionaryRow() const {
  const int position = selectedPosition();
  if (position < 0 || !sdoTable_ || !sdoIndex_ || !sdoSubIndex_ ||
      loadedSdoPosition_ != position) {
    return -1;
  }
  return sdoEvidenceTableRowsForTarget({.dictionaryTable = sdoTable_},
                                       {.position = position,
                                        .index = sdoIndex_->text(),
                                        .subIndex = sdoSubIndex_->text(),
                                        .dictionaryLoadedForPosition = true})
      .dictionaryRow;
}


// — Return the current sdo watch row
int MainWindow::currentSdoWatchRow() const {
  const int position = selectedPosition();
  if (position < 0 || !watchTable_ || !sdoIndex_ || !sdoSubIndex_) {
    return -1;
  }
  return sdoEvidenceTableRowsForTarget({.watchTable = watchTable_},
                                       {.position = position,
                                        .index = sdoIndex_->text(),
                                        .subIndex = sdoSubIndex_->text()})
      .watchRow;
}


// — Return the current sdo startup row
int MainWindow::currentSdoStartupRow() const {
  const int position = selectedPosition();
  if (position < 0 || !startupSdoTable_ || !sdoIndex_ || !sdoSubIndex_) {
    return -1;
  }
  return sdoEvidenceTableRowsForTarget({.startupTable = startupSdoTable_},
                                       {.position = position,
                                        .index = sdoIndex_->text(),
                                        .subIndex = sdoSubIndex_->text()})
      .startupRow;
}


// — Return the current sdo bookmark row
int MainWindow::currentSdoBookmarkRow() const {
  const int position = selectedPosition();
  if (position < 0 || !objectBookmarkTable_ || !sdoIndex_ || !sdoSubIndex_) {
    return -1;
  }
  return sdoEvidenceTableRowsForTarget({.bookmarkTable = objectBookmarkTable_},
                                       {.position = position,
                                        .index = sdoIndex_->text(),
                                        .subIndex = sdoSubIndex_->text()})
      .bookmarkRow;
}


// — Return the current sdo target trail row
int MainWindow::currentSdoTargetTrailRow() const {
  const int position = selectedPosition();
  if (position < 0 || !sdoTargetTrailTable_ || !sdoIndex_ || !sdoSubIndex_) {
    return -1;
  }
  return sdoEvidenceTableRowsForTarget(
             {.targetTrailTable = sdoTargetTrailTable_},
             {.position = position,
              .index = sdoIndex_->text(),
              .subIndex = sdoSubIndex_->text()})
      .targetTrailRow;
}


// — Return the current sdo preferred evidence value
QString MainWindow::currentSdoPreferredEvidenceValue(QString *source) const {
  return preferredSdoEvidenceValue(currentSdoEvidenceCandidates(), source);
}


// — Current sdo evidence candidates
SdoEvidenceCandidates MainWindow::currentSdoEvidenceCandidates() const {
  const int position = selectedPosition();
  const SdoEvidenceTableRows rows = sdoEvidenceTableRowsForTarget(
      {.dictionaryTable = sdoTable_,
       .watchTable = watchTable_,
       .startupTable = startupSdoTable_,
       .bookmarkTable = objectBookmarkTable_,
       .targetTrailTable = sdoTargetTrailTable_},
      {.position = position,
       .index = sdoIndex_ ? sdoIndex_->text() : QString(),
       .subIndex = sdoSubIndex_ ? sdoSubIndex_->text() : QString(),
       .dictionaryLoadedForPosition = loadedSdoPosition_ == position});
  return sdoEvidenceCandidatesFromTables(
      sdoValue_ ? sdoValue_->text() : QString(),
      {.dictionaryTable = sdoTable_,
       .watchTable = watchTable_,
       .startupTable = startupSdoTable_,
       .bookmarkTable = objectBookmarkTable_,
       .targetTrailTable = sdoTargetTrailTable_},
      rows,
      {.readValue = uiText("Read Value", "读回值"),
       .watch = uiText("Watch", "Watch"),
       .dictionary = uiText("OD Evidence", "OD 证据"),
       .startup = uiText("Startup SDO", "Startup SDO"),
       .bookmark = uiText("Object Bookmark", "对象书签"),
       .targetTrailWrite = uiText("Target Trail Write", "目标轨迹写入"),
       .targetTrail = uiText("Target Trail", "目标轨迹")});
}


// — Check whether current sdo evidence has conflict
bool MainWindow::currentSdoEvidenceHasConflict() const {
  return sdoEvidenceHasConflict(currentSdoEvidenceCandidates());
}


// — Check whether current sdo write delta review available
bool MainWindow::currentSdoWriteDeltaReviewAvailable() const {
  if (selectedPosition() < 0 || !sdoIndex_ || !sdoSubIndex_) {
    return false;
  }
  if (currentSdoEvidenceHasConflict()) {
    return true;
  }
  if (!sdoWriteValue_ || sdoWriteValue_->text().trimmed().isEmpty()) {
    return false;
  }
  return sdoWriteDeltaReviewEvidenceAvailable(
      sdoValue_ ? sdoValue_->text() : QString(),
      {.dictionaryTable = sdoTable_,
       .watchTable = watchTable_,
       .startupTable = startupSdoTable_,
       .bookmarkTable = objectBookmarkTable_,
       .targetTrailTable = sdoTargetTrailTable_},
      {.dictionaryRow = currentSdoDictionaryRow(),
       .watchRow = currentSdoWatchRow(),
       .startupRow = currentSdoStartupRow(),
       .bookmarkRow = currentSdoBookmarkRow(),
       .targetTrailRow = currentSdoTargetTrailRow()});
}


// — Compare the current SDO target value against the last read value
void MainWindow::reviewCurrentSdoWriteDelta() {
  if (!currentSdoWriteDeltaReviewAvailable()) {
    updateDiagnostics("Info", "Navigation",
                      uiText("No local Write Delta evidence for current SDO "
                             "target",
                             "当前 SDO 目标没有可审阅的本地写入差异证据"));
    return;
  }

  const QString writeValue =
      sdoWriteValue_ ? sdoWriteValue_->text().trimmed() : QString();
  const QString normalizedWrite = normalizeComparableValue(writeValue);
  auto differsFromWrite = [&normalizedWrite](const QString &value) {
    const QString normalized = normalizeComparableValue(value);
    return !normalized.isEmpty() && normalized != normalizedWrite;
  };
  const auto candidates = currentSdoEvidenceCandidates();
  QString baselineEvidence;
  auto conflictsWithBaseline = [&baselineEvidence](const QString &value) {
    const QString normalized = normalizeComparableValue(value);
    if (normalized.isEmpty()) {
      return false;
    }
    if (baselineEvidence.isEmpty()) {
      baselineEvidence = normalized;
      return false;
    }
    return normalized != baselineEvidence;
  };
  if (writeValue.isEmpty()) {
    for (const auto &candidate : candidates) {
      conflictsWithBaseline(candidate.second);
    }
  }
  auto shouldReviewValue = [&writeValue, &differsFromWrite,
                            &conflictsWithBaseline](const QString &value) {
    return writeValue.isEmpty() ? conflictsWithBaseline(value)
                                : differsFromWrite(value);
  };

  const int watchRow = currentSdoWatchRow();
  if (watchRow >= 0 && shouldReviewValue(tableText(watchTable_, watchRow, 4))) {
    openCurrentSdoWatchLink();
    updateDiagnostics("Info", "Navigation",
                      writeValue.isEmpty()
                          ? uiText("Reviewing Evidence Set conflict in Watch",
                                   "正在 Watch 中审阅证据集冲突")
                          : uiText("Reviewing Write Delta in Watch evidence",
                                   "正在 Watch 证据中审阅写入差异"));
    return;
  }

  const int startupRow = currentSdoStartupRow();
  if (startupRow >= 0 &&
      shouldReviewValue(tableText(startupSdoTable_, startupRow, 3))) {
    openCurrentSdoStartupLink();
    updateDiagnostics("Info", "Navigation",
                      writeValue.isEmpty()
                          ? uiText("Reviewing Evidence Set conflict in Startup "
                                   "SDO",
                                   "正在 Startup SDO 中审阅证据集冲突")
                          : uiText("Reviewing Write Delta in Startup SDO "
                                   "evidence",
                                   "正在 Startup SDO 证据中审阅写入差异"));
    return;
  }

  const int bookmarkRow = currentSdoBookmarkRow();
  const SdoObjectBookmarkRow bookmark =
      sdoObjectBookmarkRowFromTable(objectBookmarkTable_, bookmarkRow);
  if (bookmarkRow >= 0 && shouldReviewValue(bookmark.lastValue)) {
    openCurrentSdoBookmarkLink();
    updateDiagnostics("Info", "Navigation",
                      writeValue.isEmpty()
                          ? uiText("Reviewing Evidence Set conflict in Object "
                                   "Bookmark",
                                   "正在对象书签中审阅证据集冲突")
                          : uiText("Reviewing Write Delta in Object Bookmark "
                                   "evidence",
                                   "正在对象书签证据中审阅写入差异"));
    return;
  }

  const int trailRow = currentSdoTargetTrailRow();
  const SdoTargetTrailRow trail =
      sdoTargetTrailRowFromTable(sdoTargetTrailTable_, trailRow);
  if (trailRow >= 0 &&
      (shouldReviewValue(trail.writeValue) || shouldReviewValue(trail.value))) {
    openCurrentSdoTargetTrailLink();
    updateDiagnostics("Info", "Navigation",
                      writeValue.isEmpty()
                          ? uiText("Reviewing Evidence Set conflict in SDO "
                                   "Target Trail",
                                   "正在 SDO 目标轨迹中审阅证据集冲突")
                          : uiText("Reviewing Write Delta in SDO Target Trail "
                                   "evidence",
                                   "正在 SDO 目标轨迹证据中审阅写入差异"));
    return;
  }

  if (sdoValue_ && shouldReviewValue(sdoValue_->text())) {
    activateWorkspaceTab(objectDictionaryTabIndex_);
    sdoValue_->setFocus();
    sdoValue_->selectAll();
    updateDiagnostics("Info", "Navigation",
                      writeValue.isEmpty()
                          ? uiText("Reviewing Evidence Set conflict in current "
                                   "read-back field",
                                   "正在当前读回字段中审阅证据集冲突")
                          : uiText("Reviewing Write Delta in current read-back "
                                   "field",
                                   "正在当前读回字段中审阅写入差异"));
    return;
  }

  const int dictionaryRow = currentSdoDictionaryRow();
  if (dictionaryRow >= 0) {
    activateObjectDictionaryPaneFor(sdoTable_);
    if (sdoFilter_ && sdoTable_->isRowHidden(dictionaryRow)) {
      sdoFilter_->clear();
      filterSdoTable(QString());
    }
    sdoTable_->clearSelection();
    selectAndFocusTableRow(sdoTable_, dictionaryRow, 7);
    updateDiagnostics("Info", "Navigation",
                      uiText("Reviewing Write Delta in Object Dictionary "
                             "evidence",
                             "正在对象字典证据中审阅写入差异"));
    return;
  }

  activateWorkspaceTab(objectDictionaryTabIndex_);
  if (sdoValue_) {
    sdoValue_->setFocus();
    sdoValue_->selectAll();
  }
  updateDiagnostics("Info", "Navigation",
                    uiText("Reviewing Write Delta in current read-back field",
                           "正在当前读回字段中审阅写入差异"));
}


// — Populate the SDO read/write controls from the selected target panel row
bool MainWindow::openSdoTargetPanelRow(int row) {
  if (!sdoTargetTable_ || row < 0 || row >= sdoTargetTable_->rowCount()) {
    return false;
  }

  const QString key = tableText(sdoTargetTable_, row, 0);
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
    activateObjectDictionaryPaneFor(sdoTable_);
    const int dictionaryRow = currentSdoDictionaryRow();
    if (dictionaryRow >= 0 && sdoTable_) {
      if (sdoFilter_ && sdoTable_->isRowHidden(dictionaryRow)) {
        sdoFilter_->clear();
        filterSdoTable(QString());
      }
      sdoTable_->clearSelection();
      selectAndFocusTableRow(sdoTable_, dictionaryRow, 1);
    } else if (sdoIndex_) {
      sdoIndex_->setFocus();
      sdoIndex_->selectAll();
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
  if (!sdoTargetTable_ || row < 0 || row >= sdoTargetTable_->rowCount()) {
    statusBar()->showMessage(uiText("Select a Selected Object row to copy.",
                                    "请选择一行选中对象复核内容再复制。"),
                             3000);
    return false;
  }

  const QString key = tableText(sdoTargetTable_, row, 0);
  const QString value = tableText(sdoTargetTable_, row, 1);
  const SdoTargetPanelRouteDecision decision =
      sdoTargetPanelRouteDecision(key, currentSdoWriteDeltaReviewAvailable());

  QString action;
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
  const QString index = sdoIndex_ ? sdoIndex_->text().trimmed() : QString();
  const QString subIndex =
      sdoSubIndex_ ? sdoSubIndex_->text().trimmed() : QString();
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
  const QString index = sdoIndex_ ? sdoIndex_->text().trimmed() : QString();
  const QString subIndex =
      sdoSubIndex_ ? sdoSubIndex_->text().trimmed() : QString();
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

  const QString type = sdoType_ ? sdoType_->currentText().trimmed() : QString();
  const QString readValue = sdoValue_ ? sdoValue_->text().trimmed() : QString();
  const QString writeValue =
      sdoWriteValue_ ? sdoWriteValue_->text().trimmed() : QString();
  if (!type.isEmpty()) {
    lines << QString("%1: %2").arg(uiText("Type", "类型"), type);
  }
  if (!readValue.isEmpty()) {
    lines << QString("%1: %2").arg(uiText("Read Value", "读回值"), readValue);
  }
  if (!writeValue.isEmpty()) {
    lines << QString("%1: %2").arg(uiText("Write Value", "写入值"), writeValue);
  }

  if (sdoTargetTable_ && sdoTargetTable_->rowCount() > 0) {
    lines << QString();
    lines << uiText("Selected Object Review", "选中对象复核");
    for (int row = 0; row < sdoTargetTable_->rowCount(); ++row) {
      const QString key = tableText(sdoTargetTable_, row, 0);
      const QString value = tableText(sdoTargetTable_, row, 1);
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
  if (row < 0 || !watchTable_) {
    updateDiagnostics("Info", "Navigation",
                      uiText("No matching Watch row for current SDO target",
                             "当前 SDO 目标没有匹配的 Watch 行"));
    return;
  }
  activateWorkspaceTab(watchTabIndex_);
  selectAndFocusTableRow(watchTable_, row, 1);
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
  if (startupWatchDiffsOnly_) {
    startupWatchDiffsOnly_->setChecked(false);
  }
  filterStartupSdoTable();
  selectAndFocusTableRow(startupSdoTable_, row, 0);
  updateStartupSdoControls();
  updateDiagnostics("Info", "Navigation",
                    uiText("Opened matching Startup SDO row for current target",
                           "已打开当前目标匹配的 Startup SDO 行"));
}


// — Open current sdo bookmark link
void MainWindow::openCurrentSdoBookmarkLink() {
  const int row = currentSdoBookmarkRow();
  if (row < 0 || !objectBookmarkTable_) {
    updateDiagnostics("Info", "Navigation",
                      uiText("No matching Object Bookmark for current target",
                             "当前目标没有匹配的对象书签"));
    return;
  }
  activateObjectDictionaryPaneFor(objectBookmarkTable_);
  selectAndFocusTableRow(objectBookmarkTable_, row, 0);
  updateDiagnostics("Info", "Navigation",
                    uiText("Opened matching Object Bookmark for current target",
                           "已打开当前目标匹配的对象书签"));
}


// — Open current sdo target trail link
void MainWindow::openCurrentSdoTargetTrailLink() {
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
  if (!sdoTargetTable_) {
    return;
  }

  const int position = selectedPosition();
  const QString index = sdoIndex_ ? sdoIndex_->text().trimmed() : QString();
  const QString subIndex =
      sdoSubIndex_ ? sdoSubIndex_->text().trimmed() : QString();
  const QString type = sdoType_ ? sdoType_->currentText().trimmed() : QString();
  const QString readValue = sdoValue_ ? sdoValue_->text().trimmed() : QString();
  const QString writeValue =
      sdoWriteValue_ ? sdoWriteValue_->text().trimmed() : QString();
  // Normalize hex address for consistent comparison
  const QString normalizedIndex = normalizeHexText(index, 4);
  // Normalize hex address for consistent comparison
  const QString normalizedSubIndex = normalizeHexText(subIndex, 2);
  const bool hasCompleteTarget = position >= 0 && !normalizedIndex.isEmpty() &&
                                 !normalizedSubIndex.isEmpty();

  int dictionaryRow = -1;
  SdoDictionaryRow dictionary;
  if (hasCompleteTarget && sdoTable_ && loadedSdoPosition_ == position) {
    dictionaryRow = tableRowForObjectIndex(sdoTable_, normalizedIndex,
                                           normalizedSubIndex, 1, 2);
    dictionary = sdoDictionaryRowFromTable(sdoTable_, dictionaryRow);
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
  if (hasCompleteTarget && watchTable_) {
    watchRow = currentSdoWatchRow();
    if (watchRow >= 0) {
      watchValue = tableText(watchTable_, watchRow, 4);
      watchDecoded = tableText(watchTable_, watchRow, 5);
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
  if (hasCompleteTarget && objectBookmarkTable_) {
    ensureObjectBookmarkTable();
    const int primaryBookmarkRow = currentSdoBookmarkRow();
    for (int row = 0; row < objectBookmarkTable_->rowCount(); ++row) {
      if (!tableObjectAddressMatches(objectBookmarkTable_, row, position,
                                     normalizedIndex, normalizedSubIndex, 0, 2,
                                     3)) {
        continue;
      }
      ++bookmarkRows;
      const SdoObjectBookmarkRow bookmark =
          sdoObjectBookmarkRowFromTable(objectBookmarkTable_, row);
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

  const QSignalBlocker blocker(sdoTargetTable_); // prevent recursive signal updates
  sdoTargetTable_->clearContents();
  sdoTargetTable_->setRowCount(rows.size());
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
    }
    sdoTargetTable_->setItem(row, 0, keyItem);
    sdoTargetTable_->setItem(row, 1, valueItem);
    sdoTargetTable_->setItem(row, 2, actionItem);
  }
  sdoTargetTable_->resizeRowsToContents();
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
  if (sdoTargetTable_) {
    const int row = sdoTargetTable_->currentRow();
    if (row >= 0 && row < sdoTargetTable_->rowCount()) {
      field = tableText(sdoTargetTable_, row, 0);
      action = tableText(sdoTargetTable_, row, 2);
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
  if (sdoTargetTable_) {
    const int row = sdoTargetTable_->currentRow();
    if (row >= 0 && row < sdoTargetTable_->rowCount()) {
      field = tableText(sdoTargetTable_, row, 0);
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
  if (!sdoInspectorLabel_) {
    return;
  }

  const int position = selectedPosition();
  const QString index = sdoIndex_ ? sdoIndex_->text().trimmed() : QString();
  const QString subIndex =
      sdoSubIndex_ ? sdoSubIndex_->text().trimmed() : QString();
  const QString type = sdoType_ ? sdoType_->currentText().trimmed() : QString();
  const QString readValue = sdoValue_ ? sdoValue_->text().trimmed() : QString();
  const QString writeValue =
      sdoWriteValue_ ? sdoWriteValue_->text().trimmed() : QString();

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

  sdoInspectorLabel_->setText(parts.join("  -  "));
  sdoInspectorLabel_->setToolTip(parts.join("\n"));
  sdoInspectorLabel_->setProperty("state", state);
  repolish(sdoInspectorLabel_); // force QSS re-evaluation after property change
  updateSdoTargetPanel(source, detail, status, problems);
}


// — Create the SDO target trail table columns if not yet initialized
void MainWindow::ensureSdoTargetTrailTable() {
  if (!sdoTargetTrailTable_) {
    return;
  }
  if (sdoTargetTrailTable_->columnCount() != 9) {
    sdoTargetTrailTable_->setColumnCount(9);
  }
  sdoTargetTrailTable_->setHorizontalHeaderLabels(
      {uiText("Time", "时间"), uiText("Slave", "从站"), uiText("Index", "索引"),
       uiText("Sub", "子项"), uiText("Type", "类型"), uiText("Source", "来源"),
       uiText("Value", "值"), uiText("Write", "写入值"),
       uiText("Detail", "详情")});
  sdoTargetTrailTable_->horizontalHeader()->setSectionResizeMode(
      QHeaderView::ResizeToContents);
  sdoTargetTrailTable_->horizontalHeader()->setStretchLastSection(true);
}


// — Refresh the SDO target trail detail strip for the focused row
void MainWindow::updateSdoTargetTrailRowDetail() {
  if (!sdoTargetTrailDetailLabel_) {
    return;
  }
  const SdoTargetTrailDetailTexts texts = sdoTargetTrailDetailTexts();
  // Lambda to push UI state changes to the label widget
  auto applyState = [this](const SdoTargetTrailDetailUiState &state) {
    sdoTargetTrailDetailLabel_->setText(state.text);
    sdoTargetTrailDetailLabel_->setProperty("severity", state.severityKey);
    sdoTargetTrailDetailLabel_->setToolTip(state.tooltip);
    repolish(sdoTargetTrailDetailLabel_); // force QSS re-evaluation after property change
  };

  if (!sdoTargetTrailTable_) {
    applyState(sdoTargetTrailDetailUnavailableState(texts));
    return;
  }

  const int row = sdoTargetTrailTable_->currentRow();
  if (row < 0 || row >= sdoTargetTrailTable_->rowCount() ||
      sdoTargetTrailTable_->isRowHidden(row)) {
    applyState(sdoTargetTrailDetailNoSelectionState(texts));
    return;
  }

  const SdoTargetTrailRow trail =
      sdoTargetTrailRowFromTable(sdoTargetTrailTable_, row);
  applyState(buildSdoTargetTrailDetailUiState(
      trail, sdoTargetTrailRowCanCreateStartup(row), texts));
}


// — Store the current SDO target address in the target trail history
void MainWindow::rememberCurrentSdoTarget(const QString &source,
                                          const QString &detail) {
  if (!sdoTargetTrailTable_) {
    return;
  }
  const int position = selectedPosition();
  const QString index =
      // Normalize hex address for consistent comparison
      normalizeHexText(sdoIndex_ ? sdoIndex_->text().trimmed() : QString(), 4);
  // Normalize hex address for consistent comparison
  const QString subIndex = normalizeHexText(
      sdoSubIndex_ ? sdoSubIndex_->text().trimmed() : QString(), 2);
  if (position < 0 || index.isEmpty() || subIndex.isEmpty()) {
    return;
  }
  ensureSdoTargetTrailTable();

  const QString type = sdoType_ ? sdoType_->currentText().trimmed() : QString();
  const QString value = sdoValue_ ? sdoValue_->text().trimmed() : QString();
  const QString writeValue =
      sdoWriteValue_ ? sdoWriteValue_->text().trimmed() : QString();
  const QString sourceText = source.trimmed().isEmpty()
                                 ? uiText("Manual fields", "手动字段")
                                 : source.trimmed();
  const QString detailText = detail.trimmed();
  const QString key = sdoTargetTrailRowKey(position, index, subIndex, type,
                                           sourceText, detailText);

  for (int row = 0; row < sdoTargetTrailTable_->rowCount(); ++row) {
    if (sdoTargetTrailRowKeyFromTable(sdoTargetTrailTable_, row) != key) {
      continue;
    }
    sdoTargetTrailTable_->removeRow(row);
    break;
  }
  rememberedSdoTargetTrailKeys_.insert(key);

  sdoTargetTrailTable_->insertRow(0);
  const QStringList values = {
      QDateTime::currentDateTime().toString("HH:mm:ss"),
      QString::number(position),
      index,
      subIndex,
      type,
      sourceText,
      value,
      writeValue,
      detailText,
  };
  for (int column = 0; column < values.size(); ++column) {
    auto *item = new QTableWidgetItem(values.at(column));
    item->setToolTip(values.at(column));
    if (column == 5) {
      item->setForeground(settings_.theme == "Light" ? QColor("#1d4ed8")
                                                     : QColor("#93c5fd"));
    }
    sdoTargetTrailTable_->setItem(0, column, item);
  }
  while (sdoTargetTrailTable_->rowCount() > 40) {
    const int lastRow = sdoTargetTrailTable_->rowCount() - 1;
    rememberedSdoTargetTrailKeys_.remove(
        sdoTargetTrailRowKeyFromTable(sdoTargetTrailTable_, lastRow));
    sdoTargetTrailTable_->removeRow(lastRow);
  }
  sdoTargetTrailTable_->resizeColumnsToContents(); // auto-fit column widths
  updateSdoTargetTrailRowDetail();
  updateActionAvailability();
}


// — Check whether prepare sdo target trail row
bool MainWindow::prepareSdoTargetTrailRow(int row, bool reportRestoreSuccess) {
  if (!sdoTargetTrailTable_ || row < 0 ||
      row >= sdoTargetTrailTable_->rowCount()) {
    updateDiagnostics(
        "Info", "SDO Target Trail",
        uiText("Select a target trail row first", "请先选择一条目标轨迹"));
    return false;
  }

  const SdoTargetTrailRow trail =
      sdoTargetTrailRowFromTable(sdoTargetTrailTable_, row);
  if (!sdoTargetTrailRowHasTarget(trail)) {
    return false;
  }

  if (!selectSlaveForLocalEvidence(trail.position)) {
    updateDiagnostics("Warning", "SDO Target Trail",
                      uiText("Target slave #%1 is not in the current topology",
                             "目标从站 #%1 不在当前拓扑中")
                          .arg(trail.position));
    return false;
  }

  {
    const QSignalBlocker indexBlocker(sdoIndex_); // prevent recursive signal updates
    const QSignalBlocker subIndexBlocker(sdoSubIndex_); // prevent recursive signal updates
    const QSignalBlocker typeBlocker(sdoType_); // prevent recursive signal updates
    const QSignalBlocker valueBlocker(sdoValue_); // prevent recursive signal updates
    const QSignalBlocker writeBlocker(sdoWriteValue_); // prevent recursive signal updates
    if (sdoIndex_) {
      sdoIndex_->setText(trail.index);
    }
    if (sdoSubIndex_) {
      sdoSubIndex_->setText(trail.subIndex);
    }
    if (sdoType_ && !trail.type.isEmpty()) {
      const QString normalized = trail.type.toLower().replace(' ', "_");
      const int typeIndex =
          sdoType_->findText(normalized, Qt::MatchFixedString);
      sdoType_->setCurrentIndex(typeIndex >= 0 ? typeIndex : 0);
    }
    if (sdoValue_) {
      sdoValue_->setText(trail.value);
      sdoValue_->setPlaceholderText(
          trail.value.isEmpty()
              ? uiText("No trail value", "轨迹暂无值")
              : uiText("Value from target trail", "来自目标轨迹的值"));
    }
  }

  selectedSdoWritable_ = true;
  const int dictionaryRow = currentSdoDictionaryRow();
  if (dictionaryRow >= 0) {
    selectedSdoWritable_ = sdoDictionaryRowIsWritable(
        sdoDictionaryRowFromTable(sdoTable_, dictionaryRow));
  } else {
    const int bookmarkRow = currentSdoBookmarkRow();
    if (bookmarkRow >= 0) {
      selectedSdoWritable_ = !sdoObjectAccessIsReadOnly(
          sdoObjectBookmarkRowFromTable(objectBookmarkTable_, bookmarkRow)
              .access,
          uiText("只读", "只读"));
    }
  }
  if (sdoWriteValue_) {
    sdoWriteValue_->setEnabled(selectedSdoWritable_);
    sdoWriteValue_->setText(selectedSdoWritable_ ? trail.writeValue
                                                 : QString());
    sdoWriteValue_->setPlaceholderText(
        selectedSdoWritable_ ? (trail.writeValue.isEmpty()
                                    ? uiText("Value to write", "写入值")
                                    : uiText("Write value from target trail",
                                             "来自目标轨迹的写入值"))
                             : uiText("Read-only object", "只读对象"));
  }

  sdoTargetTrailTable_->selectRow(row);
  updateSdoInspector(uiText("SDO Target Trail", "SDO 目标轨迹"),
                     trail.source.isEmpty()
                         ? trail.detail
                         : QString("%1 | %2").arg(trail.source, trail.detail));
  if (reportRestoreSuccess) {
    updateDiagnostics(
        "Info", "SDO Target Trail",
        uiText("Restored local SDO target #%1 %2:%3 without bus "
               "access",
               "已本地恢复 SDO 目标 #%1 %2:%3，未访问总线")
            .arg(trail.position)
            .arg(sdoIndex_ ? sdoIndex_->text() : trail.index,
                 sdoSubIndex_ ? sdoSubIndex_->text() : trail.subIndex));
  }
  updateActionAvailability();
  return true;
}


// — Return the sdo target trail row startup value
QString MainWindow::sdoTargetTrailRowStartupValue(int row) const {
  return sdoTargetTrailStartupValueFromTable(sdoTargetTrailTable_, row);
}


// — Check whether sdo target trail row can create startup
bool MainWindow::sdoTargetTrailRowCanCreateStartup(int row) const {
  const SdoTargetTrailRow trail =
      sdoTargetTrailRowFromTable(sdoTargetTrailTable_, row);
  if (!sdoTargetTrailRowHasTarget(trail) ||
      ::sdoTargetTrailRowStartupValue(trail).isEmpty()) {
    return false;
  }

  if (sdoTable_ && loadedSdoPosition_ == trail.position) {
    for (int dictionaryRow = 0; dictionaryRow < sdoTable_->rowCount();
         ++dictionaryRow) {
      if (tableObjectIndexMatches(sdoTable_, dictionaryRow, trail.index,
                                  trail.subIndex, 1, 2)) {
        return sdoDictionaryRowIsWritable(
            sdoDictionaryRowFromTable(sdoTable_, dictionaryRow));
      }
    }
  }

  if (objectBookmarkTable_) {
    for (int bookmarkRow = 0; bookmarkRow < objectBookmarkTable_->rowCount();
         ++bookmarkRow) {
      if (!tableObjectAddressMatches(objectBookmarkTable_, bookmarkRow,
                                     trail.position, trail.index,
                                     trail.subIndex, 0, 2, 3)) {
        continue;
      }
      return !sdoObjectAccessIsReadOnly(
          sdoObjectBookmarkRowFromTable(objectBookmarkTable_, bookmarkRow)
              .access,
          uiText("只读", "只读"));
    }
  }

  return true;
}


// — Restore the SDO target panel from a historical trail entry
void MainWindow::restoreSdoTargetTrailRow(int row) {
  prepareSdoTargetTrailRow(row, true);
}


// — Add sdo target trail row to watch
void MainWindow::addSdoTargetTrailRowToWatch() {
  const int row =
      sdoTargetTrailTable_ ? sdoTargetTrailTable_->currentRow() : -1;
  if (!prepareSdoTargetTrailRow(row, false)) {
    return;
  }
  const QString index = sdoIndex_ ? sdoIndex_->text().trimmed() : QString();
  const QString subIndex =
      sdoSubIndex_ ? sdoSubIndex_->text().trimmed() : QString();
  const QString type = sdoType_ ? sdoType_->currentText().trimmed() : QString();
  const QString value = sdoValue_ ? sdoValue_->text().trimmed() : QString();
  addCurrentSdoToWatch(false);
  if (watchTable_ && watchTable_->currentRow() >= 0) {
    const int watchRow = watchTable_->currentRow();
    if (!value.isEmpty()) {
      watchTable_->setItem(watchRow, 4, new QTableWidgetItem(value));
      watchValues_
          [QString("%1|%2|%3").arg(selectedPosition()).arg(index, subIndex)] =
              value;
    }
    watchTable_->setItem(
        watchRow, 5,
        new QTableWidgetItem(decodeWatchValue(
            index, subIndex, type,
            value.isEmpty() ? tableText(watchTable_, watchRow, 4) : value,
            uiText("Target Trail", "目标轨迹"))));
    watchTable_->setItem(watchRow, 6, new QTableWidgetItem(type));
    watchTable_->setItem(
        watchRow, 7, new QTableWidgetItem(uiText("Target Trail", "目标轨迹")));
    updateWatchBaselineDelta(watchRow);
    updateWatchStartupDelta(watchRow);
  }
  if (watchTable_) {
    watchTable_->resizeColumnsToContents(); // auto-fit column widths
  }
  updateDiagnostics("Info", "SDO Target Trail",
                    uiText("Added selected target trail row to Watch without "
                           "immediate reads",
                           "已将所选目标轨迹行加入 Watch，未立即读取"));
  activateWorkspaceTab(watchTabIndex_);
}


// — Bookmark sdo target trail row
void MainWindow::bookmarkSdoTargetTrailRow() {
  const int row =
      sdoTargetTrailTable_ ? sdoTargetTrailTable_->currentRow() : -1;
  if (!prepareSdoTargetTrailRow(row, false)) {
    return;
  }
  const QString index = sdoIndex_ ? sdoIndex_->text().trimmed() : QString();
  const QString subIndex =
      sdoSubIndex_ ? sdoSubIndex_->text().trimmed() : QString();
  const QString type = sdoType_ ? sdoType_->currentText().trimmed() : QString();
  const QString value = sdoValue_ ? sdoValue_->text().trimmed() : QString();
  const SdoTargetTrailRow trail =
      sdoTargetTrailRowFromTable(sdoTargetTrailTable_, row);
  addObjectBookmark(
      selectedPosition(), index, subIndex,
      selectedSdoWritable_ ? uiText("rw", "读写") : uiText("ro", "只读"), type,
      QString(), trail.detail.isEmpty() ? trail.source : trail.detail, value,
      uiText("SDO Target Trail", "SDO 目标轨迹"));
  updateDiagnostics("Info", "SDO Target Trail",
                    uiText("Bookmarked selected target trail row without bus "
                           "access",
                           "已收藏所选目标轨迹行，未访问总线"));
}


// — Add sdo target trail row to startup
void MainWindow::addSdoTargetTrailRowToStartup() {
  const int row =
      sdoTargetTrailTable_ ? sdoTargetTrailTable_->currentRow() : -1;
  if (!sdoTargetTrailRowCanCreateStartup(row)) {
    updateDiagnostics(
        "Warning", "SDO Target Trail",
        uiText("Selected target trail row has no writable value for Startup "
               "SDO creation",
               "所选目标轨迹行没有可用于创建 Startup SDO 的可写值"));
    return;
  }
  const QString value = sdoTargetTrailRowStartupValue(row);
  if (!prepareSdoTargetTrailRow(row, false)) {
    return;
  }
  if (!selectedSdoWritable_) {
    updateDiagnostics("Warning", "SDO Target Trail",
                      uiText("Selected target trail row resolves to a "
                             "read-only object",
                             "所选目标轨迹行被识别为只读对象"));
    return;
  }
  if (sdoWriteValue_) {
    sdoWriteValue_->setEnabled(true);
    sdoWriteValue_->setText(value);
    sdoWriteValue_->setPlaceholderText(
        uiText("Value from target trail", "来自目标轨迹的值"));
  }
  addStartupSdo();
  if (startupSdoTable_ && startupSdoTable_->currentRow() >= 0) {
    const int startupRow = startupSdoTable_->currentRow();
    if (startupSdoTable_->columnCount() > 5) {
      startupSdoTable_->setItem(
          startupRow, 5,
          new QTableWidgetItem(uiText("From Target Trail", "来自目标轨迹")));
    }
    if (startupSdoTable_->columnCount() > 6) {
      startupSdoTable_->setItem(
          startupRow, 6,
          new QTableWidgetItem(uiText("Created from SDO Target Trail row %1",
                                      "由 SDO 目标轨迹第 %1 行创建")
                                   .arg(row + 1)));
    }
  }
  updateDiagnostics("Info", "Startup SDO",
                    uiText("Created a local Startup SDO candidate from the "
                           "selected target trail row",
                           "已从所选目标轨迹行创建本地 Startup SDO 候选"));
  activateWorkspaceTab(startupSdoTabIndex_);
}


// — Remove selected sdo target trail rows
void MainWindow::removeSelectedSdoTargetTrailRows() {
  if (!sdoTargetTrailTable_ || !sdoTargetTrailTable_->selectionModel()) {
    return;
  }
  QVector<int> rows = selectedTableRows(sdoTargetTrailTable_);
  std::sort(rows.begin(), rows.end(), std::greater<int>());
  for (const int row : rows) {
    if (row >= 0 && row < sdoTargetTrailTable_->rowCount()) {
      sdoTargetTrailTable_->removeRow(row);
    }
  }
  rememberedSdoTargetTrailKeys_ =
      sdoTargetTrailKeysFromTable(sdoTargetTrailTable_);
  updateDiagnostics("Info", "SDO Target Trail",
                    uiText("Removed selected local target trail row(s)",
                           "已移除所选本地目标轨迹行"));
  updateSdoTargetTrailRowDetail();
  updateActionAvailability();
}


// — Clear sdo target trail
void MainWindow::clearSdoTargetTrail() {
  if (!sdoTargetTrailTable_) {
    return;
  }
  sdoTargetTrailTable_->clearContents();
  sdoTargetTrailTable_->setRowCount(0);
  rememberedSdoTargetTrailKeys_.clear();
  ensureSdoTargetTrailTable();
  updateDiagnostics(
      "Info", "SDO Target Trail",
      uiText("Cleared local SDO target trail", "已清空本地 SDO 目标轨迹"));
  updateSdoTargetTrailRowDetail();
  updateActionAvailability();
}


// — Update sdo table evidence
void MainWindow::updateSdoTableEvidence(int position, const QString &index,
                                        const QString &subIndex,
                                        const QString &value,
                                        const QString &status,
                                        const QString &detail) {
  if (position < 0 || index.trimmed().isEmpty() ||
      subIndex.trimmed().isEmpty()) {
    return;
  }

  // Normalize hex address for consistent comparison
  const QString normalizedIndex = normalizeHexText(index, 4);
  // Normalize hex address for consistent comparison
  const QString normalizedSubIndex = normalizeHexText(subIndex, 2);
  const QString key = sdoEvidenceKey(position, index, subIndex);
  const QString time = QDateTime::currentDateTime().toString("HH:mm:ss");
  sdoEvidence_.insert(key, {value, status, detail, time});

  if (!sdoTable_ || selectedPosition() != position) {
    return;
  }

  const QColor statusColor =
      status == uiText("Complete", "完成") || status == uiText("OK", "成功") ||
              status == uiText("Write OK", "写入完成")
          ? QColor("#22c55e")
          : (status == uiText("Failed", "失败") ? QColor("#ef4444")
                                                : QColor("#f59e0b"));
  const QColor valueBackground =
      settings_.theme == "Light" ? QColor("#eef2ff") : QColor("#172036");

  for (int row = 0; row < sdoTable_->rowCount(); ++row) {
    if (!tableObjectIndexMatches(sdoTable_, row, normalizedIndex,
                                 normalizedSubIndex, 1, 2)) {
      continue;
    }

    auto ensureItem = [this, row](int column) {
      auto *item = sdoTable_->item(row, column);
      if (!item) {
        item = new QTableWidgetItem;
        sdoTable_->setItem(row, column, item);
      }
      return item;
    };
    auto *valueItem = ensureItem(7);
    auto *statusItem = ensureItem(8);
    valueItem->setText(value);
    valueItem->setToolTip(
        detail.isEmpty() ? time : QString("%1\n%2").arg(time, detail));
    valueItem->setBackground(valueBackground);
    statusItem->setText(QString("%1  %2").arg(status, time));
    statusItem->setToolTip(detail);
    statusItem->setForeground(statusColor);
    break;
  }

  if (sdoTable_->currentRow() >= 0) {
    updateSdoInspector(uiText("OD evidence", "OD 证据"), detail);
  }
  updateActionAvailability();
}


// — Use read sdo value for write
void MainWindow::useReadSdoValueForWrite() {
  if (!sdoValue_ || !sdoWriteValue_ || !selectedSdoWritable_) {
    return;
  }
  const QString value = sdoValue_->text().trimmed();
  if (value.isEmpty()) {
    return;
  }
  sdoWriteValue_->setEnabled(true);
  sdoWriteValue_->setText(value);
  sdoWriteValue_->setPlaceholderText(
      uiText("Value copied from read-back", "已从读回值复制"));
  updateDiagnostics("Info", "SDO",
                    QString("Copied read value into write field %1:%2 = %3")
                        .arg(sdoIndex_ ? sdoIndex_->text() : QString(),
                             sdoSubIndex_ ? sdoSubIndex_->text() : QString(),
                             value));
  updateSdoInspector(
      uiText("Read value", "读回值"),
      uiText("Read value copied to write field", "读回值已复制到写入框"));
  updateActionAvailability();
}


// — Use preferred sdo evidence for write
void MainWindow::usePreferredSdoEvidenceForWrite() {
  if (!sdoWriteValue_ || !selectedSdoWritable_) {
    return;
  }
  QString source;
  const QString value = currentSdoPreferredEvidenceValue(&source);
  if (value.isEmpty()) {
    updateDiagnostics("Info", "SDO",
                      uiText("No local SDO evidence value is available for the "
                             "current target",
                             "当前目标没有可用的本地 SDO 证据值"));
    return;
  }
  sdoWriteValue_->setEnabled(true);
  sdoWriteValue_->setText(value);
  sdoWriteValue_->setPlaceholderText(
      uiText("Value copied from %1", "已从 %1 复制数值").arg(source));
  updateDiagnostics("Info", "SDO",
                    uiText("Copied %1 evidence into write field %2:%3 = %4",
                           "已把 %1 证据复制到写入框 %2:%3 = %4")
                        .arg(source, sdoIndex_ ? sdoIndex_->text() : QString(),
                             sdoSubIndex_ ? sdoSubIndex_->text() : QString(),
                             value));
  updateSdoInspector(
      uiText("Local evidence", "本地证据"),
      uiText("%1 copied to write field", "已把 %1 复制到写入框").arg(source));
  updateActionAvailability();
}


// — Pick sdo evidence for write
void MainWindow::pickSdoEvidenceForWrite() {
  if (!sdoWriteValue_ || !selectedSdoWritable_) {
    return;
  }
  const auto candidates = currentSdoEvidenceCandidates();
  if (candidates.isEmpty()) {
    updateDiagnostics("Info", "SDO",
                      uiText("No local SDO evidence value is available for the "
                             "current target",
                             "当前目标没有可用的本地 SDO 证据值"));
    return;
  }

  QDialog dialog(this);
  dialog.setWindowTitle(uiText("Choose SDO Evidence", "选择 SDO 证据"));
  auto *layout = new QVBoxLayout(&dialog);
  auto *summary = new QLabel(
      uiText("Choose one local evidence value for the write field. This does "
             "not read or write the bus.",
             "选择一个本地证据值填入写入框；不会读取或写入总线。"));
  summary->setWordWrap(true);
  layout->addWidget(summary);

  auto *list = new QListWidget;
  list->setObjectName("sdoEvidenceCandidateList");
  for (int i = 0; i < candidates.size(); ++i) {
    const auto &candidate = candidates.at(i);
    auto *item = new QListWidgetItem(
        QString("%1  =  %2").arg(candidate.first, candidate.second));
    item->setData(Qt::UserRole, candidate.second);
    item->setData(Qt::UserRole + 1, candidate.first);
    if (i == 0) {
      item->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
    }
    list->addItem(item);
  }
  list->setCurrentRow(0);
  layout->addWidget(list);

  auto *buttons =
      new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  buttons->button(QDialogButtonBox::Ok)
      ->setText(uiText("Use Evidence", "使用证据"));
  buttons->button(QDialogButtonBox::Cancel)->setText(uiText("Cancel", "取消"));
  layout->addWidget(buttons);
  connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept); // wire signal to slot
  connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject); // wire signal to slot
  connect(list, &QListWidget::itemDoubleClicked, &dialog, &QDialog::accept); // wire signal to slot

  if (dialog.exec() != QDialog::Accepted || !list->currentItem()) {
    return;
  }

  const QString value = list->currentItem()->data(Qt::UserRole).toString();
  const QString source = list->currentItem()->data(Qt::UserRole + 1).toString();
  if (value.trimmed().isEmpty()) {
    return;
  }
  sdoWriteValue_->setEnabled(true);
  sdoWriteValue_->setText(value.trimmed());
  sdoWriteValue_->setPlaceholderText(
      uiText("Value chosen from %1", "已从 %1 选择数值").arg(source));
  updateDiagnostics("Info", "SDO",
                    uiText("Selected %1 evidence for write field %2:%3 = %4",
                           "已选择 %1 证据作为写入值 %2:%3 = %4")
                        .arg(source, sdoIndex_ ? sdoIndex_->text() : QString(),
                             sdoSubIndex_ ? sdoSubIndex_->text() : QString(),
                             value.trimmed()));
  updateSdoInspector(
      uiText("Local evidence", "本地证据"),
      uiText("%1 chosen for write field", "已选择 %1 作为写入值").arg(source));
  updateActionAvailability();
}


// — Write current sdo
void MainWindow::writeCurrentSdo() {
  if (!selectedSdoWritable_) {
    updateDiagnostics("Warning", "SDO",
                      uiText("Write blocked: selected object is read-only",
                             "写入已阻止：所选对象为只读"));
    updateActionAvailability();
    return;
  }
  if (selectedPosition() < 0) {
    return;
  }

  QStringList validationErrors;
  QStringList validationWarnings;
  validateSdoAddressAndValue(sdoIndex_->text(), sdoSubIndex_->text(),
                             sdoWriteValue_->text(), sdoType_->currentText(),
                             &validationErrors, &validationWarnings);
  if (!validationErrors.isEmpty()) {
    updateDiagnostics(
        "Error", "SDO",
        uiText("Write blocked by validation: %1", "写入已被校验阻止：%1")
            .arg(validationErrors.join("; ")));
    QMessageBox::warning(this, uiText("SDO Validation Failed", "SDO 校验失败"),
                         validationErrors.join("\n"));
    return;
  }

  QStringList details = {
      uiText("Master: %1", "主站：%1").arg(activeMasterName()),
      uiText("Slave: #%1", "从站：#%1").arg(selectedPosition()),
      uiText("Object: %1:%2", "对象：%1:%2")
          .arg(sdoIndex_->text(), sdoSubIndex_->text()),
      uiText("Type: %1", "类型：%1")
          .arg(sdoType_->currentText().isEmpty() ? uiText("default", "默认")
                                                 : sdoType_->currentText()),
      uiText("Value: %1", "值：%1").arg(sdoWriteValue_->text()),
      uiText("SDO writes can change persistent parameters or output behavior.",
             "SDO 写入可能改变持久参数或输出行为。"),
  };
  details << sdoWriteImpactDetails(selectedPosition(), sdoIndex_->text(),
                                   sdoSubIndex_->text(), sdoWriteValue_->text(),
                                   sdoType_->currentText());
  if (!validationWarnings.isEmpty()) {
    details << uiText("Validation warning: %1", "校验警告：%1")
                   .arg(validationWarnings.join("; "));
  }
  // Safety gate: require explicit confirmation before bus write
  if (!confirmDangerousOperation(
          uiText("Confirm SDO Write", "确认 SDO 写入"),
          uiText("This operation writes a value to the selected slave object.",
                 "此操作会向选中从站对象写入数值。"),
          details, uiText("Write SDO", "写入 SDO"))) {
    return;
  }

  const QString writeKey = sdoEvidenceKey(selectedPosition(), sdoIndex_->text(),
                                          sdoSubIndex_->text());
  pendingSdoWrites_.insert(writeKey,
                           {QString::number(selectedPosition()),
                            sdoIndex_->text(), sdoSubIndex_->text(),
                            sdoType_->currentText(), sdoWriteValue_->text()});
  updateSdoTableEvidence(
      selectedPosition(), sdoIndex_->text(), sdoSubIndex_->text(),
      sdoWriteValue_->text(), uiText("Write Pending", "写入待确认"),
      uiText("Waiting for runtime download completion; read-back verification "
             "will run automatically.",
             "等待运行时写入完成；随后会自动读回校验。"));
  appendSdoHistory(uiText("Write", "写入"), selectedPosition(),
                   sdoIndex_->text(), sdoSubIndex_->text(),
                   sdoType_->currentText(), sdoWriteValue_->text(),
                   uiText("Requested", "已请求"),
                   uiText("Manual SDO write; automatic read-back verification "
                          "will follow",
                          "手动 SDO 写入；随后自动读回校验"));
  client_.download(selectedPosition(), sdoIndex_->text(), sdoSubIndex_->text(),
                   sdoWriteValue_->text(), sdoType_->currentText());
  updateSdoInspector(
      uiText("SDO write requested", "SDO 写入已请求"),
      uiText("Automatic read-back verification will run after download "
             "completion",
             "写入完成后会自动读回校验"));
  updateDiagnostics("Info", "SDO",
                    QString("Download requested %1:%2 = %3; automatic "
                            "read-back verification will follow")
                        .arg(sdoIndex_->text(), sdoSubIndex_->text(),
                             sdoWriteValue_->text()));
}


// — Prepare cia 402 controlword
void MainWindow::prepareCia402Controlword(const QString &label,
                                          const QString &value) {
  if (selectedPosition() < 0) {
    return;
  }
  activateObjectDictionaryPaneFor(sdoTable_);
  {
    const QSignalBlocker indexBlocker(sdoIndex_); // prevent recursive signal updates
    const QSignalBlocker subIndexBlocker(sdoSubIndex_); // prevent recursive signal updates
    const QSignalBlocker typeBlocker(sdoType_); // prevent recursive signal updates
    if (sdoIndex_) {
      sdoIndex_->setText("0x6040");
    }
    if (sdoSubIndex_) {
      sdoSubIndex_->setText("0x00");
    }
    if (sdoType_) {
      sdoType_->setCurrentText("uint16");
    }
  }
  selectedSdoWritable_ = true;
  if (sdoWriteValue_) {
    sdoWriteValue_->setEnabled(true);
    sdoWriteValue_->setText(value);
    sdoWriteValue_->setPlaceholderText(
        uiText("CiA 402 controlword", "CiA 402 控制字"));
  }
  updateSdoInspector(
      uiText("CiA 402", "CiA 402"),
      uiText("Prepared controlword %1", "已准备控制字 %1").arg(label));
  rememberCurrentSdoTarget(
      uiText("CiA 402", "CiA 402"),
      uiText("Prepared controlword %1", "已准备控制字 %1").arg(label));
  updateDiagnostics(
      "Info", "SDO",
      QString("Prepared CiA 402 controlword %1 for slave #%2: 0x6040:0x00 = %3")
          .arg(label)
          .arg(selectedPosition())
          .arg(value));
  updateActionAvailability();
  writeCurrentSdo();
}


// — Check whether recommended cia 402 controlword
bool MainWindow::recommendedCia402Controlword(QString *label, QString *value,
                                              QString *reason) const {
  const int position = selectedPosition();
  if (position < 0 || !watchTable_) {
    return false;
  }

  const Cia402ControlwordRecommendation recommendation =
      selectedDriveControlwordRecommendation(watchStartupWatchRows(watchTable_),
                                             position);
  if (recommendation.label.isEmpty() || recommendation.value.isEmpty()) {
    return false;
  }

  if (label) {
    *label = recommendation.label;
  }
  if (value) {
    *value = recommendation.value;
  }
  if (reason) {
    *reason = recommendation.reason;
  }
  return true;
}


// — Check whether validate sdo address and value
bool MainWindow::validateSdoAddressAndValue(
    const QString &index, const QString &subIndex, const QString &value,
    const QString &type, QStringList *errors, QStringList *warnings) const {
  auto addError = [errors](const QString &message) {
    if (errors) {
      errors->append(message);
    }
  };
  auto addWarning = [warnings](const QString &message) {
    if (warnings) {
      warnings->append(message);
    }
  };
  // Parse a numeric string to double for delta comparison
  auto parseNumber = [](QString text, int *baseOut = nullptr) {
    text = text.trimmed();
    int base = 10;
    if (text.startsWith("0x", Qt::CaseInsensitive)) {
      base = 16;
      text = text.mid(2);
    }
    bool ok = false;
    const quint64 result = text.toULongLong(&ok, base);
    if (baseOut) {
      *baseOut = base;
    }
    return QPair<bool, quint64>{ok, result};
  };

  const auto indexParsed = parseNumber(index);
  if (index.trimmed().isEmpty()) {
    addError(uiText("empty index", "索引为空"));
  } else if (!indexParsed.first || indexParsed.second > 0xffff) {
    addError(uiText("index must be 0x0000..0xffff or decimal 0..65535",
                    "索引必须是 0x0000..0xffff 或十进制 0..65535"));
  }

  const auto subParsed = parseNumber(subIndex);
  if (subIndex.trimmed().isEmpty()) {
    addError(uiText("empty subindex", "子项为空"));
  } else if (!subParsed.first || subParsed.second > 0xff) {
    addError(uiText("subindex must be 0x00..0xff or decimal 0..255",
                    "子项必须是 0x00..0xff 或十进制 0..255"));
  }

  const QString trimmedValue = value.trimmed();
  if (trimmedValue.isEmpty()) {
    addError(uiText("empty value", "写入值为空"));
  }

  static const QSet<QString> knownTypes = {
      "",      "bool",   "int8",   "int16",       "int32",
      "int64", "uint8",  "uint16", "uint32",      "uint64",
      "float", "double", "string", "octet_string"};
  const QString normalizedType = type.trimmed().toLower().replace(' ', "_");
  if (!knownTypes.contains(normalizedType)) {
    addWarning(uiText("unknown type %1", "未知类型 %1").arg(type));
  }
  if (trimmedValue.isEmpty()) {
    return errors ? errors->isEmpty() : true;
  }

  if (normalizedType == "bool") {
    const QString normalizedValue = trimmedValue.toLower();
    if (!QStringList{"0", "1", "true", "false", "yes", "no"}.contains(
            normalizedValue)) {
      addError(uiText("bool value must be 0/1/true/false",
                      "bool 值必须是 0/1/true/false"));
    }
  } else if (normalizedType.startsWith("uint")) {
    const auto parsed = parseNumber(trimmedValue);
    if (!parsed.first) {
      addError(uiText("unsigned integer value is not numeric",
                      "无符号整数值不是有效数字"));
    } else {
      bool bitsOk = false;
      const int bits = normalizedType.mid(4).toInt(&bitsOk);
      if (bitsOk && bits > 0 && bits < 64) {
        const quint64 maxValue = (quint64{1} << bits) - 1;
        if (parsed.second > maxValue) {
          addError(uiText("%1 value exceeds range 0..%2", "%1 值超出范围 0..%2")
                       .arg(normalizedType)
                       .arg(maxValue));
        }
      }
    }
  } else if (normalizedType.startsWith("int")) {
    QString numericText = trimmedValue;
    bool negative = false;
    if (numericText.startsWith('-')) {
      negative = true;
      numericText = numericText.mid(1);
    }
    const auto parsed = parseNumber(numericText);
    if (!parsed.first) {
      addError(uiText("signed integer value is not numeric",
                      "有符号整数值不是有效数字"));
    } else {
      bool bitsOk = false;
      const int bits = normalizedType.mid(3).toInt(&bitsOk);
      if (bitsOk && bits > 1 && bits <= 64) {
        const qint64 minValue = bits == 64 ? std::numeric_limits<qint64>::min()
                                           : -(qint64{1} << (bits - 1));
        const quint64 positiveLimit = bits == 64
                                          ? quint64{9223372036854775807ULL}
                                          : (quint64{1} << (bits - 1)) - 1;
        if (negative) {
          const quint64 maxMagnitude = bits == 64
                                           ? quint64{9223372036854775808ULL}
                                           : (quint64{1} << (bits - 1));
          if (parsed.second > maxMagnitude) {
            addError(uiText("%1 value is below range minimum %2",
                            "%1 值低于范围下限 %2")
                         .arg(normalizedType)
                         .arg(minValue));
          }
        } else if (parsed.second > positiveLimit) {
          addError(uiText("%1 value exceeds range maximum %2",
                          "%1 值超出范围上限 %2")
                       .arg(normalizedType)
                       .arg(positiveLimit));
        }
      }
      if (negative && numericText.startsWith("0x", Qt::CaseInsensitive)) {
        addWarning(
            uiText("negative hexadecimal values depend on device parsing",
                   "负十六进制值依赖设备解析方式"));
      }
    }
  } else if (normalizedType == "float" || normalizedType == "double") {
    bool ok = false;
    trimmedValue.toDouble(&ok);
    if (!ok) {
      addError(
          uiText("floating-point value is not numeric", "浮点值不是有效数字"));
    }
  } else if (normalizedType.isEmpty()) {
    addWarning(uiText("empty type, runtime will infer or use default behavior",
                      "类型为空，运行时将推断或使用默认行为"));
  }

  return errors ? errors->isEmpty() : true;
}


// — Create the SDO history table columns if not yet initialized
void MainWindow::ensureSdoHistoryTable() {
  if (!sdoHistoryTable_) {
    return;
  }
  if (sdoHistoryTable_->columnCount() != 9) {
    sdoHistoryTable_->setColumnCount(9);
  }
  sdoHistoryTable_->setHorizontalHeaderLabels(
      {uiText("Time", "时间"), uiText("Action", "动作"),
       uiText("Slave", "从站"), uiText("Index", "索引"), uiText("Sub", "子项"),
       uiText("Type", "类型"), uiText("Value", "值"), uiText("Status", "状态"),
       uiText("Detail", "详情")});
}


// — Append sdo history
void MainWindow::appendSdoHistory(const QString &action, int position,
                                  const QString &index, const QString &subIndex,
                                  const QString &type, const QString &value,
                                  const QString &status,
                                  const QString &detail) {
  if (!sdoHistoryTable_) {
    return;
  }
  ensureSdoHistoryTable();
  const int row = sdoHistoryTable_->rowCount();
  sdoHistoryTable_->insertRow(row);
  const QStringList values = {
      QDateTime::currentDateTime().toString("HH:mm:ss"),
      action,
      position >= 0 ? QString::number(position) : QString(),
      index,
      subIndex,
      type,
      value,
      status,
      detail,
  };
  const QColor color = status == uiText("Failed", "失败")
                           ? QColor("#ef4444")
                           : (status == uiText("Complete", "完成") ||
                                      status == uiText("OK", "成功")
                                  ? QColor("#22c55e")
                                  : QColor("#f59e0b"));
  for (int column = 0; column < values.size(); ++column) {
    auto *item = new QTableWidgetItem(values.at(column));
    if (column == 7) {
      item->setForeground(color);
    }
    sdoHistoryTable_->setItem(row, column, item);
  }
  if (sdoHistoryTable_->rowCount() > 300) {
    sdoHistoryTable_->removeRow(0);
  }
  sdoHistoryTable_->resizeColumnsToContents(); // auto-fit column widths
  updateSdoHistoryRowDetail();
}


// — Refresh the SDO history detail strip for the focused row
void MainWindow::updateSdoHistoryRowDetail() {
  if (!sdoHistoryDetailLabel_) {
    return;
  }
  const SdoHistoryRowDetailTexts texts = sdoHistoryRowDetailTexts();
  // Lambda to push UI state changes to the label widget
  auto applyState = [this](const SdoHistoryRowDetailUiState &state) {
    sdoHistoryDetailLabel_->setText(state.text);
    sdoHistoryDetailLabel_->setProperty("severity", state.severityKey);
    sdoHistoryDetailLabel_->setToolTip(state.tooltip);
    repolish(sdoHistoryDetailLabel_); // force QSS re-evaluation after property change
  };

  if (!sdoHistoryTable_) {
    applyState(sdoHistoryRowDetailUnavailableState(texts));
    return;
  }

  const int row = sdoHistoryTable_->currentRow();
  if (row < 0 || row >= sdoHistoryTable_->rowCount() ||
      sdoHistoryTable_->isRowHidden(row)) {
    applyState(sdoHistoryRowDetailNoSelectionState(texts));
    return;
  }

  applyState(buildSdoHistoryRowDetailUiState(
      sdoHistoryRowFromTable(sdoHistoryTable_, row), texts));
}


// — Send an SDO read request to ecatd for the given object address
void MainWindow::requestSdoRead(int position, const QString &index,
                                const QString &subIndex, const QString &source,
                                const QString &type) {
  const QString trimmedIndex = index.trimmed();
  const QString trimmedSub = subIndex.trimmed();
  if (position < 0 || trimmedIndex.isEmpty() || trimmedSub.isEmpty()) {
    return;
  }
  const QString key = sdoEvidenceKey(position, trimmedIndex, trimmedSub);
  const QString readType =
      !type.trimmed().isEmpty()
          ? type.trimmed()
          : (sdoType_ ? sdoType_->currentText().trimmed() : QString());
  pendingSdoReads_.insert(key, source);
  pendingSdoReadTypes_.insert(key, readType);
  appendSdoHistory(uiText("Read", "读取"), position, trimmedIndex, trimmedSub,
                   readType, QString(), uiText("Requested", "已请求"), source);
  client_.upload(position, trimmedIndex, trimmedSub);
}


// — Fill the SDO target panel from the selected OD row
void MainWindow::applySdoSelectionFromDictionary(int row, bool readAfterFill) {
  if (!sdoTable_ || row < 0 || row >= sdoTable_->rowCount()) {
    return;
  }
  if (selectedPosition() < 0 || loadedSdoPosition_ != selectedPosition()) {
    updateDiagnostics("Warning", "SDO",
                      uiText("Ignored Object Dictionary row because it is not "
                             "loaded for the current slave",
                             "已忽略对象字典行：它不属于当前选中从站"));
    return;
  }

  const SdoDictionaryRow dictionary = sdoDictionaryRowFromTable(sdoTable_, row);
  if (!sdoDictionaryRowHasTarget(dictionary)) {
    return;
  }

  const QString access = dictionary.access.toLower();
  const bool writable = sdoDictionaryRowIsWritable(dictionary);

  const QSignalBlocker indexBlocker(sdoIndex_); // prevent recursive signal updates
  const QSignalBlocker subIndexBlocker(sdoSubIndex_); // prevent recursive signal updates
  const QSignalBlocker typeBlocker(sdoType_); // prevent recursive signal updates
  const QSignalBlocker valueBlocker(sdoValue_); // prevent recursive signal updates

  sdoIndex_->setText(dictionary.index);
  sdoSubIndex_->setText(dictionary.subIndex);
  if (sdoValue_) {
    sdoValue_->setText(dictionary.value);
    sdoValue_->setPlaceholderText(
        dictionary.value.isEmpty()
            ? uiText("No read-back for selected object", "选中对象暂无读回值")
            : uiText("Last value from Object Dictionary evidence",
                     "来自对象字典证据的最后值"));
  }

  if (!dictionary.type.isEmpty()) {
    const QString normalized = dictionary.type.toLower().replace(' ', "_");
    const int typeIndex = sdoType_->findText(normalized, Qt::MatchFixedString);
    sdoType_->setCurrentIndex(typeIndex >= 0 ? typeIndex : 0);
  }

  selectedSdoWritable_ = writable;
  sdoWriteValue_->setEnabled(writable);
  if (!writable) {
    sdoWriteValue_->clear();
    sdoWriteValue_->setPlaceholderText(uiText("Read-only object", "只读对象"));
  } else {
    sdoWriteValue_->setPlaceholderText(uiText("Value to write", "写入值"));
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
          .arg(dictionary.index, sdoSubIndex_->text(), access.toUpper(),
               dictionary.bits, dictionary.name,
               writable ? QString()
                        : uiText(" (write disabled)", "（已禁用写入）")));

  if (readAfterFill && client_.isConnected() && selectedPosition() >= 0) {
    requestSdoRead(selectedPosition(), sdoIndex_->text(), sdoSubIndex_->text(),
                   uiText("Object Dictionary", "对象字典"));
  }
  updateActionAvailability();
}


// — Create the object bookmark table columns if not yet initialized
void MainWindow::ensureObjectBookmarkTable() {
  if (!objectBookmarkTable_) {
    return;
  }
  if (objectBookmarkTable_->columnCount() != 10) {
    objectBookmarkTable_->setColumnCount(10);
  }
  objectBookmarkTable_->setHorizontalHeaderLabels(
      {uiText("Slave", "从站"), uiText("Slave Name", "从站名称"),
       uiText("Index", "索引"), uiText("Sub", "子项"), uiText("Access", "权限"),
       uiText("Type", "类型"), uiText("Bits", "位宽"), uiText("Name", "名称"),
       uiText("Last Value", "最后值"), uiText("Source", "来源")});
  objectBookmarkTable_->horizontalHeader()->setSectionResizeMode(
      QHeaderView::ResizeToContents);
  objectBookmarkTable_->horizontalHeader()->setStretchLastSection(true);
}


// — Refresh the object bookmark detail strip for the focused row
void MainWindow::updateObjectBookmarkRowDetail() {
  if (!objectBookmarkDetailLabel_) {
    return;
  }
  const ObjectBookmarkDetailTexts texts = objectBookmarkDetailTexts();
  // Lambda to push UI state changes to the label widget
  auto applyState = [this](const ObjectBookmarkDetailUiState &state) {
    objectBookmarkDetailLabel_->setText(state.text);
    objectBookmarkDetailLabel_->setProperty("severity", state.severityKey);
    objectBookmarkDetailLabel_->setToolTip(state.tooltip);
    repolish(objectBookmarkDetailLabel_); // force QSS re-evaluation after property change
  };

  if (!objectBookmarkTable_) {
    applyState(objectBookmarkDetailUnavailableState(texts));
    return;
  }

  const int row = objectBookmarkTable_->currentRow();
  if (row < 0 || row >= objectBookmarkTable_->rowCount() ||
      objectBookmarkTable_->isRowHidden(row)) {
    applyState(objectBookmarkDetailNoSelectionState(texts));
    return;
  }

  applyState(buildObjectBookmarkDetailUiState(
      sdoObjectBookmarkRowFromTable(objectBookmarkTable_, row), texts));
}


// — Return selected object bookmark rows
QVector<int> MainWindow::selectedObjectBookmarkRows() const {
  return selectedTableRows(objectBookmarkTable_);
}


// — Check whether select object bookmark slave
bool MainWindow::selectObjectBookmarkSlave(int position) {
  if (position < 0) {
    return false;
  }
  if (!topologyTree_) {
    return false;
  }
  for (int top = 0; top < topologyTree_->topLevelItemCount(); ++top) {
    auto *masterItem = topologyTree_->topLevelItem(top);
    if (!masterItem) {
      continue;
    }
    for (int child = 0; child < masterItem->childCount(); ++child) {
      auto *slaveItem = masterItem->child(child);
      if (slaveItem && slaveItem->data(0, Qt::UserRole).toInt() == position) {
        QSignalBlocker blocker(topologyTree_); // prevent recursive signal updates
        topologyTree_->setCurrentItem(slaveItem);
        selectedLabel_->setText(slaveItem->text(0));
        filterWatchTable();
        updateSelectedSlavePanel();
        updateActionAvailability();
        updateCommissioningWorkflow();
        updateIoVariableTable();
        updateStateMachineView();
        return true;
      }
    }
  }
  return false;
}


// — Add current sdo bookmark
void MainWindow::addCurrentSdoBookmark() {
  const int position = selectedPosition();
  const QString index = sdoIndex_ ? sdoIndex_->text().trimmed() : QString();
  const QString subIndex =
      sdoSubIndex_ ? sdoSubIndex_->text().trimmed() : QString();
  if (position < 0 || index.isEmpty() || subIndex.isEmpty()) {
    updateDiagnostics("Info", "Object Bookmarks",
                      uiText("Select a slave and SDO target before bookmarking",
                             "收藏前请先选择从站和 SDO 目标"));
    return;
  }
  const QString type = sdoType_ ? sdoType_->currentText().trimmed() : QString();
  const QString lastValue = sdoValue_ ? sdoValue_->text().trimmed() : QString();
  addObjectBookmark(
      position, index, subIndex,
      selectedSdoWritable_ ? uiText("rw", "读写") : uiText("ro", "只读"), type,
      QString(), uiText("Manual SDO target", "手动 SDO 目标"), lastValue,
      uiText("Current SDO", "当前 SDO"));
}


// — Add selected dictionary rows to bookmarks
void MainWindow::addSelectedDictionaryRowsToBookmarks() {
  if (selectedPosition() < 0 || !sdoTable_ ||
      loadedSdoPosition_ != selectedPosition()) {
    return;
  }
  const QVector<int> rows = selectedDictionaryRows();
  if (rows.isEmpty()) {
    return;
  }
  addDictionaryRowsToBookmarks(
      rows, uiText("selected Object Dictionary row(s)", "选中对象字典行"));
}


// — Add dictionary rows to bookmarks
void MainWindow::addDictionaryRowsToBookmarks(const QVector<int> &rows,
                                              const QString &sourceLabel) {
  if (selectedPosition() < 0 || !sdoTable_ ||
      loadedSdoPosition_ != selectedPosition() || rows.isEmpty()) {
    return;
  }
  int addedOrUpdated = 0;
  int skipped = 0;
  for (const int row : rows) {
    if (row < 0 || row >= sdoTable_->rowCount() ||
        sdoTable_->isRowHidden(row)) {
      ++skipped;
      continue;
    }
    const SdoDictionaryRow dictionary =
        sdoDictionaryRowFromTable(sdoTable_, row);
    if (!sdoDictionaryRowHasTarget(dictionary)) {
      ++skipped;
      continue;
    }
    addObjectBookmark(selectedPosition(), dictionary.index, dictionary.subIndex,
                      dictionary.access, dictionary.type, dictionary.bits,
                      dictionary.name, dictionary.value,
                      uiText("Object Dictionary", "对象字典"));
    ++addedOrUpdated;
  }
  updateDiagnostics(
      "Info", "Object Bookmarks",
      uiText("Bookmarked/reused %1 %2%3", "已收藏/复用 %1 条%2%3")
          .arg(addedOrUpdated)
          .arg(sourceLabel)
          .arg(skipped > 0 ? uiText(", skipped %1", "，跳过 %1").arg(skipped)
                           : QString()));
  updateActionAvailability();
}


// — Check whether select slave for local evidence
bool MainWindow::selectSlaveForLocalEvidence(int position) {
  if (position < 0 || !topologyTree_) {
    return false;
  }
  for (int top = 0; top < topologyTree_->topLevelItemCount(); ++top) {
    auto *masterItem = topologyTree_->topLevelItem(top);
    if (!masterItem) {
      continue;
    }
    for (int child = 0; child < masterItem->childCount(); ++child) {
      auto *slaveItem = masterItem->child(child);
      if (!slaveItem || slaveItem->data(0, Qt::UserRole).toInt() != position) {
        continue;
      }
      QSignalBlocker blocker(topologyTree_); // prevent recursive signal updates
      topologyTree_->setCurrentItem(slaveItem);
      selectedLabel_->setText(slaveItem->text(0));
      filterWatchTable();
      updateSelectedSlavePanel();
      updateActionAvailability();
      updateCommissioningWorkflow();
      updateIoVariableTable();
      updateStateMachineView();
      updateDiagnostics(
          "Info", "Navigator",
          uiText("Selected slave #%1 from local evidence without online load.",
                 "已从本地证据选择从站 #%1，未触发在线加载。")
              .arg(position));
      return true;
    }
  }
  return false;
}


// — Add object bookmark
void MainWindow::addObjectBookmark(int position, const QString &index,
                                   const QString &subIndex,
                                   const QString &access, const QString &type,
                                   const QString &bits, const QString &name,
                                   const QString &lastValue,
                                   const QString &source) {
  if (position < 0 || index.trimmed().isEmpty() ||
      subIndex.trimmed().isEmpty()) {
    return;
  }
  ensureObjectBookmarkTable();
  // Normalize hex address for consistent comparison
  const QString normalizedIndex = normalizeHexText(index, 4);
  // Normalize hex address for consistent comparison
  const QString normalizedSubIndex = normalizeHexText(subIndex, 2);
  int row =
      tableRowForObjectAddress(objectBookmarkTable_, position, normalizedIndex,
                               normalizedSubIndex, 0, 2, 3);
  if (row < 0) {
    row = objectBookmarkTable_->rowCount();
    objectBookmarkTable_->insertRow(row);
  }

  QString slaveName;
  for (const auto &slave : slaves_) {
    if (slave.position == position) {
      slaveName = slave.name;
      break;
    }
  }
  const QStringList values = {
      QString::number(position),
      slaveName,
      normalizedIndex,
      normalizedSubIndex,
      access,
      type,
      bits,
      name,
      lastValue,
      source.trimmed().isEmpty() ? uiText("Project", "工程") : source,
  };
  for (int column = 0; column < values.size(); ++column) {
    auto *item = objectBookmarkTable_->item(row, column);
    if (!item) {
      item = new QTableWidgetItem;
      objectBookmarkTable_->setItem(row, column, item);
    }
    item->setText(values.at(column));
  }
  objectBookmarkTable_->resizeColumnsToContents(); // auto-fit column widths
  objectBookmarkTable_->selectRow(row);
  updateObjectBookmarkRowDetail();
}


// — Fill the SDO target panel from the selected object bookmark row
void MainWindow::applySdoSelectionFromBookmark(int row, bool readAfterFill) {
  if (!objectBookmarkTable_ || row < 0 ||
      row >= objectBookmarkTable_->rowCount()) {
    return;
  }
  const SdoObjectBookmarkRow bookmark =
      sdoObjectBookmarkRowFromTable(objectBookmarkTable_, row);
  if (!sdoObjectBookmarkRowHasTarget(bookmark)) {
    return;
  }

  const bool slaveSelected =
      readAfterFill ? selectObjectBookmarkSlave(bookmark.position)
                    : selectSlaveForLocalEvidence(bookmark.position);
  if (!slaveSelected) {
    updateDiagnostics(
        "Warning", "Object Bookmarks",
        uiText(
            "Bookmark slave #%1 is not in the current topology; rescan or "
            "open the matching project before filling SDO fields",
            "书签从站 #%1 不在当前拓扑中；请先重新扫描或打开匹配工程，再回填 "
            "SDO 字段")
            .arg(bookmark.position));
    return;
  }
  {
    const QSignalBlocker indexBlocker(sdoIndex_); // prevent recursive signal updates
    const QSignalBlocker subIndexBlocker(sdoSubIndex_); // prevent recursive signal updates
    const QSignalBlocker typeBlocker(sdoType_); // prevent recursive signal updates
    const QSignalBlocker valueBlocker(sdoValue_); // prevent recursive signal updates
    sdoIndex_->setText(bookmark.index);
    sdoSubIndex_->setText(bookmark.subIndex);
    if (sdoType_) {
      const QString normalized = bookmark.type.toLower().replace(' ', "_");
      const int typeIndex =
          sdoType_->findText(normalized, Qt::MatchFixedString);
      sdoType_->setCurrentIndex(typeIndex >= 0 ? typeIndex : 0);
    }
    if (sdoValue_) {
      sdoValue_->setText(bookmark.lastValue);
      sdoValue_->setPlaceholderText(
          bookmark.lastValue.isEmpty()
              ? uiText("No bookmark value", "书签暂无值")
              : uiText("Value from object bookmark", "来自对象书签的值"));
    }
  }

  selectedSdoWritable_ =
      !sdoObjectAccessIsReadOnly(bookmark.access, uiText("只读", "只读"));
  if (sdoWriteValue_) {
    sdoWriteValue_->setEnabled(selectedSdoWritable_);
    sdoWriteValue_->setPlaceholderText(
        selectedSdoWritable_ ? uiText("Value to write", "写入值")
                             : uiText("Read-only object", "只读对象"));
    if (!selectedSdoWritable_) {
      sdoWriteValue_->clear();
    }
  }
  updateSdoInspector(
      uiText("Object Bookmark", "对象书签"),
      QString("%1 %2 bit %3").arg(bookmark.name, bookmark.bits, bookmark.type));
  rememberCurrentSdoTarget(
      uiText("Object Bookmark", "对象书签"),
      QString("%1 %2 bit %3").arg(bookmark.name, bookmark.bits, bookmark.type));
  updateDiagnostics("Info", "Object Bookmarks",
                    uiText("Filled SDO target from bookmark #%1 %2:%3",
                           "已从书签回填 SDO 目标 #%1 %2:%3")
                        .arg(bookmark.position)
                        .arg(sdoIndex_->text(), sdoSubIndex_->text()));
  if (readAfterFill && client_.isConnected()) {
    requestSdoRead(bookmark.position, sdoIndex_->text(), sdoSubIndex_->text(),
                   uiText("Object Bookmark", "对象书签"),
                   sdoType_ ? sdoType_->currentText() : QString());
  }
  updateActionAvailability();
}


// — Add selected object bookmarks to watch
void MainWindow::addSelectedObjectBookmarksToWatch() {
  const QVector<int> rows = selectedObjectBookmarkRows();
  if (rows.isEmpty()) {
    return;
  }
  addObjectBookmarkRowsToWatch(rows);
}


// — Add object bookmark rows to watch
void MainWindow::addObjectBookmarkRowsToWatch(const QVector<int> &rows) {
  if (!objectBookmarkTable_ || rows.isEmpty()) {
    return;
  }
  ensureWatchTable();
  int added = 0;
  int reused = 0;
  int skipped = 0;
  for (const int bookmarkRow : rows) {
    const SdoObjectBookmarkRow bookmark =
        sdoObjectBookmarkRowFromTable(objectBookmarkTable_, bookmarkRow);
    if (!sdoObjectBookmarkRowHasTarget(bookmark)) {
      ++skipped;
      continue;
    }
    int watchRow = -1;
    for (int row = 0; row < watchTable_->rowCount(); ++row) {
      const bool match =
          tableObjectAddressMatches(watchTable_, row, bookmark.position,
                                    bookmark.index, bookmark.subIndex, 1, 2, 3);
      if (match) {
        watchRow = row;
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
      watchTable_->setItem(
          watchRow, 1,
          new QTableWidgetItem(QString::number(bookmark.position)));
      watchTable_->setItem(watchRow, 2, new QTableWidgetItem(bookmark.index));
      watchTable_->setItem(watchRow, 3,
                           new QTableWidgetItem(bookmark.subIndex));
      ++added;
    } else {
      ++reused;
    }
    watchTable_->setItem(watchRow, 4, new QTableWidgetItem(bookmark.lastValue));
    watchTable_->setItem(watchRow, 5,
                         new QTableWidgetItem(decodeWatchValue(
                             bookmark.index, bookmark.subIndex, bookmark.type,
                             bookmark.lastValue, "Object Bookmark")));
    watchTable_->setItem(watchRow, 6, new QTableWidgetItem(bookmark.type));
    watchTable_->setItem(watchRow, 7,
                         new QTableWidgetItem(bookmark.name.isEmpty()
                                                  ? "Object Bookmark"
                                                  : bookmark.name));
    for (int column = 8; column < 12; ++column) {
      if (!watchTable_->item(watchRow, column)) {
        watchTable_->setItem(watchRow, column, new QTableWidgetItem);
      }
    }
    updateWatchBaselineDelta(watchRow);
    updateWatchStartupDelta(watchRow);
  }
  watchTable_->resizeColumnsToContents(); // auto-fit column widths
  filterWatchTable();
  updateWatchAutoRefresh();
  activateWorkspaceTab(watchTabIndex_);
  updateDiagnostics(
      "Info", "Object Bookmarks",
      uiText("Watch from bookmarks: added %1, reused %2, skipped %3",
             "从书签加入 Watch：新增 %1，复用 %2，跳过 %3")
          .arg(added)
          .arg(reused)
          .arg(skipped));
}


// — Add selected object bookmarks to startup sdo
void MainWindow::addSelectedObjectBookmarksToStartupSdo() {
  const QVector<int> rows = selectedObjectBookmarkRows();
  if (rows.isEmpty()) {
    return;
  }
  addObjectBookmarkRowsToStartupSdo(rows);
}


// — Add object bookmark rows to startup sdo
void MainWindow::addObjectBookmarkRowsToStartupSdo(const QVector<int> &rows) {
  if (!objectBookmarkTable_ || rows.isEmpty()) {
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
    int bookmarkRow = -1;
    int position = -1;
    QString index;
    QString subIndex;
    QString value;
    QString type;
    QString name;
    QString source;
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

  for (const int bookmarkRow : uniqueRows) {
    const SdoObjectBookmarkRow bookmark =
        sdoObjectBookmarkRowFromTable(objectBookmarkTable_, bookmarkRow);
    if (!sdoObjectBookmarkRowHasTarget(bookmark) ||
        objectBookmarkTable_->isRowHidden(bookmarkRow) ||
        bookmark.lastValue.isEmpty()) {
      ++skipped;
      continue;
    }

    const QString key = QString("%1|%2|%3")
                            .arg(bookmark.position)
                            .arg(bookmark.index, bookmark.subIndex);
    if (processedKeys.contains(key)) {
      ++duplicateSkipped;
      continue;
    }
    processedKeys.insert(key);

    Candidate candidate;
    candidate.bookmarkRow = bookmarkRow;
    candidate.position = bookmark.position;
    candidate.index = bookmark.index;
    candidate.subIndex = bookmark.subIndex;
    candidate.value = bookmark.lastValue;
    candidate.type = bookmark.type;
    candidate.name = bookmark.name;
    candidate.source = bookmark.source;

    for (int startupRow = 0; startupRow < startupSdoTable_->rowCount();
         ++startupRow) {
      if (tableObjectAddressMatches(startupSdoTable_, startupRow,
                                    bookmark.position, bookmark.index,
                                    bookmark.subIndex, 0, 1, 2)) {
        candidate.startupRows.append(startupRow);
      }
    }

    candidates.append(candidate);
  }

  if (candidates.isEmpty()) {
    updateDiagnostics(
        "Warning", "Object Bookmarks",
        uiText("Startup creation skipped: selected bookmarks have no saved "
               "Last Value%1",
               "创建启动项已跳过：所选书签没有保存的最后值%1")
            .arg(
                skipped > 0
                    ? uiText(", skipped %1 row(s)", "，跳过 %1 行").arg(skipped)
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
      uiText("Accepted bookmark values: %1", "可使用书签值：%1")
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
    details << uiText("...and %1 more bookmark value(s)", "...另有 %1 个书签值")
                   .arg(candidates.size() - previewRows);
  }
  if (skipped > 0) {
    details << uiText("Skipped bookmarks without address or saved value: %1",
                      "已跳过缺少地址或保存值的书签：%1")
                   .arg(skipped);
  }
  if (duplicateSkipped > 0) {
    details << uiText("Skipped duplicate selected bookmark address(es): %1",
                      "已跳过重复选中书签地址：%1")
                   .arg(duplicateSkipped);
  }

  // Safety gate: require explicit confirmation before bus write
  if (!confirmDangerousOperation(
          uiText("Confirm Startup from Bookmarks",
                 "确认从书签创建 Startup SDO"),
          uiText("Create or update Startup SDO rows from selected object "
                 "bookmarks.",
                 "从所选对象书签创建或更新 Startup SDO 行。"),
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
          startupRow, 5,
          new QTableWidgetItem(uiText("From Bookmark", "来自书签")));
      startupSdoTable_->setItem(
          startupRow, 6,
          new QTableWidgetItem(
              uiText("Created from Object Bookmark row %1 at %2%3",
                     "由对象书签第 %1 行在 %2 创建%3")
                  .arg(candidate.bookmarkRow + 1)
                  .arg(timestamp,
                       candidate.source.isEmpty()
                           ? QString()
                           : QString(" (%1)").arg(candidate.source))));
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
      ensureStartupCell(startupRow, 5)
          ->setText(uiText("From Bookmark", "来自书签"));
      ensureStartupCell(startupRow, 6)
          ->setText(
              previousValue.compare(candidate.value, Qt::CaseInsensitive) == 0
                  ? uiText("Confirmed from Object Bookmark row %1 at %2",
                           "由对象书签第 %1 行在 %2 确认")
                        .arg(candidate.bookmarkRow + 1)
                        .arg(timestamp)
                  : uiText("Updated from Object Bookmark row %1 at %2; "
                           "previous value: %3",
                           "由对象书签第 %1 行在 %2 更新；原值：%3")
                        .arg(candidate.bookmarkRow + 1)
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
      uiText("Startup from bookmarks: %1 updated, %2 unchanged, %3 created%4%5",
             "从书签生成 Startup：更新 %1，未变 %2，新建 %3%4%5")
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


// — Remove selected object bookmarks
void MainWindow::removeSelectedObjectBookmarks() {
  QVector<int> rows = selectedObjectBookmarkRows();
  if (rows.isEmpty() || !objectBookmarkTable_) {
    return;
  }
  std::sort(rows.begin(), rows.end(), std::greater<int>());
  for (const int row : rows) {
    if (row >= 0 && row < objectBookmarkTable_->rowCount()) {
      objectBookmarkTable_->removeRow(row);
    }
  }
  updateDiagnostics(
      "Info", "Object Bookmarks",
      uiText("Removed %1 object bookmark(s)", "已移除 %1 个对象书签")
          .arg(rows.size()));
  updateObjectBookmarkRowDetail();
  updateActionAvailability();
}


// — Read selected dictionary rows
void MainWindow::readSelectedDictionaryRows() {
  if (!client_.isConnected() || selectedPosition() < 0 || !sdoTable_ ||
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
  if (!client_.isConnected() || selectedPosition() < 0 || !sdoTable_ ||
      loadedSdoPosition_ != selectedPosition()) {
    return;
  }
  const QVector<int> rows = visibleSdoDictionaryRows(sdoTable_);
  if (rows.isEmpty()) {
    return;
  }
  readDictionaryRows(
      rows, uiText("visible object dictionary row(s)", "可见对象字典行"), true);
}


// — Read failed dictionary rows
void MainWindow::readFailedDictionaryRows() {
  if (!client_.isConnected() || selectedPosition() < 0 || !sdoTable_ ||
      loadedSdoPosition_ != selectedPosition()) {
    return;
  }

  const QVector<int> rows = failedSdoDictionaryRows(sdoTable_);
  if (rows.isEmpty()) {
    updateDiagnostics("Info", "SDO",
                      uiText("No failed Object Dictionary rows to retry",
                             "没有需要重试的对象字典失败行"));
    return;
  }

  setSdoFilterPreset("tag:failed");
  if (!rows.isEmpty()) {
    sdoTable_->clearSelection();
    sdoTable_->selectRow(rows.first());
    if (auto *item = sdoTable_->item(rows.first(), 0)) {
      sdoTable_->scrollToItem(item, QAbstractItemView::PositionAtCenter);
    }
  }
  readDictionaryRows(
      rows, uiText("failed object dictionary row(s)", "失败对象字典行"), true);
}


// — Read dictionary rows
void MainWindow::readDictionaryRows(const QVector<int> &rows,
                                    const QString &sourceLabel,
                                    bool confirmLargeBatch) {
  if (!client_.isConnected() || selectedPosition() < 0 || !sdoTable_ ||
      loadedSdoPosition_ != selectedPosition() || rows.isEmpty()) {
    return;
  }

  int skipped = 0;
  const QVector<SdoDictionaryReadObject> objects =
      sdoDictionaryReadObjectsFromRows(sdoTable_, rows, &skipped);
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


// — Add selected dictionary rows to watch
void MainWindow::addSelectedDictionaryRowsToWatch() {
  if (selectedPosition() < 0 || !sdoTable_ ||
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
  if (selectedPosition() < 0 || !sdoTable_ ||
      loadedSdoPosition_ != selectedPosition()) {
    return;
  }
  const QVector<int> rows = visibleSdoDictionaryRows(sdoTable_);
  if (rows.isEmpty()) {
    return;
  }
  addDictionaryRowsToWatch(
      rows, uiText("visible object dictionary row(s)", "可见对象字典行"));
}


// — Add dictionary rows to watch
void MainWindow::addDictionaryRowsToWatch(const QVector<int> &rows,
                                          const QString &sourceLabel) {
  if (selectedPosition() < 0 || !sdoTable_ ||
      loadedSdoPosition_ != selectedPosition() || rows.isEmpty()) {
    return;
  }

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
    if (row < 0 || row >= sdoTable_->rowCount() ||
        sdoTable_->isRowHidden(row)) {
      continue;
    }
    const SdoDictionaryRow dictionary =
        sdoDictionaryRowFromTable(sdoTable_, row);
    if (!sdoDictionaryRowHasTarget(dictionary)) {
      ++skipped;
      continue;
    }
    {
      const QSignalBlocker indexBlocker(sdoIndex_); // prevent recursive signal updates
      const QSignalBlocker subIndexBlocker(sdoSubIndex_); // prevent recursive signal updates
      const QSignalBlocker typeBlocker(sdoType_); // prevent recursive signal updates
      if (sdoIndex_) {
        sdoIndex_->setText(dictionary.index);
      }
      if (sdoSubIndex_) {
        sdoSubIndex_->setText(dictionary.subIndex);
      }
      if (sdoType_) {
        const QString normalized = dictionary.type.toLower().replace(' ', "_");
        const int typeIndex =
            sdoType_->findText(normalized, Qt::MatchFixedString);
        sdoType_->setCurrentIndex(typeIndex >= 0 ? typeIndex : 0);
      }
      if (sdoValue_) {
        sdoValue_->clear();
      }
    }
    selectedSdoWritable_ = true;
    addCurrentSdoToWatch(false);
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
  if (!pdoTable_ || row < 0 || row >= pdoTable_->rowCount()) {
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
  const PdoMapTableRow pdoRow = pdoMapTableRowFromTable(pdoTable_, row);
  if (position < 0 || !pdoMapTableRowHasTarget(pdoRow)) {
    return;
  }

  const QSignalBlocker indexBlocker(sdoIndex_); // prevent recursive signal updates
  const QSignalBlocker subIndexBlocker(sdoSubIndex_); // prevent recursive signal updates
  sdoIndex_->setText(pdoRow.index);
  sdoSubIndex_->setText(pdoRow.subIndex);
  selectedSdoWritable_ = true;
  if (sdoWriteValue_) {
    sdoWriteValue_->setEnabled(true);
    sdoWriteValue_->setPlaceholderText(uiText("Value to write", "写入值"));
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
                             sdoSubIndex_->text(), pdoRow.bits, pdoRow.name));

  if (readAfterFill && client_.isConnected()) {
    requestSdoRead(position, sdoIndex_->text(), sdoSubIndex_->text(),
                   uiText("PDO Map", "PDO 映射"));
  }
  updateActionAvailability();
}


// — Fill the SDO target panel from the selected Free Run row
void MainWindow::applySdoSelectionFromFreeRunEntry(int row,
                                                   bool readAfterFill) {
  if (!freeRunEntryTable_ || row < 0 || row >= freeRunEntryTable_->rowCount()) {
    return;
  }

  const FreeRunEntryTableRow entry =
      freeRunEntryTableRowFromTable(freeRunEntryTable_, row);
  if (!freeRunEntryTableRowHasTarget(entry)) {
    return;
  }

  if (readAfterFill) {
    setSelectedSlave(entry.position);
  } else if (!selectSlaveForLocalEvidence(entry.position)) {
    return;
  }

  const QSignalBlocker indexBlocker(sdoIndex_); // prevent recursive signal updates
  const QSignalBlocker subIndexBlocker(sdoSubIndex_); // prevent recursive signal updates
  sdoIndex_->setText(entry.index);
  sdoSubIndex_->setText(entry.subIndex);
  sdoValue_->setText(entry.raw);
  selectedSdoWritable_ = true;
  if (sdoWriteValue_) {
    sdoWriteValue_->setEnabled(true);
    sdoWriteValue_->setPlaceholderText(uiText("Value to write", "写入值"));
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
  if (!ioVariableTable_ || row < 0 || row >= ioVariableTable_->rowCount()) {
    return;
  }

  const IoVariableTableRow variable =
      ioVariableTableRowFromTable(ioVariableTable_, row);
  if (!ioVariableTableRowHasTarget(variable)) {
    return;
  }

  if (readAfterFill) {
    setSelectedSlave(variable.position);
  } else if (!selectSlaveForLocalEvidence(variable.position)) {
    return;
  }
  const QSignalBlocker indexBlocker(sdoIndex_); // prevent recursive signal updates
  const QSignalBlocker subIndexBlocker(sdoSubIndex_); // prevent recursive signal updates
  const QSignalBlocker typeBlocker(sdoType_); // prevent recursive signal updates
  if (sdoIndex_) {
    sdoIndex_->setText(variable.index);
  }
  if (sdoSubIndex_) {
    sdoSubIndex_->setText(variable.subIndex);
  }
  if (sdoType_) {
    const QString type = ioVariableTableRowTypeFromBits(variable);
    if (!type.isEmpty()) {
      sdoType_->setCurrentText(type);
    }
  }
  if (sdoValue_) {
    sdoValue_->setText(ioVariableTableRowPreferredValue(variable));
  }
  selectedSdoWritable_ = true;
  if (sdoWriteValue_) {
    sdoWriteValue_->setEnabled(true);
    sdoWriteValue_->setPlaceholderText(uiText("Value to write", "写入值"));
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
                   sdoType_ ? sdoType_->currentText() : QString());
  }
  updateActionAvailability();
}


// — Add io variable rows to watch
void MainWindow::addIoVariableRowsToWatch(const QVector<int> &rows,
                                          const QString &sourceLabel) {
  if (!ioVariableTable_ || rows.isEmpty()) {
    return;
  }
  ensureWatchTable();

  int added = 0;
  int reused = 0;
  int skipped = 0;
  for (const int row : rows) {
    if (row < 0 || row >= ioVariableTable_->rowCount() ||
        ioVariableTable_->isRowHidden(row)) {
      ++skipped;
      continue;
    }
    const IoVariableTableRow variable =
        ioVariableTableRowFromTable(ioVariableTable_, row);
    if (!ioVariableTableRowHasTarget(variable)) {
      ++skipped;
      continue;
    }
    const QString type = ioVariableTableRowTypeFromBits(variable);

    int existing = -1;
    for (int watchRow = 0; watchRow < watchTable_->rowCount(); ++watchRow) {
      const bool match =
          tableText(watchTable_, watchRow, 1).toInt() == variable.position &&
          tableText(watchTable_, watchRow, 2)
                  .compare(variable.index, Qt::CaseInsensitive) == 0 &&
          tableText(watchTable_, watchRow, 3)
                  .compare(variable.subIndex, Qt::CaseInsensitive) == 0;
      if (match) {
        existing = watchRow;
        break;
      }
    }

    if (existing >= 0) {
      if (!type.isEmpty() && tableText(watchTable_, existing, 6).isEmpty()) {
        watchTable_->setItem(existing, 6, new QTableWidgetItem(type));
      }
      watchTable_->selectRow(existing);
      ++reused;
      continue;
    }

    const int watchRow = watchTable_->rowCount();
    watchTable_->insertRow(watchRow);
    watchTable_->setItem(
        watchRow, 0,
        new QTableWidgetItem(
            QDateTime::currentDateTime().toString("HH:mm:ss")));
    watchTable_->setItem(
        watchRow, 1, new QTableWidgetItem(QString::number(variable.position)));
    watchTable_->setItem(watchRow, 2, new QTableWidgetItem(variable.index));
    watchTable_->setItem(watchRow, 3, new QTableWidgetItem(variable.subIndex));
    watchTable_->setItem(watchRow, 4, new QTableWidgetItem(variable.raw));
    watchTable_->setItem(
        watchRow, 5,
        new QTableWidgetItem(
            variable.decoded.isEmpty()
                ? decodeWatchValue(variable.index, variable.subIndex, type,
                                   variable.raw, "I/O Variables")
                : variable.decoded));
    watchTable_->setItem(watchRow, 6, new QTableWidgetItem(type));
    watchTable_->setItem(watchRow, 7,
                         new QTableWidgetItem(variable.meaning.isEmpty()
                                                  ? "I/O Variables"
                                                  : variable.meaning));
    watchTable_->setItem(watchRow, 8, new QTableWidgetItem);
    watchTable_->setItem(watchRow, 9, new QTableWidgetItem);
    watchTable_->setItem(watchRow, 10, new QTableWidgetItem);
    watchTable_->setItem(watchRow, 11, new QTableWidgetItem);
    updateWatchStartupDelta(watchRow);
    watchTable_->selectRow(watchRow);
    ++added;
  }

  watchTable_->resizeColumnsToContents(); // auto-fit column widths
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
  if (!ioVariableTable_) {
    return;
  }
  addIoVariableRowsToWatch(selectedIoVariableRows(false),
                           uiText("selected I/O variables", "所选 I/O 变量"));
}


// — Add visible io variables to watch
void MainWindow::addVisibleIoVariablesToWatch() {
  QVector<int> rows;
  if (!ioVariableTable_) {
    return;
  }
  // Iterate all rows and apply active filter predicates
  for (int row = 0; row < ioVariableTable_->rowCount(); ++row) {
    if (!ioVariableTable_->isRowHidden(row)) {
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
  if (!ioVariableTable_ || rows.isEmpty()) {
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
    if (ioRow < 0 || ioRow >= ioVariableTable_->rowCount() ||
        ioVariableTable_->isRowHidden(ioRow)) {
      ++skipped;
      continue;
    }

    const IoVariableTableRow variable =
        ioVariableTableRowFromTable(ioVariableTable_, ioRow);
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
  if (!watchTable_ || row < 0 || row >= watchTable_->rowCount()) {
    return;
  }

  auto textAt = [this, row](int column) {
    const auto *item = watchTable_->item(row, column);
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

  const QSignalBlocker indexBlocker(sdoIndex_); // prevent recursive signal updates
  const QSignalBlocker subIndexBlocker(sdoSubIndex_); // prevent recursive signal updates
  const QSignalBlocker typeBlocker(sdoType_); // prevent recursive signal updates
  sdoIndex_->setText(index);
  sdoSubIndex_->setText(subIndex);
  sdoValue_->setText(value);
  if (!type.isEmpty() && sdoType_) {
    sdoType_->setCurrentText(type);
  }
  selectedSdoWritable_ = true;
  if (sdoWriteValue_) {
    sdoWriteValue_->setEnabled(true);
    sdoWriteValue_->setPlaceholderText(uiText("Value to write", "写入值"));
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

  const QSignalBlocker indexBlocker(sdoIndex_); // prevent recursive signal updates
  const QSignalBlocker subIndexBlocker(sdoSubIndex_); // prevent recursive signal updates
  const QSignalBlocker typeBlocker(sdoType_); // prevent recursive signal updates
  sdoIndex_->setText(index);
  sdoSubIndex_->setText(subIndex);
  if (sdoValue_) {
    sdoValue_->setText(value);
  }
  if (!type.isEmpty() && sdoType_) {
    const QString normalized = type.toLower().replace(' ', "_");
    const int typeIndex = sdoType_->findText(normalized, Qt::MatchFixedString);
    sdoType_->setCurrentIndex(typeIndex >= 0 ? typeIndex : 0);
  }
  selectedSdoWritable_ = true;
  if (sdoWriteValue_) {
    sdoWriteValue_->setEnabled(true);
    sdoWriteValue_->setText(value);
    sdoWriteValue_->setPlaceholderText(
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

  const QSignalBlocker indexBlocker(sdoIndex_); // prevent recursive signal updates
  const QSignalBlocker subIndexBlocker(sdoSubIndex_); // prevent recursive signal updates
  const QSignalBlocker typeBlocker(sdoType_); // prevent recursive signal updates
  sdoIndex_->setText(index);
  sdoSubIndex_->setText(subIndex);
  if (sdoValue_) {
    sdoValue_->setText(value);
  }
  if (!type.isEmpty() && sdoType_) {
    const QString normalized = type.toLower().replace(' ', "_");
    const int typeIndex = sdoType_->findText(normalized, Qt::MatchFixedString);
    sdoType_->setCurrentIndex(typeIndex >= 0 ? typeIndex : 0);
  }
  selectedSdoWritable_ = true;
  if (sdoWriteValue_) {
    sdoWriteValue_->setEnabled(true);
    sdoWriteValue_->setText(value);
    sdoWriteValue_->setPlaceholderText(
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


// — Apply a preset filter string to the OD table and activate the pane
void MainWindow::setSdoFilterPreset(const QString &query) {
  if (!sdoFilter_) {
    return;
  }
  sdoFilter_->setText(query);
  filterSdoTable(query);
  activateObjectDictionaryPaneFor(sdoTable_);
}


// — Check whether any OD evidence rows have a Failed status
bool MainWindow::hasFailedSdoEvidence() const {
  return firstFailedSdoEvidenceRow() >= 0;
}


// — Return the first OD row with a Failed status, or -1
int MainWindow::firstFailedSdoEvidenceRow() const {
  if (!sdoTable_ || selectedPosition() < 0 ||
      loadedSdoPosition_ != selectedPosition()) {
    return -1;
  }
  const QVector<int> rows = failedSdoDictionaryRows(sdoTable_);
  return rows.isEmpty() ? -1 : rows.first();
}


// — Scroll to and select the first failed OD evidence row
void MainWindow::focusFailedSdoEvidence() {
  if (!sdoTable_) {
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
  activateObjectDictionaryPaneFor(sdoTable_);
  sdoTable_->clearSelection();
  sdoTable_->setCurrentCell(row, 0);
  sdoTable_->selectRow(row);
  if (auto *item = sdoTable_->item(row, 0)) {
    sdoTable_->scrollToItem(item, QAbstractItemView::PositionAtCenter);
  }
  applySdoSelectionFromDictionary(row, false);

  const SdoDictionaryRow dictionary = sdoDictionaryRowFromTable(sdoTable_, row);
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
    const auto *item = sdoTable_->item(row, column);
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
  for (int row = 0; row < sdoTable_->rowCount(); ++row) {
    bool match = needle.isEmpty();
    const QString lastValue = textAt(row, 7);
    const QString lastStatus = textAt(row, 8);
    const QString accessForSummary = textAt(row, 3).toLower();
    const bool hasEvidence = !lastValue.isEmpty() || !lastStatus.isEmpty();
    const bool failedEvidence =
        lastStatus.contains(uiText("Failed", "失败"), Qt::CaseInsensitive) ||
        lastStatus.contains("failed", Qt::CaseInsensitive);
    if (accessForSummary.contains('w')) {
    }
    if (match) {
      ++visible;
    }
    sdoTable_->setRowHidden(row, !match); // show/hide based on filter match
  }
  if (tagMode) {
    updateDiagnostics("Info", "SDO",
                      QString("Object Dictionary filter '%1': %2/%3 visible, "
                              "%4 with evidence, %5 failed")
                          .arg(needle)
                          .arg(visible)
                          .arg(sdoTable_->rowCount())
                          .arg(evidenceRows)
                          .arg(failedRows));
  }
  if (sdoSummaryLabel_) {
    sdoSummaryLabel_->setText(uiText("%1/%2 | W %3 | E %4 | F %5",
                                     "%1/%2 | 可写 %3 | 证据 %4 | 失败 %5")
                                  .arg(visible)
                                  .arg(sdoTable_->rowCount())
                                  .arg(writableRows)
                                  .arg(evidenceRows)
                                  .arg(failedRows));
    sdoSummaryLabel_->setToolTip(
        uiText("%1/%2 visible, %3 writable, %4 with evidence, %5 failed",
               "%1/%2 可见，%3 可写，%4 有证据，%5 失败")
            .arg(visible)
            .arg(sdoTable_->rowCount())
            .arg(writableRows)
            .arg(evidenceRows)
            .arg(failedRows));
  }
  updateActionAvailability();
}
