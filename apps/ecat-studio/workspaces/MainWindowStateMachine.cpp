// State machine recommendations, host diagnostics, and slave operations.
// Main application window: workspace tabs, toolbars, wiring, and all workspace methods.
#include "MainWindow.h"
#include "Cia402DriveModel.h"
#include "CommissioningWorkflowModel.h"
#include "CommissioningWorkflowStepDetailUiState.h"
#include "CommissioningWorkflowTableAdapter.h"
#include "CommissioningWorkflowUiState.h"
#include "ConsistencyDetailUiState.h"
#include "ConsistencyEvidenceRouteModel.h"
#include "ConsistencyGateModel.h"
#include "ConsistencyTableAdapter.h"
#include "DiagnosticsEventUiState.h"
#include "EvidenceStatusModel.h"
#include "FreeRunEntryDetailUiState.h"
#include "HostHealthUiState.h"
#include "IoVariableBulkNamingModel.h"
#include "IoVariableDetailUiState.h"
#include "IoVariableFilterModel.h"
#include "IoVariableHandoffModel.h"
#include "NextBestActionModel.h"
#include "NextBestActionUiState.h"
#include "ObjectBookmarkDetailUiState.h"
#include "PdoMapDetailUiState.h"
#include "ProcessDataRowModel.h"
#include "ProcessDataTableAdapter.h"
#include "SdoDictionaryTableAdapter.h"
#include "SdoEvidenceModel.h"
#include "SdoEvidenceTableAdapter.h"
#include "SdoHistoryRowDetailUiState.h"
#include "SdoTargetPanelRouteModel.h"
#include "SdoTargetTrailDetailUiState.h"
#include "SelectedDriveSummaryUiState.h"
#include "SelectedSlaveEvidenceSummaryUiState.h"
#include "SessionBriefModel.h"
#include "SessionBriefTableAdapter.h"
#include "SessionBriefUiState.h"
#include "SlaveEvidenceModel.h"
#include "SlaveEvidenceTableAdapter.h"
#include "SlaveEvidenceUiState.h"
#include "StartupSdoRowDetailUiState.h"
#include "StateMachineRowDetailUiState.h"
#include "StateMachineTableAdapter.h"
#include "StateRecommendationModel.h"
#include "StudioDocumentation.h"
#include "StudioTableHelpers.h"
#include "StudioTextHelpers.h"
#include "StudioUiHelpers.h"
#include "TopologyBaselineModel.h"
#include "TopologyChangeModel.h"
#include "WatchRowDetailUiState.h"
#include "WatchStartupModel.h"
#include "WatchStartupTableAdapter.h"
#include "WatchStartupUiState.h"
#include "WorkspaceBoundaryUiState.h"
#include "WorkspaceTabBadgeTableAdapter.h"
#include "WorkspaceTabBadgeUiState.h"

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
// Recommend the next EtherCAT state for a slave based on current state and diagnostics.
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

QString MainWindow::recommendedEthercatState(const SlaveInfo &slave) const {
  const int position = slave.position;
  const QString state = slave.state.trimmed().toUpper();
  const bool pdoLoaded =
      loadedPdoPosition_ == position && sdo_->pdoTable && sdo_->pdoTable->rowCount() > 0;

  int watchValueRows = 0;
  if (watch_->watchTable) {
    for (int row = 0; row < watch_->watchTable->rowCount(); ++row) {
      if (tableText(watch_->watchTable, row, 1).toInt() != position) {
        continue;
      }
      if (!tableText(watch_->watchTable, row, 4).isEmpty()) {
        ++watchValueRows;
      }
    }
  }

  int startupDiffs = 0;
  if (startupSdoTable_) {
    for (int row = 0; row < startupSdoTable_->rowCount(); ++row) {
      if (tableText(startupSdoTable_, row, 0).toInt() != position) {
        continue;
      }
      if (hasStartupDiffEvidence(tableText(startupSdoTable_, row, 8))) {
        ++startupDiffs;
      }
    }
// Rebuild the state machine recommendation table from slave topology and diagnostics.
  }

  int freeRunRows = 0;
  int mapIssues = 0;
  if (freeRunWidgets_->freeRunEntryTable) {
    for (int row = 0; row < freeRunWidgets_->freeRunEntryTable->rowCount(); ++row) {
      if (tableText(freeRunWidgets_->freeRunEntryTable, row, 0).toInt() != position) {
        continue;
      }
      ++freeRunRows;
      if (hasPdoMapIssueEvidence(tableText(freeRunWidgets_->freeRunEntryTable, row, 13))) {
        ++mapIssues;
      }
    }
  }

  int consistencyErrors = 0;
  int consistencyWarnings = 0;
  bool consistencyOk = false;
  if (state.contains("SAFEOP")) {
    consistencyIssueCounts(&consistencyErrors, &consistencyWarnings, nullptr,
                           nullptr);
    consistencyOk = consistencyFresh_ && consistencyCheckAvailable() &&
                    !consistencyHasBlockingIssues(
                        {consistencyErrors, consistencyWarnings, 0, 0});
  }

  EthercatStateEvidence evidence;
  evidence.currentState = state;
  evidence.pdoLoaded = pdoLoaded;
  evidence.watchValueRows = watchValueRows;
  evidence.startupDiffs = startupDiffs;
  evidence.freeRunRows = freeRunRows;
  evidence.mapIssues = mapIssues;
  evidence.consistencyOk = consistencyOk;
  return ::recommendedEthercatState(evidence);
}

