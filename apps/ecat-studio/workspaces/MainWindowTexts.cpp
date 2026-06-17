// Localized UI text builders for detail panels.

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


// — Return localized text constants for the Startup SDO detail panel
StartupSdoRowDetailTexts MainWindow::startupSdoRowDetailTexts() const {
  return {
      .unavailableText = uiText("Startup SDO evidence is not available.",
                                "当前没有可用的 Startup SDO 证据。"),
      .unavailableTip =
          uiText("This preview is local only and does not write the bus.",
                 "此预览仅在本地工作，不写入总线。"),
      .noSelectionText =
          uiText("Select a visible Startup SDO row to review target, expected "
                 "value, Watch comparison, status, and write boundary.",
                 "选择一条可见 Startup SDO 行，以复核目标、期望值、Watch 对照、"
                 "状态和写入边界。"),
      .noSelectionTip = uiText(
          "Selecting rows, Diffs Only, and this detail strip are local review "
          "actions. Verify reads SDOs; Apply writes SDO values after "
          "confirmation.",
          "选择行、只看偏差和此详情条都是本地审阅动作；校验会读取 SDO，应用会"
          "在确认后写入 SDO 值。"),
      .defaultType = uiText("default type", "默认类型"),
      .emptyValue = uiText("Empty", "空"),
      .pendingStatus = uiText("Pending", "待应用"),
      .noWatchValue = uiText("No value", "无值"),
      .watchMismatch = uiText("Watch mismatch", "Watch 不一致"),
      .noWatchEvidence = uiText("No Watch evidence", "无 Watch 证据"),
      .pendingComparison = uiText("Pending comparison", "待比较"),
      .watchMatches = uiText("Watch matches", "Watch 匹配"),
      .reviewRow = uiText("Review row", "复核行"),
      .summaryPattern =
          uiText("Row %1 | #%2 %3:%4 | %5 = %6 | Status: %7 | Watch: %8 | %9",
                 "第 %1 行 | #%2 %3:%4 | %5 = %6 | 状态：%7 | Watch：%8 | %9"),
      .selectedTitle =
          uiText("Selected Startup SDO row", "选中的 Startup SDO 行"),
      .rowLabel = uiText("Row", "行"),
      .slaveLabel = uiText("Slave", "从站"),
      .objectLabel = uiText("Object", "对象"),
      .valueLabel = uiText("Value", "值"),
      .typeLabel = uiText("Type", "类型"),
      .statusLabel = uiText("Status", "状态"),
      .detailLabel = uiText("Detail", "详情"),
      .watchValueLabel = uiText("Watch Value", "Watch 值"),
      .watchDeltaLabel = uiText("Watch Delta", "Watch 偏差"),
      .localBoundary = uiText(
          "Local preview boundary: selecting this row, Diffs Only, row "
          "movement, preflight display, and this detail strip do not read "
          "SDOs, write SDOs, change state, toggle Free Run, or run Host "
          "Health.",
          "本地预览边界：选择此行、只看偏差、行移动、预检查显示和此详情条都不读"
          "取 "
          "SDO、不写 SDO、不切换状态、不改变 Free Run，也不运行 Host Health。"),
      .executionBoundary = uiText(
          "Execution boundary: Verify reads the target SDO; Apply Row, Apply "
          "Selected, Apply Diffs, and Apply Startup write SDO values only "
          "after the normal confirmation dialog.",
          "执行边界：校验会读取目标 "
          "SDO；应用此行、应用所选、应用偏差和应用启动项"
          "只会在常规确认对话框后写入 SDO 值。"),
  };
}


// — Return localized text constants for the SDO history detail panel
SdoHistoryRowDetailTexts MainWindow::sdoHistoryRowDetailTexts() const {
  return {
      .unavailableText = uiText("SDO history evidence is not available.",
                                "当前没有可用的 SDO 历史证据。"),
      .unavailableTip =
          uiText("This preview is local only and does not access the bus.",
                 "此预览仅在本地工作，不访问总线。"),
      .noSelectionText = uiText(
          "Select an SDO history row to review target, value, status, detail, "
          "reuse options, and operation boundary.",
          "选择一条 SDO 历史行，以复核目标、值、状态、详情、复用选项和操作边"
          "界。"),
      .noSelectionTip = uiText(
          "Selecting rows and reading this detail strip are local audit "
          "actions. Watch Selected creates Watch rows without immediate reads; "
          "Create Startup edits the Startup table; double-click fills and "
          "reads the SDO through the normal explicit read path.",
          "选择行和查看此详情条都是本地审计动作；监视所选只创建 Watch 行且不"
          "立即读取；创建启动项只编辑 Startup 表；双击会通过普通显式读取路径"
          "填充并读取 SDO。"),
      .timeFallback = uiText("Time?", "时间?"),
      .actionFallback = uiText("Action?", "动作?"),
      .typeFallback = uiText("type?", "类型?"),
      .noValue = uiText("No value", "无值"),
      .noStatus = uiText("No status", "无状态"),
      .fillTargetOnly = uiText("Fill target only", "仅回填目标"),
      .reusableValue =
          uiText("Can seed Watch or Startup", "可作为 Watch 或 Startup 种子"),
      .reviewFailure =
          uiText("Review failure before reuse", "复用前先复核失败原因"),
      .waitingRuntime = uiText("Waiting for runtime result", "等待运行时结果"),
      .summaryPattern =
          uiText("%1 %2 | #%3 %4:%5 %6 | Value: %7 | Status: %8 | %9",
                 "%1 %2 | #%3 %4:%5 %6 | 值：%7 | 状态：%8 | %9"),
      .selectedTitle = uiText("Selected SDO history row", "选中的 SDO 历史行"),
      .timeLabel = uiText("Time", "时间"),
      .actionLabel = uiText("Action", "动作"),
      .slaveLabel = uiText("Slave", "从站"),
      .objectLabel = uiText("Object", "对象"),
      .typeLabel = uiText("Type", "类型"),
      .valueLabel = uiText("Value", "值"),
      .statusLabel = uiText("Status", "状态"),
      .detailLabel = uiText("Detail", "详情"),
      .reuseLabel = uiText("Reuse", "复用"),
      .localBoundary = uiText(
          "Local audit boundary: selecting this row, reading this detail "
          "strip, filling SDO fields from history, adding selected history "
          "rows to Watch, and creating Startup SDO candidates from completed "
          "history values do not read SDOs, write SDOs, change state, toggle "
          "Free Run, or run Host Health.",
          "本地审计边界：选择此行、查看此详情条、从历史回填 SDO 字段、把所选"
          "历史行加入 Watch，以及用已完成且带值的历史行创建 Startup SDO 候选，"
          "都不读取 SDO、不写 SDO、不切换状态、不改变 Free Run，也不运行 Host "
          "Health。"),
      .executionBoundary = uiText(
          "Execution boundary: double-click and Fill and Read explicitly read "
          "the selected SDO; Refresh Watch later reads Watch rows; Startup "
          "Apply later writes Startup rows through the normal confirmation "
          "flow.",
          "执行边界：双击和填充并读取会显式读取所选 SDO；后续刷新 Watch 才读取 "
          "Watch 行；后续应用 Startup 会通过普通确认流程写入 Startup 行。"),
  };
}


