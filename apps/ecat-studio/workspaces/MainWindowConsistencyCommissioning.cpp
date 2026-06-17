// Consistency checks and commissioning workflow management.

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


// — Switch to the consistency check tab and refresh its contents
void MainWindow::openConsistencyView() {
  updateConsistencyView();
  activateWorkspaceTab(consistencyTabIndex_);
}


// — Rebuild the consistency matrix from current project and online evidence
void MainWindow::updateConsistencyView() {
  if (!consistency_->consistencyTable) {
    return;
  }
  updateIoVariableTable();

  QList<QStringList> rows;
  // Helper to append a consistency row
  auto addRow = [&](const QString &level, const QString &scope,
                    const QString &target, const QString &evidence,
                    const QString &expected, const QString &actual,
                    const QString &action) {
    rows.append({level, scope, target, evidence, expected, actual, action});
  };
  if (topologyBaseline_.isEmpty()) {
    addRow(
        uiText("Warning", "警告"), uiText("Topology", "拓扑"),
        uiText("Topology baseline", "拓扑基线"),
        uiText("No offline baseline has been captured", "尚未捕获离线拓扑基线"),
        uiText("Captured baseline", "已捕获基线"), uiText("Missing", "缺失"),
        uiText("Capture baseline after a known-good scan",
               "在确认正常的扫描后捕获基线"));
  } else {
    // Collect issue details from the evidence model
    for (const auto &issue :
         compareTopologyBaseline(topologyBaseline_, slaves_)) {
      const QString target =
          QString("#%1 %2")
              .arg(issue.position)
              .arg(issue.kind == TopologyBaselineIssueKind::UnexpectedSlave
                       ? issue.current.name
                       : topologySlaveDisplayName(issue.baseline));
      // Map each baseline deviation type to a human-readable warning row
      switch (issue.kind) {
      case TopologyBaselineIssueKind::MissingSlave:
        addRow(
            uiText("Error", "错误"), uiText("Topology", "拓扑"), target,
            uiText("Baseline slave is missing online", "基线从站当前在线缺失"),
            issue.baseline.name, uiText("Missing", "缺失"),
            uiText("Check cabling, power, and bus order",
                   "检查线缆、供电和总线顺序"));
        break;
      case TopologyBaselineIssueKind::NameChanged:
        addRow(
            uiText("Warning", "警告"), uiText("Topology", "拓扑"), target,
            uiText("Slave name differs from baseline", "从站名称和基线不一致"),
            issue.baseline.name, issue.current.name,
            uiText("Confirm device replacement or ESI/name evidence",
                   "确认是否更换设备或需要复核 ESI/name 证据"));
        break;
      case TopologyBaselineIssueKind::StateChanged:
        addRow(
            uiText("Warning", "警告"), uiText("Topology", "拓扑"), target,
            uiText("Slave state differs from baseline", "从站状态和基线不一致"),
            issue.baseline.state, issue.current.state,
            uiText("Review state machine before OP", "进入 OP 前复核状态机"));
        break;
      case TopologyBaselineIssueKind::UnexpectedSlave:
        addRow(uiText("Warning", "警告"), uiText("Topology", "拓扑"), target,
               uiText("Online slave is not in baseline", "在线从站不在基线中"),
               uiText("No baseline entry", "无基线条目"), issue.current.name,
               uiText("Recapture baseline or verify bus order",
                      "重新捕获基线或确认总线顺序"));
        break;
      }
    }
  }

  QSet<QString> onlineObjects;
  if (ioVar_->ioVariableTable) {
    // Iterate all rows and apply active filter predicates
    for (int row = 0; row < ioVar_->ioVariableTable->rowCount(); ++row) {
      const IoVariableTableRow variable =
          ioVariableTableRowFromTable(ioVar_->ioVariableTable, row);
      if (!ioVariableTableRowHasTarget(variable)) {
        continue;
      }
      const QString key = ioVariableTableRowKey(variable);
      if (!key.isEmpty()) {
        onlineObjects.insert(key);
      }
      const QString target =
          QString("#%1 %2:%3 %4")
              .arg(variable.position)
              .arg(variable.index, variable.subIndex, variable.symbol);
      const bool missingValue = ioVariableTableRowHasMissingValue(variable);
      const bool startupDiff = ioVariableTableRowHasStartupDiff(variable);
      const bool mapIssue = ioVariableTableRowHasPdoMapIssue(variable);
      if (startupDiff) {
        addRow(uiText("Error", "错误"), uiText("Startup", "启动"), target,
               uiText("Startup expectation differs from live/Watch evidence",
                      "启动期望和实时/Watch 证据不一致"),
               variable.startup,
               variable.watch.isEmpty() ? variable.raw : variable.watch,
               uiText("Review Startup SDO diffs before applying",
                      "应用前审阅 Startup SDO 偏差"));
      }
      if (mapIssue) {
        addRow(uiText("Warning", "警告"), uiText("I/O Variables", "I/O 变量"),
               target,
               uiText("PDO/process map evidence is incomplete or inconsistent",
                      "PDO/过程映像证据缺失或不一致"),
               uiText("Mapped process signal", "已映射过程信号"), variable.map,
               uiText("Reload PDO Map and refresh Free Run evidence",
                      "重新加载 PDO Map 并刷新 Free Run 证据"));
      }
      if (missingValue) {
        addRow(uiText("Warning", "警告"), uiText("I/O Variables", "I/O 变量"),
               target,
               uiText("No Raw or Watch value evidence",
                      "缺少 Raw 或 Watch 值证据"),
               uiText("Current value evidence", "当前值证据"),
               uiText("Missing", "缺失"),
               uiText("Refresh Watch or Free Run before commissioning",
                      "调试前刷新 Watch 或 Free Run"));
      }
      if (variable.alias.isEmpty() &&
          !variable.tags.contains("ignore", Qt::CaseInsensitive)) {
        addRow(uiText("Info", "信息"), uiText("I/O Variables", "I/O 变量"),
               target,
               uiText("No engineering alias assigned", "尚未设置工程别名"),
               uiText("Alias for handoff", "交接用别名"), uiText("Empty", "空"),
               uiText("Set Alias/Tags for reusable engineering context",
                      "设置 Alias/Tags 以便工程复用"));
      }
      if (ioVariableTableRowHasPlcIssue(variable, uiText("Ready", "就绪"))) {
        addRow(uiText("Info", "信息"), uiText("I/O Variables", "I/O 变量"),
               target,
               uiText("PLC handoff symbol quality needs review",
                      "PLC 交接符号质量需要复核"),
               uiText("Ready PLC symbol", "就绪 PLC 符号"), variable.plcQuality,
               uiText("Use Bulk Name or edit Alias/Tags before PLC export",
                      "导出 PLC 前使用批量命名或编辑 Alias/Tags"));
      }
    }
  }

  QStringList metadataKeys = ioVariableMetadata_.keys();
  metadataKeys.sort();
  for (const QString &key : metadataKeys) {
    if (onlineObjects.contains(key)) {
      continue;
    }
    const QStringList parts = key.split('|');
    const QStringList metadata = ioVariableMetadata_.value(key);
    addRow(
        uiText("Warning", "警告"), uiText("I/O Variables", "I/O 变量"),
        QString("#%1 %2:%3 %4")
            .arg(parts.value(0), parts.value(1), parts.value(2),
                 metadata.value(0)),
        uiText("Project variable metadata has no online evidence",
               "工程变量元数据没有在线证据"),
        uiText("Online PDO/Watch/Free Run row", "在线 PDO/Watch/Free Run 行"),
        uiText("Missing", "缺失"),
        uiText("Load PDO/Free Run evidence or remove stale metadata",
               "加载 PDO/Free Run 证据或移除过期元数据"));
  }

  if (startupSdoTable_) {
    ensureStartupSdoTable();
    for (int row = 0; row < startupSdoTable_->rowCount(); ++row) {
      bool ok = false;
      const int position = tableText(startupSdoTable_, row, 0).toInt(&ok);
      const QString index = tableText(startupSdoTable_, row, 1);
      const QString subIndex = tableText(startupSdoTable_, row, 2);
      const QString value = tableText(startupSdoTable_, row, 3);
      const QString delta = tableText(startupSdoTable_, row, 8);
      if (!ok || position < 0 || index.isEmpty() || subIndex.isEmpty()) {
        addRow(uiText("Error", "错误"), uiText("Startup", "启动"),
               uiText("Startup row %1", "启动行 %1").arg(row + 1),
               uiText("Startup row has invalid address", "Startup 行地址无效"),
               uiText("Valid slave/index/sub", "有效从站/索引/子项"),
               QString("#%1 %2:%3").arg(position).arg(index, subIndex),
               uiText("Fix row before applying Startup SDO",
                      "应用 Startup SDO 前修复该行"));
        continue;
      }
      const QString normalizedDelta = delta.toLower();
      const bool diff = !delta.isEmpty() && normalizedDelta != "match" &&
                        delta != "匹配" && normalizedDelta != "pending" &&
                        delta != "待比较";
      if (diff) {
        addRow(
            uiText("Error", "错误"), uiText("Startup", "启动"),
            QString("#%1 %2:%3")
                .arg(position)
                // Normalize hex address for consistent comparison
                .arg(normalizeHexText(index, 4), normalizeHexText(subIndex, 2)),
            uiText("Startup row differs from Watch evidence",
                   "Startup 行和 Watch 证据不一致"),
            value, delta,
            uiText("Use Review Diffs, Sync Startup, or edit expected value",
                   "使用审阅偏差、同步启动或编辑期望值"));
      } else if (delta.isEmpty() || normalizedDelta == "pending" ||
                 delta == "待比较") {
        addRow(
            uiText("Warning", "警告"), uiText("Startup", "启动"),
            QString("#%1 %2:%3")
                .arg(position)
                // Normalize hex address for consistent comparison
                .arg(normalizeHexText(index, 4), normalizeHexText(subIndex, 2)),
            uiText("Startup row has no Watch comparison",
                   "Startup 行缺少 Watch 对比"),
            value, uiText("Pending", "待比较"),
            uiText("Add/refresh matching Watch row before applying",
                   "应用前添加或刷新匹配 Watch 行"));
      }
    }
  }

  if (rows.isEmpty()) {
    addRow(uiText("Ready", "就绪"), uiText("Project", "工程"),
           uiText("Current evidence", "当前证据"),
           uiText("No consistency issues found in loaded evidence",
                  "当前已加载证据未发现一致性问题"),
           uiText("Consistent", "一致"), uiText("Consistent", "一致"),
           uiText("Continue commissioning", "继续调试"));
  }

  setTableRows(consistency_->consistencyTable,
               {uiText("Level", "级别"), uiText("Scope", "范围"),
                uiText("Target", "目标"), uiText("Evidence", "证据"),
                uiText("Expected", "期望"), uiText("Actual", "实际"),
                uiText("Action", "建议动作")},
               rows);

  const QColor errorBackground =
      settings_.theme == "Light" ? QColor("#fee2e2") : QColor("#3a1218");
  const QColor warningBackground =
      settings_.theme == "Light" ? QColor("#fff7cc") : QColor("#3a2f16");
  const QColor infoBackground =
      settings_.theme == "Light" ? QColor("#eef6ff") : QColor("#122033");
  for (int row = 0; row < consistency_->consistencyTable->rowCount(); ++row) {
    const QString level = tableText(consistency_->consistencyTable, row, 0);
    QColor background;
    QColor foreground;
    if (level == uiText("Error", "错误")) {
      background = errorBackground;
      foreground = QColor("#ef4444");
    } else if (level == uiText("Warning", "警告")) {
      background = warningBackground;
      foreground = QColor("#f59e0b");
    } else if (level == uiText("Ready", "就绪")) {
      foreground = QColor("#22c55e");
    } else {
      background = infoBackground;
      foreground = QColor("#60a5fa");
    }
    for (int column = 0; column < consistency_->consistencyTable->columnCount(); ++column) {
      if (auto *item = consistency_->consistencyTable->item(row, column)) {
        if (background.isValid()) {
          item->setBackground(background);
        }
        if (column == 0 && foreground.isValid()) {
          item->setForeground(foreground);
        }
      }
    }
  }
  consistency_->consistencyTable->resizeColumnsToContents(); // auto-fit column widths
  if (consistency_->consistencySummaryLabel) {
    const ConsistencyIssueCounts counts =
        consistencyTableIssueCounts(consistency_->consistencyTable);
    consistency_->consistencySummaryLabel->setText(
        uiText("%1 rows | errors %2 | warnings %3 | info %4 | ready %5",
               "%1 行 | 错误 %2 | 警告 %3 | 信息 %4 | 就绪 %5")
            .arg(consistency_->consistencyTable->rowCount())
            .arg(counts.errors)
            .arg(counts.warnings)
            .arg(counts.infos)
            .arg(counts.ready));
  }
  consistencyFresh_ = true;
  filterConsistencyTable();
  updateCommissioningWorkflow();
  updateNextBestAction();
}