// Rebuild the state-machine table with per-slave state, evidence score, and recommendations
void MainWindow::updateStateMachineView() {
  if (!stateMachine_->stateMachineTable) {
    return;
  }

  int previousPosition = selectedPosition();
  if (stateMachine_->stateMachineTable->currentRow() >= 0) {
    const int rowPosition = stateMachinePositionFromTable(
        stateMachine_->stateMachineTable, stateMachine_->stateMachineTable->currentRow());
    if (rowPosition >= 0) {
      previousPosition = rowPosition;
    }
  }

  QList<QStringList> rows;
  int op = 0;
  int safeop = 0;
  int preop = 0;
  int init = 0;
  int other = 0;
  int recommended = 0;
  int riskRows = 0;

  const QStringList topologyIssues = topologyBaselineIssues();
  for (const auto &slave : slaves_) {
    const int position = slave.position;
    const QString state = slave.state.trimmed().isEmpty()
                              ? uiText("Unknown", "未知")
                              : slave.state.trimmed();
    const QString normalizedState = state.toUpper();
    if (normalizedState.contains("SAFEOP")) {
      ++safeop;
    } else if (normalizedState.contains("PREOP")) {
      ++preop;
    } else if (normalizedState.contains("INIT")) {
      ++init;
    } else if (normalizedState == "OP" || normalizedState.startsWith("OP ")) {
      ++op;
    } else {
      ++other;
    }

    const int identityRows =
        loadedSlaveInfoPosition_ == position && identityTable_
            ? identityTable_->rowCount()
            : 0;
    const int odRows =
        loadedSdoPosition_ == position && sdo_->sdoTable ? sdo_->sdoTable->rowCount() : 0;
    const int pdoRows =
        loadedPdoPosition_ == position && sdo_->pdoTable ? sdo_->pdoTable->rowCount() : 0;

    int watchRows = 0;
    int watchValueRows = 0;
    QString statusword;
    QString modeDisplay;
    QString errorCode;
    if (watch_->watchTable) {
      for (int row = 0; row < watch_->watchTable->rowCount(); ++row) {
        if (tableText(watch_->watchTable, row, 1).toInt() != position) {
          continue;
        }
        ++watchRows;
        const QString value = tableText(watch_->watchTable, row, 4);
        const QString decoded = tableText(watch_->watchTable, row, 5);
        if (!value.isEmpty()) {
          ++watchValueRows;
        }
        const QString index =
            normalizeHexText(tableText(watch_->watchTable, row, 2), 4);
        if (index == "0x6041" && !decoded.isEmpty()) {
          statusword = decoded;
        } else if (index == "0x6061" && !decoded.isEmpty()) {
          modeDisplay = decoded;
        } else if (index == "0x603f" && !decoded.isEmpty() && value != "0" &&
                   value.toLower() != "0x0000") {
          errorCode = decoded;
        }
      }
    }

    int startupRows = 0;
    int startupDiffs = 0;
    if (startupSdoTable_) {
      for (int row = 0; row < startupSdoTable_->rowCount(); ++row) {
        if (tableText(startupSdoTable_, row, 0).toInt() != position) {
          continue;
        }
        ++startupRows;
        if (hasStartupDiffEvidence(tableText(startupSdoTable_, row, 8))) {
          ++startupDiffs;
        }
      }
    }

    int freeRunRows = 0;
    int mapIssues = 0;
    if (freeRunWidgets_->freeRunEntryTable) {
      for (int row = 0; row < freeRunWidgets_->freeRunEntryTable->rowCount(); ++row) {
        if (tableText(freeRunWidgets_->freeRunEntryTable, row, 0).toInt() != position) {
          continue;
        }
        ++freeRunRows;
        if (hasPdoMapIssueEvidence(tableText(freeRunWidgets_->freeRunEntryTable, row, 13))) {
          ++mapIssues;
        }
      }
    }

    QStringList evidence;
    evidence << (identityRows > 0 ? uiText("ID ready", "身份就绪")
                                  : uiText("ID missing", "身份缺失"));
    evidence << uiText("OD %1", "OD %1").arg(odRows);
    evidence << uiText("Watch %1/%2 values", "Watch %1/%2 有值")
                    .arg(watchValueRows)
                    .arg(watchRows);

    QStringList driveFacts;
    if (!statusword.isEmpty()) {
      driveFacts << statusword;
    }
    if (!modeDisplay.isEmpty()) {
      driveFacts << uiText("mode %1", "模式 %1").arg(modeDisplay);
    }
    if (!errorCode.isEmpty()) {
      driveFacts << errorCode;
    }
    const QString drive = driveFacts.isEmpty()
                              ? uiText("No CiA 402 Watch", "无 CiA 402 监视")
                              : driveFacts.join(" | ");

    QStringList risks;
    if (!client_.isConnected()) {
      risks << uiText("runtime offline", "运行时离线");
    }
    if (identityRows <= 0) {
      risks << uiText("identity missing", "身份缺失");
    }
    if ((normalizedState.contains("PREOP") ||
         normalizedState.contains("SAFEOP")) &&
        pdoRows <= 0) {
      risks << uiText("PDO missing", "PDO 缺失");
    }
    if ((normalizedState.contains("PREOP") ||
         normalizedState.contains("SAFEOP")) &&
        watchValueRows <= 0) {
      risks << uiText("Watch missing", "Watch 缺失");
    }
    if (normalizedState.contains("SAFEOP") && freeRunRows <= 0) {
      risks << uiText("process evidence missing", "过程证据缺失");
    }
    if (startupDiffs > 0) {
      risks << uiText("Startup diff %1", "启动偏差 %1").arg(startupDiffs);
    }
    if (mapIssues > 0) {
      risks << uiText("PDO map issue %1", "PDO 映射问题 %1").arg(mapIssues);
    }
    if (!topologyIssues.isEmpty()) {
      risks << uiText("topology baseline issue", "拓扑基线问题");
    }
    if (hasDriveFaultEvidence(drive)) {
      risks << uiText("drive fault evidence", "驱动故障证据");
    }

    const QString target = recommendedEthercatState(slave);
    if (!target.isEmpty()) {
      ++recommended;
    }
    if (!risks.isEmpty()) {
      ++riskRows;
    }
    const QString action =
        !target.isEmpty()
            ? uiText("Send %1", "发送 %1").arg(target)
            : ((normalizedState == "OP" || normalizedState.startsWith("OP ")) &&
                       risks.isEmpty()
                   ? uiText("Ready", "就绪")
                   : uiText("Review evidence", "复核证据"));

    rows.append({QString::number(position),
                 slave.name.trimmed().isEmpty() ? uiText("Unnamed", "未命名")
                                                : slave.name.trimmed(),
                 state, target, evidence.join(" | "), drive,
                 uiText("%1 row(s), %2 diff(s)", "%1 行，%2 个偏差")
                     .arg(startupRows)
                     .arg(startupDiffs),
                 uiText("PDO %1 | Free Run %2 | Map issue %3",
                        "PDO %1 | Free Run %2 | 映射问题 %3")
                     .arg(pdoRows)
                     .arg(freeRunRows)
                     .arg(mapIssues),
                 risks.join("; "), action});
  }

  setTableRows(stateMachine_->stateMachineTable,
               {uiText("Slave", "从站"), uiText("Name", "名称"),
                uiText("Current", "当前"), uiText("Recommended", "推荐"),
                uiText("Evidence", "证据"), uiText("Drive", "驱动"),
                uiText("Startup", "启动"), uiText("PDO/Process", "PDO/过程"),
                uiText("Risk", "风险"), uiText("Action", "动作")},
               rows);

  const QColor okColor("#22c55e");
  const QColor actionColor("#f59e0b");
  const QColor warningColor("#ef4444");
  const QColor infoColor("#60a5fa");
  int restoreRow = -1;
  for (int row = 0; row < stateMachine_->stateMachineTable->rowCount(); ++row) {
    const StateMachineTableRow tableRow =
        stateMachineTableRowFromTable(stateMachine_->stateMachineTable, row);
    int rowPosition = -1;
    if (stateMachineTableRowPosition(tableRow, &rowPosition) &&
        rowPosition == previousPosition) {
      restoreRow = row;
    }
    const QString current = tableRow.current.toUpper();
    const QString target = tableRow.recommended;
    const QString risk = tableRow.risk;
    const QColor currentColor =
        (current == "OP" || current.startsWith("OP "))
            ? okColor
            : (current.contains("SAFEOP") || current.contains("PREOP")
                   ? actionColor
                   : infoColor);
    if (auto *item = stateMachine_->stateMachineTable->item(row, 2)) {
      item->setForeground(currentColor);
    }
    if (auto *item = stateMachine_->stateMachineTable->item(row, 3)) {
      item->setForeground(target.isEmpty() ? QColor("#64748b") : actionColor);
    }
    if (auto *item = stateMachine_->stateMachineTable->item(row, 8)) {
      item->setForeground(risk.isEmpty() ? okColor : warningColor);
    }
    if (auto *item = stateMachine_->stateMachineTable->item(row, 9)) {
      item->setForeground(target.isEmpty() ? infoColor : actionColor);
    }
  }
  if (restoreRow >= 0) {
    stateMachine_->stateMachineTable->setCurrentCell(restoreRow, 0);
  }
  stateMachine_->stateMachineTable->resizeColumnsToContents();

  if (stateMachine_->stateMachineSummaryLabel) {
    const QString summary =
        slaves_.isEmpty()
            ? uiText("No slaves in current scan", "当前扫描没有从站")
            : uiText("Slaves %1 | OP %2 SAFEOP %3 PREOP %4 INIT %5 other %6 | "
                     "recommended %7 | risk rows %8",
                     "从站 %1 | OP %2 SAFEOP %3 PREOP %4 INIT %5 其他 %6 | "
                     "推荐 %7 | 风险行 %8")
                  .arg(slaves_.size())
                  .arg(op)
                  .arg(safeop)
                  .arg(preop)
                  .arg(init)
                  .arg(other)
                  .arg(recommended)
                  .arg(riskRows);
    stateMachine_->stateMachineSummaryLabel->setText(summary);
    stateMachine_->stateMachineSummaryLabel->setToolTip(
        topologyIssues.isEmpty()
            ? uiText(
// Update the state machine detail panel for the currently selected row.
                  "State recommendations are based on slave state, loaded "
                  "OD/PDO evidence, Watch values, Startup diffs, Free Run "
                  "process evidence, and PDO map evidence.",
                  "状态推荐基于从站状态、已加载 OD/PDO 证据、Watch 值、Startup "
                  "偏差、Free Run 过程证据和 PDO 映射证据。")
            : uiText("Topology baseline issue(s):\n%1", "拓扑基线问题：\n%1")
                  .arg(topologyIssues.join('\n')));
    stateMachine_->stateMachineSummaryLabel->setProperty(
        "severity",
        riskRows > 0 ? "warning" : (recommended > 0 ? "action" : "ok"));
    repolish(stateMachine_->stateMachineSummaryLabel);
  }

  const bool canSend = client_.isConnected() && !slaves_.isEmpty();
  const bool hasRecommendedState =
      canSend && stateMachine_->stateMachineTable->currentRow() >= 0 &&
      stateMachineRowHasRecommendation(stateMachine_->stateMachineTable,
                                       stateMachine_->stateMachineTable->currentRow());
  for (const char *name : {"stateSelectedNext", "stateSelectedPreOp",
                           "stateSelectedSafeOp", "stateSelectedOp"}) {
    if (auto *button = findChild<QPushButton *>(name)) {
      button->setEnabled(QString::fromLatin1(name) == "stateSelectedNext"
                             ? hasRecommendedState
                             : canSend &&
                                   stateMachine_->stateMachineTable->currentRow() >= 0);
    }
  }
  for (const char *name : {"stateAllPreOp", "stateAllSafeOp"}) {
    if (auto *button = findChild<QPushButton *>(name)) {
      button->setEnabled(canSend);
// Request a slave state transition with user confirmation and evidence summary.
    }
  }
  updateStateMachineRowDetail();
}