// — Return localized text constants for the object bookmark panel
ObjectBookmarkDetailTexts MainWindow::objectBookmarkDetailTexts() const {
  return {
      .unavailableText = uiText("Object bookmark evidence is not available.",
                                "当前没有可用的对象书签证据。"),
      .unavailableTip =
          uiText("This preview is local only and does not access the bus.",
                 "此预览仅在本地工作，不访问总线。"),
      .noSelectionText = uiText(
          "Select an object bookmark to review target, access, saved value, "
          "source, reuse options, and operation boundary.",
          "选择一个对象书签，以复核目标、权限、保存值、来源、复用选项和操作边"
          "界。"),
      .noSelectionTip = uiText(
          "Selecting bookmarks and reading this detail strip are local project "
          "review actions. Fill only updates SDO fields; Watch only creates or "
          "reuses Watch rows; Startup only edits Startup rows until Apply is "
          "explicitly confirmed.",
          "选择书签和查看此详情条都是本地工程复核动作；回填只更新 SDO 字段；"
          "Watch 只创建或复用 Watch 行；Startup 只编辑启动行，直到显式确认 "
          "Apply。"),
      .readOnlyText = uiText("只读", "只读"),
      .typeFallback = uiText("type?", "类型?"),
      .accessFallback = uiText("Access?", "权限?"),
      .unnamed = uiText("Unnamed", "未命名"),
      .noValue = uiText("No value", "无值"),
      .projectSource = uiText("Project", "工程"),
      .fillTarget = uiText("Fill target locally", "本地回填目标"),
      .readyForWatchStartup =
          uiText("Ready for Watch or Startup", "可加入 Watch 或生成 Startup"),
      .readOnlyWatchOnly =
          uiText("Read-only evidence; Watch only", "只读证据；适合 Watch"),
      .missingAddress = uiText("Missing bookmark address", "缺少书签地址"),
      .noSavedValue =
          uiText("No saved value; fill target only", "无保存值；仅回填目标"),
      .summaryPattern =
          uiText("#%1 %2:%3 %4 | %5 | %6 | Last: %7 | Source: %8 | %9",
                 "#%1 %2:%3 %4 | %5 | %6 | 最后值：%7 | 来源：%8 | %9"),
      .selectedTitle = uiText("Selected object bookmark", "选中的对象书签"),
      .slaveLabel = uiText("Slave", "从站"),
      .objectLabel = uiText("Object", "对象"),
      .accessLabel = uiText("Access", "权限"),
      .typeLabel = uiText("Type", "类型"),
      .bitsLabel = uiText("Bits", "位宽"),
      .nameLabel = uiText("Name", "名称"),
      .lastValueLabel = uiText("Last Value", "最后值"),
      .sourceLabel = uiText("Source", "来源"),
      .reuseLabel = uiText("Reuse", "复用"),
      .localBoundary = uiText(
          "Local bookmark boundary: selecting this bookmark, reading this "
          "detail strip, filling SDO fields, adding selected bookmarks to "
          "Watch, removing bookmarks, and creating Startup SDO candidates from "
          "saved values do not read SDOs, write SDOs, change state, toggle "
          "Free Run, rescan/connect, or run Host Health.",
          "本地书签边界：选择此书签、查看此详情条、回填 SDO "
          "字段、把所选书签加入 "
          "Watch、移除书签，以及用保存值创建 Startup SDO 候选，都不读取 SDO、"
          "不写 SDO、不切换状态、不改变 Free Run、不重扫/连接，也不运行 Host "
          "Health。"),
      .executionBoundary = uiText(
          "Execution boundary: Fill and Read explicitly reads the bookmarked "
          "SDO; Refresh Watch later reads Watch rows; Startup Apply later "
          "writes Startup rows through the normal confirmation flow.",
          "执行边界：填充并读取会显式读取书签 SDO；后续 Refresh Watch 才读取 "
          "Watch 行；后续 Startup Apply 会通过普通确认流程写入 Startup 行。"),
  };
}


