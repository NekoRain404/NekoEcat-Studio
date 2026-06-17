// Session brief, next-best-action, and workspace badges.

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


namespace {

QStyle::StandardPixmap
nextBestActionStandardPixmap(NextBestActionIconKey icon) {
  switch (icon) {
  case NextBestActionIconKey::DriveNet:
    return QStyle::SP_DriveNetIcon;
  case NextBestActionIconKey::Warning:
    return QStyle::SP_MessageBoxWarning;
  case NextBestActionIconKey::DetailedView:
    return QStyle::SP_FileDialogDetailedView;
  case NextBestActionIconKey::ListView:
    return QStyle::SP_FileDialogListView;
  case NextBestActionIconKey::NewFolder:
    return QStyle::SP_FileDialogNewFolder;
  case NextBestActionIconKey::MediaPlay:
    return QStyle::SP_MediaPlay;
  case NextBestActionIconKey::ContentsView:
    return QStyle::SP_FileDialogContentsView;
  }
  return QStyle::SP_FileDialogContentsView;
}


// — Map a host health severity key to its display color
QColor hostHealthColorForKey(const QString &colorKey) {
  if (colorKey == QStringLiteral("error")) {
    return QColor("#ef4444");
  }
  if (colorKey == QStringLiteral("warning")) {
    return QColor("#f59e0b");
  }
  return QColor("#22c55e");
}


// — Map a diagnostics severity key to its display color
QColor diagnosticsEventColorForKey(const QString &colorKey) {
  if (colorKey == QStringLiteral("error")) {
    return QColor("#ef4444");
  }
  if (colorKey == QStringLiteral("warning")) {
    return QColor("#f59e0b");
  }
  return QColor("#60a5fa");
}

} // namespace