// — Filter consistency table
void MainWindow::filterConsistencyTable() {
  if (!consistency_->consistencyTable) {
    return;
  }
  const QString needle =
      consistency_->consistencyFilter ? consistency_->consistencyFilter->text().trimmed() : QString();
  const QString scope = consistency_->consistencyScopeFilter
                            ? consistency_->consistencyScopeFilter->currentData().toString()
                            : QStringLiteral("all");
  const ConsistencyTableFilterStats stats =
      filterConsistencyTableRows(consistency_->consistencyTable, scope, needle);
  if (consistency_->consistencySummaryLabel) {
    const QString label = consistency_->consistencyScopeFilter
                              ? consistency_->consistencyScopeFilter->currentText()
                              : uiText("All", "全部");
    consistency_->consistencySummaryLabel->setToolTip(
        uiText("Visible consistency rows: %1/%2\nScope: %3",
               "可见一致性行：%1/%2\n范围：%3")
            .arg(stats.visible)
            .arg(stats.total)
            .arg(label));
  }
  updateConsistencyRowDetail();
  updateActionAvailability();
}


// — Refresh the consistency detail strip for the focused row
void MainWindow::updateConsistencyRowDetail() {
  if (!consistency_->consistencyDetailLabel) {
    return;
  }
  // Lambda to push UI state changes to the label widget
  auto applyState = [this](const ConsistencyDetailUiState &state) {
    consistency_->consistencyDetailLabel->setText(state.text);
    consistency_->consistencyDetailLabel->setProperty("severity", state.severityKey);
    consistency_->consistencyDetailLabel->setToolTip(state.tooltip);
    repolish(consistency_->consistencyDetailLabel); // force QSS re-evaluation after property change
  };

  if (!consistency_->consistencyTable) {
    applyState(consistencyDetailUnavailableState(consistencyDetailTexts()));
    return;
  }

  const int row = consistency_->consistencyTable->currentRow();
  if (row < 0 || row >= consistency_->consistencyTable->rowCount() ||
      consistency_->consistencyTable->isRowHidden(row)) {
    applyState(consistencyDetailNoSelectionState(consistencyDetailTexts()));
    return;
  }

  auto textAt = [this, row](int column) {
    const auto *item = consistency_->consistencyTable->item(row, column);
    return item ? item->text().trimmed() : QString();
  };

  applyState(buildConsistencyDetailUiState(
      {.level = textAt(kConsistencyLevelColumn),
       .scope = textAt(kConsistencyScopeColumn),
       .target = textAt(kConsistencyTargetColumn),
       .evidence = textAt(kConsistencyEvidenceColumn),
       .expected = textAt(kConsistencyExpectedColumn),
       .actual = textAt(kConsistencyActualColumn),
       .action = textAt(kConsistencyActionColumn)},
      consistencyDetailTexts()));
}