// Update the detail strip below the state-machine table for the current row
void MainWindow::updateStateMachineRowDetail() {
  if (!stateMachine_->stateMachineDetailLabel) {
    return;
  }
  const StateMachineRowDetailTexts texts = stateMachineRowDetailTexts();
  auto applyState = [this](const StateMachineRowDetailUiState &state) {
    stateMachine_->stateMachineDetailLabel->setText(state.text);
    stateMachine_->stateMachineDetailLabel->setProperty("severity", state.severityKey);
    stateMachine_->stateMachineDetailLabel->setToolTip(state.tooltip);
    repolish(stateMachine_->stateMachineDetailLabel);
  };

  if (!stateMachine_->stateMachineTable) {
    applyState(stateMachineRowDetailUnavailableState(texts));
    return;
  }

  const int row = stateMachine_->stateMachineTable->currentRow();
  if (row < 0 || row >= stateMachine_->stateMachineTable->rowCount() ||
      stateMachine_->stateMachineTable->isRowHidden(row)) {
    applyState(stateMachineRowDetailNoSelectionState(texts));
    return;
  }

  applyState(buildStateMachineRowDetailUiState(
      stateMachineTableRowFromTable(stateMachine_->stateMachineTable, row), texts));
}

// Request a single-slave state change with safety confirmation dialog
void MainWindow::requestSlaveStateWithConfirmation(int position,
                                                   const QString &state) {
  if (!client_.isConnected() || position < 0) {
    return;
  }
// Broadcast a state transition command to all slaves (INIT/PREOP/SAFEOP/OP).

  QString currentState = uiText("Unknown", "未知");
  QString slaveName = uiText("Unnamed", "未命名");
  for (const auto &slave : slaves_) {
    if (slave.position == position) {
      currentState = slave.state.trimmed().isEmpty() ? currentState
                                                     : slave.state.trimmed();
      slaveName =
          slave.name.trimmed().isEmpty() ? slaveName : slave.name.trimmed();
      break;
    }
  }

  QStringList details = {
      uiText("Master: %1", "主站：%1").arg(activeMasterName()),
      uiText("Slave: #%1 %2", "从站：#%1 %2").arg(position).arg(slaveName),
      uiText("Current state: %1", "当前状态：%1").arg(currentState),
      uiText("Requested state: %1", "目标状态：%1").arg(state),
      uiText("Confirm machine safety, drive enable state, and expected PDO "
             "outputs before continuing.",
             "继续前请确认设备安全、驱动使能状态和预期 PDO 输出。"),
  };
  details << stateTransitionImpactDetails(position, state);
  if (!confirmDangerousOperation(
          uiText("Confirm Slave State Change", "确认从站状态切换"),
          uiText("This operation changes the EtherCAT state of the selected "
                 "slave.",
                 "此操作会切换选中从站的 EtherCAT 状态。"),
          details, uiText("Send State Request", "发送状态请求"))) {
    return;
  }
  client_.setState(position, state);
}

