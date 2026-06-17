// Consistency checks and commissioning workflow management.

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



// — Commissioning workflow input
CommissioningWorkflowInput MainWindow::commissioningWorkflowInput() const {
  CommissioningWorkflowInput input;
  input.connected = client_.isConnected();
  input.hasSlaves = !slaves_.isEmpty();
  const int selected = selectedPosition();
  input.hasSelectedSlave = selected >= 0;
  input.hasSdoRows = input.hasSelectedSlave && loadedSdoPosition_ == selected &&
                     sdo_->sdoTable && sdo_->sdoTable->rowCount() > 0;
  input.hasPdoRows = input.hasSelectedSlave && loadedPdoPosition_ == selected &&
                     sdo_->pdoTable && sdo_->pdoTable->rowCount() > 0;
  input.hasWatchRows = watch_->watchTable && watch_->watchTable->rowCount() > 0;
  input.hasFreeRunRows =
      freeRunWidgets_->freeRunEntryTable && freeRunWidgets_->freeRunEntryTable->rowCount() > 0;
  input.hasFailedOdEvidence = hasFailedSdoEvidence();
  input.hasStartupWatchDiffs = !startupSdoRowsWithWatchDiffs().isEmpty();

  int consistencyErrors = 0;
  int consistencyWarnings = 0;
  consistencyIssueCounts(&consistencyErrors, &consistencyWarnings, nullptr,
                         nullptr);
  input.hasConsistencyCheck = consistencyFresh_ && consistencyCheckAvailable();
  input.hasConsistencyBlockingIssues = consistencyHasBlockingIssues(
      {consistencyErrors, consistencyWarnings, 0, 0});
  input.freeRunEnabled = freeRun_;
  return input;
}