// — Route to the evidence workspace referenced by the consistency row
void MainWindow::focusEvidenceFromConsistency(int row) {
  if (!consistency_->consistencyTable) {
    return;
  }
  if (row < 0 || row >= consistency_->consistencyTable->rowCount()) {
    row = consistency_->consistencyTable->currentRow();
  }
  if (row < 0 || row >= consistency_->consistencyTable->rowCount()) {
    return;
  }

  const ConsistencyDetailRow consistencyRow = {
      .level = tableText(consistency_->consistencyTable, row, kConsistencyLevelColumn),
      .scope = tableText(consistency_->consistencyTable, row, kConsistencyScopeColumn),
      .target = tableText(consistency_->consistencyTable, row, kConsistencyTargetColumn),
      .evidence = tableText(consistency_->consistencyTable, row, kConsistencyEvidenceColumn),
      .expected = tableText(consistency_->consistencyTable, row, kConsistencyExpectedColumn),
      .actual = tableText(consistency_->consistencyTable, row, kConsistencyActualColumn),
      .action = tableText(consistency_->consistencyTable, row, kConsistencyActionColumn),
  };
  const ConsistencyEvidenceRouteDecision route =
      consistencyEvidenceRouteDecision(consistencyRow);

  if (route.kind == ConsistencyEvidenceRouteKind::Topology) {
    updateStateMachineView();
    const int stateRow =
        tableRowForPosition(stateMachine_->stateMachineTable, route.address.position, 0);
    activateTabContainingWidget(tabs_, stateMachine_->stateMachineTable);
    if (stateRow >= 0) {
      selectAndFocusTableRow(stateMachine_->stateMachineTable, stateRow, 0);
    }
    updateDiagnostics("Info", "Consistency",
                      uiText("Opened state/topology evidence from Consistency",
                             "已从一致性打开状态/拓扑证据"));
    return;
  }

  if (route.kind == ConsistencyEvidenceRouteKind::Startup) {
    ensureStartupSdoTable();
    updateStartupSdoWatchEvidence();
    int startupRow = tableRowForObjectAddress(
        startupSdoTable_, route.address.position, route.address.index,
        route.address.subIndex, 0, 1, 2);
    if (startupRow < 0 && startupSdoTable_ && route.startupRow >= 0 &&
        route.startupRow < startupSdoTable_->rowCount()) {
      startupRow = route.startupRow;
    }
    if (startupRow >= 0) {
      if (startupWatchDiffsOnly_) {
        startupWatchDiffsOnly_->setChecked(false);
      }
      filterStartupSdoTable();
      activateTabContainingWidget(tabs_, startupSdoTable_);
      selectAndFocusTableRow(startupSdoTable_, startupRow, 0);
      updateStartupSdoControls();
      updateDiagnostics("Info", "Consistency",
                        uiText("Opened Startup SDO evidence from Consistency",
                               "已从一致性打开 Startup SDO 证据"));
      return;
    }
  }

  if (route.kind == ConsistencyEvidenceRouteKind::Watch) {
    const int watchRow = tableRowForObjectAddress(
        watchTable_, route.address.position, route.address.index,
        route.address.subIndex, 1, 2, 3);
    if (watchRow >= 0) {
      if (watchScopeFilter_) {
        const int index = watchScopeFilter_->findData("all");
        if (index >= 0) {
          watchScopeFilter_->setCurrentIndex(index);
        }
      }
      if (watchChangedOnly_) {
        watchChangedOnly_->setChecked(false);
      }
      filterWatchTable();
      activateTabContainingWidget(tabs_, watchTable_);
      selectAndFocusTableRow(watchTable_, watchRow, 1);
      updateDiagnostics("Info", "Consistency",
                        uiText("Opened Watch evidence from Consistency",
                               "已从一致性打开 Watch 证据"));
      return;
    }
  }

  focusIoVariablesFromConsistency(row);
}


// — Route to the I/O variable tab filtered by the consistency issue
void MainWindow::focusIoVariablesFromConsistency(int row) {
  if (!ioVar_->ioVariableTable || !tabs_ || ioVariableTabIndex_ < 0 ||
      ioVariableTabIndex_ >= tabs_->count()) {
    return;
  }
  if (!consistency_->consistencyTable || row < 0 || row >= consistency_->consistencyTable->rowCount()) {
    row = consistency_->consistencyTable ? consistency_->consistencyTable->currentRow() : -1;
  }

  QString target;
  QString evidence;
  QString actual;
  QString action;
  if (consistency_->consistencyTable && row >= 0 && row < consistency_->consistencyTable->rowCount()) {
    target = tableText(consistency_->consistencyTable, row, kConsistencyTargetColumn);
    evidence = tableText(consistency_->consistencyTable, row, kConsistencyEvidenceColumn);
    actual = tableText(consistency_->consistencyTable, row, kConsistencyActualColumn);
    action = tableText(consistency_->consistencyTable, row, kConsistencyActionColumn);
  }
  const ConsistencyEvidenceRouteDecision route =
      consistencyEvidenceRouteDecision({.target = target,
                                        .evidence = evidence,
                                        .actual = actual,
                                        .action = action});

  if (ioVar_->ioVariableScopeFilter) {
    const int scopeIndex = ioVar_->ioVariableScopeFilter->findData(route.ioScope);
    if (scopeIndex >= 0) {
      ioVar_->ioVariableScopeFilter->setCurrentIndex(scopeIndex);
    }
  }
  updateIoVariableTable();
  activateWorkspaceTab(ioVariableTabIndex_);

  int selectedRow = tableRowForObjectAddress(
      ioVar_->ioVariableTable, route.address.position, route.address.index,
      route.address.subIndex, 0, 3, 4);
  if (selectedRow < 0) {
    selectedRow = firstVisibleTableRow(ioVar_->ioVariableTable);
  }
  if (selectedRow >= 0) {
    selectAndFocusTableRow(ioVar_->ioVariableTable, selectedRow, 0);
  }

  updateDiagnostics("Info", "Consistency",
                    uiText("Focused I/O Variables from Consistency row",
                           "已从一致性行定位到 I/O 变量"));
}