// — Rebuild the session brief table summarizing current workspace health
void MainWindow::updateSessionBrief() {
  // Dispatch Alt+Enter to the correct evidence action for this table type
  if (!session_->sessionBriefTable) {
    return;
  }

  const bool connected = client_.isConnected();
  const bool hasSlaves = !slaves_.isEmpty();
  const int selected = selectedPosition();
  const bool hasSelectedSlave = selected >= 0;
  const bool hasSdoRows = hasSelectedSlave && loadedSdoPosition_ == selected &&
                          sdo_->sdoTable && sdo_->sdoTable->rowCount() > 0;
  const bool hasPdoRows = hasSelectedSlave && loadedPdoPosition_ == selected &&
                          sdo_->pdoTable && sdo_->pdoTable->rowCount() > 0;
  const bool hasWatchRows = watch_->watchTable && watch_->watchTable->rowCount() > 0;
  const bool hasFreeRunRows =
      freeRunWidgets_->freeRunEntryTable && freeRunWidgets_->freeRunEntryTable->rowCount() > 0;
  const bool hasFailedOdEvidence = hasFailedSdoEvidence();
  const int startupDiffs = startupSdoRowsWithWatchDiffs().size();

  QString selectedSlave = uiText("No selected slave", "未选择从站");
  QString selectedState = uiText("none", "无");
  if (hasSelectedSlave) {
    selectedSlave = QString("#%1").arg(selected);
    for (const auto &slave : slaves_) {
      if (slave.position == selected) {
        const QString name = slave.name.trimmed();
        selectedSlave = name.isEmpty()
                            ? selectedSlave
                            : QString("#%1 %2").arg(selected).arg(name);
        selectedState = slave.state.trimmed().isEmpty()
                            ? uiText("unknown", "未知")
                            : slave.state.trimmed();
        break;
      }
    }
  }

  SessionBriefInput briefInput;
  briefInput.connected = connected;
  briefInput.hasSlaves = hasSlaves;
  briefInput.hasSelectedSlave = hasSelectedSlave;
  briefInput.hasSdoRows = hasSdoRows;
  briefInput.hasPdoRows = hasPdoRows;
  briefInput.hasFailedOdEvidence = hasFailedOdEvidence;
  briefInput.hasWatchRows = hasWatchRows;
  briefInput.freeRunEnabled = freeRun_;
  briefInput.hasFreeRunRows = hasFreeRunRows;
  briefInput.startupDiffs = startupDiffs;

  int consistencyErrors = 0;
  int consistencyWarnings = 0;
  int consistencyInfos = 0;
  int consistencyReady = 0;
  consistencyIssueCounts(&consistencyErrors, &consistencyWarnings,
                         &consistencyInfos, &consistencyReady);
  const bool hasConsistencyCheck =
      consistencyFresh_ && consistencyCheckAvailable();
  const bool hasConsistencyBlockingIssues =
      consistencyHasBlockingIssues({consistencyErrors, consistencyWarnings,
                                    consistencyInfos, consistencyReady});
  briefInput.hasConsistencyCheck = hasConsistencyCheck;
  briefInput.consistencyErrors = consistencyErrors;
  briefInput.consistencyWarnings = consistencyWarnings;

  QString sdoEvidenceText =
      uiText("No complete SDO target", "尚无完整 SDO 目标");
  QString sdoNext = uiText("Select an OD row or fill SDO fields",
                           "选择 OD 行或填写 SDO 字段");
  if (hasSelectedSlave && sdoInspector_->sdoIndex && sdoInspector_->sdoSubIndex &&
      !sdoInspector_->sdoIndex->text().trimmed().isEmpty() &&
      !sdoInspector_->sdoSubIndex->text().trimmed().isEmpty()) {
    briefInput.currentSdoComplete = true;
    const QString address =
        // Normalize hex address for consistent comparison
        QString("%1:%2").arg(normalizeHexText(sdoInspector_->sdoIndex->text(), 4),
                             // Normalize hex address for consistent comparison
                             normalizeHexText(sdoInspector_->sdoSubIndex->text(), 2));
    const auto candidates = currentSdoEvidenceCandidates();
    QStringList facts;
    QStringList groups;
    for (const auto &candidate : candidates) {
      const QString normalized = normalizeComparableValue(candidate.second);
      if (normalized.isEmpty()) {
        continue;
      }
      bool seen = false;
      const auto &existingGroups = groups;
      for (const QString &group : existingGroups) {
        if (group == normalized) {
          seen = true;
          break;
        }
      }
      if (!seen) {
        groups << normalized;
      }
      facts << QString("%1=%2").arg(candidate.first, candidate.second);
    }

    const QString writeValue =
        sdoInspector_->sdoWriteValue ? sdoInspector_->sdoWriteValue->text().trimmed() : QString();
    bool writeDiffers = false;
    bool writeMatches = false;
    if (!writeValue.isEmpty() && !groups.isEmpty()) {
      const QString normalizedWrite = normalizeComparableValue(writeValue);
      const auto &existingGroups = groups;
      for (const QString &group : existingGroups) {
        if (group == normalizedWrite) {
          writeMatches = true;
        } else {
          writeDiffers = true;
        }
      }
    }
    briefInput.currentSdoEvidenceGroups = groups.size();
    briefInput.currentSdoHasEvidence = !candidates.isEmpty();
    briefInput.currentSdoWriteDiffers = writeDiffers;
    briefInput.currentSdoWriteMatches = writeMatches;

    if (groups.size() > 1) {
      sdoNext =
          uiText("Review evidence delta before writing", "写入前审阅证据差异");
    } else if (writeDiffers && !writeMatches) {
      sdoNext = uiText("Confirm write delta", "确认写入差异");
    } else if (!candidates.isEmpty()) {
      sdoNext =
          writeValue.isEmpty()
              ? uiText("Use evidence or read target", "使用证据或读取目标")
              : uiText("Write uses normal confirmation", "写入走普通确认流程");
    } else {
      sdoNext = uiText("Read target or add Watch evidence",
                       "读取目标或添加 Watch 证据");
    }
    sdoEvidenceText =
        uiText("%1 | evidence %2 group(s)%3", "%1 | 证据 %2 组%3")
            .arg(address)
            .arg(groups.size())
            .arg(facts.isEmpty() ? QString()
                                 : QString(" | %1").arg(facts.join("; ")));
    if (!writeValue.isEmpty()) {
      sdoEvidenceText += uiText(" | write %1", " | 写入 %1").arg(writeValue);
    }
  }

  QString nextAction = uiText("Open command palette", "打开命令面板");
  const int nextStep = nextCommissioningWorkflowStep();
  briefInput.nextWorkflowStep = nextStep;
  if (nextStep >= 0 && workflow_->workflowTable &&
      nextStep < workflow_->workflowTable->rowCount()) {
    nextAction =
        commissioningWorkflowTableRowFromTable(workflow_->workflowTable, nextStep)
            .nextAction;
  } else if (nextBestActionButton_) {
    nextAction = nextBestActionButton_->text();
  }

  const QVector<SessionBriefRow> briefRows = buildSessionBriefRows(briefInput);
  const SessionBriefUiTexts uiTexts = sessionBriefUiTexts();
  QList<SessionBriefUiRow> rows;
  rows.reserve(briefRows.size());
  rows.append(sessionBriefUiRow(
      briefRows.at(0), uiText("Target", "目标"),
      uiText("%1 | runtime %2 | %3 slave(s)", "%1 | 运行时 %2 | %3 个从站")
          .arg(selectedSlave, connected ? uiText("connected", "已连接")
                                        : uiText("offline", "离线"))
          .arg(slaves_.size()),
      !connected ? uiText("Connect runtime", "连接运行时")
                 : (!hasSlaves ? uiText("Rescan bus", "重新扫描总线")
                               : (!hasSelectedSlave
                                      ? uiText("Select a slave", "选择从站")
                                      : uiText("Continue evidence review",
                                               "继续证据复核"))),
      uiTexts));
  rows.append(sessionBriefUiRow(
      briefRows.at(1), uiText("Gate", "门禁"),
      hasConsistencyCheck
          ? uiText("Consistency: %1 error(s), %2 warning(s), %3 info, %4 "
                   "ready",
                   "一致性：%1 错误，%2 警告，%3 信息，%4 就绪")
                .arg(consistencyErrors)
                .arg(consistencyWarnings)
                .arg(consistencyInfos)
                .arg(consistencyReady)
          : uiText("Consistency check is not fresh", "一致性检查尚未刷新"),
      hasConsistencyCheck && hasConsistencyBlockingIssues
          ? uiText("Open blocking evidence", "打开阻塞证据")
          : (hasConsistencyCheck
                 ? uiText("Proceed when machine is safe", "设备安全时继续")
                 : uiText("Run read-only consistency gate",
                          "运行只读一致性门禁")),
      uiTexts));
  rows.append(sessionBriefUiRow(
      briefRows.at(2), uiText("Map", "映射"),
      uiText("OD %1 row(s) | PDO %2 row(s) | failed OD %3",
             "OD %1 行 | PDO %2 行 | 失败 OD %3")
          .arg(sdo_->sdoTable ? sdo_->sdoTable->rowCount() : 0)
          .arg(sdo_->pdoTable ? sdo_->pdoTable->rowCount() : 0)
          .arg(hasFailedOdEvidence ? uiText("yes", "有") : uiText("no", "无")),
      !hasSelectedSlave
          ? uiText("Select slave", "选择从站")
          : (!hasSdoRows
                 ? uiText("Load Object Dictionary", "加载对象字典")
                 : (!hasPdoRows ? uiText("Load PDO Map", "加载 PDO 映射")
                                : (hasFailedOdEvidence
                                       ? uiText("Review failed OD evidence",
                                                "审阅失败 OD 证据")
                                       : uiText("Map evidence ready",
                                                "映射证据就绪")))),
      uiTexts));
  rows.append(sessionBriefUiRow(briefRows.at(3),
                                uiText("Current SDO", "当前 SDO"),
                                sdoEvidenceText, sdoNext, uiTexts));
  rows.append(sessionBriefUiRow(
      briefRows.at(4), uiText("Runtime Evidence", "运行证据"),
      uiText("Watch %1 row(s) | Startup diff %2 | Free Run %3 | process %4",
             "Watch %1 行 | Startup 偏差 %2 | Free Run %3 | 过程项 %4")
          .arg(watch_->watchTable ? watch_->watchTable->rowCount() : 0)
          .arg(startupDiffs)
          .arg(freeRun_ ? uiText("on", "开启") : uiText("off", "关闭"))
          .arg(freeRunWidgets_->freeRunEntryTable ? freeRunWidgets_->freeRunEntryTable->rowCount() : 0),
      startupDiffs > 0
          ? uiText("Review Startup diffs", "审阅 Startup 偏差")
          : (!hasWatchRows
                 ? uiText("Add Watch evidence", "添加 Watch 证据")
                 : (!freeRun_ && !hasFreeRunRows
                        ? uiText("Validate process image when safe",
                                 "安全时验证过程映像")
                        : uiText("Runtime evidence ready", "运行证据就绪"))),
      uiTexts));
  rows.append(sessionBriefUiRow(
      briefRows.at(5), uiText("Next", "下一步"),
      uiText("State %1 | workflow row %2", "状态 %1 | 工作流行 %2")
          .arg(selectedState)
          .arg(nextStep >= 0 ? QString::number(nextStep + 1)
                             : uiText("ready", "就绪")),
      nextAction, uiTexts));

  QList<QStringList> tableRows;
  for (const auto &row : rows) {
    tableRows.append(row.cells);
  }
  setTableRows(session_->sessionBriefTable, sessionBriefTableHeaders(uiTexts),
               tableRows);

  const QColor readyColor("#22c55e");
  const QColor actionColor("#f59e0b");
  const QColor warningColor("#f59e0b");
  const QColor errorColor("#ef4444");
  for (int row = 0; row < session_->sessionBriefTable->rowCount(); ++row) {
    const SessionBriefUiRow uiRow = rows.value(row);
    const QString colorKey = sessionBriefStatusColorKey(uiRow.status);
    const QColor color =
        colorKey == QStringLiteral("ready")
            ? readyColor
            : (colorKey == QStringLiteral("error")
                   ? errorColor
                   : (colorKey == QStringLiteral("warning") ? warningColor
                                                            : actionColor));
    for (int column = 0; column < session_->sessionBriefTable->columnCount(); ++column) {
      if (auto *item = session_->sessionBriefTable->item(row, column)) {
        item->setForeground(color);
        item->setToolTip(uiRow.tooltips.value(column));
      }
    }
    setSessionBriefActionKey(session_->sessionBriefTable, row, uiRow.actionKey);
  }
  fitTableColumnsToViewport(session_->sessionBriefTable, kSessionBriefEvidenceColumn);
  updateSessionBriefCopyButton();
}