// — Rebuild the commissioning workflow step table from current evidence
void MainWindow::updateCommissioningWorkflow() {
  // Dispatch Alt+Enter to the correct evidence action for this table type
  if (!workflow_->workflowTable) {
    return;
  }

  const CommissioningWorkflowInput workflowInput = commissioningWorkflowInput();
  const QVector<CommissioningWorkflowStepState> stepStates =
      buildCommissioningWorkflowStepStates(workflowInput);
  const int selected = selectedPosition();
  int consistencyErrors = 0;
  int consistencyWarnings = 0;
  int consistencyInfos = 0;
  int consistencyReady = 0;
  consistencyIssueCounts(&consistencyErrors, &consistencyWarnings,
                         &consistencyInfos, &consistencyReady);

  const CommissioningWorkflowTexts uiTexts = commissioningWorkflowTexts();
  QVector<CommissioningWorkflowRow> workflowRows;
  workflowRows.reserve(10);
  auto addWorkflowRow =
      [&workflowRows](const QString &phase, CommissioningWorkflowStatus status,
                      const QString &step, const QString &risk,
                      const QString &evidence, const QString &action) {
        workflowRows.append({phase, status, step, risk, evidence, action});
      };

  addWorkflowRow(
      uiText("Runtime", "运行时"),
      commissioningWorkflowStepStatus(
          stepStates, CommissioningWorkflowStep::ConnectRuntime),
      uiText("Connect runtime", "连接运行时"),
      workflowInput.connected ? uiText("None", "无")
                              : uiText("Runtime offline", "运行时离线"),
      workflowInput.connected
          ? uiText("ecatd is connected", "ecatd 已连接")
          : uiText("No active runtime connection", "运行时尚未连接"),
      workflowInput.connected ? uiText("Refresh online data", "刷新在线数据")
                              : uiText("Click Connect", "点击连接"));
  addWorkflowRow(
      uiText("Runtime", "运行时"),
      commissioningWorkflowStepStatus(stepStates,
                                      CommissioningWorkflowStep::ScanTopology),
      uiText("Scan bus topology", "扫描总线拓扑"),
      workflowInput.hasSlaves
          ? uiText("None", "无")
// ── Commissioning Workflow Input ────────────────────────────────────
          : (workflowInput.connected ? uiText("No topology", "无拓扑")
                                     : uiText("Runtime offline", "运行时离线")),
      workflowInput.hasSlaves
          ? uiText("%1 slave(s) detected", "检测到 %1 个从站")
                .arg(slaves_.size())
          : uiText("No slaves in current scan", "当前扫描没有从站"),
      workflowInput.connected
          ? uiText("Refresh or Rescan", "刷新或重新扫描")
          : uiText("Connect runtime first", "先连接运行时"));
  addWorkflowRow(
      uiText("Target", "目标"),
      commissioningWorkflowStepStatus(stepStates,
                                      CommissioningWorkflowStep::SelectSlave),
      uiText("Select a slave", "选择从站"),
      workflowInput.hasSelectedSlave
          ? uiText("None", "无")
          : (workflowInput.hasSlaves ? uiText("No target", "无目标")
                                     : uiText("No topology", "无拓扑")),
      workflowInput.hasSelectedSlave
          ? QString("#%1").arg(selected)
          : uiText("No slave selected", "尚未选择从站"),
      workflowInput.hasSlaves
          ? uiText("Pick a slave in the I/O tree", "在 I/O 树中选择从站")
          : uiText("Scan topology first", "先扫描拓扑"));
  addWorkflowRow(
      uiText("Evidence", "证据"),
      commissioningWorkflowStepStatus(
          stepStates, CommissioningWorkflowStep::InspectObjectDictionary),
      uiText("Inspect Object Dictionary", "检查对象字典"),
      workflowInput.hasSdoRows ? uiText("None", "无")
                               : uiText("OD missing", "OD 缺失"),
      workflowInput.hasSdoRows
          ? uiText("%1 object row(s)", "%1 个对象条目")
                .arg(sdo_->sdoTable->rowCount())
          : uiText("Object data not loaded", "对象数据尚未加载"),
      workflowInput.hasSelectedSlave
          ? uiText("Open Object Dictionary", "打开对象字典")
          : uiText("Select a slave first", "先选择从站"));
  addWorkflowRow(uiText("Evidence", "证据"),
                 commissioningWorkflowStepStatus(
                     stepStates,
                     CommissioningWorkflowStep::ReviewObjectDictionaryEvidence),
                 uiText("Review OD evidence", "审阅 OD 证据"),
                 workflowInput.hasFailedOdEvidence
                     ? uiText("Failed mailbox evidence", "邮箱失败证据")
                     : uiText("None", "无"),
                 workflowInput.hasFailedOdEvidence
                     ? uiText("Failed SDO evidence needs review",
                              "存在需要复核的 SDO 失败证据")
                     : uiText("No failed Object Dictionary evidence",
                              "没有失败对象字典证据"),
                 workflowInput.hasFailedOdEvidence
                     ? uiText("Open Failed OD Evidence", "打开失败 OD 证据")
                     : uiText("Continue", "继续"));
  addWorkflowRow(
      uiText("Map", "映射"),
      commissioningWorkflowStepStatus(stepStates,
                                      CommissioningWorkflowStep::ReviewPdoMap),
      uiText("Review PDO Map", "核对 PDO 映射"),
      workflowInput.hasPdoRows ? uiText("None", "无")
                               : uiText("PDO missing", "PDO 缺失"),
      workflowInput.hasPdoRows
          ? uiText("%1 PDO row(s)", "%1 个 PDO 条目").arg(sdo_->pdoTable->rowCount())
          : uiText("PDO data not loaded", "PDO 数据尚未加载"),
      workflowInput.hasSelectedSlave
          ? uiText("Open PDO Map", "打开 PDO 映射")
          : uiText("Select a slave first", "先选择从站"));
  addWorkflowRow(uiText("Watch", "监视"),
                 commissioningWorkflowStepStatus(
                     stepStates, CommissioningWorkflowStep::MonitorWatch),
                 uiText("Monitor key SDOs", "监视关键 SDO"),
                 workflowInput.hasWatchRows
                     ? uiText("None", "无")
                     : uiText("No live SDO evidence", "无实时 SDO 证据"),
                 workflowInput.hasWatchRows
                     ? uiText("%1 watch item(s)", "%1 个监视项")
                           .arg(watch_->watchTable->rowCount())
                     : uiText("No watch items", "暂无监视项"),
                 workflowInput.hasSelectedSlave
                     ? uiText("Add current SDO to Watch", "添加当前 SDO 到监视")
                     : uiText("Select a slave first", "先选择从站"));
  addWorkflowRow(
      uiText("Startup", "启动"),
      commissioningWorkflowStepStatus(
          stepStates, CommissioningWorkflowStep::ReviewStartupDiffs),
      uiText("Review Startup diffs", "审阅启动偏差"),
      workflowInput.hasStartupWatchDiffs
          ? uiText("Startup mismatch", "启动值不一致")
          : uiText("None", "无"),
      workflowInput.hasStartupWatchDiffs
          ? uiText("Startup values differ from Watch", "启动值和 Watch 不一致")
          : uiText("No Startup Watch diffs", "没有 Startup Watch 偏差"),
      workflowInput.hasStartupWatchDiffs
          ? uiText("Open Startup Diffs", "打开启动偏差")
          : uiText("Continue", "继续"));
  addWorkflowRow(
      uiText("Gate", "门禁"),
      commissioningWorkflowStepStatus(
          stepStates, CommissioningWorkflowStep::RunConsistencyGate),
      uiText("Run consistency gate", "运行一致性门禁"),
      !consistencyCheckAvailable()
          ? uiText("Not run", "未运行")
          : (!consistencyFresh_
                 ? uiText("Stale", "已过期")
                 : (workflowInput.hasConsistencyBlockingIssues
                        ? uiText("Errors %1 / Warnings %2", "错误 %1 / 警告 %2")
                              .arg(consistencyErrors)
                              .arg(consistencyWarnings)
                        : uiText("None", "无"))),
      !consistencyCheckAvailable()
          ? uiText("Consistency check has not been run", "尚未运行一致性检查")
          : (!consistencyFresh_
                 ? uiText("Consistency check is stale", "一致性检查结果已过期")
                 : (workflowInput.hasConsistencyBlockingIssues
                        ? uiText("Consistency has %1 error(s), %2 warning(s)",
                                 "一致性检查有 %1 个错误、%2 个警告")
                              .arg(consistencyErrors)
                              .arg(consistencyWarnings)
                        : uiText("Consistency gate passed",
                                 "一致性门禁已通过"))),
      workflowInput.hasConsistencyCheck &&
              workflowInput.hasConsistencyBlockingIssues
          ? uiText("Open Evidence", "打开证据")
          : uiText("Refresh Consistency", "刷新一致性"));
  addWorkflowRow(
      uiText("Process", "过程"),
      commissioningWorkflowStepStatus(
          stepStates, CommissioningWorkflowStep::ValidateProcessImage),
      uiText("Validate process image", "验证过程映像"),
      workflowInput.freeRunEnabled || workflowInput.hasFreeRunRows
          ? uiText("None", "无")
          : uiText("No process image", "无过程映像"),
      workflowInput.freeRunEnabled
          ? uiText("Free Run active", "自由运行中")
          : (workflowInput.hasFreeRunRows
                 ? uiText("%1 process entries", "%1 个过程项")
                       .arg(freeRunWidgets_->freeRunEntryTable->rowCount())
                 : uiText("Free Run stopped", "自由运行已停止")),
      workflowInput.connected && workflowInput.hasSelectedSlave
          ? uiText("Open or toggle Free Run", "打开或切换自由运行")
          : uiText("Connect and select a slave", "连接并选择从站"));

  QVector<CommissioningWorkflowUiRow> uiRows;
  uiRows.reserve(workflowRows.size());
  for (const auto &row : workflowRows) {
    uiRows.append(commissioningWorkflowUiRow(row, uiTexts));
  }

  setTableRows(workflow_->workflowTable, commissioningWorkflowHeaders(uiTexts),
               commissioningWorkflowTableRows(uiRows));

  const CommissioningWorkflowStats stats = commissioningWorkflowStats(uiRows);
  const QColor readyColor("#22c55e");
  const QColor actionColor("#f59e0b");
  const QColor blockedColor("#ef4444");
  for (int row = 0; row < workflow_->workflowTable->rowCount(); ++row) {
    const CommissioningWorkflowUiRow uiRow = uiRows.value(row);
    const QColor color =
        uiRow.colorKey == QStringLiteral("ready")
            ? readyColor
            : (uiRow.colorKey == QStringLiteral("action") ? actionColor
                                                          : blockedColor);
    for (int column = 0; column < workflow_->workflowTable->columnCount(); ++column) {
      if (auto *item = workflow_->workflowTable->item(row, column)) {
    // Apply color coding for status
        item->setForeground(color);
        item->setToolTip(uiRow.tooltip);
      }
    }
    setCommissioningWorkflowStatusKey(workflow_->workflowTable, row, uiRow.colorKey);
  }
  fitTableColumnsToViewport(workflow_->workflowTable,
                            kCommissioningWorkflowEvidenceColumn);

  const int nextStep = nextCommissioningWorkflowStep();
  QString nextAction;
  if (nextStep >= 0 && nextStep < workflow_->workflowTable->rowCount() &&
      workflow_->workflowTable->item(nextStep, 5)) {
    nextAction = workflow_->workflowTable->item(nextStep, 5)->text().trimmed();
  }
  if (workflow_->workflowSummaryLabel) {
    const int total = std::max(1, workflow_->workflowTable->rowCount());
    const int readiness = std::clamp((stats.ready * 100) / total, 0, 100);
    const QString percent = QString("%1%").arg(readiness);
    const QString nextText = nextStep >= 0 && !nextAction.isEmpty()
                                 ? nextAction
                                 : uiText("Ready", "已就绪");
    const int openItems = stats.action + stats.blocked;
    // Set display text
    workflow_->workflowSummaryLabel->setText(uiText("Readiness %1 | Next: %2 | Open %3",
                                          "就绪度 %1 | 下一步：%2 | 未完成 %3")
                                       .arg(percent, nextText)
                                       .arg(openItems));
    workflow_->workflowSummaryLabel->setProperty(
        "severity",
        stats.blocked > 0 ? "warning" : (stats.action > 0 ? "action" : "ok"));

    QStringList tip;
    tip << uiText("Commissioning readiness: %1", "调试就绪度：%1").arg(percent);
    tip << QString("%1: %2   %3: %4   %5: %6")
               .arg(uiText("Ready", "就绪"))
               .arg(stats.ready)
// ── Commissioning Workflow Update ───────────────────────────────────
               .arg(uiText("Action", "待执行"))
               .arg(stats.action)
               .arg(uiText("Blocked", "受阻"))
               .arg(stats.blocked);
    if (nextStep >= 0 && !nextAction.isEmpty()) {
      tip << uiText("Next action: %1", "下一步动作：%1").arg(nextAction);
    }
    for (int row = 0; row < workflow_->workflowTable->rowCount(); ++row) {
      const QString statusKey =
          commissioningWorkflowStatusKeyForRow(workflow_->workflowTable, row);
      const QString status =
          workflow_->workflowTable->item(row, 1)
              ? workflow_->workflowTable->item(row, 1)->text().trimmed()
              : QString();
      if (statusKey == QStringLiteral("ready") ||
          (statusKey.isEmpty() && status == uiText("Ready", "就绪"))) {
        continue;
      }
      const QString phase = workflow_->workflowTable->item(row, 0)
                                ? workflow_->workflowTable->item(row, 0)->text().trimmed()
                                : QString();
      const QString step = workflow_->workflowTable->item(row, 2)
                               ? workflow_->workflowTable->item(row, 2)->text().trimmed()
                               : QString();
      const QString risk = workflow_->workflowTable->item(row, 3)
                               ? workflow_->workflowTable->item(row, 3)->text().trimmed()
                               : QString();
      const QString evidence =
          workflow_->workflowTable->item(row, 4)
              ? workflow_->workflowTable->item(row, 4)->text().trimmed()
              : QString();
      const QString actionText =
          workflow_->workflowTable->item(row, 5)
              ? workflow_->workflowTable->item(row, 5)->text().trimmed()
              : QString();
      tip << QString("#%1 [%2/%3] %4 | %5: %6 -> %7")
                 .arg(row + 1)
                 .arg(phase, status, step, risk, evidence, actionText);
    }
    workflow_->workflowSummaryLabel->setToolTip(tip.join('\n'));
    repolish(workflow_->workflowSummaryLabel); // force QSS re-evaluation after property change
  }

  if (auto *runNextButton = findChild<QPushButton *>("overviewRunNext")) {
    runNextButton->setEnabled(nextStep >= 0);
    // Set display text
    runNextButton->setText(
        nextStep >= 0 && !nextAction.isEmpty()
            ? uiText("Next: %1", "下一步：%1").arg(nextAction)
            : uiText("Run Next", "执行下一步"));
    runNextButton->setToolTip(
        nextStep >= 0 && !nextAction.isEmpty()
            ? uiText("Run workflow step %1: %2", "执行工作流第 %1 步：%2")
                  .arg(nextStep + 1)
                  .arg(nextAction)
            : uiText("All commissioning workflow steps are ready or blocked.",
                     "调试工作流已就绪或当前没有可执行步骤。"));
  }
  filterCommissioningWorkflow();
  updateSelectedSlaveEvidenceSummary();
  updateSlaveEvidenceMatrix();
  updateSessionBrief();
}


