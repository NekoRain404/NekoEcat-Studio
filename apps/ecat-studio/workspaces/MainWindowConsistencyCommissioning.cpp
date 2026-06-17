// Consistency checks and commissioning workflow management.

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
    // Define color for visual feedback
      settings_.theme == "Light" ? QColor("#fee2e2") : QColor("#3a1218");
  const QColor warningBackground =
    // Define color for visual feedback
      settings_.theme == "Light" ? QColor("#fff7cc") : QColor("#3a2f16");
  const QColor infoBackground =
    // Define color for visual feedback
      settings_.theme == "Light" ? QColor("#eef6ff") : QColor("#122033");
  for (int row = 0; row < consistency_->consistencyTable->rowCount(); ++row) {
    const QString level = tableText(consistency_->consistencyTable, row, 0);
    QColor background;
    QColor foreground;
    if (level == uiText("Error", "错误")) {
      background = errorBackground;
    // Define color for visual feedback
      foreground = QColor("#ef4444");
    } else if (level == uiText("Warning", "警告")) {
      background = warningBackground;
    // Define color for visual feedback
      foreground = QColor("#f59e0b");
    } else if (level == uiText("Ready", "就绪")) {
    // Define color for visual feedback
      foreground = QColor("#22c55e");
    } else {
      background = infoBackground;
    // Define color for visual feedback
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


// Apply text and severity filters to the consistency check table, hiding rows that do not match
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
    // Set severity property for styling/theming
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
      if (watch_->startupWatchDiffsOnly) {
        watch_->startupWatchDiffsOnly->setChecked(false);
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
        watch_->watchTable, route.address.position, route.address.index,
        route.address.subIndex, 1, 2, 3);
    if (watchRow >= 0) {
      if (watch_->watchScopeFilter) {
        const int index = watch_->watchScopeFilter->findData("all");
        if (index >= 0) {
          watch_->watchScopeFilter->setCurrentIndex(index);
        }
      }
      if (watch_->watchChangedOnly) {
        watch_->watchChangedOnly->setChecked(false);
      }
      filterWatchTable();
      activateTabContainingWidget(tabs_, watch_->watchTable);
      selectAndFocusTableRow(watch_->watchTable, watchRow, 1);
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


// Count consistency issues by severity level (errors, warnings, info, ready) for the gate check
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