// — Update session brief copy button
void MainWindow::updateSessionBriefCopyButton() {
  // Dispatch Alt+Enter to the correct evidence action for this table type
  if (!session_->sessionBriefCopyButton || !session_->sessionBriefTable) {
    return;
  }
  const int row = session_->sessionBriefTable->currentRow();
  const bool hasRow = row >= 0 && row < session_->sessionBriefTable->rowCount();
  session_->sessionBriefCopyButton->setEnabled(hasRow);
  if (!hasRow) {
    session_->sessionBriefCopyButton->setText(uiText("Copy Row", "复制本行"));
    session_->sessionBriefCopyButton->setToolTip(uiText(
        "Select a Session Brief row to copy its local evidence summary. No "
        "bus request is sent.",
        "选择一条会话简报行以复制本地证据摘要；不会发送总线请求。"));
    return;
  }

  const QString area =
      sessionBriefTableRowFromTable(session_->sessionBriefTable, row).area.trimmed();
  session_->sessionBriefCopyButton->setText(
      area.isEmpty() ? uiText("Copy Row", "复制本行")
                     : uiText("Copy: %1", "复制：%1").arg(area));
  session_->sessionBriefCopyButton->setToolTip(
      uiText("Copy the Session Brief row \"%1\" with status, evidence, next "
             "action, and local boundary. Clipboard only; no bus access.",
             "复制会话简报行“%1”的状态、依据、下一步和本地边界。只写剪贴板；"
             "不访问总线。")
          .arg(area.isEmpty() ? uiText("selected row", "所选行") : area));
}


// — Check whether copy session brief row digest
bool MainWindow::copySessionBriefRowDigest(int row) {
  if (!session_->sessionBriefTable || row < 0 || row >= session_->sessionBriefTable->rowCount()) {
    statusBar()->showMessage(uiText("Select a Session Brief row to copy.",
                                    "请选择一条会话简报行再复制。"),
                             3000);
    return false;
  }

  const SessionBriefTableRow briefRow =
      sessionBriefTableRowFromTable(session_->sessionBriefTable, row);

  QStringList lines;
  lines << uiText("NekoEcat Studio Session Brief Row",
                  "NekoEcat Studio 会话简报本行");
  lines << QString("%1: %2").arg(uiText("Master", "主站"), activeMasterName());
  lines << QString("%1: %2").arg(uiText("Area", "区域"), briefRow.area);
  lines << QString("%1: %2").arg(uiText("Status", "状态"), briefRow.status);
  lines << QString("%1: %2").arg(uiText("Evidence", "依据"), briefRow.evidence);
  lines << QString("%1: %2").arg(uiText("Next", "下一步"), briefRow.next);
  if (!briefRow.actionKey.isEmpty()) {
    lines << QString("%1: %2").arg(uiText("Local Route", "本地路由"),
                                   briefRow.actionKey);
  }
  if (nextBestActionButton_) {
    lines << QString("%1: %2").arg(
        uiText("Current Next Best Action", "当前下一最佳动作"),
        nextBestActionButton_->text().trimmed());
  }
  if (!briefRow.firstTooltip.isEmpty()) {
    lines << QString();
    lines << uiText("Row Detail", "行详情");
    const QStringList detailLines =
        briefRow.firstTooltip.split('\n', Qt::SkipEmptyParts);
    for (const QString &detail : detailLines) {
      lines << QString("- %1").arg(detail.trimmed());
    }
  }

  lines << QString();
  lines << uiText(
      "Local Action: Open Session Brief Evidence routes this row "
      "to the already loaded local evidence surface.",
      "本地动作：打开会话简报证据会把该行路由到已加载的本地证据界面。");
  lines << uiText("Boundary: clipboard copy only; no bus read, no OD/PDO/ESI "
                  "load, no SDO write, no state change, no Free Run change, "
                  "and no Host Health.",
                  "边界：只复制到剪贴板；不读取总线、不加载 OD/PDO/ESI、不写 "
                  "SDO、不切换状态、不改变 Free Run，也不运行 Host Health。");

  QApplication::clipboard()->setText(lines.join('\n')); // copy to system clipboard
  updateDiagnostics("Info", "Session Brief",
                    uiText("Copied Session Brief row #%1 to clipboard",
                           "已复制会话简报第 %1 行到剪贴板")
                        .arg(row + 1));
  statusBar()->showMessage(
      uiText("Copied Session Brief row. No bus access was requested.",
             "已复制会话简报行；未请求总线访问。"),
      3000);
  return true;
}


// — Update workflow step copy button
void MainWindow::updateWorkflowStepCopyButton() {
  // Dispatch Alt+Enter to the correct evidence action for this table type
  if (!workflow_->workflowStepCopyButton || !workflow_->workflowTable) {
    return;
  }
  const int row = workflow_->workflowTable->currentRow();
  const bool hasRow = row >= 0 && row < workflow_->workflowTable->rowCount() &&
                      !workflow_->workflowTable->isRowHidden(row);
  workflow_->workflowStepCopyButton->setEnabled(hasRow);
  if (!hasRow) {
    workflow_->workflowStepCopyButton->setText(uiText("Copy Step", "复制步骤"));
    workflow_->workflowStepCopyButton->setToolTip(uiText(
        "Select a commissioning workflow step to copy its local evidence "
        "summary. No bus request is sent.",
        "选择一条调试工作流步骤以复制本地证据摘要；不会发送总线请求。"));
    return;
  }

  const QString step =
      commissioningWorkflowTableRowFromTable(workflow_->workflowTable, row)
          .step.trimmed();
  workflow_->workflowStepCopyButton->setText(
      step.isEmpty() ? uiText("Copy Step", "复制步骤")
                     : uiText("Copy: %1", "复制：%1").arg(step));
  workflow_->workflowStepCopyButton->setToolTip(
      uiText("Copy workflow step \"%1\" with phase, status, risk, evidence, "
             "next action, tooltip detail, and local boundary. Clipboard "
             "only; no bus access.",
             "复制工作流步骤“%1”的阶段、状态、风险、依据、下一步、提示详情和"
             "本地边界。只写剪贴板；不访问总线。")
          .arg(step.isEmpty() ? uiText("selected step", "所选步骤") : step));
}