// — Slave evidence ui texts
SlaveEvidenceUiTexts MainWindow::slaveEvidenceUiTexts() const {
  return {
      .p0Fault = uiText("P0 Fault", "P0 故障"),
      .p1Risk = uiText("P1 Risk", "P1 风险"),
      .p2Action = uiText("P2 Action", "P2 待执行"),
      .p3Ready = uiText("P3 Ready", "P3 就绪"),
      .reviewOd = uiText("Review OD", "复核 OD"),
      .loadPdo = uiText("Load PDO", "加载 PDO"),
      .addWatch = uiText("Add Watch", "添加 Watch"),
      .reviewStartup = uiText("Review Startup", "复核 Startup"),
      .validateProcess = uiText("Validate Process", "验证过程"),
      .reviewRisk = uiText("Review Risk", "复核风险"),
      .ready = uiText("Ready", "就绪"),
      .identityMissing = uiText("identity missing", "身份缺失"),
      .odMissing = uiText("OD missing", "OD 缺失"),
      .pdoMissing = uiText("PDO missing", "PDO 缺失"),
      .watchMissing = uiText("Watch missing", "Watch 缺失"),
      .processMissing = uiText("process evidence missing", "过程证据缺失"),
      .startupDiffPattern = uiText("Startup diff %1", "启动偏差 %1"),
      .pdoMapIssuePattern = uiText("PDO map issue %1", "PDO 映射问题 %1"),
      .topologyBaselineIssue =
          uiText("topology baseline issue", "拓扑基线问题"),
      .driveFaultEvidence = uiText("drive fault evidence", "驱动故障证据"),
      .unknownEvidenceRisk = uiText("unknown evidence risk", "未知证据风险"),
      .unnamed = uiText("Unnamed", "未命名"),
      .unknown = uiText("Unknown", "未知"),
      .missing = uiText("Missing", "缺失"),
      .noRows = uiText("No rows", "无行"),
      .none = uiText("None", "无"),
      .watchValuesPattern = uiText("%1/%2 values", "%1/%2 有值"),
      .startupRowsPattern = uiText("%1 row(s), %2 diff(s)", "%1 行，%2 个偏差"),
      .processRowsPattern =
          uiText("%1 row(s), %2 issue(s)", "%1 行，%2 个问题"),
      .modePattern = uiText("mode %1", "模式 %1"),
      .slavePattern = uiText("Slave #%1 %2", "从站 #%1 %2"),
      .priorityPattern = uiText("Priority: %1", "优先级：%1"),
      .statePattern = uiText("State: %1", "状态：%1"),
      .identityRowsPattern = uiText("Identity rows: %1", "身份行：%1"),
      .odRowsPattern = uiText("Object Dictionary rows: %1", "对象字典行：%1"),
      .pdoRowsPattern = uiText("PDO rows: %1", "PDO 行：%1"),
      .watchValuesDetailPattern =
          uiText("Watch values: %1/%2", "Watch 有值：%1/%2"),
      .startupRowsDetailPattern =
          uiText("Startup rows: %1, diffs: %2", "Startup 行：%1，偏差：%2"),
      .processRowsDetailPattern = uiText("Process rows: %1, map issues: %2",
                                         "过程行：%1，映射问题：%2"),
      .drivePattern = uiText("Drive: %1", "驱动：%1"),
      .nextPattern = uiText("Next: %1", "下一步：%1"),
      .riskPattern = uiText("Risk: %1", "风险：%1"),
      .priorityHeader = uiText("Priority", "优先级"),
      .slaveHeader = uiText("Slave", "从站"),
      .nameHeader = uiText("Name", "名称"),
      .stateHeader = uiText("State", "状态"),
      .readinessHeader = uiText("Readiness", "就绪度"),
      .odHeader = uiText("OD", "OD"),
      .pdoHeader = uiText("PDO", "PDO"),
      .watchHeader = uiText("Watch", "Watch"),
      .startupHeader = uiText("Startup", "Startup"),
      .processHeader = uiText("Process", "过程"),
      .riskHeader = uiText("Risk", "风险"),
      .nextHeader = uiText("Next", "下一步"),
  };
}