// — Return localized text constants for the SDO target trail panel
SdoTargetTrailDetailTexts MainWindow::sdoTargetTrailDetailTexts() const {
  return {
      .unavailableText = uiText("SDO target trail evidence is not available.",
                                "当前没有可用的 SDO 目标轨迹证据。"),
      .unavailableTip =
          uiText("This preview is local only and does not access the bus.",
                 "此预览仅在本地工作，不访问总线。"),
      .noSelectionText = uiText(
          "Select an SDO target trail row to review target, source, value, "
          "write value, reuse readiness, and operation boundary.",
          "选择一条 SDO 目标轨迹行，以复核目标、来源、值、写入值、复用就绪度和"
          "操作边界。"),
      .noSelectionTip = uiText(
          "Selecting rows and reading this detail strip are local review "
          "actions. Restore only fills fields; Watch only creates or reuses "
          "Watch rows; Bookmark only edits project bookmarks; Startup only "
          "edits Startup rows until Apply is explicitly confirmed.",
          "选择行和查看此详情条都是本地复核动作；恢复只回填字段；Watch 只创建"
          "或复用 Watch 行；书签只编辑工程书签；Startup 只编辑启动行，直到显式"
          "确认 Apply。"),
      .timeFallback = uiText("Time?", "时间?"),
      .typeFallback = uiText("type?", "类型?"),
      .unknownSource = uiText("Unknown", "未知"),
      .noValue = uiText("No value", "无值"),
      .noWriteValue = uiText("No write value", "无写入值"),
      .restoreTarget = uiText("Restore target locally", "本地恢复目标"),
      .readyForReuse = uiText("Ready for Watch, Bookmark, or Startup",
                              "可加入 Watch、收藏或生成 Startup"),
      .watchBookmarkOnly =
          uiText("Can seed Watch or Bookmark; Startup needs writable value",
                 "可作为 Watch 或书签种子；Startup 需要可写值"),
      .missingAddress = uiText("Missing target address", "缺少目标地址"),
      .noSavedValue =
          uiText("No saved value; fill target only", "无保存值；仅回填目标"),
      .summaryPattern =
          uiText("%1 | #%2 %3:%4 %5 | Source: %6 | Value: %7 | Write: %8 | %9",
                 "%1 | #%2 %3:%4 %5 | 来源：%6 | 值：%7 | 写入：%8 | %9"),
      .selectedTitle =
          uiText("Selected SDO target trail row", "选中的 SDO 目标轨迹行"),
      .timeLabel = uiText("Time", "时间"),
      .slaveLabel = uiText("Slave", "从站"),
      .objectLabel = uiText("Object", "对象"),
      .typeLabel = uiText("Type", "类型"),
      .sourceLabel = uiText("Source", "来源"),
      .valueLabel = uiText("Value", "值"),
      .writeValueLabel = uiText("Write Value", "写入值"),
      .startupCandidateLabel = uiText("Startup Candidate", "Startup 候选"),
      .detailLabel = uiText("Detail", "详情"),
      .reuseLabel = uiText("Reuse", "复用"),
      .localBoundary = uiText(
          "Local trail boundary: selecting this row, reading this detail "
          "strip, restoring the target, adding the trail row to Watch, "
          "bookmarking it, and creating a Startup SDO candidate from a saved "
          "value do not read SDOs, write SDOs, change state, toggle Free Run, "
          "rescan/connect, or run Host Health.",
          "本地轨迹边界：选择此行、查看此详情条、恢复目标、把轨迹行加入 Watch、"
          "收藏轨迹，以及用保存值创建 Startup SDO 候选，都不读取 SDO、不写 "
          "SDO、不切换状态、不改变 Free Run、不重扫/连接，也不运行 Host "
          "Health。"),
      .executionBoundary = uiText(
          "Execution boundary: explicit Read SDO reads the current target; "
          "Refresh Watch later reads Watch rows; Startup Apply later writes "
          "Startup rows through the normal confirmation flow.",
          "执行边界：显式 Read SDO 会读取当前目标；后续 Refresh Watch 才读取 "
          "Watch 行；后续 Startup Apply 会通过普通确认流程写入 Startup 行。"),
  };
}


// — Watch startup delta texts
WatchStartupDeltaTexts MainWindow::watchStartupDeltaTexts() const {
  return {uiText("no watch", "无监视"), uiText("pending", "待比较"),
          uiText("match", "匹配"), uiText("diff", "不一致")};
}