// Broadcast a state request to every detected slave with confirmation
void MainWindow::requestAllSlaveState(const QString &state) {
  if (!client_.isConnected() || slaves_.isEmpty()) {
    return;
  }

  int op = 0;
  int safeop = 0;
  int preop = 0;
  int init = 0;
  int other = 0;
  for (const auto &slave : slaves_) {
    const QString current = slave.state.trimmed().toUpper();
    if (current == "OP") {
      ++op;
    } else if (current == "SAFEOP") {
      ++safeop;
    } else if (current == "PREOP") {
      ++preop;
    } else if (current == "INIT") {
      ++init;
    } else {
      ++other;
    }
  }

  QStringList details = {
      uiText("Master: %1", "主站：%1").arg(activeMasterName()),
      uiText("Detected slaves: %1", "检测到从站：%1").arg(slaves_.size()),
      uiText("Requested state: %1", "目标状态：%1").arg(state),
      uiText("Current state mix: OP %1, SAFEOP %2, PREOP %3, INIT %4, other "
             "%5",
             "当前状态分布：OP %1，SAFEOP %2，PREOP %3，INIT %4，其他 %5")
          .arg(op)
          .arg(safeop)
          .arg(preop)
          .arg(init)
          .arg(other),
      uiText("This sends a state request to every detected slave on the active "
             "master.",
             "此操作会向当前主站下全部已检测从站发送状态请求。"),
  };
  const QStringList topologyIssues = topologyBaselineIssues();
  if (!topologyIssues.isEmpty()) {
    details << uiText("Topology baseline: %1 issue(s); review before changing "
                      "all slaves",
// Run host diagnostics and update the health summary.
                      "拓扑基线：%1 个问题；切换全部从站前请复核")
                   .arg(topologyIssues.size());
  }
  if (state.trimmed().compare("OP", Qt::CaseInsensitive) == 0) {
    details << uiText("Risk: OP can make outputs and drive behavior active on "
                      "every detected slave",
                      "风险：OP 可能让所有已检测从站的输出和驱动行为生效");
  }
  if (state.trimmed().compare("OP", Qt::CaseInsensitive) == 0 ||
      state.trimmed().compare("SAFEOP", Qt::CaseInsensitive) == 0) {
// Copy the selected host diagnostic command to clipboard.
    details << consistencyGateDetails(
        uiText("all-slave state change", "全部从站状态切换"));
  }
  if (!confirmDangerousOperation(
          uiText("Confirm All-Slave State Change", "确认全部从站状态切换"),
          uiText("This operation changes every detected slave on the active "
                 "EtherCAT bus.",
                 "此操作会切换当前 EtherCAT 总线上所有已检测从站。"),
          details, uiText("Send All-State Request", "发送全部状态请求"))) {
    return;
  }

  updateDiagnostics("Warning", "State",
                    QString("All-slave state request: %1 on %2")
                        .arg(state, activeMasterName()));
  client_.setAllStates(state);
}