SelectedSlaveEvidenceSummaryTexts
MainWindow::selectedSlaveEvidenceSummaryTexts() const {
  return {
      .selectSlaveText = uiText("Evidence: select a slave", "证据：请选择从站"),
      .ready = uiText("ready", "就绪"),
      .missing = uiText("missing", "缺失"),
      .summaryPattern =
          uiText("Evidence %1/5 | ID %2 | OD %3 | PDO %4 | Watch %5/%6 "
                 "values | Startup %7 diff %8 | Free %9 map issue %10",
                 "证据 %1/5 | 身份 %2 | OD %3 | PDO %4 | Watch %5/%6 有值 | "
                 "启动 %7 偏差 %8 | Free %9 映射问题 %10"),
      .scorePattern = uiText("Selected slave evidence score: %1/5",
                             "选中从站证据完整度：%1/5"),
      .missingIdentity =
          uiText("Missing identity evidence: run Snapshot or Refresh.",
                 "缺少身份信息：运行准备快照或刷新。"),
      .missingOd =
          uiText("Missing Object Dictionary evidence: load OD metadata.",
                 "缺少对象字典证据：加载 OD 元数据。"),
      .missingPdo = uiText("Missing PDO evidence: load PDO Map.",
                           "缺少 PDO 证据：加载 PDO 映射。"),
      .missingWatch =
          uiText("Missing live SDO values: add Watch rows and refresh.",
                 "缺少实时 SDO 值：添加 Watch 并刷新。"),
      .missingProcess =
          uiText("No process-image evidence yet: run Free Run when safe.",
                 "暂无过程映像证据：安全时运行 Free Run。"),
      .startupDiffPattern = uiText("Startup SDO has %1 Watch mismatch row(s).",
                                   "Startup SDO 有 %1 条 Watch 不一致。"),
      .mapIssuePattern = uiText("Free Run has %1 PDO map evidence issue(s).",
                                "Free Run 有 %1 个 PDO 映射证据问题。"),
      .topologyIssuePattern = uiText("Topology baseline has %1 issue(s).",
                                     "拓扑基线有 %1 个问题。"),
  };
}


// — Return localized text constants for the drive summary label
SelectedDriveSummaryTexts MainWindow::selectedDriveSummaryTexts() const {
  return {
      .noWatchEvidence =
          uiText("Drive: no Watch evidence", "驱动：暂无监视证据"),
      .noCia402Evidence = uiText("Drive: no CiA 402 Watch evidence",
                                 "驱动：暂无 CiA 402 监视证据"),
      .summaryPattern = uiText("Drive: %1", "驱动：%1"),
      .modePattern = uiText("mode %1", "模式 %1"),
      .controlwordPattern = uiText("cw %1", "控制字 %1"),
  };
}