// — Return localized text constants for the state machine detail panel
StateMachineRowDetailTexts MainWindow::stateMachineRowDetailTexts() const {
  return {
      .unavailableText = uiText("State evidence is not available.",
                                "当前没有可用的状态机证据。"),
      .unavailableTip =
          uiText("This preview is local only and does not access the bus.",
                 "此预览仅在本地工作，不访问总线。"),
      .noSelectionText = uiText(
          "Select a visible slave row to review current state, recommendation, "
          "evidence, risk, and confirmation boundary.",
          "选择一条可见从站行，以复核当前状态、推荐状态、证据、风险和确认边界"
          "。"),
      .noSelectionTip =
          uiText("Selection only updates local evidence. State buttons and row "
                 "double-click remain the explicit request paths.",
                 "选择只更新本地证据；状态按钮和双击行仍是显式请求路径。"),
      .unnamed = uiText("Unnamed", "未命名"),
      .unknown = uiText("Unknown", "未知"),
      .none = uiText("None", "无"),
      .noRisk = uiText("No risk", "无风险"),
      .reviewEvidence = uiText("Review evidence", "复核证据"),
      .confirmedRequestBoundary =
          uiText("Explicit confirmed state request", "显式确认状态请求"),
      .localReviewBoundaryLabel =
          uiText("Local evidence review", "本地证据复核"),
      .opBoundary =
          uiText("OP transition requires confirmation", "OP 切换需要确认"),
      .safeopBoundary = uiText("SAFEOP transition requires confirmation",
                               "SAFEOP 切换需要确认"),
      .preopBoundary = uiText("PREOP transition requires confirmation",
                              "PREOP 切换需要确认"),
      .summaryPattern =
          uiText("#%1 %2 | Current: %3 | Recommended: %4 | %5 | Risk: %6 | "
                 "Action: %7",
                 "#%1 %2 | 当前：%3 | 推荐：%4 | %5 | 风险：%6 | 动作：%7"),
      .selectedTitle = uiText("Selected state-machine row", "选中的状态机行"),
      .slaveLabel = uiText("Slave", "从站"),
      .currentStateLabel = uiText("Current state", "当前状态"),
      .recommendedStateLabel = uiText("Recommended state", "推荐状态"),
      .evidenceLabel = uiText("Evidence", "证据"),
      .driveLabel = uiText("Drive", "驱动"),
      .startupLabel = uiText("Startup", "启动"),
      .processLabel = uiText("PDO/Process", "PDO/过程"),
      .riskLabel = uiText("Risk", "风险"),
      .boundaryLabel = uiText("Boundary", "边界"),
      .localBoundary = uiText(
          "Local preview boundary: selecting this row and reading this detail "
          "strip do not read the bus, write SDOs, change state, toggle Free "
          "Run, or run Host Health.",
          "本地预览边界：选择此行和查看此详情条都不读取总线、不写 SDO、不切换"
          "状态、不改变 Free Run，也不运行 Host Health。"),
      .executionBoundary = uiText(
          "Execution boundary: Send Recommended, PREOP, SAFEOP, OP, All PREOP, "
          "All SAFEOP, and row double-click remain explicit actions and keep "
          "the normal confirmation flow.",
          "执行边界：发送推荐状态、PREOP、SAFEOP、OP、全部 PREOP、全部 SAFEOP "
          "和双击行仍是显式动作，并继续走常规确认流程。"),
  };
}

CommissioningWorkflowStepDetailTexts
MainWindow::commissioningWorkflowStepDetailTexts() const {
  return {
      .unavailableText = uiText("Workflow evidence is not available.",
                                "当前没有可用的工作流证据。"),
      .unavailableTip =
          uiText("The detail strip is local only and does not access the bus.",
                 "详情条仅在本地工作，不访问总线。"),
      .noSelectionText =
          uiText("Select a visible workflow row to review risk, evidence, "
                 "next action, and execution boundary.",
                 "选择一条可见工作流行，以复核风险、依据、下一步和执行边界。"),
      .noSelectionTip = uiText(
          "Selection, filtering, review, and copy are local UI actions. Run "
          "Next or row double-click is the explicit execution path.",
          "选择、过滤、审阅和复制都是本地界面动作；执行下一步或双击行才是显式"
          "执行路径。"),
      .none = uiText("None", "无"),
      .noRisk = uiText("No risk", "无风险"),
      .summaryPattern =
          uiText("#%1 %2 / %3 | %4 | Risk: %5 | Evidence: %6 | Next: %7",
                 "#%1 %2 / %3 | %4 | 风险：%5 | 依据：%6 | 下一步：%7"),
      .selectedTitle = uiText("Selected workflow step", "选中的工作流步骤"),
      .phaseLabel = uiText("Phase", "阶段"),
      .statusLabel = uiText("Status", "状态"),
      .stepLabel = uiText("Step", "步骤"),
      .riskLabel = uiText("Risk", "风险"),
      .evidenceLabel = uiText("Evidence", "依据"),
      .nextActionLabel = uiText("Next Action", "下一步动作"),
      .boundaryLabel = uiText("Boundary", "边界"),
      .localReviewBoundary = uiText(
          "Local review boundary: selecting, filtering, Review First/Next, "
          "Copy Step, and this detail strip do not read the bus, load "
          "OD/PDO/ESI, write SDOs, change state, toggle Free Run, or run Host "
          "Health.",
          "本地审阅边界：选择、过滤、审阅首个/下个、复制步骤和此详情条都不读取"
          "总线、不加载 OD/PDO/ESI、不写 SDO、不切换状态、不改变 Free Run，也"
          "不运行 Host Health。"),
  };
}