// Trigger a host-level health check (network, kernel, IgH module)
void MainWindow::runHostDiagnostics() {
// Capture a point-in-time snapshot of the selected slave's configuration.
  if (!client_.isConnected()) {
    updateDiagnostics("Warning", "Host",
                      "Host check skipped: runtime is not connected");
    return;
  }

  updateDiagnostics("Info", "Host", "Host check requested");
  client_.hostDiagnostics();
}

// Copy the fix command for the selected host health issue to clipboard
void MainWindow::copySelectedHostCommand() {
  if (!hostHealthTable_ || hostHealthTable_->currentRow() < 0) {
    return;
  }

  auto *item = hostHealthTable_->item(hostHealthTable_->currentRow(), 4);
  const QString command = item ? item->text().trimmed() : QString();
  if (command.isEmpty()) {
    updateDiagnostics("Info", "Host",
                      "Selected health check has no command to copy");
    updateActionAvailability();
    return;
  }

  QApplication::clipboard()->setText(command);
  updateDiagnostics("Info", "Host", "Copied health fix command: " + command);
  updateActionAvailability();
}

// Collect a full read-only evidence snapshot for the selected slave
void MainWindow::prepareSelectedSlaveSnapshot() {
  if (!client_.isConnected()) {
    updateDiagnostics("Warning", "Snapshot",
                      "Snapshot skipped: runtime is not connected");
    return;
  }

  const int position = selectedPosition();
  if (position < 0) {
    updateDiagnostics("Warning", "Snapshot",
                      "Snapshot skipped: no slave selected");
    return;
  }

  QString slaveName = uiText("unknown slave", "未知从站");
  QString state = uiText("unknown", "未知");
  for (const auto &slave : slaves_) {
    if (slave.position == position) {
      if (!slave.name.trimmed().isEmpty()) {
        slaveName = slave.name.trimmed();
      }
      if (!slave.state.trimmed().isEmpty()) {
        state = slave.state.trimmed();
      }
      break;
    }
  }

  client_.slaveInfo(position);
// Begin asynchronous online load of slave data (SDO dictionary, PDO map, etc.).
  client_.sdos(position);
  client_.pdos(position);
  client_.xml(position);
  addCia402WatchPreset();

  QStringList notes;
  notes << uiText("identity", "身份") << uiText("Object Dictionary", "对象字典")
        << uiText("PDO Map", "PDO 映射") << uiText("ESI XML", "ESI XML")
        << uiText("CiA 402 Watch", "CiA 402 Watch");
  const QStringList topologyIssues = topologyBaselineIssues();
  if (!topologyIssues.isEmpty()) {
    notes << uiText("topology baseline has %1 issue(s)", "拓扑基线有 %1 个问题")
                 .arg(topologyIssues.size());
  }

  updateDiagnostics(
      "Info", "Snapshot",
      uiText("Prepared read-only snapshot for slave #%1 %2, state %3: %4",
             "已为从站 #%1 %2 准备只读快照，状态 %3：%4")
          .arg(position)
          .arg(slaveName, state, notes.join(" | ")));
  activateWorkspaceTab(watchTabIndex_);
  updateCommissioningWorkflow();
  updateNextBestAction();
}