// — Return the first consistency io issue row
int MainWindow::firstConsistencyIoIssueRow() const {
  return firstConsistencyTableIoIssueRow(consistency_->consistencyTable);
}


// — Return the first consistency blocking issue row
int MainWindow::firstConsistencyBlockingIssueRow() const {
  return firstConsistencyTableBlockingIssueRow(consistency_->consistencyTable);
}


// — Check whether consistency check available
bool MainWindow::consistencyCheckAvailable() const {
  return consistencyTableAvailable(consistency_->consistencyTable);
}


// — Consistency issue counts
void MainWindow::consistencyIssueCounts(int *errors, int *warnings, int *infos,
                                        int *ready) const {
  const ConsistencyIssueCounts counts =
      consistencyTableIssueCounts(consistency_->consistencyTable);
  if (errors) {
    *errors = counts.errors;
  }
  if (warnings) {
    *warnings = counts.warnings;
  }
  if (infos) {
    *infos = counts.infos;
  }
  if (ready) {
    *ready = counts.ready;
  }
}


// — Return a list of consistency gate details
QStringList MainWindow::consistencyGateDetails(const QString &operation) const {
  ConsistencyIssueCounts counts;
  consistencyIssueCounts(&counts.errors, &counts.warnings, &counts.infos,
                         &counts.ready);

  QStringList details;
  const QString operationText = operation.trimmed().isEmpty()
                                    ? uiText("this operation", "此操作")
                                    : operation.trimmed();
  switch (consistencyGateState(consistencyCheckAvailable(), consistencyFresh_,
                               counts)) {
  case ConsistencyGateState::NotRun:
    details << uiText("Consistency gate: not run before %1",
                      "一致性门禁：%1 前尚未运行")
                   .arg(operationText);
    details << uiText(
        "Run the read-only Consistency Check to compare topology, "
        "Startup, Watch, I/O Variables, and project metadata.",
        "请运行只读一致性检查，对比拓扑、Startup、Watch、I/O 变量"
        "和工程元数据。");
    break;
  case ConsistencyGateState::Stale:
    details << uiText("Consistency gate: stale before %1",
                      "一致性门禁：%1 前结果已过期")
                   .arg(operationText);
    details << uiText("Refresh Consistency because online/project evidence has "
                      "changed since the last check.",
                      "上次检查后在线或工程证据发生变化，请刷新一致性检查。");
    break;
  case ConsistencyGateState::Blocking:
    details << uiText("Consistency gate: %1 error(s), %2 warning(s)",
                      "一致性门禁：%1 个错误，%2 个警告")
                   .arg(counts.errors)
                   .arg(counts.warnings);
    details << uiText("Review Consistency rows before %1",
                      "%1 前请审阅一致性行")
                   .arg(operationText);
    break;
  case ConsistencyGateState::Passed:
    details << uiText("Consistency gate: passed (%1 info, %2 ready row(s))",
                      "一致性门禁：已通过（%1 条信息，%2 条就绪）")
                   .arg(counts.infos)
                   .arg(counts.ready);
    break;
  }
  return details;
}