// — Commissioning workflow texts
CommissioningWorkflowTexts MainWindow::commissioningWorkflowTexts() const {
  return {
      .ready = uiText("Ready", "就绪"),
      .action = uiText("Action", "待执行"),
      .blocked = uiText("Blocked", "受阻"),
      .onlineRuntimeAction = uiText("Online runtime action", "在线运行时动作"),
      .onlineTopologyAction = uiText("Online topology action", "在线拓扑动作"),
      .localTargetSelection = uiText("Local target selection", "本地目标选择"),
      .onlineOdRead = uiText("Online OD read", "在线 OD 读取"),
      .localEvidenceReview = uiText("Local evidence review", "本地证据审阅"),
      .onlinePdoRead = uiText("Online PDO read", "在线 PDO 读取"),
      .localWatchEdit = uiText("Local Watch edit", "本地 Watch 编辑"),
      .localStartupReview = uiText("Local Startup review", "本地 Startup 审阅"),
      .consistencyGate = uiText("Consistency gate", "一致性门禁"),
      .processDataAction = uiText("Process data action", "过程数据动作"),
      .connectRuntimeBoundary =
          uiText("Run Next or double-click may connect to the runtime or "
                 "refresh online master data.",
                 "执行下一步或双击可能连接运行时，或刷新在线主站数据。"),
      .scanTopologyBoundary =
          uiText("Run Next or double-click may request a topology scan and "
                 "online refresh.",
                 "执行下一步或双击可能请求总线重扫和在线刷新。"),
      .selectSlaveBoundary =
          uiText("Run Next or double-click focuses the topology tree only.",
                 "执行下一步或双击只会聚焦拓扑树。"),
      .inspectObjectDictionaryBoundary =
          uiText("Run Next or double-click opens Object Dictionary and may "
                 "load SDO metadata for the selected slave.",
                 "执行下一步或双击会打开对象字典，并可能加载当前从站的 SDO "
                 "元数据。"),
      .reviewObjectDictionaryEvidenceBoundary =
          uiText("Run Next or double-click opens already loaded failed OD "
                 "evidence.",
                 "执行下一步或双击会打开已加载的失败 OD 证据。"),
      .reviewPdoMapBoundary =
          uiText("Run Next or double-click opens PDO Map and may load process "
                 "data mapping for the selected slave.",
                 "执行下一步或双击会打开 PDO 映射，并可能加载当前从站的过程数据"
                 "映射。"),
      .monitorWatchBoundary = uiText(
          "Run Next or double-click opens Watch and adds the current "
          "SDO target as a Watch row; value refresh remains a separate "
          "explicit Watch action.",
          "执行下一步或双击会打开 Watch，并把当前 SDO 目标加入 Watch 行；"
          "数值刷新仍是单独的显式 Watch 动作。"),
      .reviewStartupDiffsBoundary =
          uiText("Run Next or double-click opens already loaded Startup diff "
                 "evidence.",
                 "执行下一步或双击会打开已加载的 Startup 偏差证据。"),
      .runConsistencyGateBoundary = uiText(
          "Run Next or double-click opens Consistency evidence or the "
          "Consistency workspace; refreshing the gate is an explicit "
          "read-only check from that workspace.",
          "执行下一步或双击会打开一致性证据或一致性工作区；刷新门禁是在该"
          "工作区中显式触发的只读检查。"),
      .validateProcessImageBoundary =
          uiText("Run Next or double-click opens Free Run and may toggle "
                 "process image telemetry.",
                 "执行下一步或双击会打开 Free Run，并可能切换过程映像遥测。"),
      .phaseHeader = uiText("Phase", "阶段"),
      .statusHeader = uiText("Status", "状态"),
      .stepHeader = uiText("Step", "步骤"),
      .riskHeader = uiText("Risk", "风险"),
      .evidenceHeader = uiText("Evidence", "依据"),
      .nextActionHeader = uiText("Next Action", "下一步动作"),
      .tooltipPattern =
          uiText("Phase: %1\nStatus: %2\nRisk: %3\nEvidence: %4\nNext: %5",
                 "阶段：%1\n状态：%2\n风险：%3\n依据：%4\n下一步：%5"),
  };
}


// — Next best action texts
NextBestActionTexts MainWindow::nextBestActionTexts() const {
  return {
      .commands = uiText("Commands", "命令"),
      .commandPaletteTip =
          uiText("Open the command palette for focused operations.",
                 "打开命令面板执行聚焦操作。"),
      .nextConnect = uiText("Next: Connect", "下一步：连接"),
      .connectTip = uiText("Connect to the local ecatd runtime.",
                           "连接本机 ecatd 运行时。"),
      .reviewDiagnostics = uiText("Review Diagnostics", "查看诊断"),
      .diagnosticsTip =
          uiText("Open Diagnostics to review current runtime or host errors.",
                 "打开诊断页复核当前运行时或主机错误。"),
      .nextRescan = uiText("Next: Rescan", "下一步：重扫"),
      .rescanTip = uiText("Rescan and refresh the active EtherCAT master.",
                          "重新扫描并刷新当前 EtherCAT 主站。"),
      .nextSelectSlave = uiText("Next: Select Slave", "下一步：选从站"),
      .selectSlaveTip =
          uiText("Focus the I/O tree so a target slave can be selected.",
                 "聚焦 I/O 树以选择目标从站。"),
      .nextLoadOd = uiText("Next: Load OD", "下一步：加载 OD"),
      .loadOdTip = uiText("Open Object Dictionary and load SDO metadata for "
                          "the selected slave.",
                          "打开对象字典并加载当前从站的 SDO 元数据。"),
      .reviewOdEvidence = uiText("Review OD Evidence", "审阅 OD 证据"),
      .failedOdEvidenceTip =
          uiText("Open Object Dictionary and focus the first failed SDO "
                 "evidence row.",
                 "打开对象字典并聚焦第一条失败 SDO 证据行。"),
      .nextLoadPdo = uiText("Next: Load PDO", "下一步：加载 PDO"),
      .loadPdoTip =
          uiText("Open PDO Map and load process-data mapping for the selected "
                 "slave.",
                 "打开 PDO 映射并加载当前从站的过程数据映射。"),
      .nextAddWatch = uiText("Next: Add Watch", "下一步：加入监视"),
      .addWatchTip = uiText(
          "Add the current SDO fields to Watch, then refresh key values.",
          "把当前 SDO 字段加入 Watch，然后刷新关键值。"),
      .reviewStartupDiffs = uiText("Review Startup Diffs", "审阅启动偏差"),
      .startupDiffsTip =
          uiText("Open Startup SDO and focus rows whose expected values differ "
                 "from current Watch evidence.",
                 "打开 Startup SDO 并聚焦期望值和当前 Watch 证据不一致的行。"),
      .reviewEvidence = uiText("Review Evidence", "审阅证据"),
      .consistencyEvidenceTip =
          uiText("Open the best loaded evidence table for the first blocking "
                 "Consistency row without bus access.",
                 "打开第一条阻塞一致性行最相关的已加载证据表，不访问总线。"),
      .reviewConsistency = uiText("Review Consistency", "审阅一致性"),
      .reviewConsistencyTip =
          uiText("Open Consistency Check and review blocking online/offline "
                 "evidence before continuing.",
                 "打开一致性检查，继续前复核阻塞性的 Online/Offline 证据。"),
      .runConsistency = uiText("Run Consistency", "运行一致性"),
      .runConsistencyTip =
          uiText("Run the read-only Consistency Check before process image "
                 "validation or state progression.",
                 "在验证过程映像或推进状态前运行只读一致性检查。"),
      .nextFreeRun = uiText("Next: Free Run", "下一步：自由运行"),
      .freeRunTip = uiText("Open Free Run and start process-image telemetry.",
                           "打开 Free Run 并启动过程映像遥测。"),
      .reviewMatrixRisk = uiText("Review Matrix Risk", "审阅矩阵风险"),
      .reviewMatrixAction = uiText("Review Matrix Action", "审阅矩阵动作"),
      .matrixTipPattern =
          uiText("Open the highest-priority visible row in the Overview slave "
                 "evidence matrix using local loaded evidence only. P0 %1; P1 "
                 "%2; P2 %3.",
                 "只使用已加载本地证据打开总览从站证据矩阵中优先级最高的可见"
                 "行。P0 %1；P1 %2；P2 %3。"),
  };
}