// — Update workflow step detail
void MainWindow::updateWorkflowStepDetail() {
  if (!workflow_->workflowStepDetailLabel) {
    return;
  }
  const CommissioningWorkflowStepDetailTexts texts =
      commissioningWorkflowStepDetailTexts();
  auto applyState =
      [this](const CommissioningWorkflowStepDetailUiState &state) {
        workflow_->workflowStepDetailLabel->setText(state.text);
        workflow_->workflowStepDetailLabel->setProperty("severity", state.severityKey);
        workflow_->workflowStepDetailLabel->setToolTip(state.tooltip);
        repolish(workflow_->workflowStepDetailLabel); // force QSS re-evaluation after property change
      };

  // Dispatch Alt+Enter to the correct evidence action for this table type
  if (!workflow_->workflowTable) {
    applyState(commissioningWorkflowStepDetailUnavailableState(texts));
    return;
  }

  const int row = workflow_->workflowTable->currentRow();
  if (row < 0 || row >= workflow_->workflowTable->rowCount() ||
      workflow_->workflowTable->isRowHidden(row)) {
    applyState(commissioningWorkflowStepDetailNoSelectionState(texts));
    return;
  }

  applyState(buildCommissioningWorkflowStepDetailUiState(
      commissioningWorkflowTableRowFromTable(workflow_->workflowTable, row),
      commissioningWorkflowTexts(), texts));
}


// — Navigate to the evidence workspace referenced by the session brief row
void MainWindow::openSessionBriefRow(int row) {
  // Dispatch Alt+Enter to the correct evidence action for this table type
  if (!session_->sessionBriefTable) {
    return;
  }
  if (row < 0 || row >= session_->sessionBriefTable->rowCount()) {
    row = session_->sessionBriefTable->currentRow();
  }
  if (row < 0 || row >= session_->sessionBriefTable->rowCount()) {
    return;
  }

  const SessionBriefTableRow briefRow =
      sessionBriefTableRowFromTable(session_->sessionBriefTable, row);
  QString actionKey = briefRow.actionKey;
  const QString area = briefRow.area.toLower();
  const QString status = briefRow.status.toLower();
  const QString evidence = briefRow.evidence.toLower();
  const QString next = briefRow.next.toLower();
  const QString combined =
      QString("%1 %2 %3 %4").arg(area, status, evidence, next);

  auto logNavigation = [this](const QString &target) {
    updateDiagnostics("Info", "Session Brief",
                      uiText("Opened local Session Brief evidence: %1",
                             "已打开会话简报本地证据：%1")
                          .arg(target));
  };

  if (actionKey.isEmpty()) {
    if (area == uiText("Target", "目标").toLower()) {
      actionKey = QStringLiteral("target");
    } else if (area == uiText("Gate", "门禁").toLower()) {
      actionKey = QStringLiteral("gate");
    } else if (area == uiText("Map", "映射").toLower()) {
      actionKey = QStringLiteral("map");
    } else if (area == uiText("Current SDO", "当前 SDO").toLower()) {
      actionKey = QStringLiteral("currentSdo");
    } else if (area == uiText("Runtime Evidence", "运行证据").toLower()) {
      actionKey = QStringLiteral("runtime");
    } else if (area == uiText("Next", "下一步").toLower()) {
      actionKey = QStringLiteral("next");
    }
  }

  if (actionKey == QStringLiteral("target")) {
    if (topologyTree_) {
      topologyTree_->setFocus();
      if (topologyTree_->currentItem()) {
        topologyTree_->scrollToItem(topologyTree_->currentItem(),
                                    QAbstractItemView::PositionAtCenter);
      }
    }
    logNavigation(uiText("Target", "目标"));
    return;
  }

  if (actionKey == QStringLiteral("gate")) {
    const int blockingRow = firstConsistencyBlockingIssueRow();
    activateWorkspaceTab(consistencyTabIndex_);
    if (blockingRow >= 0) {
      selectAndFocusTableRow(consistency_->consistencyTable, blockingRow, 0);
    } else if (!consistency_->consistencyTable || consistency_->consistencyTable->rowCount() <= 0) {
      openConsistencyView();
    }
    logNavigation(uiText("Consistency gate", "一致性门禁"));
    return;
  }

  if (actionKey == QStringLiteral("map")) {
    if (hasFailedSdoEvidence()) {
      focusFailedSdoEvidence();
      logNavigation(uiText("Failed OD evidence", "失败 OD 证据"));
      return;
    }
    if (sdo_->sdoTable && selectedPosition() >= 0 &&
        loadedSdoPosition_ == selectedPosition() && sdo_->sdoTable->rowCount() > 0) {
      activateObjectDictionaryPaneFor(sdo_->sdoTable);
      if (sdo_->sdoTable->currentRow() >= 0) {
        selectAndFocusTableRow(sdo_->sdoTable, sdo_->sdoTable->currentRow(), 1);
      }
      logNavigation(uiText("Object Dictionary", "对象字典"));
      return;
    }
    if (sdo_->pdoTable && selectedPosition() >= 0 &&
        loadedPdoPosition_ == selectedPosition() && sdo_->pdoTable->rowCount() > 0) {
      activateWorkspaceTab(pdoMapTabIndex_);
      if (sdo_->pdoTable->currentRow() >= 0) {
        selectAndFocusTableRow(sdo_->pdoTable, sdo_->pdoTable->currentRow(), 2);
      }
      logNavigation(uiText("PDO Map", "PDO 映射"));
      return;
    }
    activateObjectDictionaryPaneFor(sdo_->sdoTable);
    logNavigation(uiText("Object Dictionary", "对象字典"));
    return;
  }

  if (actionKey == QStringLiteral("currentSdo")) {
    if (currentSdoWriteDeltaReviewAvailable()) {
      reviewCurrentSdoWriteDelta();
      logNavigation(uiText("Current SDO delta", "当前 SDO 差异"));
      return;
    }
    const int dictionaryRow = currentSdoDictionaryRow();
    activateObjectDictionaryPaneFor(sdo_->sdoTable);
    if (dictionaryRow >= 0) {
      selectAndFocusTableRow(sdo_->sdoTable, dictionaryRow, 1);
    }
    logNavigation(uiText("Current SDO target", "当前 SDO 目标"));
    return;
  }

  if (actionKey == QStringLiteral("runtime")) {
    if (startupSdoRowsWithWatchDiffs().size() > 0) {
      focusStartupSdoWatchDiffs();
      logNavigation(uiText("Startup diffs", "Startup 偏差"));
      return;
    }
    if (watch_->watchTable && watch_->watchTable->rowCount() > 0) {
      activateWorkspaceTab(watchTabIndex_);
      const int watchRow = currentSdoWatchRow();
      if (watchRow >= 0) {
        selectAndFocusTableRow(watch_->watchTable, watchRow, 1);
      } else if (watch_->watchTable->currentRow() >= 0) {
        selectAndFocusTableRow(watch_->watchTable, watch_->watchTable->currentRow(), 1);
      }
      logNavigation(uiText("Watch evidence", "Watch 证据"));
      return;
    }
    activateWorkspaceTab(freeRunTabIndex_);
    if (freeRunWidgets_->freeRunEntryTable && freeRunWidgets_->freeRunEntryTable->currentRow() >= 0) {
      selectAndFocusTableRow(freeRunWidgets_->freeRunEntryTable,
                             freeRunWidgets_->freeRunEntryTable->currentRow(), 0);
    }
    logNavigation(uiText("Free Run evidence", "Free Run 证据"));
    return;
  }

  if (actionKey == QStringLiteral("next")) {
    const int nextStep = nextCommissioningWorkflowStep();
    activateWorkspaceTab(overviewTabIndex_);
    // Dispatch Alt+Enter to the correct evidence action for this table type
    if (nextStep >= 0 && workflow_->workflowTable) {
      selectAndFocusTableRow(workflow_->workflowTable, nextStep, 2);
    }
    logNavigation(uiText("Workflow next row", "工作流下一行"));
    return;
  }

  if (combined.contains("diagnostic") || combined.contains("诊断")) {
    activateWorkspaceTab(diagnosticsTabIndex_);
    logNavigation(uiText("Diagnostics", "诊断"));
  }
}