// Reset cached data and begin loading all slave evidence from the daemon
void MainWindow::beginSelectedSlaveOnlineLoad(int position) {
  loadedSlaveInfoPosition_ = -1;
  loadedPdoPosition_ = -1;
  loadedSdoPosition_ = -1;
  loadedXmlPosition_ = -1;
  lastSlaveInfoText_.clear();
  lastPdoText_.clear();
  lastSdoText_.clear();
  lastXmlText_.clear();

  const QString target = position >= 0
                             ? QString("#%1").arg(position)
                             : uiText("no slave selected", "尚未选择从站");
  if (rawText_->infoText) {
    rawText_->infoText->setPlainText(uiText("Loading selected slave identity for %1...",
                                   "正在加载 %1 的选中从站身份信息...")
                                .arg(target));
  }
  if (rawText_->pdoText) {
    rawText_->pdoText->setPlainText(
        uiText("Loading PDO Map for %1...", "正在加载 %1 的 PDO 映射...")
            .arg(target));
  }
  if (rawText_->sdoText) {
    rawText_->sdoText->setPlainText(uiText("Loading Object Dictionary for %1...",
                                  "正在加载 %1 的对象字典...")
                               .arg(target));
  }
  if (rawText_->xmlText) {
    rawText_->xmlText->setPlainText(
        uiText("Loading ESI XML for %1...", "正在加载 %1 的 ESI XML...")
            .arg(target));
  }
// Clear all online-loaded views (SDO, PDO, identity, mailbox, etc.).

  setTableRows(identityTable_, {"Field", "Value"},
               {{uiText("Loading", "加载中"), target}});
  setTableRows(portTable_, {"Port", "Type", "Link", "Loop", "Signal"}, {});
  setTableRows(mailboxTable_, {"Mailbox", "Value"}, {});
  setTableRows(sdo_->pdoTable, {"SM", "PDO", "Index", "Sub", "Bits", "Name"}, {});
  setTableRows(sdo_->sdoTable,
               {"Object", "Index", "Sub", "Access", "Type", "Bits", "Name",
                "Last Value", "Last Status"},
               {});
  if (sdo_->pdoSummaryLabel) {
    sdo_->pdoSummaryLabel->setText(
        uiText("Loading PDO Map for %1", "正在加载 %1 的 PDO 映射")
            .arg(target));
  }
  if (sdo_->sdoSummaryLabel) {
    sdo_->sdoSummaryLabel->setText(
        uiText("Loading Object Dictionary for %1", "正在加载 %1 的对象字典")
            .arg(target));
  }
  updateSdoInspector(uiText("Selected slave changed", "选中从站已切换"),
                     uiText("Waiting for OD/PDO/identity evidence for %1",
                            "等待 %1 的 OD/PDO/身份信息证据")
                         .arg(target));
  updateSelectedSlaveEvidenceSummary();
  updateActionAvailability();
}

// Wipe all cached online data — called on disconnect or master switch