// — Host health texts
HostHealthTexts MainWindow::hostHealthTexts() const {
  return {
      .levelHeader = uiText("Level", "级别"),
      .checkHeader = uiText("Check", "检查项"),
      .resultHeader = uiText("Result", "结果"),
      .actionHeader = uiText("Action", "建议动作"),
      .commandHeader = uiText("Command", "命令"),
      .detailHeader = uiText("Detail", "细节"),
      .unchecked =
          uiText("Host health has not been checked", "尚未运行主机健康检查"),
      .needsAttention = uiText("Needs attention", "需要处理"),
      .usableWithWarnings = uiText("Usable with warnings", "可用但有警告"),
      .ready = uiText("Host environment ready", "主机环境就绪"),
      .warningLabel = QStringLiteral("Warning"),
      .okLabel = QStringLiteral("OK"),
  };
}


// — Return localized text constants for diagnostics event labels
DiagnosticsEventTexts MainWindow::diagnosticsEventTexts() const {
  return {
      .timeHeader = uiText("Time", "时间"),
      .levelHeader = uiText("Level", "级别"),
      .sourceHeader = uiText("Source", "来源"),
      .messageHeader = uiText("Message", "消息"),
      .noDiagnostics = uiText("No diagnostics", "暂无诊断"),
      .shown = uiText("shown", "条显示"),
      .errorLabel = QStringLiteral("Error"),
      .warningLabel = QStringLiteral("Warning"),
      .infoLabel = QStringLiteral("Info"),
  };
}


// — Return localized text constants for the consistency detail panel
ConsistencyDetailTexts MainWindow::consistencyDetailTexts() const {
  return {
      .unavailableText = uiText("Consistency evidence is not available.",
                                "当前没有可用的一致性证据。"),
      .unavailableTip =
          uiText("This preview is local only and does not access the bus.",
                 "此预览仅在本地工作，不访问总线。"),
      .selectVisibleRowText =
          uiText("Select a visible Consistency row to review severity, scope, "
                 "target, expected/actual evidence, recommended action, and "
                 "local route.",
                 "选择一条可见一致性行，以复核级别、范围、目标、期望/实际证"
                 "据、建议动作和本地路由。"),
      .selectVisibleRowTip =
          uiText("Filtering, selecting, reading this detail strip, and Open "
                 "Evidence are local review/navigation actions. Refresh Check "
                 "rebuilds the gate from already loaded UI and project "
                 "evidence; it does not read SDOs, write SDOs, change state, "
                 "toggle Free Run, or run Host Health.",
                 "筛选、选择、查看此详情条和打开证据都是本地审阅/导航动"
                 "作；刷新检查会用已加载界面和工程证据重建门禁，不读取 "
                 "SDO、不写 SDO、不切换状态、不改变 Free Run，也不运行 Host "
                 "Health。"),
      .summaryPattern =
          uiText("%1 | %2 | %3 | Expected: %4 | Actual: %5 | Action: %6",
                 "%1 | %2 | %3 | 期望：%4 | 实际：%5 | 动作：%6"),
      .selectedRowTitle = uiText("Selected Consistency row", "选中的一致性行"),
      .levelLabel = uiText("Level", "级别"),
      .scopeLabel = uiText("Scope", "范围"),
      .targetLabel = uiText("Target", "目标"),
      .evidenceLabel = uiText("Evidence", "证据"),
      .expectedLabel = uiText("Expected", "期望"),
      .actualLabel = uiText("Actual", "实际"),
      .actionLabel = uiText("Action", "建议动作"),
      .routeLabel = uiText("Open Evidence Route", "打开证据路由"),
      .levelFallback = uiText("Level?", "级别?"),
      .scopeFallback = uiText("Scope?", "范围?"),
      .targetFallback = uiText("No target", "无目标"),
      .expectedFallback = uiText("No expected evidence", "无期望证据"),
      .actualFallback = uiText("No actual evidence", "无实际证据"),
      .routeIo = uiText("Open I/O Variables evidence", "打开 I/O 变量证据"),
      .routeTopology =
          uiText("Open State Machine/topology evidence", "打开状态机/拓扑证据"),
      .routeStartup =
          uiText("Open Startup SDO evidence", "打开 Startup SDO 证据"),
      .routeWatchOrIo =
          uiText("Open Watch or I/O evidence", "打开 Watch 或 I/O 证据"),
      .routeReady =
          uiText("Ready row; continue commissioning", "就绪行；继续调试"),
      .localBoundary =
          uiText("Local gate boundary: selecting this row, filtering "
                 "consistency scopes, reading this detail strip, Open "
                 "Evidence, row double-click, row context menu evidence "
                 "navigation, and Command Palette evidence navigation only "
                 "route to already loaded local evidence tables.",
                 "本地门禁边界：选择此行、筛选一致性范围、查看此详情条、打开"
                 "证据、双击行、右键证据导航和命令面板证据导航只会路由到已"
                 "加载的本地证据表。"),
      .executionBoundary =
          uiText("Execution boundary: Refresh Check rebuilds the Consistency "
                 "table from current UI/project evidence. It does not read "
                 "SDOs, write SDOs, change state, toggle Free Run, "
                 "rescan/connect, or run Host Health.",
                 "执行边界：刷新检查会用当前界面/工程证据重建 Consistency "
                 "表，不读取 SDO、不写 SDO、不切换状态、不改变 Free Run、不重"
                 "扫/连接，也不运行 Host Health。"),
  };
}