// — Recompute and display the highest-priority next-best-action suggestion
void MainWindow::updateNextBestAction() {
  if (!nextBestActionButton_) {
    return;
  }

  NextBestActionInput actionInput;
  actionInput.workflow = commissioningWorkflowInput();
  actionInput.consistencyBlockingIssueRow = firstConsistencyBlockingIssueRow();
  const SlaveEvidenceMatrixPriorityCounts priorityCounts =
      slaveEvidenceMatrixPriorityCounts(slaveEvidence_->slaveEvidenceMatrixTable);
  actionInput.matrixP0 = priorityCounts.p0;
  actionInput.matrixP1 = priorityCounts.p1;
  actionInput.matrixP2 = priorityCounts.p2;

  if (diagnostics_->diagnosticsTable) {
    for (int row = diagnostics_->diagnosticsTable->rowCount() - 1; row >= 0; --row) {
      const QString level = diagnostics_->diagnosticsTable->item(row, 1)
                                ? diagnostics_->diagnosticsTable->item(row, 1)->text()
                                : QString();
      if (level == "Error") {
        actionInput.hasDiagnosticError = true;
        break;
      }
    }
  }

  const NextBestActionDecision decision = chooseNextBestAction(actionInput);
  const NextBestActionUiState uiState =
      buildNextBestActionUiState(decision, actionInput, nextBestActionTexts());

  nextBestActionButton_->setText(uiState.text);
  nextBestActionButton_->setToolTip(uiState.tip);
  nextBestActionButton_->setStatusTip(uiState.tip);
  nextBestActionButton_->setIcon(
      style()->standardIcon(nextBestActionStandardPixmap(uiState.icon)));
  nextBestActionButton_->setEnabled(uiState.enabled);
  nextBestActionButton_->setProperty("action", uiState.actionKey);
  nextBestActionButton_->setProperty("severity", uiState.severityKey);
  repolish(nextBestActionButton_); // force QSS re-evaluation after property change
}


// — Run next best action
void MainWindow::runNextBestAction() {
  if (!nextBestActionButton_) {
    return;
  }

  const QString action = nextBestActionButton_->property("action").toString();
  if (action == nextBestActionKey(NextBestActionKind::Connect)) {
    client_.connectToDaemon();
  } else if (action == nextBestActionKey(NextBestActionKind::Rescan)) {
    client_.rescan();
    requestRefresh();
  } else if (action == nextBestActionKey(NextBestActionKind::SelectSlave)) {
    if (topologyTree_) {
      topologyTree_->setFocus();
      if (!slaves_.isEmpty() && topologyTree_->topLevelItemCount() > 0) {
        auto *master = topologyTree_->topLevelItem(0);
        if (master && master->childCount() > 0) {
          topologyTree_->setCurrentItem(master->child(0));
        }
      }
    }
  } else if (action == nextBestActionKey(NextBestActionKind::LoadOd)) {
    activateObjectDictionaryPaneFor(sdo_->sdoTable);
    if (client_.isConnected() && selectedPosition() >= 0) {
      client_.sdos(selectedPosition());
    }
  } else if (action ==
             nextBestActionKey(NextBestActionKind::FailedOdEvidence)) {
    focusFailedSdoEvidence();
  } else if (action == nextBestActionKey(NextBestActionKind::LoadPdo)) {
    activateWorkspaceTab(pdoMapTabIndex_);
    if (client_.isConnected() && selectedPosition() >= 0) {
      client_.pdos(selectedPosition());
    }
  } else if (action == nextBestActionKey(NextBestActionKind::AddWatch)) {
    activateWorkspaceTab(watchTabIndex_);
    if (selectedPosition() >= 0) {
      addCurrentSdoToWatch();
    }
  } else if (action == nextBestActionKey(NextBestActionKind::FreeRun)) {
    activateWorkspaceTab(freeRunTabIndex_);
    if (client_.isConnected() && selectedPosition() >= 0) {
      if (auto *freeRunAction = findChild<QAction *>("freeRunAction")) {
        freeRunAction->trigger();
      } else {
        setFreeRun(!freeRun_);
      }
    }
  } else if (action == nextBestActionKey(NextBestActionKind::Diagnostics)) {
    activateWorkspaceTab(diagnosticsTabIndex_);
  } else if (action == nextBestActionKey(NextBestActionKind::StartupDiffs)) {
    focusStartupSdoWatchDiffs();
  } else if (action ==
             nextBestActionKey(NextBestActionKind::ConsistencyEvidenceIssue)) {
    focusEvidenceFromConsistency(firstConsistencyBlockingIssueRow());
  } else if (action == nextBestActionKey(NextBestActionKind::Consistency)) {
    openConsistencyView();
  } else if (action == nextBestActionKey(NextBestActionKind::MatrixReview)) {
    activateWorkspaceTab(overviewTabIndex_);
    if (slaveEvidence_->slaveEvidenceMatrixScopeFilter) {
      const int allScope = slaveEvidence_->slaveEvidenceMatrixScopeFilter->findData("all");
      if (allScope >= 0) {
        slaveEvidence_->slaveEvidenceMatrixScopeFilter->setCurrentIndex(allScope);
      }
    }
    if (slaveEvidence_->slaveEvidenceMatrixFilter) {
      slaveEvidence_->slaveEvidenceMatrixFilter->clear();
    }
    reviewFirstSlaveEvidenceMatrixIssue();
  } else {
    showCommandPalette();
  }

  updateDiagnostics(
      "Info", "Navigator",
      uiText("Next Best Action executed: %1", "已执行下一最佳动作：%1")
          .arg(nextBestActionButton_->text()));
  updateNextBestAction();
}


