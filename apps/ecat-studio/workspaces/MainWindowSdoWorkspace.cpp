// SDO inspector, target panel, evidence trail, and history.

#include "MainWindow.h"

#include "models/Cia402DriveModel.h"
#include "models/CommissioningWorkflowModel.h"
#include "detail/CommissioningWorkflowStepDetail.h"
#include "adapters/CommissioningWorkflowTableAdapter.h"
#include "detail/CommissioningWorkflowDetail.h"
#include "detail/ConsistencyDetail.h"
#include "models/ConsistencyModel.h"
#include "models/ConsistencyModel.h"
#include "adapters/ConsistencyTableAdapter.h"
#include "detail/DiagnosticsEventDetail.h"
#include "models/EvidenceModel.h"
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
#include "models/EvidenceModel.h"
#include "utils/Documentation.h"
#include "utils/TableHelpers.h"
#include "utils/TextHelpers.h"
#include "utils/UiHelpers.h"
#include "models/TopologyModel.h"
#include "models/TopologyModel.h"
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
  if (sdo_->sdoTable && selectedPosition() == position &&
      loadedSdoPosition_ == position) {
    const SdoDictionaryRow dictionary = sdoDictionaryRowForTarget(
        sdo_->sdoTable, normalizedIndex, normalizedSubIndex);
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
          sdoInspector_->sdoValue ? sdoInspector_->sdoValue->text() : QString(), cachedEvidence.value(0),
          sdoInspector_->sdoValue &&
              isCurrentSdoTarget(position, normalizedIndex, normalizedSubIndex),
          sdo_->sdoTable && loadedSdoPosition_ == position,
          {.dictionaryTable = sdo_->sdoTable,
           .watchTable = watch_->watchTable,
           .startupTable = startupSdoTable_,
           .bookmarkTable = bookmark_->objectBookmarkTable},
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
  if (watch_->watchTable) {
    for (int row = 0; row < watch_->watchTable->rowCount(); ++row) {
      const QString value = tableText(watch_->watchTable, row, 4);
      if (tableObjectAddressMatches(watch_->watchTable, row, position, normalizedIndex,
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
  if (sdoInspector_->sdoWriteValue) {
    sdoInspector_->sdoWriteValue->setEnabled(true);
    sdoInspector_->sdoWriteValue->setPlaceholderText(uiText("Value to write", "写入值"));
  }
  updateSdoInspector(uiText("Manual edit", "手动编辑"));
  updateActionAvailability();
}


// — Check whether is current sdo target
bool MainWindow::isCurrentSdoTarget(int position, const QString &index,
                                    const QString &subIndex) const {
  if (position < 0 || selectedPosition() < 0 || !sdoInspector_->sdoIndex || !sdoInspector_->sdoSubIndex) {
    return false;
  }
  const QString currentIndex = sdoInspector_->sdoIndex->text().trimmed();
  const QString currentSubIndex = sdoInspector_->sdoSubIndex->text().trimmed();
  if (currentIndex.isEmpty() || currentSubIndex.isEmpty()) {
    return false;
  }
  return sdoEvidenceKey(position, index, subIndex) ==
         sdoEvidenceKey(selectedPosition(), currentIndex, currentSubIndex);
}


// — Return the current sdo dictionary row
int MainWindow::currentSdoDictionaryRow() const {
  const int position = selectedPosition();
  if (position < 0 || !sdo_->sdoTable || !sdoInspector_->sdoIndex || !sdoInspector_->sdoSubIndex ||
      loadedSdoPosition_ != position) {
    return -1;
  }
  return sdoEvidenceTableRowsForTarget({.dictionaryTable = sdo_->sdoTable},
                                       {.position = position,
                                        .index = sdoInspector_->sdoIndex->text(),
                                        .subIndex = sdoInspector_->sdoSubIndex->text(),
                                        .dictionaryLoadedForPosition = true})
      .dictionaryRow;
}


// — Return the current sdo watch row
int MainWindow::currentSdoWatchRow() const {
  const int position = selectedPosition();
  if (position < 0 || !watch_->watchTable || !sdoInspector_->sdoIndex || !sdoInspector_->sdoSubIndex) {
    return -1;
  }
  return sdoEvidenceTableRowsForTarget({.watchTable = watch_->watchTable},
                                       {.position = position,
                                        .index = sdoInspector_->sdoIndex->text(),
                                        .subIndex = sdoInspector_->sdoSubIndex->text()})
      .watchRow;
}


// — Return the current sdo startup row
int MainWindow::currentSdoStartupRow() const {
  const int position = selectedPosition();
  if (position < 0 || !startupSdoTable_ || !sdoInspector_->sdoIndex || !sdoInspector_->sdoSubIndex) {
    return -1;
  }
  return sdoEvidenceTableRowsForTarget({.startupTable = startupSdoTable_},
                                       {.position = position,
                                        .index = sdoInspector_->sdoIndex->text(),
                                        .subIndex = sdoInspector_->sdoSubIndex->text()})
      .startupRow;
}


// — Return the current sdo bookmark row
int MainWindow::currentSdoBookmarkRow() const {
  const int position = selectedPosition();
  if (position < 0 || !bookmark_->objectBookmarkTable || !sdoInspector_->sdoIndex || !sdoInspector_->sdoSubIndex) {
    return -1;
  }
  return sdoEvidenceTableRowsForTarget({.bookmarkTable = bookmark_->objectBookmarkTable},
                                       {.position = position,
                                        .index = sdoInspector_->sdoIndex->text(),
                                        .subIndex = sdoInspector_->sdoSubIndex->text()})
      .bookmarkRow;
}


// — Return the current sdo target trail row
int MainWindow::currentSdoTargetTrailRow() const {
  const int position = selectedPosition();
  if (position < 0 || !sdoTargetTrailTable_ || !sdoInspector_->sdoIndex || !sdoInspector_->sdoSubIndex) {
    return -1;
  }
  return sdoEvidenceTableRowsForTarget(
             {.targetTrailTable = sdoTargetTrailTable_},
             {.position = position,
              .index = sdoInspector_->sdoIndex->text(),
              .subIndex = sdoInspector_->sdoSubIndex->text()})
      .targetTrailRow;
}


// — Return the current sdo preferred evidence value
QString MainWindow::currentSdoPreferredEvidenceValue(QString *source) const {
  return preferredSdoEvidenceValue(currentSdoEvidenceCandidates(), source);
}