// — Return localized text constants for the workspace boundary label
WorkspaceBoundaryTexts MainWindow::workspaceBoundaryTexts() const {
  return {
      .workspacePattern = uiText("Workspace: %1", "工作区：%1"),
      .overviewLabel = uiText("Boundary: Mixed", "边界：混合"),
      .overviewMixedActions =
          uiText("Overview mixes local evidence navigation with explicit "
                 "online actions such as Connect, Refresh, Run Next, and Drive "
                 "Next.",
                 "总览页混合本地证据导航和显式在线动作，例如连接、刷新、执行"
                 "下一步和驱动下一步。"),
      .overviewLocalEvidence =
          uiText("Session Brief rows and Slave Evidence Matrix review remain "
                 "local evidence navigation.",
                 "会话简报行和从站证据矩阵审阅仍是本地证据导航。"),
      .overviewMatrixPattern =
          uiText("Matrix queue: P0 %1 | P1 %2 | P2 %3 | P3 %4",
                 "矩阵队列：P0 %1 | P1 %2 | P2 %3 | P3 %4"),
      .objectDictionaryLabel = uiText("Boundary: SDO", "边界：SDO"),
      .objectDictionaryLocalFill =
          uiText("Selecting rows and Alt+Enter only fill local SDO context.",
                 "选择行和 Alt+Enter 只回填本地 SDO 上下文。"),
      .objectDictionaryOnlineAccess =
          uiText("Read, Write, batch reads, and failed-row retry explicitly "
                 "use mailbox SDO access.",
                 "读取、写入、批量读取和失败重试会显式使用 mailbox SDO 访问。"),
      .pdoMapLabel = uiText("Boundary: Online PDO", "边界：在线 PDO"),
      .pdoMapLoadedLocal =
          uiText("PDO Map review is local once loaded; loading or refreshing "
                 "PDO data is an online action.",
                 "PDO 映射加载后可本地审阅；加载或刷新 PDO 数据属于在线动作。"),
      .pdoMapLocalFill =
          uiText("Alt+Enter only fills the SDO target from the selected PDO "
                 "row.",
                 "Alt+Enter 只从选中 PDO 行回填 SDO 目标。"),
      .watchLabel = uiText("Boundary: Watch Reads", "边界：Watch 读取"),
      .watchReadsOnline =
          uiText("Watch rows and scopes are local evidence; Refresh and Auto "
                 "poll SDO objects.",
                 "Watch 行和范围是本地证据；刷新和自动模式会轮询 SDO 对象。"),
      .watchStartupLocal =
          uiText("Startup sync changes only the Startup table until Apply is "
                 "used.",
                 "同步 Startup 只改 Startup 表，直到使用应用动作才写总线。"),
      .startupSdoLabel =
          uiText("Boundary: Startup Danger", "边界：Startup 风险"),
      .startupSdoLocalEditing =
          uiText("Editing, preflight, Alt+Enter, and Watch diff review are "
                 "local.",
                 "编辑、预检查、Alt+Enter 和 Watch 偏差审阅是本地动作。"),
      .startupSdoOnlineApply = uiText(
          "Apply, Apply Selected, Apply Diffs, and Verify use online "
          "SDO access and keep confirmation/review flows.",
          "应用、应用所选、应用偏差和校验会使用在线 SDO 访问，并保留确认/"
          "审阅流程。"),
      .freeRunLabel = uiText("Boundary: Process Data", "边界：过程数据"),
      .freeRunProcessData =
          uiText("Free Run telemetry can exchange process data with devices.",
                 "Free Run 遥测可能与设备交换过程数据。"),
      .freeRunLocalFiltering =
          uiText("Filtering rows and Alt+Enter are local; toggling Free Run "
                 "uses the existing confirmation path.",
                 "过滤行和 Alt+Enter 是本地动作；切换 Free Run 使用既有确认流"
                 "程。"),
      .ioVariablesLabel = uiText("Boundary: Engineering", "边界：工程"),
      .ioVariablesMergedEvidence =
          uiText("I/O Variables merges loaded PDO, Free Run, Watch, Startup, "
                 "and project metadata.",
                 "I/O 变量合并已加载 PDO、Free Run、Watch、Startup 和工程元数"
                 "据。"),
      .ioVariablesLocalEditing = uiText(
          "Alias, tags, exports, Alt+Enter, and Watch creation are "
          "local; Read SDO is explicit online access.",
          "Alias、标签、导出、Alt+Enter 和创建 Watch 是本地动作；Read SDO "
          "是显式在线访问。"),
      .consistencyLabel = uiText("Boundary: Local Gate", "边界：本地门禁"),
      .consistencyLoadedEvidence =
          uiText("Consistency uses already loaded session and project "
                 "evidence.",
                 "一致性只使用已加载会话和工程证据。"),
      .consistencyLocalNavigation =
          uiText("Open Evidence and Alt+Enter navigate local evidence only.",
                 "打开证据和 Alt+Enter 只导航本地证据。"),
      .stateMachineLabel = uiText("Boundary: State Danger", "边界：状态风险"),
      .stateMachineOnlineRequests = uiText(
          "The matrix is local evidence, but Send Recommended and "
          "state buttons request real EtherCAT state changes.",
          "状态矩阵是本地证据，但发送推荐状态和状态按钮会请求真实 EtherCAT "
          "状态切换。"),
      .stateMachineConfirmation =
          uiText("State requests keep the grouped risk confirmation flow.",
                 "状态请求保留风险分组确认流程。"),
      .diagnosticsLabel = uiText("Boundary: Host", "边界：主机"),
      .diagnosticsHostOnly =
          uiText("Diagnostics is the only Host Health workspace.",
                 "诊断页是唯一的 Host Health 工作区。"),
      .diagnosticsHostCheck =
          uiText("Running Host Check inspects the host; Copy Command only "
                 "copies repair text.",
                 "运行 Host Check 会检查主机；复制命令只复制修复文本。"),
      .rtTestLabel = uiText("Boundary: RT Test", "边界：RT 测试"),
      .rtTestOnlineCycle =
          uiText("RT test exercises the bus with ecrt receive/send cycles.",
                 "RT 测试通过 ecrt 收发周期对总线进行压力测试。"),
      .rtTestLocalStats =
          uiText("Timeline and statistics are local display only.",
                 "时间线和统计仅为本地显示。"),
      .esiLabel = uiText("Boundary: File/ESI", "边界：文件/ESI"),
      .esiFileEvidence =
          uiText("ESI repository and XML review are file/project evidence.",
                 "ESI 仓库和 XML 查看属于文件/工程证据。"),
      .esiImportAction = uiText("Importing ESI XML is an explicit file action.",
                                "导入 ESI XML 是显式文件动作。"),
      .notesLabel = uiText("Boundary: Project", "边界：工程"),
      .notesLocalRecords =
          uiText("Notes are project-local commissioning records.",
                 "备注是工程内调试记录。"),
      .rawEvidenceLabel = uiText("Boundary: Raw Evidence", "边界：原始证据"),
      .rawEvidenceCachedOutput =
          uiText("Raw output pages show cached command output and do not act "
                 "on the bus by themselves.",
                 "原始输出页显示缓存命令输出，本身不会操作总线。"),
  };
}