// — Filter commissioning workflow
void MainWindow::filterCommissioningWorkflow() {
  // Dispatch Alt+Enter to the correct evidence action for this table type
  if (!workflow_->workflowTable) {
    return;
  }

  const QString needle =
      workflow_->workflowFilter ? workflow_->workflowFilter->text().trimmed() : QString();
  const QString scope =
      workflow_->workflowScopeFilter
          ? workflow_->workflowScopeFilter->currentData().toString()
          : QString::fromLatin1(kCommissioningWorkflowScopeAll);
  const CommissioningWorkflowFilterStats stats =
      filterCommissioningWorkflowTable(workflow_->workflowTable, scope, needle,
                                       uiText("None", "无"));

  const int current = workflow_->workflowTable->currentRow();
  if (current < 0 || current >= workflow_->workflowTable->rowCount() ||
      workflow_->workflowTable->isRowHidden(current)) {
    if (stats.firstVisible >= 0) {
      workflow_->workflowTable->setCurrentCell(stats.firstVisible,
                                     kCommissioningWorkflowStepColumn);
      workflow_->workflowTable->selectRow(stats.firstVisible);
    } else {
      workflow_->workflowTable->clearSelection();
    }
  }

  if (workflow_->workflowSummaryLabel) {
    const QString scopeLabel = workflow_->workflowScopeFilter
                                   ? workflow_->workflowScopeFilter->currentText()
                                   : uiText("All", "全部");
    const QString currentDetail = workflow_->workflowSummaryLabel->toolTip().trimmed();
    workflow_->workflowSummaryLabel->setToolTip(
        uiText("%1\n\nVisible workflow rows: %2/%3\nScope: %4\nOpen: %5 | "
               "Blocked: %6 | Action: %7 | Ready: %8 | Risk: %9 | Evidence "
               "gaps: %10\nFiltering is local only and does not read the bus.",
               "%1\n\n可见工作流行：%2/%3\n范围：%4\n未完成：%5 | 受阻：%6 | "
               "待执行：%7 | 就绪：%8 | 风险：%9 | 证据缺口：%10\n过滤仅在本地"
               "完成，不读取总线。")
            .arg(currentDetail.isEmpty()
                     ? uiText("Commissioning Workflow", "调试工作流")
                     : currentDetail)
            .arg(stats.visible)
            .arg(workflow_->workflowTable->rowCount())
            .arg(scopeLabel)
            .arg(stats.open)
            .arg(stats.blocked)
            .arg(stats.action)
            .arg(stats.ready)
            .arg(stats.risk)
            .arg(stats.gaps));
  }
  if (workflow_->workflowReviewButton) {
    workflow_->workflowReviewButton->setEnabled(stats.hasVisibleIssue);
  }
  if (workflow_->workflowReviewNextButton) {
    workflow_->workflowReviewNextButton->setEnabled(stats.hasVisibleIssue);
  }
  updateWorkflowStepCopyButton();
  updateWorkflowStepDetail();
}


// — Review first commissioning workflow issue
void MainWindow::reviewFirstCommissioningWorkflowIssue() {
  // Dispatch Alt+Enter to the correct evidence action for this table type
  if (!workflow_->workflowTable) {
    return;
  }
  filterCommissioningWorkflow();
  // Dispatch Alt+Enter to the correct evidence action for this table type
  const int row = firstCommissioningWorkflowIssueRow(workflow_->workflowTable);
  if (row >= 0) {
    selectAndFocusTableRow(workflow_->workflowTable, row, 2);
    updateDiagnostics(
        "Info", "Commissioning Workflow",
        uiText("Selected first visible workflow issue #%1 without running it",
               "已选择首个可见工作流问题 #%1，未执行该步骤")
            .arg(row + 1));
    statusBar()->showMessage(
        uiText("Selected first workflow issue. No bus access was requested.",
               "已选择首个工作流问题；未请求总线访问。"),
        3000);
    return;
  }
  statusBar()->showMessage(uiText("No visible workflow issue to review.",
                                  "当前没有可见工作流问题可审阅。"),
                           3000);
}


// — Review next commissioning workflow issue
void MainWindow::reviewNextCommissioningWorkflowIssue() {
  // Dispatch Alt+Enter to the correct evidence action for this table type
  if (!workflow_->workflowTable) {
    return;
  }
  filterCommissioningWorkflow();
  const int rowCount = workflow_->workflowTable->rowCount();
  if (rowCount <= 0) {
    statusBar()->showMessage(
        uiText("No workflow issue to review.", "当前没有工作流问题可审阅。"),
        3000);
    return;
  }

  const int row = nextCommissioningWorkflowIssueRow(
      workflow_->workflowTable, workflow_->workflowTable->currentRow());
  if (row >= 0) {
    selectAndFocusTableRow(workflow_->workflowTable, row, 2);
    updateDiagnostics(
        "Info", "Commissioning Workflow",
        uiText("Selected next visible workflow issue #%1 without running it",
               "已选择下一个可见工作流问题 #%1，未执行该步骤")
            .arg(row + 1));
    statusBar()->showMessage(
        uiText("Selected next workflow issue. No bus access was requested.",
               "已选择下个工作流问题；未请求总线访问。"),
        3000);
    return;
  }

  statusBar()->showMessage(uiText("No visible workflow issue to review.",
                                  "当前没有可见工作流问题可审阅。"),
                           3000);
}