// — Commissioning workflow input
CommissioningWorkflowInput MainWindow::commissioningWorkflowInput() const {
  CommissioningWorkflowInput input;
  input.connected = client_.isConnected();
  input.hasSlaves = !slaves_.isEmpty();
  const int selected = selectedPosition();
  input.hasSelectedSlave = selected >= 0;
  input.hasSdoRows = input.hasSelectedSlave && loadedSdoPosition_ == selected &&
                     sdoTable_ && sdoTable_->rowCount() > 0;
  input.hasPdoRows = input.hasSelectedSlave && loadedPdoPosition_ == selected &&
                     pdoTable_ && pdoTable_->rowCount() > 0;
  input.hasWatchRows = watchTable_ && watchTable_->rowCount() > 0;
  input.hasFreeRunRows =
      freeRunEntryTable_ && freeRunEntryTable_->rowCount() > 0;
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
  if (!workflowTable_) {
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
                .arg(sdoTable_->rowCount())
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
          ? uiText("%1 PDO row(s)", "%1 个 PDO 条目").arg(pdoTable_->rowCount())
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
                           .arg(watchTable_->rowCount())
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
                       .arg(freeRunEntryTable_->rowCount())
                 : uiText("Free Run stopped", "自由运行已停止")),
      workflowInput.connected && workflowInput.hasSelectedSlave
          ? uiText("Open or toggle Free Run", "打开或切换自由运行")
          : uiText("Connect and select a slave", "连接并选择从站"));

  QVector<CommissioningWorkflowUiRow> uiRows;
  uiRows.reserve(workflowRows.size());
  for (const auto &row : workflowRows) {
    uiRows.append(commissioningWorkflowUiRow(row, uiTexts));
  }

  setTableRows(workflowTable_, commissioningWorkflowHeaders(uiTexts),
               commissioningWorkflowTableRows(uiRows));

  const CommissioningWorkflowStats stats = commissioningWorkflowStats(uiRows);
  const QColor readyColor("#22c55e");
  const QColor actionColor("#f59e0b");
  const QColor blockedColor("#ef4444");
  for (int row = 0; row < workflowTable_->rowCount(); ++row) {
    const CommissioningWorkflowUiRow uiRow = uiRows.value(row);
    const QColor color =
        uiRow.colorKey == QStringLiteral("ready")
            ? readyColor
            : (uiRow.colorKey == QStringLiteral("action") ? actionColor
                                                          : blockedColor);
    for (int column = 0; column < workflowTable_->columnCount(); ++column) {
      if (auto *item = workflowTable_->item(row, column)) {
        item->setForeground(color);
        item->setToolTip(uiRow.tooltip);
      }
    }
    setCommissioningWorkflowStatusKey(workflowTable_, row, uiRow.colorKey);
  }
  fitTableColumnsToViewport(workflowTable_,
                            kCommissioningWorkflowEvidenceColumn);

  const int nextStep = nextCommissioningWorkflowStep();
  QString nextAction;
  if (nextStep >= 0 && nextStep < workflowTable_->rowCount() &&
      workflowTable_->item(nextStep, 5)) {
    nextAction = workflowTable_->item(nextStep, 5)->text().trimmed();
  }
  if (workflowSummaryLabel_) {
    const int total = std::max(1, workflowTable_->rowCount());
    const int readiness = std::clamp((stats.ready * 100) / total, 0, 100);
    const QString percent = QString("%1%").arg(readiness);
    const QString nextText = nextStep >= 0 && !nextAction.isEmpty()
                                 ? nextAction
                                 : uiText("Ready", "已就绪");
    const int openItems = stats.action + stats.blocked;
    workflowSummaryLabel_->setText(uiText("Readiness %1 | Next: %2 | Open %3",
                                          "就绪度 %1 | 下一步：%2 | 未完成 %3")
                                       .arg(percent, nextText)
                                       .arg(openItems));
    workflowSummaryLabel_->setProperty(
        "severity",
        stats.blocked > 0 ? "warning" : (stats.action > 0 ? "action" : "ok"));

    QStringList tip;
    tip << uiText("Commissioning readiness: %1", "调试就绪度：%1").arg(percent);
    tip << QString("%1: %2   %3: %4   %5: %6")
               .arg(uiText("Ready", "就绪"))
               .arg(stats.ready)
               .arg(uiText("Action", "待执行"))
               .arg(stats.action)
               .arg(uiText("Blocked", "受阻"))
               .arg(stats.blocked);
    if (nextStep >= 0 && !nextAction.isEmpty()) {
      tip << uiText("Next action: %1", "下一步动作：%1").arg(nextAction);
    }
    for (int row = 0; row < workflowTable_->rowCount(); ++row) {
      const QString statusKey =
          commissioningWorkflowStatusKeyForRow(workflowTable_, row);
      const QString status =
          workflowTable_->item(row, 1)
              ? workflowTable_->item(row, 1)->text().trimmed()
              : QString();
      if (statusKey == QStringLiteral("ready") ||
          (statusKey.isEmpty() && status == uiText("Ready", "就绪"))) {
        continue;
      }
      const QString phase = workflowTable_->item(row, 0)
                                ? workflowTable_->item(row, 0)->text().trimmed()
                                : QString();
      const QString step = workflowTable_->item(row, 2)
                               ? workflowTable_->item(row, 2)->text().trimmed()
                               : QString();
      const QString risk = workflowTable_->item(row, 3)
                               ? workflowTable_->item(row, 3)->text().trimmed()
                               : QString();
      const QString evidence =
          workflowTable_->item(row, 4)
              ? workflowTable_->item(row, 4)->text().trimmed()
              : QString();
      const QString actionText =
          workflowTable_->item(row, 5)
              ? workflowTable_->item(row, 5)->text().trimmed()
              : QString();
      tip << QString("#%1 [%2/%3] %4 | %5: %6 -> %7")
                 .arg(row + 1)
                 .arg(phase, status, step, risk, evidence, actionText);
    }
    workflowSummaryLabel_->setToolTip(tip.join('\n'));
    repolish(workflowSummaryLabel_); // force QSS re-evaluation after property change
  }

  if (auto *runNextButton = findChild<QPushButton *>("overviewRunNext")) {
    runNextButton->setEnabled(nextStep >= 0);
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


// — Rebuild the slave evidence overview matrix from all loaded evidence sources
void MainWindow::updateSlaveEvidenceMatrix() {
  if (!slaveEvidenceMatrixTable_) {
    return;
  }

  int previousPosition = selectedPosition();
  if (slaveEvidenceMatrixTable_->currentRow() >= 0) {
    bool ok = false;
    const int rowPosition = tableText(slaveEvidenceMatrixTable_,
                                      slaveEvidenceMatrixTable_->currentRow(),
                                      kSlaveEvidenceMatrixPositionColumn)
                                .toInt(&ok);
    if (ok) {
      previousPosition = rowPosition;
    }
  }

  const SlaveEvidenceUiTexts uiTexts = slaveEvidenceUiTexts();

  QVector<SlaveEvidenceInput> evidenceInputs;
  evidenceInputs.reserve(slaves_.size());
  const QStringList topologyIssues = topologyBaselineIssues();
  const SlaveEvidenceLoadedPositions loadedPositions = {
      loadedSlaveInfoPosition_, identityTable_ ? identityTable_->rowCount() : 0,
      loadedSdoPosition_,       sdoTable_ ? sdoTable_->rowCount() : 0,
      loadedPdoPosition_,       pdoTable_ ? pdoTable_->rowCount() : 0,
  };
  const SlaveEvidenceLoadedTables loadedTables = {
      watchTable_,
      startupSdoTable_,
      freeRunEntryTable_,
  };

  for (const auto &slave : slaves_) {
    SlaveEvidenceInput input;
    input.position = slave.position;
    input.name = slave.name.trimmed();
    input.state = slave.state.trimmed();
    applyLoadedSlaveEvidence(&input, loadedPositions, loadedTables);
    input.topologyIssue = !topologyIssues.isEmpty();
    evidenceInputs.append(input);
  }

  const SlaveEvidenceMatrix evidenceMatrix =
      buildSlaveEvidenceMatrix(evidenceInputs);

  QList<QStringList> rows;
  for (const auto &row : evidenceMatrix.rows) {
    rows.append(slaveEvidenceUiRow(row, uiTexts).cells);
  }

  setTableRows(slaveEvidenceMatrixTable_, slaveEvidenceMatrixHeaders(uiTexts),
               rows);

  const QColor okColor("#22c55e");
  const QColor actionColor("#f59e0b");
  const QColor warningColor("#ef4444");
  const QColor infoColor("#60a5fa");
  int restoreRow = -1;
  for (int rowIndex = 0; rowIndex < slaveEvidenceMatrixTable_->rowCount();
       ++rowIndex) {
    bool ok = false;
    const int rowPosition = tableText(slaveEvidenceMatrixTable_, rowIndex,
                                      kSlaveEvidenceMatrixPositionColumn)
                                .toInt(&ok);
    if (ok && rowPosition == previousPosition) {
      restoreRow = rowIndex;
    }
    const auto evidence = evidenceMatrix.rows.value(rowIndex);
    const SlaveEvidenceUiRow uiRow = slaveEvidenceUiRow(evidence, uiTexts);
    // Route to the appropriate evidence workspace
    setSlaveEvidenceMatrixRouteTarget(slaveEvidenceMatrixTable_, rowIndex,
                                      // Route to the appropriate evidence workspace
                                      slaveEvidenceRouteTarget(evidence));
    const QColor readinessColor =
        evidence.risks.isEmpty() && evidence.readiness >= evidence.maxReadiness
            ? okColor
            : (!evidence.risks.isEmpty() ? warningColor : actionColor);
    const QString tooltip = uiRow.detailLines.join('\n');
    for (int column = 0; column < slaveEvidenceMatrixTable_->columnCount();
         ++column) {
      if (auto *item = slaveEvidenceMatrixTable_->item(rowIndex, column)) {
        item->setToolTip(tooltip);
      }
    }
    if (auto *item = slaveEvidenceMatrixTable_->item(
            rowIndex, kSlaveEvidenceMatrixPriorityColumn)) {
      const int priorityRank = slaveEvidencePriorityRank(evidence.priority);
      item->setForeground(priorityRank <= 1   ? warningColor
                          : priorityRank == 2 ? actionColor
                                              : okColor);
    }
    if (auto *item = slaveEvidenceMatrixTable_->item(
            rowIndex, kSlaveEvidenceMatrixStateColumn)) {
      const QString current = item->text().toUpper();
      item->setForeground(
          (current == "OP" || current.startsWith("OP ")) ? okColor
          : (current.contains("SAFEOP") || current.contains("PREOP"))
              ? actionColor
              : infoColor);
    }
    if (auto *item = slaveEvidenceMatrixTable_->item(
            rowIndex, kSlaveEvidenceMatrixReadinessColumn)) {
      item->setForeground(readinessColor);
    }
    for (int column : {5, 6, 7, 9}) {
      if (auto *item = slaveEvidenceMatrixTable_->item(rowIndex, column)) {
        const QString text = item->text().toLower();
        item->setForeground((text.contains("missing") || text.contains("缺失"))
                                ? warningColor
                                : okColor);
      }
    }
    if (auto *item = slaveEvidenceMatrixTable_->item(
            rowIndex, kSlaveEvidenceMatrixStartupColumn)) {
      item->setForeground(evidence.startupDiffs > 0 ? warningColor : okColor);
    }
    if (auto *item = slaveEvidenceMatrixTable_->item(
            rowIndex, kSlaveEvidenceMatrixRiskColumn)) {
      item->setForeground(evidence.risks.isEmpty() ? okColor : warningColor);
    }
    if (auto *item = slaveEvidenceMatrixTable_->item(
            rowIndex, kSlaveEvidenceMatrixNextColumn)) {
      item->setForeground(evidence.nextAction == SlaveEvidenceNextAction::Ready
                              ? okColor
                              : actionColor);
    }
  }

  if (restoreRow >= 0) {
    slaveEvidenceMatrixTable_->setCurrentCell(
        restoreRow, kSlaveEvidenceMatrixPositionColumn);
  }
  fitTableColumnsToViewport(slaveEvidenceMatrixTable_,
                            kSlaveEvidenceMatrixNextColumn);

  if (slaveEvidenceMatrixSummaryLabel_) {
    const QString summary =
        slaves_.isEmpty()
            ? uiText("No slaves in current scan", "当前扫描没有从站")
            : uiText("Priority queue | slaves %1 | ready %2 | action %3 | "
                     "risk %4 | evidence gaps %5",
                     "优先级队列 | 从站 %1 | 就绪 %2 | 待执行 %3 | 风险 %4 | "
                     "证据缺口 %5")
                  .arg(slaves_.size())
                  .arg(evidenceMatrix.readyRows)
                  .arg(evidenceMatrix.actionRows)
                  .arg(evidenceMatrix.riskRows)
                  .arg(evidenceMatrix.evidenceGaps);
    slaveEvidenceMatrixSummaryLabel_->setText(summary);
    slaveEvidenceMatrixSummaryLabel_->setToolTip(uiText(
        "This matrix is read-only and uses already loaded UI evidence only. "
        "Double-click or Alt+Enter selects the slave locally and routes to the "
        "best loaded evidence table without loading OD/PDO/ESI data from the "
        "bus.",
        "该矩阵只读且只使用已加载界面证据。双击或 Alt+Enter 会本地选择从站并"
        "路由到最相关的已加载证据表，不会从总线加载 OD/PDO/ESI 数据。"));
    slaveEvidenceMatrixSummaryLabel_->setProperty(
        "severity", evidenceMatrix.riskRows > 0
                        ? "warning"
                        : (evidenceMatrix.actionRows > 0 ? "action" : "ok"));
    repolish(slaveEvidenceMatrixSummaryLabel_); // force QSS re-evaluation after property change
  }
  filterSlaveEvidenceMatrix();
}


// — Filter slave evidence matrix
void MainWindow::filterSlaveEvidenceMatrix() {
  if (!slaveEvidenceMatrixTable_) {
    return;
  }

  const QString needle = slaveEvidenceMatrixFilter_
                             ? slaveEvidenceMatrixFilter_->text().trimmed()
                             : QString();
  const QString scope =
      slaveEvidenceMatrixScopeFilter_
          ? slaveEvidenceMatrixScopeFilter_->currentData().toString()
          : QString::fromLatin1(kSlaveEvidenceScopeAll);
  const SlaveEvidenceMatrixFilterStats stats =
      filterSlaveEvidenceMatrixTable(slaveEvidenceMatrixTable_, scope, needle);

  if (slaveEvidenceMatrixReviewButton_) {
    slaveEvidenceMatrixReviewButton_->setEnabled(stats.hasVisibleIssue);
  }
  if (slaveEvidenceMatrixReviewNextButton_) {
    slaveEvidenceMatrixReviewNextButton_->setEnabled(stats.hasVisibleIssue);
  }
  if (slaveEvidenceMatrixCopyButton_) {
    const int current = slaveEvidenceMatrixTable_->currentRow();
    slaveEvidenceMatrixCopyButton_->setEnabled(
        current >= 0 && current < slaveEvidenceMatrixTable_->rowCount() &&
        !slaveEvidenceMatrixTable_->isRowHidden(current));
  }

  if (slaveEvidenceMatrixSummaryLabel_) {
    const QString scopeLabel =
        slaveEvidenceMatrixScopeFilter_
            ? slaveEvidenceMatrixScopeFilter_->currentText()
            : uiText("All", "全部");
    slaveEvidenceMatrixSummaryLabel_->setToolTip(
        uiText("Visible matrix rows: %1/%2\nScope: %3\nPriority: P0 %4 | P1 "
               "%5 | P2 %6 | P3 %7\nRisk: %8 | Action: %9 | Ready: "
               "%10\nFiltering is local only and does not read the bus.",
               "可见矩阵行：%1/%2\n范围：%3\n优先级：P0 %4 | P1 %5 | P2 %6 | "
               "P3 %7\n风险：%8 | 待执行：%9 | 就绪：%10\n过滤仅在本地完成，不"
               "读取总线。")
            .arg(stats.visible)
            .arg(slaveEvidenceMatrixTable_->rowCount())
            .arg(scopeLabel)
            .arg(stats.p0)
            .arg(stats.p1)
            .arg(stats.p2)
            .arg(stats.p3)
            .arg(stats.risk)
            .arg(stats.action)
            .arg(stats.ready));
  }
  updateSlaveEvidenceMatrixTriageButtons();
  updateTabBadges();
  updateWorkspaceBoundary();
}


// — Update slave evidence matrix triage buttons
void MainWindow::updateSlaveEvidenceMatrixTriageButtons() {
  if (slaveEvidenceMatrixTriageButtons_.isEmpty()) {
    return;
  }
  const SlaveEvidenceMatrixPriorityCounts priorityCounts =
      slaveEvidenceMatrixPriorityCounts(slaveEvidenceMatrixTable_);
  const QHash<QString, int> counts = {
      {QString::fromLatin1(kSlaveEvidenceScopePriorityP0), priorityCounts.p0},
      {QString::fromLatin1(kSlaveEvidenceScopePriorityP1), priorityCounts.p1},
      {QString::fromLatin1(kSlaveEvidenceScopePriorityP2), priorityCounts.p2},
      {QString::fromLatin1(kSlaveEvidenceScopePriorityP3), priorityCounts.p3},
  };
  const QHash<QString, QString> labels = {
      {QString::fromLatin1(kSlaveEvidenceScopePriorityP0), uiText("P0", "P0")},
      {QString::fromLatin1(kSlaveEvidenceScopePriorityP1), uiText("P1", "P1")},
      {QString::fromLatin1(kSlaveEvidenceScopePriorityP2), uiText("P2", "P2")},
      {QString::fromLatin1(kSlaveEvidenceScopePriorityP3), uiText("P3", "P3")},
  };
  const QString currentScope =
      slaveEvidenceMatrixScopeFilter_
          ? slaveEvidenceMatrixScopeFilter_->currentData().toString()
          : QString();
  for (auto *button : slaveEvidenceMatrixTriageButtons_) {
    if (!button) {
      continue;
    }
    const QString scope = button->property("scope").toString();
    const int count = counts.value(scope, 0);
    button->setText(
        QString("%1 %2").arg(labels.value(scope, scope)).arg(count));
    button->setCheckable(true);
    button->setChecked(scope == currentScope);
    button->setEnabled(slaveEvidenceMatrixTable_ &&
                       slaveEvidenceMatrixTable_->rowCount() > 0);
    button->setToolTip(
        uiText("Show %1 matrix rows. Count: %2. This is local UI filtering "
               "only and does not read the bus.",
               "显示 %1 矩阵行。数量：%2。这只是本地界面过滤，不读取总线。")
            .arg(labels.value(scope, scope))
            .arg(count));
  }
}


// — Review first slave evidence matrix issue
void MainWindow::reviewFirstSlaveEvidenceMatrixIssue() {
  if (!slaveEvidenceMatrixTable_) {
    return;
  }
  filterSlaveEvidenceMatrix();
  int fallbackRow = -1;
  for (int row = 0; row < slaveEvidenceMatrixTable_->rowCount(); ++row) {
    if (slaveEvidenceMatrixTable_->isRowHidden(row)) {
      continue;
    }
    const SlaveEvidenceMatrixRowState state =
        slaveEvidenceMatrixRowState(slaveEvidenceMatrixTable_, row);
    if (!state.reviewIssue) {
      continue;
    }
    if (state.hasRisk) {
      openSlaveEvidenceMatrixRow(row);
      return;
    }
    if (fallbackRow < 0) {
      fallbackRow = row;
    }
  }
  if (fallbackRow >= 0) {
    openSlaveEvidenceMatrixRow(fallbackRow);
    return;
  }
  statusBar()->showMessage(uiText("No visible matrix issue to review.",
                                  "当前没有可见的矩阵问题可审阅。"),
                           3000);
}


// — Review next slave evidence matrix issue
void MainWindow::reviewNextSlaveEvidenceMatrixIssue() {
  if (!slaveEvidenceMatrixTable_) {
    return;
  }
  filterSlaveEvidenceMatrix();
  const int rowCount = slaveEvidenceMatrixTable_->rowCount();
  if (rowCount <= 0) {
    statusBar()->showMessage(
        uiText("No matrix issue to review.", "当前没有矩阵问题可审阅。"), 3000);
    return;
  }

  auto isIssueRow = [this](int row) {
    if (row < 0 || row >= slaveEvidenceMatrixTable_->rowCount() ||
        slaveEvidenceMatrixTable_->isRowHidden(row)) {
      return false;
    }
    return slaveEvidenceMatrixRowState(slaveEvidenceMatrixTable_, row)
        .reviewIssue;
  };

  const int current = slaveEvidenceMatrixTable_->currentRow();
  for (int offset = 1; offset <= rowCount; ++offset) {
    const int row = (qMax(0, current) + offset) % rowCount;
    if (isIssueRow(row)) {
      openSlaveEvidenceMatrixRow(row);
      return;
    }
  }

  statusBar()->showMessage(uiText("No visible matrix issue to review.",
                                  "当前没有可见的矩阵问题可审阅。"),
                           3000);
}


// — Check whether copy slave evidence matrix row digest
bool MainWindow::copySlaveEvidenceMatrixRowDigest(int row) {
  if (!slaveEvidenceMatrixTable_ || row < 0 ||
      row >= slaveEvidenceMatrixTable_->rowCount() ||
      slaveEvidenceMatrixTable_->isRowHidden(row)) {
    statusBar()->showMessage(
        uiText("Select a visible slave evidence matrix row to copy.",
               "请选择一条可见的从站证据矩阵行再复制。"),
        3000);
    return false;
  }

  QStringList lines;
  lines << uiText("NekoEcat Studio Slave Evidence Matrix Row",
                  "NekoEcat Studio 从站证据矩阵本行");
  lines << QString("%1: %2").arg(uiText("Master", "主站"), activeMasterName());
  if (slaveEvidenceMatrixScopeFilter_) {
    lines << QString("%1: %2").arg(
        uiText("Matrix Scope", "矩阵范围"),
        slaveEvidenceMatrixScopeFilter_->currentText());
  }
  if (slaveEvidenceMatrixFilter_ &&
      !slaveEvidenceMatrixFilter_->text().trimmed().isEmpty()) {
    lines << QString("%1: %2").arg(
        uiText("Search", "搜索"), slaveEvidenceMatrixFilter_->text().trimmed());
  }
  if (slaveEvidenceMatrixSummaryLabel_ &&
      !slaveEvidenceMatrixSummaryLabel_->text().trimmed().isEmpty()) {
    lines << QString("%1: %2").arg(
        uiText("Matrix Summary", "矩阵摘要"),
        slaveEvidenceMatrixSummaryLabel_->text().trimmed());
  }

  lines << QString();
  lines << uiText("Row Evidence", "本行证据");
  for (int column = 0; column < slaveEvidenceMatrixTable_->columnCount();
       ++column) {
    const auto *header =
        slaveEvidenceMatrixTable_->horizontalHeaderItem(column);
    const QString field =
        header ? header->text().trimmed() : QString::number(column + 1);
    const QString value = tableText(slaveEvidenceMatrixTable_, row, column);
    lines << QString("- %1: %2")
                 .arg(field, value.isEmpty() ? uiText("Empty", "空") : value);
  }

  QString details;
  for (int column = 0; column < slaveEvidenceMatrixTable_->columnCount();
       ++column) {
    if (auto *item = slaveEvidenceMatrixTable_->item(row, column)) {
      details = item->toolTip().trimmed();
      if (!details.isEmpty()) {
        break;
      }
    }
  }
  if (!details.isEmpty()) {
    lines << QString();
    lines << uiText("Detailed Evidence", "详细证据");
    const QStringList detailLines = details.split('\n', Qt::SkipEmptyParts);
    for (const QString &detail : detailLines) {
      lines << QString("- %1").arg(detail.trimmed());
    }
  }

  lines << QString();
  lines << uiText("Local Action: Open Matrix Evidence routes this slave to the "
                  "best already loaded evidence table; Review First/Next walks "
                  "visible matrix issues.",
                  "本地动作：打开矩阵证据会把该从站路由到最相关的已加载证据表；"
                  "审阅首个/下个问题会遍历可见矩阵问题。");
  lines << uiText("Boundary: clipboard copy only; no bus read, no OD/PDO/ESI "
                  "load, no SDO write, no state change, no Free Run change, "
                  "and no Host Health.",
                  "边界：只复制到剪贴板；不读取总线、不加载 OD/PDO/ESI、不写 "
                  "SDO、不切换状态、不改变 Free Run，也不运行 Host Health。");

  QApplication::clipboard()->setText(lines.join('\n')); // copy to system clipboard
  updateDiagnostics("Info", "Slave Evidence Matrix",
                    uiText("Copied slave evidence matrix row #%1 to clipboard",
                           "已复制从站证据矩阵第 %1 行到剪贴板")
                        .arg(row + 1));
  statusBar()->showMessage(
      uiText("Copied slave evidence matrix row. No bus access was requested.",
             "已复制从站证据矩阵行；未请求总线访问。"),
      3000);
  return true;
}


// — Route to the detailed evidence workspace for the selected slave matrix row
void MainWindow::openSlaveEvidenceMatrixRow(int row) {
  if (!slaveEvidenceMatrixTable_ || row < 0 ||
      row >= slaveEvidenceMatrixTable_->rowCount()) {
    statusBar()->showMessage(uiText("Select a slave evidence row first.",
                                    "请先选择一条从站证据矩阵行。"),
                             3000);
    return;
  }

  bool ok = false;
  const int position = tableText(slaveEvidenceMatrixTable_, row,
                                 kSlaveEvidenceMatrixPositionColumn)
                           .toInt(&ok);
  if (!ok || position < 0) {
    return;
  }

  if (!selectSlaveForLocalEvidence(position)) {
    statusBar()->showMessage(
        uiText("Slave is not present in the local topology tree.",
               "本地拓扑树中没有该从站。"),
        3000);
    return;
  }

  selectAndFocusTableRow(slaveEvidenceMatrixTable_, row,
                         kSlaveEvidenceMatrixPositionColumn);

  QString target = uiText("Overview matrix row", "总览矩阵行");
  bool routed = false;
  switch (
      // Dispatch to the evidence workspace matching the route target
      slaveEvidenceMatrixRouteTargetForRow(slaveEvidenceMatrixTable_, row)) {
  // Route to the appropriate evidence workspace
  case SlaveEvidenceRouteTarget::ObjectDictionary:
    activateObjectDictionaryPaneFor(sdoTable_);
    if (loadedSdoPosition_ == position && sdoTable_ &&
        sdoTable_->rowCount() > 0) {
      selectAndFocusTableRow(
          sdoTable_, sdoTable_->currentRow() >= 0 ? sdoTable_->currentRow() : 0,
          0);
    }
    target = uiText("Object Dictionary evidence", "对象字典证据");
    routed = true;
    break;
  // Route to the appropriate evidence workspace
  case SlaveEvidenceRouteTarget::PdoMap:
    activateWorkspaceTab(pdoMapTabIndex_);
    if (loadedPdoPosition_ == position && pdoTable_ &&
        pdoTable_->rowCount() > 0) {
      selectAndFocusTableRow(
          pdoTable_, pdoTable_->currentRow() >= 0 ? pdoTable_->currentRow() : 0,
          0);
    }
    target = uiText("PDO Map evidence", "PDO 映射证据");
    routed = true;
    break;
  // Route to the appropriate evidence workspace
  case SlaveEvidenceRouteTarget::Watch: {
    const int watchRow = firstSlaveEvidenceDriveWatchRow(watchTable_, position);
    activateWorkspaceTab(watchTabIndex_);
    if (watchScopeFilter_) {
      const int scope = watchScopeFilter_->findData("all");
      if (scope >= 0) {
        watchScopeFilter_->setCurrentIndex(scope);
      }
    }
    if (watchChangedOnly_) {
      watchChangedOnly_->setChecked(false);
    }
    filterWatchTable();
    if (watchRow >= 0) {
      selectAndFocusTableRow(watchTable_, watchRow,
                             kSlaveEvidenceWatchPositionColumn);
    } else {
      const int anyWatchRow = firstSlaveEvidenceRowForPosition(
          watchTable_, position, kSlaveEvidenceWatchPositionColumn);
      if (anyWatchRow >= 0) {
        selectAndFocusTableRow(watchTable_, anyWatchRow,
                               kSlaveEvidenceWatchPositionColumn);
      }
    }
    target = uiText("Watch evidence", "Watch 证据");
    routed = true;
    break;
  }
  // Route to the appropriate evidence workspace
  case SlaveEvidenceRouteTarget::Startup: {
    updateStartupSdoWatchEvidence();
    const int startupRow =
        firstSlaveEvidenceStartupDiffRow(startupSdoTable_, position);
    activateWorkspaceTab(startupSdoTabIndex_);
    if (startupWatchDiffsOnly_) {
      startupWatchDiffsOnly_->setChecked(false);
    }
    filterStartupSdoTable();
    if (startupRow >= 0) {
      selectAndFocusTableRow(startupSdoTable_, startupRow,
                             kSlaveEvidenceStartupPositionColumn);
    }
    updateStartupSdoControls();
    target = uiText("Startup SDO evidence", "Startup SDO 证据");
    routed = true;
    break;
  }
  // Route to the appropriate evidence workspace
  case SlaveEvidenceRouteTarget::Process: {
    const int processRow =
        firstSlaveEvidenceProcessIssueRow(freeRunEntryTable_, position);
    activateWorkspaceTab(freeRunTabIndex_);
    filterFreeRunEntryTable();
    if (processRow >= 0) {
      selectAndFocusTableRow(freeRunEntryTable_, processRow,
                             kSlaveEvidenceProcessPositionColumn);
    } else {
      const int anyProcessRow = firstSlaveEvidenceRowForPosition(
          freeRunEntryTable_, position, kSlaveEvidenceProcessPositionColumn);
      if (anyProcessRow >= 0) {
        selectAndFocusTableRow(freeRunEntryTable_, anyProcessRow,
                               kSlaveEvidenceProcessPositionColumn);
      }
    }
    target = uiText("Free Run process evidence", "Free Run 过程证据");
    routed = true;
    break;
  }
  // Route to the appropriate evidence workspace
  case SlaveEvidenceRouteTarget::StateMachine: {
    updateStateMachineView();
    activateWorkspaceTab(stateMachineTabIndex_);
    const int stateRow =
        firstSlaveEvidenceRowForPosition(stateMachine_->stateMachineTable, position, 0);
    if (stateRow >= 0) {
      selectAndFocusTableRow(stateMachine_->stateMachineTable, stateRow, 0);
    }
    target = uiText("State Machine risk evidence", "状态机风险证据");
    routed = true;
    break;
  }
  // Route to the appropriate evidence workspace
  case SlaveEvidenceRouteTarget::Overview:
    break;
  }

  if (!routed) {
    activateWorkspaceTab(overviewTabIndex_);
  }
  updateDiagnostics(
      "Info", "Slave Evidence Matrix",
      uiText("Opened %1 for slave #%2 from the read-only evidence matrix "
             "without bus access.",
             "已从只读证据矩阵为从站 #%2 打开 %1，未访问总线。")
          .arg(target)
          .arg(position));
  statusBar()->showMessage(
      uiText("Local slave evidence opened. No bus access was requested.",
             "已打开本地从站证据；未请求总线访问。"),
      3000);
}


// — Return the matrix table row index matching the given slave position
int MainWindow::slaveEvidenceMatrixRowForPosition(int position) const {
  return firstSlaveEvidenceRowForPosition(slaveEvidenceMatrixTable_, position,
                                          kSlaveEvidenceMatrixPositionColumn);
}