// — Return localized text constants for tab badge tooltips
WorkspaceTabBadgeTexts MainWindow::workspaceTabBadgeTexts() const {
  return {
      .overview = uiText("Overview", "总览"),
      .watch = uiText("Watch", "监视"),
      .startupSdo = uiText("Startup SDO", "启动 SDO"),
      .freeRun = uiText("Free Run", "自由运行"),
      .ioVariables = uiText("I/O Variables", "I/O 变量"),
      .consistency = uiText("Consistency", "一致性"),
      .stateMachine = uiText("State Machine", "状态机"),
      .diagnostics = uiText("Diagnostics", "诊断"),
      .overviewTipPattern =
          uiText("Slave matrix priority queue: P0 %1; P1 %2; P2 %3; P3 %4",
                 "从站矩阵优先级队列：P0 %1；P1 %2；P2 %3；P3 %4"),
      .watchTipPattern = uiText("Watch rows: %1; Startup mismatches: %2",
                                "Watch 行：%1；Startup 不一致：%2"),
      .startupSdoTipPattern =
          uiText("Startup SDO rows: %1; Watch mismatches: %2",
                 "Startup SDO 行：%1；Watch 不一致：%2"),
      .freeRunTipPattern = uiText("Process-image rows: %1", "过程映像行：%1"),
      .ioVariablesTipPattern =
          uiText("I/O variable rows: %1; review issues: %2",
                 "I/O 变量行：%1；需复核问题：%2"),
      .consistencyTipPattern =
          uiText("Consistency errors: %1; warnings: %2; info: %3; ready: %4",
                 "一致性错误：%1；警告：%2；信息：%3；就绪：%4"),
      .stateMachineTipPattern =
          uiText("State-machine risk rows: %1", "状态机风险行：%1"),
      .diagnosticsTipPattern =
          uiText("Diagnostics errors: %1; warnings: %2; info: %3",
                 "诊断错误：%1；警告：%2；信息：%3"),
  };
}


// — Return localized text constants for the session brief table headers
SessionBriefUiTexts MainWindow::sessionBriefUiTexts() const {
  return {
      .ready = uiText("Ready", "就绪"),
      .action = uiText("Action", "待执行"),
      .warning = uiText("Warning", "警告"),
      .error = uiText("Error", "错误"),
      .info = uiText("Info", "信息"),
      .areaHeader = uiText("Area", "区域"),
      .statusHeader = uiText("Status", "状态"),
      .evidenceHeader = uiText("Evidence", "依据"),
      .nextHeader = uiText("Next", "下一步"),
      .openLocalEvidenceTooltipPattern =
          uiText("%1\nOpen local evidence with Enter, double-click, the row "
                 "menu, or Command Palette.",
                 "%1\n可用 Enter、双击、行菜单或命令面板打开本地证据。"),
  };
}