// — Copy a human-readable summary of the workflow step to clipboard
bool MainWindow::copyWorkflowStepDigest(int row) {
  if (!workflow_->workflowTable || row < 0 || row >= workflow_->workflowTable->rowCount() ||
      workflow_->workflowTable->isRowHidden(row)) {
    statusBar()->showMessage(uiText("Select a visible workflow step to copy.",
                                    "请选择一条可见的工作流步骤再复制。"),
                             3000);
    return false;
  }

  QString details;
  for (int column = 0; column < workflow_->workflowTable->columnCount(); ++column) {
    if (auto *item = workflow_->workflowTable->item(row, column)) {
      details = item->toolTip().trimmed();
      if (!details.isEmpty()) {
        break;
      }
    }
  }

  QStringList lines;
  const CommissioningWorkflowTableRow workflowRow =
      commissioningWorkflowTableRowFromTable(workflow_->workflowTable, row);
  lines << uiText("NekoEcat Studio Commissioning Workflow Step",
                  "NekoEcat Studio 调试工作流步骤");
  lines << QString("%1: %2").arg(uiText("Master", "主站"), activeMasterName());
  lines << QString("%1: %2").arg(uiText("Row", "行"), row + 1);
  lines << QString("%1: %2").arg(uiText("Phase", "阶段"), workflowRow.phase);
  lines << QString("%1: %2").arg(uiText("Status", "状态"), workflowRow.status);
  lines << QString("%1: %2").arg(uiText("Step", "步骤"), workflowRow.step);
  lines << QString("%1: %2").arg(uiText("Risk", "风险"), workflowRow.risk);
  lines << QString("%1: %2").arg(uiText("Evidence", "依据"),
                                 workflowRow.evidence);
  lines << QString("%1: %2").arg(uiText("Next Action", "下一步动作"),
                                 workflowRow.nextAction);
  if (auto *runNextButton = findChild<QPushButton *>("overviewRunNext")) {
    lines << QString("%1: %2").arg(
        uiText("Workflow Run Next", "工作流执行下一步"),
        runNextButton->text().trimmed());
  }
  if (nextBestActionButton_) {
    lines << QString("%1: %2").arg(
        uiText("Current Next Best Action", "当前下一最佳动作"),
        nextBestActionButton_->text().trimmed());
  }
  if (workflow_->workflowSummaryLabel &&
      !workflow_->workflowSummaryLabel->text().trimmed().isEmpty()) {
    lines << QString("%1: %2").arg(uiText("Workflow Summary", "工作流摘要"),
                                   workflow_->workflowSummaryLabel->text().trimmed());
  }
  if (!details.isEmpty()) {
    lines << QString();
    lines << uiText("Step Detail", "步骤详情");
    const QStringList detailLines = details.split('\n', Qt::SkipEmptyParts);
    for (const QString &detail : detailLines) {
      lines << QString("- %1").arg(detail.trimmed());
    }
  }

  lines << QString();
  lines << uiText(
      "Local Action: Copy Step Evidence prepares this workflow row for "
      "handoff or pre-action review.",
      "本地动作：复制步骤证据用于交接或执行前复核该工作流行。");
  lines << uiText("Boundary: clipboard copy only; no bus read, no OD/PDO/ESI "
                  "load, no SDO write, no state change, no Free Run change, "
                  "and no Host Health.",
                  "边界：只复制到剪贴板；不读取总线、不加载 OD/PDO/ESI、不写 "
                  "SDO、不切换状态、不改变 Free Run，也不运行 Host Health。");

  QApplication::clipboard()->setText(lines.join('\n')); // copy to system clipboard
  updateDiagnostics("Info", "Commissioning Workflow",
                    uiText("Copied commissioning workflow step #%1 to "
                           "clipboard",
                           "已复制调试工作流第 %1 步到剪贴板")
                        .arg(row + 1));
  statusBar()->showMessage(
      uiText("Copied workflow step. No bus access was requested.",
             "已复制工作流步骤；未请求总线访问。"),
      3000);
  return true;
}


// — Return the next commissioning workflow step
int MainWindow::nextCommissioningWorkflowStep() const {
  return nextCommissioningWorkflowStepIndex(commissioningWorkflowInput());
}


// — Run next commissioning workflow step
void MainWindow::runNextCommissioningWorkflowStep() {
  const int row = nextCommissioningWorkflowStep();
  if (row < 0) {
    updateDiagnostics("Info", "Workflow",
                      uiText("Commissioning workflow is already ready",
                             "调试工作流当前已就绪"));
    return;
  }
  runCommissioningWorkflowStep(row);
}


// — Run commissioning workflow step
void MainWindow::runCommissioningWorkflowStep(int row) {
  if (!tabs_) {
    return;
  }

  switch (commissioningWorkflowStepForIndex(row)) {
  case CommissioningWorkflowStep::ConnectRuntime:
    if (!client_.isConnected()) {
      client_.connectToDaemon();
    } else {
      requestRefresh();
    }
    break;
  case CommissioningWorkflowStep::ScanTopology:
    if (client_.isConnected()) {
      client_.rescan();
      requestRefresh();
    }
    break;
  case CommissioningWorkflowStep::SelectSlave:
    topologyTree_->setFocus();
    if (topologyTree_->currentItem()) {
      topologyTree_->scrollToItem(topologyTree_->currentItem());
    }
    break;
  case CommissioningWorkflowStep::InspectObjectDictionary:
    activateObjectDictionaryPaneFor(sdo_->sdoTable);
    if (client_.isConnected() && selectedPosition() >= 0) {
      client_.sdos(selectedPosition());
    }
    break;
  case CommissioningWorkflowStep::ReviewObjectDictionaryEvidence:
    focusFailedSdoEvidence();
    break;
  case CommissioningWorkflowStep::ReviewPdoMap:
    activateWorkspaceTab(pdoMapTabIndex_);
    if (client_.isConnected() && selectedPosition() >= 0) {
      client_.pdos(selectedPosition());
    }
    break;
  case CommissioningWorkflowStep::MonitorWatch:
    activateWorkspaceTab(watchTabIndex_);
    if (selectedPosition() >= 0) {
      addCurrentSdoToWatch();
    }
    break;
  case CommissioningWorkflowStep::ReviewStartupDiffs:
    focusStartupSdoWatchDiffs();
    break;
  case CommissioningWorkflowStep::RunConsistencyGate: {
    int consistencyErrors = 0;
    int consistencyWarnings = 0;
    consistencyIssueCounts(&consistencyErrors, &consistencyWarnings, nullptr,
                           nullptr);
    const bool hasConsistencyCheck =
        consistencyFresh_ && consistencyCheckAvailable();
    const bool hasConsistencyBlockingIssues = consistencyHasBlockingIssues(
        {consistencyErrors, consistencyWarnings, 0, 0});
    if (hasConsistencyCheck && hasConsistencyBlockingIssues) {
      focusEvidenceFromConsistency(firstConsistencyBlockingIssueRow());
    } else {
      openConsistencyView();
    }
  } break;
  case CommissioningWorkflowStep::ValidateProcessImage:
    activateWorkspaceTab(freeRunTabIndex_);
    if (client_.isConnected() && selectedPosition() >= 0) {
      if (auto *action = findChild<QAction *>("freeRunAction")) {
        action->trigger();
      } else {
        setFreeRun(!freeRun_);
      }
    }
    break;
  }
  updateCommissioningWorkflow();
  updateSelectedDriveSummary();
}


// — Refresh the host health label from the latest host check result
void MainWindow::updateHostHealth(const QJsonArray &checks) {
  if (!hostHealthTable_) {
    return;
  }

  const HostHealthUiState uiState =
      buildHostHealthUiState(checks, hostHealthTexts());
  setTableRows(hostHealthTable_, uiState.headers, uiState.rows);

  for (int row = 0; row < hostHealthTable_->rowCount(); ++row) {
    const QColor color = hostHealthColorForKey(uiState.colorKeys.value(row));
    for (int column = 0; column < hostHealthTable_->columnCount(); ++column) {
      if (auto *item = hostHealthTable_->item(row, column)) {
        item->setForeground(color);
      }
    }
  }
  hostHealthTable_->resizeColumnsToContents(); // auto-fit column widths

  if (diagnostics_->hostHealthSummaryLabel) {
    diagnostics_->hostHealthSummaryLabel->setText(uiState.summary);
  }
  updateActionAvailability();
  updateCommissioningWorkflow();
}


// — Recount diagnostics events by level and update the summary label
void MainWindow::updateDiagnosticsSummary() {
  if (!diagnostics_->diagnosticsSummaryLabel || !diagnostics_->diagnosticsTable) {
    return;
  }

  QList<DiagnosticsEventRowState> rows;
  rows.reserve(diagnostics_->diagnosticsTable->rowCount());
  for (int row = 0; row < diagnostics_->diagnosticsTable->rowCount(); ++row) {
    const QString level = diagnostics_->diagnosticsTable->item(row, 1)
                              ? diagnostics_->diagnosticsTable->item(row, 1)->text()
                              : QString();
    rows.append({level, !diagnostics_->diagnosticsTable->isRowHidden(row)});
  }
  diagnostics_->diagnosticsSummaryLabel->setText(
      diagnosticsEventSummary(rows, diagnosticsEventTexts()).text);
  updateTabBadges();
}


// — Apply foreground color to a diagnostics table row based on severity level
void MainWindow::styleDiagnosticsRow(int row, const QString &level) {
  if (!diagnostics_->diagnosticsTable || row < 0 || row >= diagnostics_->diagnosticsTable->rowCount()) {
    return;
  }
  const QColor color =
      diagnosticsEventColorForKey(diagnosticsEventColorKey(level));
  for (int column = 0; column < diagnostics_->diagnosticsTable->columnCount(); ++column) {
    if (auto *item = diagnostics_->diagnosticsTable->item(row, column)) {
      item->setForeground(color);
    }
  }
}


// — Append a timestamped entry to the diagnostics log table
void MainWindow::updateDiagnostics(const QString &level, const QString &source,
                                   const QString &message) {
  if (!diagnostics_->diagnosticsTable->columnCount()) {
    diagnostics_->diagnosticsTable->setColumnCount(4);
    diagnostics_->diagnosticsTable->setHorizontalHeaderLabels(
        diagnosticsEventHeaders(diagnosticsEventTexts()));
  }
  const int row = diagnostics_->diagnosticsTable->rowCount();
  diagnostics_->diagnosticsTable->insertRow(row);
  diagnostics_->diagnosticsTable->setItem(
      row, 0,
    // Create table cell
      new QTableWidgetItem(QDateTime::currentDateTime().toString("HH:mm:ss")));
    // Create table cell
  diagnostics_->diagnosticsTable->setItem(row, 1, new QTableWidgetItem(level));
    // Create table cell
  diagnostics_->diagnosticsTable->setItem(row, 2, new QTableWidgetItem(source));
    // Create table cell
  diagnostics_->diagnosticsTable->setItem(row, 3, new QTableWidgetItem(message));
  styleDiagnosticsRow(row, level);
  filterDiagnosticsTable();
  diagnostics_->diagnosticsTable->resizeColumnsToContents(); // auto-fit column widths
  updateNextBestAction();
}


// — Recompute badge counts and tooltips for all workspace tabs
void MainWindow::updateTabBadges() {
  if (!tabs_) {
    return;
  }

  // Helper to set text and tooltip for a tab by index
  auto setTab = [this](int index, const QString &text, const QString &tip) {
    if (index >= 0 && index < tabs_->count()) {
      tabs_->setTabText(index, text);
      tabs_->setTabToolTip(index, tip);
    }
  };

  const WorkspaceTabBadgeCounts badgeCounts = workspaceTabBadgeCounts(
      {.watchTable = watch_->watchTable,
       .startupSdoTable = startupSdoTable_,
       .freeRunEntryTable = freeRunWidgets_->freeRunEntryTable,
       .ioVariableTable = ioVar_->ioVariableTable,
       .consistencyTable = consistency_->consistencyTable,
       .stateMachineTable = stateMachine_->stateMachineTable,
       .diagnosticsTable = diagnostics_->diagnosticsTable,
       .slaveEvidenceMatrixTable = slaveEvidence_->slaveEvidenceMatrixTable});
  const WorkspaceTabBadgeUiState badges =
      buildWorkspaceTabBadgeUiState(badgeCounts, workspaceTabBadgeTexts());

  setTab(overviewTabIndex_, badges.overview.text, badges.overview.tip);
  setTab(watchTabIndex_, badges.watch.text, badges.watch.tip);
  setTab(startupSdoTabIndex_, badges.startupSdo.text, badges.startupSdo.tip);
  setTab(freeRunTabIndex_, badges.freeRun.text, badges.freeRun.tip);
  setTab(ioVariableTabIndex_, badges.ioVariables.text, badges.ioVariables.tip);
  setTab(consistencyTabIndex_, badges.consistency.text, badges.consistency.tip);
  setTab(stateMachineTabIndex_, badges.stateMachine.text,
         badges.stateMachine.tip);
  setTab(diagnosticsTabIndex_, badges.diagnostics.text, badges.diagnostics.tip);
}


// — Refresh the status bar with connection, master, slave, and Free Run state
void MainWindow::updateStatusBar() {
  if (!statusSummaryLabel_) {
    return;
  }
  statusSummaryLabel_->setText(
      QString("%1: %2 | %3: %4 | %5: %6 | %7: %8")
          .arg(uiText("Runtime", "运行时"))
          .arg(client_.isConnected() ? uiText("connected", "已连接")
                                     : uiText("offline", "离线"))
          .arg(uiText("Master", "主站"))
          .arg(activeMasterName())
          .arg(uiText("Slaves", "从站"))
          .arg(slaves_.size())
          .arg(uiText("Free Run", "自由运行"))
          .arg(freeRun_ ? uiText("On", "开启") : uiText("Off", "关闭")));
  updateWorkspaceBoundary();
  updateNextBestAction();
  updateTabBadges();
  updateSessionBrief();
}


// — Update the workspace boundary label with the current tab's access scope
void MainWindow::updateWorkspaceBoundary() {
  if (!workspaceBoundaryLabel_ || !tabs_) {
    return;
  }

  const SlaveEvidenceMatrixPriorityCounts priorityCounts =
      slaveEvidenceMatrixPriorityCounts(slaveEvidence_->slaveEvidenceMatrixTable);

  const QString workspaceName = tabs_->tabText(tabs_->currentIndex())
                                    .remove(QRegularExpression(R"( !?\d+$)"));
  const WorkspaceBoundaryUiState state = buildWorkspaceBoundaryUiState(
      workspaceBoundaryKindForPage(tabs_->currentWidget()), workspaceName,
      {.matrixP0 = priorityCounts.p0,
       .matrixP1 = priorityCounts.p1,
       .matrixP2 = priorityCounts.p2,
       .matrixP3 = priorityCounts.p3},
      workspaceBoundaryTexts());

  workspaceBoundaryLabel_->setText(state.label);
  workspaceBoundaryLabel_->setToolTip(state.tooltip);
  workspaceBoundaryLabel_->setStatusTip(workspaceBoundaryLabel_->toolTip());
  workspaceBoundaryLabel_->setProperty("severity", state.severityKey);
  repolish(workspaceBoundaryLabel_); // force QSS re-evaluation after property change
}

