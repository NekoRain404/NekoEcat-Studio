// Full UI construction: tabs, toolbars, panels, and metric cards.

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


// — Construct the entire main window layout, tabs, tables, and toolbars from scratch
void MainWindow::buildUi() {
  // Reset all page pointers before constructing the layout
  overviewPage_ = nullptr;
  objectDictionaryPage_ = nullptr;
  pdoMapPage_ = nullptr;
  watchPage_ = nullptr;
  startupSdoPage_ = nullptr;
  freeRunPage_ = nullptr;
  ioVariablePage_ = nullptr;
  consistencyPage_ = nullptr;
  stateMachinePage_ = nullptr;
  diagnosticsPage_ = nullptr;
  esiRepositoryPage_ = nullptr;
  notesPage_ = nullptr;
  rtTestPage_ = nullptr;
  esiXmlPage_ = nullptr;
  masterRawPage_ = nullptr;
  slaveRawPage_ = nullptr;
  pdoRawPage_ = nullptr;
  sdoRawPage_ = nullptr;
  sdoModeTabs_ = nullptr;
  pdoDetailLabel_ = nullptr;
  sdoTargetTrailDetailLabel_ = nullptr;
  objectBookmarkDetailLabel_ = nullptr;
  sdoHistoryDetailLabel_ = nullptr;
  slaveEvidenceMatrixReviewButton_ = nullptr;
  slaveEvidenceMatrixReviewNextButton_ = nullptr;
  slaveEvidenceMatrixCopyButton_ = nullptr;
  stateMachine_ = nullptr;
  freeRunEntryDetailLabel_ = nullptr;
  session_ = nullptr;
  workflowScopeFilter_ = nullptr;
  workflowFilter_ = nullptr;
  workflowStepDetailLabel_ = nullptr;
  workflowReviewButton_ = nullptr;
  workflowReviewNextButton_ = nullptr;
  workflowStepCopyButton_ = nullptr;
  watchDetailLabel_ = nullptr;
  startupSdoDetailLabel_ = nullptr;
  ioVar_->ioVariableDetailLabel = nullptr;
  consistency_ = nullptr;
  ioVar_ = nullptr;
  diagnostics_ = nullptr;
  slaveEvidenceMatrixTriageButtons_.clear();
  slaveEvidenceMatrixFilter_ = nullptr;
  slaveEvidenceMatrixScopeFilter_ = nullptr;
  workspaceBackStack_.clear();
  workspaceForwardStack_.clear();
  suppressWorkspaceHistory_ = false;
  overviewTabIndex_ = -1;
  objectDictionaryTabIndex_ = -1;
  pdoMapTabIndex_ = -1;
  watchTabIndex_ = -1;
  startupSdoTabIndex_ = -1;
  freeRunTabIndex_ = -1;
  ioVariableTabIndex_ = -1;
  consistencyTabIndex_ = -1;
  stateMachineTabIndex_ = -1;
  diagnosticsTabIndex_ = -1;
  esiRepositoryTabIndex_ = -1;
  notesTabIndex_ = -1;
  rtTestTabIndex_ = -1;
  esiXmlTabIndex_ = -1;
  masterRawTabIndex_ = -1;
  slaveRawTabIndex_ = -1;
  pdoRawTabIndex_ = -1;
  sdoRawTabIndex_ = -1;
  setWindowTitle(
      projectName_ == "Untitled"
          ? uiText("NekoEcat Studio - Untitled", "NekoEcat Studio - 未命名")
          : QString("NekoEcat Studio - %1").arg(projectName_));

  auto *fileMenu = menuBar()->addMenu(uiText("Project", "工程"));
  auto *newProjectAction =
      fileMenu->addAction(style()->standardIcon(QStyle::SP_FileDialogNewFolder),
                          uiText("New Project", "新建工程"));
  newProjectAction->setObjectName("newProjectAction");
  newProjectAction->setShortcut(QKeySequence::New);
  auto *openProjectAction =
      fileMenu->addAction(style()->standardIcon(QStyle::SP_DialogOpenButton),
                          uiText("Open Project", "打开工程"));
  openProjectAction->setObjectName("openProjectAction");
  openProjectAction->setShortcut(QKeySequence::Open);
  auto *saveProjectAction =
      fileMenu->addAction(style()->standardIcon(QStyle::SP_DialogSaveButton),
                          uiText("Save Project", "保存工程"));
  saveProjectAction->setObjectName("saveProjectAction");
  saveProjectAction->setShortcut(QKeySequence::Save);
  auto *saveProjectAsAction =
      fileMenu->addAction(style()->standardIcon(QStyle::SP_DialogSaveButton),
                          uiText("Save Project As", "工程另存为"));
  saveProjectAsAction->setObjectName("saveProjectAsAction");
  saveProjectAsAction->setShortcut(QKeySequence::SaveAs);
  fileMenu->addSeparator();
  auto *exportAction = fileMenu->addAction(
      style()->standardIcon(QStyle::SP_FileDialogDetailedView),
      uiText("Export Diagnostics Report", "导出诊断报告"));
  exportAction->setObjectName("exportReportAction");
  auto *exportIoVariablesAction = fileMenu->addAction(
      style()->standardIcon(QStyle::SP_DialogSaveButton),
      uiText("Export I/O Variables CSV", "导出 I/O 变量 CSV"));
  exportIoVariablesAction->setObjectName("exportIoVariablesAction");
  auto *exportIoPlcAction = fileMenu->addAction(
      style()->standardIcon(QStyle::SP_DialogSaveButton),
      uiText("Export PLC Symbols CSV", "导出 PLC 符号 CSV"));
  exportIoPlcAction->setObjectName("exportIoPlcSymbolsAction");
  auto *exportPlcDeclarationsAction = fileMenu->addAction(
      style()->standardIcon(QStyle::SP_DialogSaveButton),
      uiText("Export PLC Declarations ST", "导出 PLC 声明 ST"));
  exportPlcDeclarationsAction->setObjectName("exportPlcDeclarationsAction");
  auto *reviewPlcHandoffAction = fileMenu->addAction(
      style()->standardIcon(QStyle::SP_FileDialogDetailedView),
      uiText("Review PLC Handoff Issues", "审阅 PLC 交接问题"));
  reviewPlcHandoffAction->setObjectName("reviewPlcHandoffAction");
  fileMenu->addSeparator();
  fileMenu->addAction(style()->standardIcon(QStyle::SP_DialogCloseButton),
                      uiText("Exit", "退出"), this, &QWidget::close);

  auto *onlineMenu = menuBar()->addMenu(uiText("Online", "在线"));
  auto *menuConnectAction =
      onlineMenu->addAction(style()->standardIcon(QStyle::SP_DriveNetIcon),
                            uiText("Connect", "连接"));
  menuConnectAction->setObjectName("menuConnectAction");
  menuConnectAction->setShortcut(QKeySequence("Ctrl+K"));
  auto *menuRefreshAction =
      onlineMenu->addAction(style()->standardIcon(QStyle::SP_BrowserReload),
                            uiText("Refresh", "刷新"));
  menuRefreshAction->setObjectName("menuRefreshAction");
  menuRefreshAction->setShortcut(QKeySequence::Refresh);
  auto *menuRescanAction = onlineMenu->addAction(
      style()->standardIcon(QStyle::SP_FileDialogDetailedView),
      uiText("Rescan", "重新扫描"));
  menuRescanAction->setObjectName("menuRescanAction");
  menuRescanAction->setShortcut(QKeySequence("Ctrl+Shift+R"));
  onlineMenu->addSeparator();
  auto *allInitAction = onlineMenu->addAction(
      uiText("Set All Slaves to INIT", "全部从站切换到 INIT"));
  allInitAction->setObjectName("allInitAction");
  auto *allPreOpAction = onlineMenu->addAction(
      uiText("Set All Slaves to PREOP", "全部从站切换到 PREOP"));
  allPreOpAction->setObjectName("allPreOpAction");
  auto *allSafeOpAction = onlineMenu->addAction(
      uiText("Set All Slaves to SAFEOP", "全部从站切换到 SAFEOP"));
  allSafeOpAction->setObjectName("allSafeOpAction");
  auto *allOpAction = onlineMenu->addAction(
      uiText("Set All Slaves to OP", "全部从站切换到 OP"));
  allOpAction->setObjectName("allOpAction");

  auto *viewMenu = menuBar()->addMenu(uiText("View", "视图"));
  auto *showLogAction =
      viewMenu->addAction(style()->standardIcon(QStyle::SP_FileDialogInfoView),
                          uiText("Show Runtime Log", "显示运行日志"));
  showLogAction->setObjectName("showLogAction");
  showLogAction->setShortcut(QKeySequence("Ctrl+`"));
  viewMenu->addSeparator();
  auto *workspaceBackAction =
      viewMenu->addAction(style()->standardIcon(QStyle::SP_ArrowBack),
                          uiText("Back Workspace", "后退工作区"));
  workspaceBackAction->setObjectName("workspaceBackAction");
  workspaceBackAction->setShortcut(QKeySequence("Alt+Left"));
  workspaceBackAction->setStatusTip(
      uiText("Return to the previous workspace without running online commands",
             "返回上一个工作区，不执行在线命令"));
  auto *workspaceForwardAction =
      viewMenu->addAction(style()->standardIcon(QStyle::SP_ArrowForward),
                          uiText("Forward Workspace", "前进工作区"));
  workspaceForwardAction->setObjectName("workspaceForwardAction");
  workspaceForwardAction->setShortcut(QKeySequence("Alt+Right"));
  workspaceForwardAction->setStatusTip(uiText(
      "Move forward in workspace history without running online commands",
      "前进到下一个工作区，不执行在线命令"));
  viewMenu->addSeparator();
  auto *workspaceMenu =
      viewMenu->addMenu(style()->standardIcon(QStyle::SP_FileDialogListView),
                        uiText("Workspaces", "工作区"));
  struct WorkspaceActionSpec {
    QString text;
    const char *objectName;
    QString shortcut;
  };
  const QList<WorkspaceActionSpec> workspaceActions = {
      {uiText("Overview", "总览"), "goOverviewAction", "Ctrl+Alt+1"},
      {uiText("Object Dictionary", "对象字典"), "goObjectDictionaryAction",
       "Ctrl+Alt+2"},
      {uiText("PDO Map", "PDO 映射"), "goPdoMapAction", "Ctrl+Alt+3"},
      {uiText("Watch", "监视"), "goWatchAction", "Ctrl+Alt+4"},
      {uiText("Startup SDO", "启动 SDO"), "goStartupSdoAction", "Ctrl+Alt+5"},
      {uiText("Free Run", "自由运行"), "goFreeRunAction", "Ctrl+Alt+6"},
      {uiText("I/O Variables", "I/O 变量"), "goIoVariablesAction",
       "Ctrl+Alt+7"},
      {uiText("Consistency", "一致性"), "goConsistencyAction", "Ctrl+Alt+8"},
      {uiText("State Machine", "状态机"), "goStateMachineAction", "Ctrl+Alt+9"},
      {uiText("Diagnostics", "诊断"), "goDiagnosticsAction", "Ctrl+Alt+0"},
      {uiText("ESI Repository", "ESI 仓库"), "goEsiRepositoryAction", {}},
      {uiText("Notes", "备注"), "goNotesAction", {}},
      {uiText("RT Test", "RT 测试"), "goRtTestAction", {}},
      {uiText("ESI XML", "ESI XML"), "goEsiXmlAction", {}},
      {uiText("Master Raw", "主站原始输出"), "goMasterRawAction", {}},
      {uiText("Slave Raw", "从站原始输出"), "goSlaveRawAction", {}},
      {uiText("PDO Raw", "PDO 原始输出"), "goPdoRawAction", {}},
      {uiText("SDO Raw", "SDO 原始输出"), "goSdoRawAction", {}},
  };
  for (const auto &entry : workspaceActions) {
    auto *action = workspaceMenu->addAction(
        style()->standardIcon(QStyle::SP_FileDialogListView), entry.text);
    action->setObjectName(entry.objectName);
    if (!entry.shortcut.isEmpty()) {
      action->setShortcut(QKeySequence(entry.shortcut));
    }
    action->setStatusTip(
        uiText("Switch workspace without running online commands",
               "切换工作区，不执行在线命令"));
  }

  auto *toolsMenu = menuBar()->addMenu(uiText("Tools", "工具"));
  auto *commandPaletteAction = toolsMenu->addAction(
      style()->standardIcon(QStyle::SP_FileDialogContentsView),
      uiText("Command Palette", "命令面板"));
  commandPaletteAction->setObjectName("commandPaletteAction");
  commandPaletteAction->setShortcut(QKeySequence("Ctrl+P"));
  toolsMenu->addSeparator();
  auto *importEsiAction =
      toolsMenu->addAction(style()->standardIcon(QStyle::SP_DialogOpenButton),
                           uiText("Import ESI XML", "导入 ESI XML"));
  importEsiAction->setObjectName("importEsiAction");
  auto *settingsAction =
      toolsMenu->addAction(style()->standardIcon(QStyle::SP_ComputerIcon),
                           uiText("Settings", "设置"));
  settingsAction->setObjectName("settingsAction");
  settingsAction->setShortcut(QKeySequence::Preferences);

  auto *helpMenu = menuBar()->addMenu(uiText("Help", "帮助"));
  auto *manualAction =
      helpMenu->addAction(style()->standardIcon(QStyle::SP_FileDialogInfoView),
                          uiText("User Manual", "使用说明书"));
  manualAction->setObjectName("manualAction");
  manualAction->setShortcut(QKeySequence::HelpContents);
  helpMenu->addSeparator();
  auto *aboutAction = helpMenu->addAction(
      style()->standardIcon(QStyle::SP_MessageBoxInformation),
      uiText("About NekoEcat Studio", "关于 NekoEcat Studio"));
  aboutAction->setObjectName("aboutAction");

  auto *toolbar = addToolBar(uiText("Operations", "操作"));
  toolbar->setMovable(false);
  toolbar->setIconSize(QSize(16, 16));
  toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  auto *commandPaletteToolbarAction = toolbar->addAction(
      style()->standardIcon(QStyle::SP_FileDialogContentsView),
      uiText("Commands", "命令"));
  commandPaletteToolbarAction->setObjectName("commandPaletteToolbarAction");
  commandPaletteToolbarAction->setToolTip(
      uiText("Open command palette (Ctrl+P)", "打开命令面板 (Ctrl+P)"));
  toolbar->addSeparator();
  struct ToolbarActionSpec {
    QString text;
    const char *objectName;
    QStyle::StandardPixmap icon;
    QString tip;
    QKeySequence shortcut;
  };
  const QList<ToolbarActionSpec> actions = {
      {uiText("Connect", "连接"), "connectAction", QStyle::SP_DriveNetIcon,
       uiText("Connect to the EtherCAT runtime", "连接 EtherCAT 运行时"),
       QKeySequence("Ctrl+K")},
      {uiText("Refresh", "刷新"), "refreshAction", QStyle::SP_BrowserReload,
       uiText("Refresh master and slave data", "刷新主站和从站数据"),
       QKeySequence::Refresh},
      {uiText("Rescan", "重新扫描"), "rescanAction",
       QStyle::SP_FileDialogDetailedView,
       uiText("Rescan the active master bus", "重新扫描当前主站总线"),
       QKeySequence("Ctrl+Shift+R")},
      {uiText("Free Run", "自由运行"), "freeRunAction", QStyle::SP_MediaPlay,
       uiText("Toggle cyclic Free Run telemetry", "切换周期性自由运行遥测"),
       QKeySequence("Ctrl+F")},
      {"INIT", "initAction", QStyle::SP_DialogResetButton,
       uiText("Set selected slave to INIT", "将选中从站切换到 INIT"),
       QKeySequence("Alt+1")},
      {"PREOP", "preOpAction", QStyle::SP_CommandLink,
       uiText("Set selected slave to PREOP", "将选中从站切换到 PREOP"),
       QKeySequence("Alt+2")},
      {"SAFEOP", "safeOpAction", QStyle::SP_DialogApplyButton,
       uiText("Set selected slave to SAFEOP", "将选中从站切换到 SAFEOP"),
       QKeySequence("Alt+3")},
      {"OP", "opAction", QStyle::SP_DialogYesButton,
       uiText("Set selected slave to OP", "将选中从站切换到 OP"),
       QKeySequence("Alt+4")},
  };
  for (const auto &entry : actions) {
    auto *action =
        toolbar->addAction(style()->standardIcon(entry.icon), entry.text);
    action->setObjectName(entry.objectName);
    const QString objectName = QString::fromLatin1(entry.objectName);
    const bool shortcutAlreadyOnMenu = objectName == "connectAction" ||
                                       objectName == "refreshAction" ||
                                       objectName == "rescanAction";
    if (!entry.shortcut.isEmpty() && !shortcutAlreadyOnMenu) {
      action->setShortcut(entry.shortcut);
    }
    const QString shortcut =
        action->shortcut().toString(QKeySequence::NativeText);
    const QString visibleShortcut =
        shortcut.isEmpty() ? entry.shortcut.toString(QKeySequence::NativeText)
                           : shortcut;
    action->setToolTip(
        visibleShortcut.isEmpty()
            ? entry.tip
            : QString("%1 (%2)").arg(entry.tip, visibleShortcut));
    action->setStatusTip(entry.tip);
    if (objectName == "freeRunAction") {
      action->setCheckable(true);
    }
    if (objectName == "rescanAction" || objectName == "initAction") {
      toolbar->addSeparator();
    }
  }

  toolbar->addWidget(makeToolbarLabel(uiText("Master", "主站")));
  masterCombo_ = new QComboBox;
  masterCombo_->setObjectName("masterSelector");
  masterCombo_->setMinimumWidth(180);
  toolbar->addWidget(masterCombo_);
  auto *manageMastersAction =
      toolbar->addAction(style()->standardIcon(QStyle::SP_ComputerIcon),
                         uiText("Manage Masters", "管理主站"));
  manageMastersAction->setObjectName("manageMastersAction");
  manageMastersAction->setToolTip(uiText(
      "Configure master profiles and preferences", "配置主站档案和偏好设置"));

  connectionLabel_ = new QLabel(uiText("Starting runtime", "正在启动运行时"));
  connectionLabel_->setObjectName("connectionPill");
  connectionLabel_->setProperty("state", "pending");
  toolbar->addSeparator();
  toolbar->addWidget(connectionLabel_);

  auto *root = new QSplitter;
  root->setChildrenCollapsible(false);
  setCentralWidget(root);

  auto *left = new QWidget;
  auto *leftLayout = new QVBoxLayout(left);
  leftLayout->setContentsMargins(14, 14, 8, 14);
  leftLayout->setSpacing(10);
  auto *topologyHeader = new QHBoxLayout;
  topologyHeader->setSpacing(6);
  auto *topologyTitle = new QLabel(uiText("I/O Tree", "I/O 树"));
  topologyTitle->setObjectName("paneTitle");
  auto *captureTopology = new QPushButton(uiText("Baseline", "基线"));
  captureTopology->setObjectName("captureTopologyBaseline");
  captureTopology->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
  captureTopology->setToolTip(
      uiText("Capture current online topology as expected baseline",
             "将当前在线拓扑捕获为期望基线"));
  auto *clearTopology = new QPushButton(uiText("Clear", "清除"));
  clearTopology->setObjectName("clearTopologyBaseline");
  clearTopology->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
  clearTopology->setToolTip(
      uiText("Clear expected topology baseline", "清除期望拓扑基线"));
  topologyHeader->addWidget(topologyTitle);
  topologyHeader->addStretch(1);
  topologyHeader->addWidget(captureTopology);
  topologyHeader->addWidget(clearTopology);
  topologyTree_ = new QTreeWidget;
  topologyTree_->setHeaderLabels(
      {uiText("Device", "设备"), uiText("State", "状态")});
  topologyTree_->header()->setSectionResizeMode(0, QHeaderView::Stretch);
  topologyTree_->header()->setSectionResizeMode(1,
                                                QHeaderView::ResizeToContents);
  topologyTree_->setAnimated(true);
  topologyTree_->setIndentation(18);
  topologyTree_->setUniformRowHeights(true);
  topologyTree_->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  topologyTree_->setContextMenuPolicy(Qt::CustomContextMenu);
  diagnostics_->topologyBaselineLabel =
      new QLabel(uiText("No topology baseline", "未设置拓扑基线"));
  diagnostics_->topologyBaselineLabel->setObjectName("diagnosticsSummary");
  diagnostics_->topologyBaselineLabel->setWordWrap(true);
  leftLayout->addLayout(topologyHeader);
  leftLayout->addWidget(diagnostics_->topologyBaselineLabel);
  leftLayout->addWidget(topologyTree_, 1);

  auto *slavePanel = new QFrame;
  slavePanel->setObjectName("metricCard");
  auto *slavePanelLayout = new QVBoxLayout(slavePanel);
  slavePanelLayout->setContentsMargins(12, 12, 12, 12);
  slavePanelLayout->setSpacing(8);
  auto *slavePanelTitle = new QLabel(uiText("Selected Slave", "选中从站"));
  slavePanelTitle->setObjectName("sectionTitle");
  selectedSlaveNameLabel_ =
      new QLabel(uiText("No slave selected", "尚未选择从站"));
  selectedSlaveNameLabel_->setObjectName("selectedSlaveName");
  selectedSlaveNameLabel_->setWordWrap(true);
  selectedSlaveNameLabel_->setTextInteractionFlags(Qt::TextSelectableByMouse);
  selectedSlaveStateLabel_ = new QLabel(uiText("State: none", "状态：无"));
  selectedSlaveStateLabel_->setObjectName("statusSummary");
  selectedSlaveStateLabel_->setTextInteractionFlags(Qt::TextSelectableByMouse);
  selectedSlaveFlagsLabel_ = new QLabel(uiText("Flags: none", "标志：无"));
  selectedSlaveFlagsLabel_->setObjectName("statusSummary");
  selectedSlaveFlagsLabel_->setWordWrap(true);
  selectedSlaveFlagsLabel_->setTextInteractionFlags(Qt::TextSelectableByMouse);
  selectedSlaveEvidenceLabel_ =
      new QLabel(uiText("Evidence: select a slave", "证据：请选择从站"));
  selectedSlaveEvidenceLabel_->setObjectName("statusSummary");
  selectedSlaveEvidenceLabel_->setProperty("severity", "neutral");
  selectedSlaveEvidenceLabel_->setWordWrap(true);
  selectedSlaveEvidenceLabel_->setTextInteractionFlags(
      Qt::TextSelectableByMouse);
  selectedDriveSummaryLabel_ =
      new QLabel(uiText("Drive: no Watch evidence", "驱动：暂无监视证据"));
  selectedDriveSummaryLabel_->setObjectName("statusSummary");
  selectedDriveSummaryLabel_->setWordWrap(true);
  selectedDriveSummaryLabel_->setTextInteractionFlags(
      Qt::TextSelectableByMouse);
  selectedSlaveHintLabel_ = new QLabel(
      uiText("Select a slave in the I/O tree to enable contextual actions.",
             "在 I/O 树中选择从站后启用上下文操作。"));
  selectedSlaveHintLabel_->setObjectName("diagnosticsSummary");
  selectedSlaveHintLabel_->setWordWrap(true);
  slavePanelLayout->addWidget(slavePanelTitle);
  slavePanelLayout->addWidget(selectedSlaveNameLabel_);
  slavePanelLayout->addWidget(selectedSlaveStateLabel_);
  slavePanelLayout->addWidget(selectedSlaveFlagsLabel_);
  slavePanelLayout->addWidget(selectedSlaveEvidenceLabel_);
  slavePanelLayout->addWidget(selectedDriveSummaryLabel_);
  slavePanelLayout->addWidget(selectedSlaveHintLabel_);

  auto *slaveActions = new QGridLayout;
  slaveActions->setHorizontalSpacing(6);
  slaveActions->setVerticalSpacing(6);
  auto makeContextButton = [this](const QString &text, const char *objectName,
                                  QStyle::StandardPixmap icon) {
    auto *button = new QPushButton(text);
    button->setObjectName(objectName);
    button->setIcon(style()->standardIcon(icon));
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    return button;
  };
  slaveActions->addWidget(makeContextButton(uiText("OD", "对象"),
                                            "contextObjectDictionary",
                                            QStyle::SP_FileDialogDetailedView),
                          0, 0);
  slaveActions->addWidget(makeContextButton(uiText("PDO", "PDO"),
                                            "contextPdoMap",
                                            QStyle::SP_FileDialogListView),
                          0, 1);
  slaveActions->addWidget(makeContextButton(uiText("Watch", "监视"),
                                            "contextWatch",
                                            QStyle::SP_FileDialogContentsView),
                          0, 2);
  slaveActions->addWidget(makeContextButton(uiText("Free", "自由"),
                                            "contextFreeRun",
                                            QStyle::SP_MediaPlay),
                          1, 0);
  slaveActions->addWidget(makeContextButton(uiText("Diag", "诊断"),
                                            "contextDiagnostics",
                                            QStyle::SP_ComputerIcon),
                          1, 1);
  slaveActions->addWidget(makeContextButton(uiText("Refresh", "刷新"),
                                            "contextRefreshSlave",
                                            QStyle::SP_BrowserReload),
                          1, 2);
  slaveActions->addWidget(
      makeContextButton("INIT", "contextInit", QStyle::SP_DialogResetButton), 2,
      0);
  slaveActions->addWidget(
      makeContextButton("PREOP", "contextPreOp", QStyle::SP_CommandLink), 2, 1);
  slaveActions->addWidget(makeContextButton("SAFEOP", "contextSafeOp",
                                            QStyle::SP_DialogApplyButton),
                          2, 2);
  slaveActions->addWidget(
      makeContextButton("OP", "contextOp", QStyle::SP_DialogYesButton), 3, 0, 1,
      3);
  auto *driveNext =
      makeContextButton(uiText("Drive Next", "驱动下一步"), "contextDriveNext",
                        QStyle::SP_DialogApplyButton);
  driveNext->setToolTip(
      uiText("Use Watch statusword evidence to prepare the recommended CiA 402 "
             "controlword, then confirm the normal SDO write.",
             "根据 Watch 状态字证据准备推荐的 CiA 402 控制字，然后走普通 SDO "
             "写入确认。"));
  slaveActions->addWidget(driveNext, 4, 0, 1, 3);
  auto *prepareSnapshot = makeContextButton(uiText("Snapshot", "准备快照"),
                                            "contextPrepareSnapshot",
                                            QStyle::SP_FileDialogDetailedView);
  prepareSnapshot->setToolTip(uiText(
      "Prepare read-only evidence for the selected slave: refresh identity, "
      "Object Dictionary, PDO Map, ESI XML, and CiA 402 Watch evidence.",
      "为选中从站准备只读证据：刷新身份、对象字典、PDO 映射、ESI XML 和 "
      "CiA 402 Watch 证据。"));
  slaveActions->addWidget(prepareSnapshot, 5, 0, 1, 3);
  slavePanelLayout->addLayout(slaveActions);
  leftLayout->addWidget(slavePanel);
  root->addWidget(left);

  auto *right = new QWidget;
  auto *rightLayout = new QVBoxLayout(right);
  rightLayout->setContentsMargins(8, 14, 14, 14);
  rightLayout->setSpacing(10);

  selectedLabel_ = new QLabel(activeMasterName());
  selectedLabel_->setObjectName("heroTitle");
  rightLayout->addWidget(selectedLabel_);

  auto *cards = new QGridLayout;
  cards->setContentsMargins(0, 0, 0, 2);
  cards->setHorizontalSpacing(10);
  cards->setVerticalSpacing(10);
  cards->addWidget(makeMetricCard(uiText("Master", "主站"),
                                  uiText("Idle", "空闲"), &masterStateLabel_),
                   0, 0);
  cards->addWidget(makeMetricCard(uiText("Slaves", "从站"),
                                  QString::number(slaves_.size()),
                                  &slaveCountLabel_),
                   0, 1);
  cards->addWidget(makeMetricCard(uiText("Link", "链路"),
                                  uiText("Unknown", "未知"), &linkStateLabel_),
                   0, 2);
  cards->addWidget(
      makeMetricCard(uiText("Frame Loss", "丢帧"), "0", &lossLabel_), 0, 3);
  cards->addWidget(
      makeMetricCard(uiText("Free Run", "自由运行"),
                     freeRun_ ? uiText("On", "开启") : uiText("Off", "关闭"),
                     &freeRunLabel_),
      0, 4);
  for (int column = 0; column < 5; ++column) {
    cards->setColumnStretch(column, 1);
  }
  rightLayout->addLayout(cards);

  tabs_ = new QTabWidget;
  configureWorkspaceTabsForRelease(tabs_);
  metricTable_ = new QTableWidget;
  session_ = new SessionWorkspaceWidgets;
  session_->sessionBriefTable = new QTableWidget;
  workflowTable_ = new QTableWidget;
  slaveEvidenceMatrixTable_ = new QTableWidget;
  stateMachine_ = new StateMachineWorkspaceWidgets;
  stateMachine_->stateMachineTable = new QTableWidget;
  identityTable_ = new QTableWidget;
  portTable_ = new QTableWidget;
  mailboxTable_ = new QTableWidget;
  pdoTable_ = new QTableWidget;
  sdoTable_ = new QTableWidget;
  sdoTargetTrailTable_ = new QTableWidget;
  sdoHistoryTable_ = new QTableWidget;
  freeRunTable_ = new QTableWidget;
  freeRunEntryTable_ = new QTableWidget;
  ioVar_ = new IoVariableWorkspaceWidgets;
  ioVar_->ioVariableTable = new QTableWidget;
  consistency_ = new ConsistencyWorkspaceWidgets;
  consistency_->consistencyTable = new QTableWidget;
  hostHealthTable_ = new QTableWidget;
  diagnostics_ = new DiagnosticsWorkspaceWidgets;
  diagnostics_->diagnosticsTable = new QTableWidget;
  watchTable_ = new QTableWidget;
  esiTable_ = new QTableWidget;
  startupSdoTable_ = new QTableWidget;
  for (auto *table : {metricTable_,
                      session_->sessionBriefTable,
                      workflowTable_,
                      slaveEvidenceMatrixTable_,
                      stateMachine_->stateMachineTable,
                      identityTable_,
                      portTable_,
                      mailboxTable_,
                      pdoTable_,
                      sdoTable_,
                      sdoTargetTrailTable_,
                      sdoHistoryTable_,
                      freeRunTable_,
                      freeRunEntryTable_,
                      ioVar_->ioVariableTable,
                      consistency_->consistencyTable,
                      hostHealthTable_,
                      diagnostics_->diagnosticsTable,
                      watchTable_,
                      esiTable_,
                      startupSdoTable_}) {
    table->setAlternatingRowColors(true);
    table->verticalHeader()->setVisible(false);
    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft |
                                                   Qt::AlignVCenter);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::ExtendedSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setShowGrid(false);
    table->setWordWrap(false);
    table->setCornerButtonEnabled(false);
    table->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    table->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    table->verticalHeader()->setDefaultSectionSize(30);
    table->setContextMenuPolicy(Qt::CustomContextMenu);
  }

  auto *overview = new QWidget;
  overviewPage_ = overview;
  auto *overviewLayout = new QVBoxLayout(overview);
  overviewLayout->setContentsMargins(14, 14, 14, 14);
  overviewLayout->setSpacing(10);
  auto *overviewTabs = new QTabWidget;
  overviewTabs->setObjectName("overviewModeTabs");
  configureWorkspaceTabsForRelease(overviewTabs);
  overviewTabs->setMovable(false);
  auto *briefPage = new QWidget;
  auto *briefLayout = new QVBoxLayout(briefPage);
  briefLayout->setContentsMargins(10, 10, 10, 10);
  briefLayout->setSpacing(10);
  auto *workflowPage = new QWidget;
  auto *workflowLayout = new QVBoxLayout(workflowPage);
  workflowLayout->setContentsMargins(10, 10, 10, 10);
  workflowLayout->setSpacing(10);
  auto *matrixPage = new QWidget;
  auto *matrixLayout = new QVBoxLayout(matrixPage);
  matrixLayout->setContentsMargins(10, 10, 10, 10);
  matrixLayout->setSpacing(10);
  auto *detailsPage = new QWidget;
  auto *detailsLayout = new QVBoxLayout(detailsPage);
  detailsLayout->setContentsMargins(10, 10, 10, 10);
  detailsLayout->setSpacing(10);
  auto *sessionHeader = new QHBoxLayout;
  sessionHeader->setSpacing(8);
  sessionHeader->addWidget(
      makeSectionTitle(uiText("Session Brief", "会话简报")));
  auto *sessionHint = new QLabel(uiText(
      "Read-only decision context for the selected slave and current SDO.",
      "选中从站和当前 SDO 的只读决策上下文。"));
  sessionHint->setObjectName("diagnosticsSummary");
  sessionHint->setTextInteractionFlags(Qt::TextSelectableByMouse);
  session_->sessionBriefCopyButton = new QPushButton(uiText("Copy Row", "复制本行"));
  session_->sessionBriefCopyButton->setIcon(
      style()->standardIcon(QStyle::SP_FileDialogContentsView));
  session_->sessionBriefCopyButton->setEnabled(false);
  session_->sessionBriefCopyButton->setToolTip(uiText(
      "Copy the selected Session Brief row summary to the clipboard. No bus "
      "request is sent.",
      "把当前会话简报行摘要复制到剪贴板；不会发送总线请求。"));
  sessionHeader->addWidget(sessionHint, 1);
  sessionHeader->addWidget(session_->sessionBriefCopyButton);
  briefLayout->addLayout(sessionHeader);
  setTableRows(session_->sessionBriefTable,
               {uiText("Area", "区域"), uiText("Status", "状态"),
                uiText("Evidence", "依据"), uiText("Next", "下一步")},
               {});
  session_->sessionBriefTable->setMinimumHeight(320);
  session_->sessionBriefTable->setToolTip(uiText(
      "A read-only brief assembled from loaded UI evidence. It does not read "
      "the bus, write SDOs, change state, toggle Free Run, or run Host Health. "
      "Double-click a row to open the matching local evidence surface.",
      "基于已加载界面证据生成的只读简报；不会读取总线、写入 SDO、切换状态、"
      "启动 Free Run 或运行 Host Health。双击、按 Enter 或右键行可打开匹配的"
      "本地证据界面。"));
  briefLayout->addWidget(session_->sessionBriefTable, 1);
  auto *workflowHeader = new QHBoxLayout;
  workflowHeader->setSpacing(8);
  workflowSummaryLabel_ =
      new QLabel(uiText("Follow the commissioning workflow from left to right.",
                        "按调试流程从上到下推进。"));
  workflowSummaryLabel_->setObjectName("statusSummary");
  workflowSummaryLabel_->setProperty("severity", "neutral");
  workflowSummaryLabel_->setTextInteractionFlags(Qt::TextSelectableByMouse);
  auto *overviewConnect = new QPushButton(uiText("Connect", "连接"));
  overviewConnect->setObjectName("overviewConnect");
  overviewConnect->setIcon(style()->standardIcon(QStyle::SP_DriveNetIcon));
  auto *overviewRefresh = new QPushButton(uiText("Refresh", "刷新"));
  overviewRefresh->setObjectName("overviewRefresh");
  overviewRefresh->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
  auto *overviewRunNext = new QPushButton(uiText("Run Next", "执行下一步"));
  overviewRunNext->setObjectName("overviewRunNext");
  overviewRunNext->setIcon(style()->standardIcon(QStyle::SP_CommandLink));
  overviewRunNext->setToolTip(
      uiText("Run the first actionable commissioning workflow step.",
             "执行调试工作流中第一条可执行步骤。"));
  workflowScopeFilter_ = new QComboBox;
  workflowScopeFilter_->setObjectName("workflowScopeFilter");
  workflowScopeFilter_->addItem(uiText("All", "全部"), QStringLiteral("all"));
  workflowScopeFilter_->addItem(uiText("Open", "未完成"),
                                QStringLiteral("open"));
  workflowScopeFilter_->addItem(uiText("Blocked", "受阻"),
                                QStringLiteral("blocked"));
  workflowScopeFilter_->addItem(uiText("Action", "待执行"),
                                QStringLiteral("action"));
  workflowScopeFilter_->addItem(uiText("Ready", "就绪"),
                                QStringLiteral("ready"));
  workflowScopeFilter_->addItem(uiText("Risk", "风险"), QStringLiteral("risk"));
  workflowScopeFilter_->addItem(uiText("Evidence Gap", "证据缺口"),
                                QStringLiteral("gap"));
  workflowScopeFilter_->setToolTip(uiText(
      "Filter workflow steps by local status or evidence state. Filtering "
      "does not read the bus.",
      "按本地状态或证据状态过滤工作流步骤；过滤不会读取总线。"));
  workflowFilter_ = new QLineEdit;
  workflowFilter_->setPlaceholderText(
      uiText("Search phase, step, risk, evidence, or next action",
             "搜索阶段、步骤、风险、依据或下一步"));
  workflowFilter_->setToolTip(
      uiText("Search the current workflow table locally without bus access.",
             "只在当前工作流表格内本地搜索，不访问总线。"));
  workflowReviewButton_ = new QPushButton(uiText("Review First", "审阅首个"));
  workflowReviewButton_->setIcon(
      style()->standardIcon(QStyle::SP_FileDialogDetailedView));
  workflowReviewButton_->setEnabled(false);
  workflowReviewButton_->setToolTip(uiText(
      "Select the first visible workflow issue without running the step.",
      "选择当前可见的首个工作流问题；不会执行该步骤。"));
  workflowReviewNextButton_ =
      new QPushButton(uiText("Review Next", "审阅下个"));
  workflowReviewNextButton_->setIcon(
      style()->standardIcon(QStyle::SP_ArrowForward));
  workflowReviewNextButton_->setEnabled(false);
  workflowReviewNextButton_->setToolTip(uiText(
      "Select the next visible workflow issue with wraparound. No bus request "
      "is sent.",
      "选择下一个可见工作流问题并在末尾回绕；不会发送总线请求。"));
  workflowStepCopyButton_ = new QPushButton(uiText("Copy Step", "复制步骤"));
  workflowStepCopyButton_->setIcon(
      style()->standardIcon(QStyle::SP_FileDialogContentsView));
  workflowStepCopyButton_->setEnabled(false);
  workflowStepCopyButton_->setToolTip(uiText(
      "Copy the selected workflow step evidence summary to the clipboard. No "
      "bus request is sent.",
      "把当前工作流步骤证据摘要复制到剪贴板；不会发送总线请求。"));
  workflowHeader->addWidget(
      makeSectionTitle(uiText("Commissioning Workflow", "调试工作流")));
  workflowHeader->addWidget(workflowSummaryLabel_, 1);
  workflowHeader->addWidget(new QLabel(uiText("Scope", "范围")));
  workflowHeader->addWidget(workflowScopeFilter_);
  workflowHeader->addWidget(workflowFilter_);
  workflowHeader->addWidget(workflowReviewButton_);
  workflowHeader->addWidget(workflowReviewNextButton_);
  workflowHeader->addWidget(workflowStepCopyButton_);
  workflowHeader->addWidget(overviewRunNext);
  workflowHeader->addWidget(overviewConnect);
  workflowHeader->addWidget(overviewRefresh);
  workflowLayout->addLayout(workflowHeader);
  workflowStepDetailLabel_ = makeStatusSummaryLabel(
      uiText("Select a workflow row to review its evidence boundary.",
             "选择工作流行以复核它的证据边界。"),
      uiText("This detail strip is local only. It summarizes the selected "
             "workflow row and explains what Run Next or double-click may do.",
             "此详情条仅使用本地界面数据。它汇总当前工作流行，并说明执行下一步"
             "或双击可能触发的动作。"));
  workflowLayout->addWidget(workflowStepDetailLabel_);
  setTableRows(workflowTable_,
               commissioningWorkflowHeaders(commissioningWorkflowTexts()), {});
  workflowTable_->setMinimumHeight(420);
  workflowTable_->setToolTip(uiText(
      "Select a workflow row to review its local evidence boundary. "
      "Double-click a row only when you intend to run or open the suggested "
      "action.",
      "选择工作流行可先复核本地证据边界；只有明确要执行或打开建议动作时，"
      "才双击该行。"));
  workflowLayout->addWidget(workflowTable_, 1);

  auto *matrixHeader = new QHBoxLayout;
  matrixHeader->setSpacing(8);
  matrixHeader->addWidget(
      makeSectionTitle(uiText("Slave Evidence Matrix", "从站证据矩阵")));
  slaveEvidenceMatrixSummaryLabel_ =
      new QLabel(uiText("No slave evidence matrix yet", "暂无从站证据矩阵"));
  slaveEvidenceMatrixSummaryLabel_->setObjectName("statusSummary");
  slaveEvidenceMatrixSummaryLabel_->setProperty("severity", "neutral");
  slaveEvidenceMatrixSummaryLabel_->setTextInteractionFlags(
      Qt::TextSelectableByMouse);
  slaveEvidenceMatrixScopeFilter_ = new QComboBox;
  slaveEvidenceMatrixScopeFilter_->addItem(
      uiText("All", "全部"), QString::fromLatin1(kSlaveEvidenceScopeAll));
  slaveEvidenceMatrixScopeFilter_->addItem(
      uiText("P0 Fault", "P0 故障"),
      QString::fromLatin1(kSlaveEvidenceScopePriorityP0));
  slaveEvidenceMatrixScopeFilter_->addItem(
      uiText("P1 Risk", "P1 风险"),
      QString::fromLatin1(kSlaveEvidenceScopePriorityP1));
  slaveEvidenceMatrixScopeFilter_->addItem(
      uiText("P2 Action", "P2 待执行"),
      QString::fromLatin1(kSlaveEvidenceScopePriorityP2));
  slaveEvidenceMatrixScopeFilter_->addItem(
      uiText("P3 Ready", "P3 就绪"),
      QString::fromLatin1(kSlaveEvidenceScopePriorityP3));
  slaveEvidenceMatrixScopeFilter_->addItem(
      uiText("Risk", "风险"), QString::fromLatin1(kSlaveEvidenceScopeRisk));
  slaveEvidenceMatrixScopeFilter_->addItem(
      uiText("Action", "待执行"),
      QString::fromLatin1(kSlaveEvidenceScopeAction));
  slaveEvidenceMatrixScopeFilter_->addItem(
      uiText("Ready", "就绪"), QString::fromLatin1(kSlaveEvidenceScopeReady));
  slaveEvidenceMatrixScopeFilter_->addItem(
      uiText("Missing OD", "缺 OD"),
      QString::fromLatin1(kSlaveEvidenceScopeMissingOd));
  slaveEvidenceMatrixScopeFilter_->addItem(
      uiText("Missing PDO", "缺 PDO"),
      QString::fromLatin1(kSlaveEvidenceScopeMissingPdo));
  slaveEvidenceMatrixScopeFilter_->addItem(
      uiText("Missing Watch", "缺 Watch"),
      QString::fromLatin1(kSlaveEvidenceScopeMissingWatch));
  slaveEvidenceMatrixScopeFilter_->addItem(
      uiText("Startup Diff", "启动偏差"),
      QString::fromLatin1(kSlaveEvidenceScopeStartupDiff));
  slaveEvidenceMatrixScopeFilter_->addItem(
      uiText("Process Missing", "缺过程证据"),
      QString::fromLatin1(kSlaveEvidenceScopeProcessMissing));
  slaveEvidenceMatrixScopeFilter_->setToolTip(uiText(
      "Filter the matrix by local evidence state. Filtering does not read the "
      "bus.",
      "按本地证据状态过滤矩阵；过滤不会读取总线。"));
  slaveEvidenceMatrixFilter_ = new QLineEdit;
  slaveEvidenceMatrixFilter_->setPlaceholderText(
      uiText("Search slave, state, risk, or next action",
             "搜索从站、状态、风险或下一步"));
  slaveEvidenceMatrixFilter_->setToolTip(
      uiText("Search the loaded matrix rows only; no EtherCAT request is sent.",
             "仅搜索已加载矩阵行；不会发送 EtherCAT 请求。"));
  slaveEvidenceMatrixReviewButton_ =
      new QPushButton(uiText("Review First", "审阅首个问题"));
  slaveEvidenceMatrixReviewButton_->setIcon(
      style()->standardIcon(QStyle::SP_FileDialogDetailedView));
  slaveEvidenceMatrixReviewButton_->setToolTip(uiText(
      "Open the first visible Risk or Action matrix row in local evidence. No "
      "bus request is sent.",
      "打开首个可见风险或待执行矩阵行的本地证据；不会发送总线请求。"));
  slaveEvidenceMatrixReviewNextButton_ =
      new QPushButton(uiText("Review Next", "审阅下个问题"));
  slaveEvidenceMatrixReviewNextButton_->setIcon(
      style()->standardIcon(QStyle::SP_ArrowForward));
  slaveEvidenceMatrixReviewNextButton_->setToolTip(uiText(
      "Open the next visible Risk or Action matrix row after the current row; "
      "wraps to the first issue. No bus request is sent.",
      "打开当前行之后的下一个可见风险或待执行矩阵行；到末尾后回到首个问题。"
      "不会发送总线请求。"));
  slaveEvidenceMatrixCopyButton_ =
      new QPushButton(uiText("Copy Row", "复制本行"));
  slaveEvidenceMatrixCopyButton_->setIcon(
      style()->standardIcon(QStyle::SP_FileDialogContentsView));
  slaveEvidenceMatrixCopyButton_->setEnabled(false);
  slaveEvidenceMatrixCopyButton_->setToolTip(uiText(
      "Copy the selected matrix row evidence summary to the clipboard. No bus "
      "request is sent.",
      "把当前矩阵行证据摘要复制到剪贴板；不会发送总线请求。"));
  slaveEvidenceMatrixTriageButtons_.clear();
  struct TriageScopeButton {
    QString label;
    QString scope;
  };
  const QVector<TriageScopeButton> triageScopes = {
      {uiText("P0", "P0"), QString::fromLatin1(kSlaveEvidenceScopePriorityP0)},
      {uiText("P1", "P1"), QString::fromLatin1(kSlaveEvidenceScopePriorityP1)},
      {uiText("P2", "P2"), QString::fromLatin1(kSlaveEvidenceScopePriorityP2)},
      {uiText("P3", "P3"), QString::fromLatin1(kSlaveEvidenceScopePriorityP3)},
  };
  for (const auto &triage : triageScopes) {
    auto *button = new QPushButton(triage.label);
    button->setProperty("scope", triage.scope);
    button->setToolTip(uiText(
        "Filter the slave evidence matrix by this priority. This is local UI "
        "filtering only.",
        "按该优先级过滤从站证据矩阵；这只是本地界面过滤。"));
    slaveEvidenceMatrixTriageButtons_.append(button);
  }
  matrixHeader->addWidget(slaveEvidenceMatrixSummaryLabel_, 1);
  for (auto *button : slaveEvidenceMatrixTriageButtons_) {
    matrixHeader->addWidget(button);
  }
  matrixHeader->addWidget(slaveEvidenceMatrixReviewButton_);
  matrixHeader->addWidget(slaveEvidenceMatrixReviewNextButton_);
  matrixHeader->addWidget(slaveEvidenceMatrixCopyButton_);
  matrixHeader->addWidget(slaveEvidenceMatrixScopeFilter_);
  matrixHeader->addWidget(slaveEvidenceMatrixFilter_);
  matrixLayout->addLayout(matrixHeader);
  setTableRows(slaveEvidenceMatrixTable_,
               slaveEvidenceMatrixHeaders(slaveEvidenceUiTexts()), {});
  slaveEvidenceMatrixTable_->setMinimumHeight(420);
  slaveEvidenceMatrixTable_->setToolTip(uiText(
      "Read-only multi-slave evidence matrix built from loaded UI evidence. "
      "Double-click a row to open the most relevant local evidence table.",
      "基于已加载界面证据生成的只读多从站证据矩阵。双击行会打开最相关的本地证据"
      "表。"));
  matrixLayout->addWidget(slaveEvidenceMatrixTable_, 1);

  auto *overviewGrid = new QGridLayout;
  overviewGrid->setHorizontalSpacing(12);
  overviewGrid->setVerticalSpacing(10);
  overviewGrid->addWidget(
      makeSectionTitle(uiText("Master Metrics", "主站指标")), 0, 0);
  overviewGrid->addWidget(makeSectionTitle(uiText("Identity", "身份信息")), 0,
                          1);
  overviewGrid->addWidget(metricTable_, 1, 0);
  overviewGrid->addWidget(identityTable_, 1, 1);
  overviewGrid->addWidget(makeSectionTitle(uiText("Ports", "端口")), 2, 0);
  overviewGrid->addWidget(makeSectionTitle(uiText("Mailboxes", "邮箱")), 2, 1);
  overviewGrid->addWidget(portTable_, 3, 0);
  overviewGrid->addWidget(mailboxTable_, 3, 1);
  overviewGrid->setRowStretch(1, 1);
  overviewGrid->setRowStretch(3, 1);
  detailsLayout->addLayout(overviewGrid, 1);
  overviewTabs->addTab(briefPage, uiText("Brief", "简报"));
  overviewTabs->addTab(workflowPage, uiText("Workflow", "工作流"));
  overviewTabs->addTab(matrixPage, uiText("Matrix", "矩阵"));
  overviewTabs->addTab(detailsPage, uiText("Details", "细节"));
  overviewLayout->addWidget(overviewTabs, 1);

  auto *sdoPage = new QWidget;
  objectDictionaryPage_ = sdoPage;
  auto *sdoLayout = new QVBoxLayout(sdoPage);
  sdoLayout->setContentsMargins(14, 14, 14, 14);
  sdoLayout->setSpacing(10);
  auto *sdoControls = new QGridLayout;
  sdoControls->setHorizontalSpacing(8);
  sdoControls->setVerticalSpacing(8);
  sdoControls->setColumnStretch(6, 1);
  sdoFilter_ = new QLineEdit;
  sdoFilter_->setPlaceholderText(
      uiText("Filter object dictionary", "过滤对象字典"));
  sdoFilter_->setToolTip(
      uiText("Select an object row to fill the SDO command fields; "
             "double-click a row to read it.",
             "选中对象行会自动填充 SDO 指令字段；双击行会直接读取。"));
  sdoIndex_ = new QLineEdit("0x1000");
  sdoIndex_->setMaximumWidth(110);
  sdoSubIndex_ = new QLineEdit("0x00");
  sdoSubIndex_->setMaximumWidth(86);
  sdoValue_ = new QLineEdit;
  sdoValue_->setReadOnly(true);
  sdoValue_->setPlaceholderText(uiText("Last read value", "最后读回值"));
  sdoWriteValue_ = new QLineEdit;
  sdoWriteValue_->setPlaceholderText(uiText("Value to write", "写入值"));
  sdoType_ = new QComboBox;
  sdoType_->addItems({"", "bool", "int8", "int16", "int32", "int64", "uint8",
                      "uint16", "uint32", "uint64", "float", "double", "string",
                      "octet_string"});
  auto *readSdo = new QPushButton(uiText("Read", "读取"));
  readSdo->setObjectName("readSdo");
  auto *readSelectedDictionary =
      new QPushButton(uiText("Read Selected", "读取所选"));
  readSelectedDictionary->setObjectName("readSelectedDictionary");
  readSelectedDictionary->setIcon(
      style()->standardIcon(QStyle::SP_BrowserReload));
  useSdoValueButton_ = new QPushButton(uiText("Use Read Value", "使用读回值"));
  useSdoValueButton_->setObjectName("useSdoValue");
  useSdoValueButton_->setIcon(style()->standardIcon(QStyle::SP_ArrowForward));
  useSdoValueButton_->setToolTip(
      uiText("Copy the current read value into the write field for tuning or "
             "Startup SDO creation.",
             "把当前读回值复制到写入框，便于微调或创建 Startup SDO。"));
  auto *useSdoEvidenceButton =
      new QPushButton(uiText("Use Evidence", "使用证据"));
  useSdoEvidenceButton->setObjectName("useSdoEvidence");
  useSdoEvidenceButton->setIcon(style()->standardIcon(QStyle::SP_ArrowForward));
  useSdoEvidenceButton->setToolTip(
      uiText("Copy the best local Read/Watch/OD/Startup/Bookmark/Target Trail "
             "evidence into the write field without bus access.",
             "把最佳本地读回、Watch、OD、Startup "
             "或书签/目标轨迹证据复制到写入框，不访问总线。"));
  auto *pickSdoEvidenceButton =
      new QPushButton(uiText("Pick Evidence", "选择证据"));
  pickSdoEvidenceButton->setObjectName("pickSdoEvidence");
  pickSdoEvidenceButton->setIcon(
      style()->standardIcon(QStyle::SP_FileDialogContentsView));
  pickSdoEvidenceButton->setToolTip(uiText(
      "Choose one local evidence value for the write field without bus access.",
      "从本地证据中选择一个值填入写入框，不访问总线。"));
  auto *writeSdo = new QPushButton(uiText("Write", "写入"));
  writeSdo->setObjectName("writeSdo");
  auto *watchSelectedDictionary =
      new QPushButton(uiText("Watch Selected", "监视所选"));
  watchSelectedDictionary->setObjectName("watchSelectedDictionary");
  watchSelectedDictionary->setIcon(
      style()->standardIcon(QStyle::SP_FileDialogNewFolder));
  watchSelectedDictionary->setToolTip(uiText(
      "Add selected Object Dictionary rows to Watch without duplicating items.",
      "把选中的对象字典行加入 Watch，并自动复用已有监视项。"));
  auto *startupSelectedEvidence =
      new QPushButton(uiText("Startup Evidence", "证据启动项"));
  startupSelectedEvidence->setObjectName("startupSelectedEvidence");
  startupSelectedEvidence->setIcon(
      style()->standardIcon(QStyle::SP_DialogApplyButton));
  startupSelectedEvidence->setToolTip(uiText(
      "Create or update Startup SDO rows from selected Object Dictionary rows "
      "that already have Last Value evidence. This does not read or write the "
      "bus.",
      "用选中且已有 Last Value 证据的对象字典行创建或更新 Startup SDO；不会读写"
      "总线。"));
  sdoControls->addWidget(new QLabel(uiText("Index", "索引")), 0, 0);
  sdoControls->addWidget(sdoIndex_, 0, 1);
  sdoControls->addWidget(new QLabel(uiText("Sub", "子项")), 0, 2);
  sdoControls->addWidget(sdoSubIndex_, 0, 3);
  sdoControls->addWidget(readSdo, 0, 4);
  sdoControls->addWidget(readSelectedDictionary, 0, 5);
  sdoControls->addWidget(sdoValue_, 0, 6);
  sdoControls->addWidget(useSdoValueButton_, 0, 7);
  sdoControls->addWidget(useSdoEvidenceButton, 0, 8);
  sdoControls->addWidget(pickSdoEvidenceButton, 0, 9);
  sdoControls->addWidget(new QLabel(uiText("Type", "类型")), 1, 0);
  sdoControls->addWidget(sdoType_, 1, 1, 1, 2);
  sdoControls->addWidget(new QLabel(uiText("Write", "写入")), 1, 3);
  sdoControls->addWidget(sdoWriteValue_, 1, 4, 1, 3);
  sdoControls->addWidget(writeSdo, 1, 7);
  sdoControls->addWidget(watchSelectedDictionary, 1, 8);
  sdoControls->addWidget(startupSelectedEvidence, 1, 9);
  sdoLayout->addLayout(sdoControls);
  sdoInspectorLabel_ = new QLabel;
  sdoInspectorLabel_->setObjectName("sdoInspector");
  sdoInspectorLabel_->setWordWrap(true);
  sdoInspectorLabel_->setTextInteractionFlags(Qt::TextSelectableByMouse);
  sdoLayout->addWidget(sdoInspectorLabel_);

  sdoModeTabs_ = new QTabWidget;
  sdoModeTabs_->setObjectName("sdoModeTabs");
  configureWorkspaceTabsForRelease(sdoModeTabs_);
  sdoModeTabs_->setMovable(false);
  auto *sdoDictionaryPage = new QWidget;
  auto *sdoDictionaryLayout = new QVBoxLayout(sdoDictionaryPage);
  sdoDictionaryLayout->setContentsMargins(0, 0, 0, 0);
  sdoDictionaryLayout->setSpacing(8);
  auto *sdoSelectedPage = new QWidget;
  auto *sdoSelectedLayout = new QVBoxLayout(sdoSelectedPage);
  sdoSelectedLayout->setContentsMargins(0, 0, 0, 0);
  sdoSelectedLayout->setSpacing(8);
  auto *sdoTrailPage = new QWidget;
  auto *sdoTrailLayout = new QVBoxLayout(sdoTrailPage);
  sdoTrailLayout->setContentsMargins(0, 0, 0, 0);
  sdoTrailLayout->setSpacing(8);
  auto *sdoBookmarkPage = new QWidget;
  auto *sdoBookmarkLayout = new QVBoxLayout(sdoBookmarkPage);
  sdoBookmarkLayout->setContentsMargins(0, 0, 0, 0);
  sdoBookmarkLayout->setSpacing(8);
  auto *sdoHistoryPage = new QWidget;
  auto *sdoHistoryLayout = new QVBoxLayout(sdoHistoryPage);
  sdoHistoryLayout->setContentsMargins(0, 0, 0, 0);
  sdoHistoryLayout->setSpacing(8);
  auto *sdoTargetHeader = new QHBoxLayout;
  sdoTargetHeader->setSpacing(8);
  sdoTargetHeader->addWidget(
      makeSectionTitle(uiText("Selected Object", "选中对象")));
  sdoTargetHeader->addStretch(1);
  auto *sdoTargetActions = new QGridLayout;
  sdoTargetActions->setHorizontalSpacing(8);
  sdoTargetActions->setVerticalSpacing(8);
  auto *reviewWriteDeltaSdo = new QPushButton(uiText("Delta", "差异"));
  reviewWriteDeltaSdo->setObjectName("reviewSdoWriteDelta");
  reviewWriteDeltaSdo->setIcon(
      style()->standardIcon(QStyle::SP_MessageBoxWarning));
  reviewWriteDeltaSdo->setToolTip(uiText(
      "Open the local evidence that explains the current Evidence Set conflict "
      "or Write Delta without reading or writing the bus.",
      "打开解释当前证据集冲突或写入差异的本地证据，不读取也不写入总线。"));
  auto *runSelectedObjectRowAction =
      new QPushButton(uiText("Run Row", "执行行"));
  runSelectedObjectRowAction->setObjectName("runSdoTargetRowAction");
  runSelectedObjectRowAction->setIcon(
      style()->standardIcon(QStyle::SP_CommandLink));
  runSelectedObjectRowAction->setToolTip(uiText(
      "Select a Selected Object row to run its local Action column entry.",
      "选择一行选中对象后，执行其 Action/动作列中的本地动作。"));
  auto *copySelectedObjectRow = new QPushButton(uiText("Copy Row", "复制行"));
  copySelectedObjectRow->setObjectName("copySdoTargetRowEvidence");
  copySelectedObjectRow->setIcon(
      style()->standardIcon(QStyle::SP_FileDialogContentsView));
  copySelectedObjectRow->setToolTip(uiText(
      "Copy the selected Selected Object row evidence without bus access.",
      "复制所选选中对象行证据，不访问总线。"));
  auto *copyEvidenceDigestSdo = new QPushButton(uiText("Digest", "摘要"));
  copyEvidenceDigestSdo->setObjectName("copySdoEvidenceDigest");
  copyEvidenceDigestSdo->setIcon(
      style()->standardIcon(QStyle::SP_FileDialogContentsView));
  copyEvidenceDigestSdo->setToolTip(uiText(
      "Copy the current target, Evidence Set, Write Delta, and local evidence "
      "links for review. This does not access the bus.",
      "复制当前目标、证据集、写入差异和本地证据链接用于复核；不访问总线。"));
  auto *openWatchLinkSdo = new QPushButton(uiText("Watch", "Watch"));
  openWatchLinkSdo->setObjectName("openSdoWatchLink");
  openWatchLinkSdo->setIcon(
      style()->standardIcon(QStyle::SP_FileDialogDetailedView));
  openWatchLinkSdo->setToolTip(uiText(
      "Open the matching Watch row for the current SDO target without reading "
      "the bus.",
      "打开当前 SDO 目标匹配的 Watch 行，不读取总线。"));
  auto *openStartupLinkSdo = new QPushButton(uiText("Startup", "Startup"));
  openStartupLinkSdo->setObjectName("openSdoStartupLink");
  openStartupLinkSdo->setIcon(
      style()->standardIcon(QStyle::SP_FileDialogDetailedView));
  openStartupLinkSdo->setToolTip(
      uiText("Open the matching Startup SDO row for the current target without "
             "writing the bus.",
             "打开当前目标匹配的 Startup SDO 行，不写入总线。"));
  auto *openBookmarkLinkSdo = new QPushButton(uiText("Bookmark", "书签"));
  openBookmarkLinkSdo->setObjectName("openSdoBookmarkLink");
  openBookmarkLinkSdo->setIcon(
      style()->standardIcon(QStyle::SP_FileDialogDetailedView));
  openBookmarkLinkSdo->setToolTip(uiText(
      "Open the matching Object Bookmark row for the current target without "
      "bus access.",
      "打开当前目标匹配的对象书签行，不访问总线。"));
  auto *openTargetTrailLinkSdo = new QPushButton(uiText("Trail", "轨迹"));
  openTargetTrailLinkSdo->setObjectName("openSdoTargetTrailLink");
  openTargetTrailLinkSdo->setIcon(
      style()->standardIcon(QStyle::SP_FileDialogDetailedView));
  openTargetTrailLinkSdo->setToolTip(uiText(
      "Open the matching SDO Target Trail row for the current target without "
      "bus access.",
      "打开当前目标匹配的 SDO 目标轨迹行，不访问总线。"));
  auto *readTargetSdo = new QPushButton(uiText("Read", "读取"));
  readTargetSdo->setObjectName("readTargetSdo");
  readTargetSdo->setIcon(style()->standardIcon(QStyle::SP_ArrowDown));
  auto *writeTargetSdo = new QPushButton(uiText("Write", "写入"));
  writeTargetSdo->setObjectName("writeTargetSdo");
  writeTargetSdo->setIcon(style()->standardIcon(QStyle::SP_ArrowUp));
  auto *watchTargetSdo = new QPushButton(uiText("Add Watch", "加监视"));
  watchTargetSdo->setObjectName("watchTargetSdo");
  watchTargetSdo->setIcon(
      style()->standardIcon(QStyle::SP_FileDialogNewFolder));
  auto *bookmarkTargetSdo = new QPushButton(uiText("Bookmark", "书签"));
  bookmarkTargetSdo->setObjectName("bookmarkTargetSdo");
  bookmarkTargetSdo->setIcon(
      style()->standardIcon(QStyle::SP_FileDialogInfoView));
  bookmarkTargetSdo->setToolTip(uiText(
      "Save the current SDO target as a project bookmark without bus access.",
      "把当前 SDO 目标保存为工程书签，不访问总线。"));
  auto *startupTargetSdo = new QPushButton(uiText("Startup", "启动"));
  startupTargetSdo->setObjectName("startupTargetSdo");
  startupTargetSdo->setIcon(
      style()->standardIcon(QStyle::SP_DialogApplyButton));
  sdoSelectedLayout->addLayout(sdoTargetHeader);
  sdoTargetActions->addWidget(reviewWriteDeltaSdo, 0, 0);
  sdoTargetActions->addWidget(runSelectedObjectRowAction, 0, 1);
  sdoTargetActions->addWidget(copySelectedObjectRow, 0, 2);
  sdoTargetActions->addWidget(copyEvidenceDigestSdo, 0, 3);
  sdoTargetActions->addWidget(openWatchLinkSdo, 0, 4);
  sdoTargetActions->addWidget(openStartupLinkSdo, 0, 5);
  sdoTargetActions->addWidget(openBookmarkLinkSdo, 0, 6);
  sdoTargetActions->addWidget(openTargetTrailLinkSdo, 0, 7);
  sdoTargetActions->addWidget(readTargetSdo, 1, 0);
  sdoTargetActions->addWidget(writeTargetSdo, 1, 1);
  sdoTargetActions->addWidget(watchTargetSdo, 1, 2);
  sdoTargetActions->addWidget(bookmarkTargetSdo, 1, 3);
  sdoTargetActions->addWidget(startupTargetSdo, 1, 4);
  sdoTargetActions->setColumnStretch(8, 1);
  sdoSelectedLayout->addLayout(sdoTargetActions);
  sdoTargetTable_ = new QTableWidget(0, 3);
  sdoTargetTable_->setObjectName("sdoTargetTable");
  sdoTargetTable_->setHorizontalHeaderLabels({uiText("Field", "字段"),
                                              uiText("Value", "值"),
                                              uiText("Action", "动作")});
  sdoTargetTable_->verticalHeader()->setVisible(false);
  sdoTargetTable_->horizontalHeader()->setSectionResizeMode(
      0, QHeaderView::ResizeToContents);
  sdoTargetTable_->horizontalHeader()->setSectionResizeMode(
      2, QHeaderView::ResizeToContents);
  sdoTargetTable_->horizontalHeader()->setSectionResizeMode(
      1, QHeaderView::Stretch);
  sdoTargetTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  sdoTargetTable_->setSelectionMode(QAbstractItemView::SingleSelection);
  sdoTargetTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
  sdoTargetTable_->setFocusPolicy(Qt::StrongFocus);
  sdoTargetTable_->setContextMenuPolicy(Qt::CustomContextMenu);
  sdoTargetTable_->setMinimumHeight(170);
  sdoTargetTable_->setMaximumHeight(QWIDGETSIZE_MAX);
  sdoTargetTable_->setToolTip(uiText(
      "Live context for the currently selected Object Dictionary, Watch, PDO, "
      "Free Run, History, Startup, or manual SDO target. Write Delta compares "
      "the pending write value with local "
      "Read/OD/Watch/Startup/Bookmark/Target Trail evidence before "
      "confirmation. Double-click or press Alt+Enter on actionable rows to "
      "open local evidence, review delta, or copy the evidence digest. "
      "Right-click a row for explicit row evidence actions.",
      "显示当前对象字典、Watch、PDO、Free Run、历史、Startup 或手动 SDO 目标的"
      "实时上下文；写入差异会在确认前把待写值与本地读回、OD、Watch、Startup、"
      "书签或目标轨迹证据进行比较。双击或在可操作行按 Alt+Enter，可打开本地"
      "证据、审阅差异或复制证据摘要。右键某一行可执行明确的本行证据动作。"));
  sdoSelectedLayout->addWidget(sdoTargetTable_, 1);

  auto *sdoTargetTrailHeader = new QHBoxLayout;
  sdoTargetTrailHeader->setSpacing(8);
  sdoTargetTrailHeader->addWidget(
      makeSectionTitle(uiText("SDO Target Trail", "SDO 目标轨迹")));
  auto *sdoTargetTrailHint = new QLabel(uiText(
      "Recent local SDO targets from OD, PDO, Watch, Free Run, I/O, Startup, "
      "History, Bookmarks, and manual fields.",
      "来自对象字典、PDO、Watch、Free Run、I/O、Startup、历史、书签和手动字段"
      "的最近本地 SDO 目标。"));
  sdoTargetTrailHint->setObjectName("diagnosticsSummary");
  sdoTargetTrailHint->setTextInteractionFlags(Qt::TextSelectableByMouse);
  auto *restoreSdoTargetTrail =
      new QPushButton(uiText("Restore Target", "恢复目标"));
  restoreSdoTargetTrail->setObjectName("restoreSdoTargetTrail");
  restoreSdoTargetTrail->setIcon(
      style()->standardIcon(QStyle::SP_ArrowForward));
  restoreSdoTargetTrail->setToolTip(uiText(
      "Restore the selected target into the SDO fields. This only changes "
      "local UI context.",
      "把所选目标恢复到 SDO 字段；只改变本地界面上下文。"));
  auto *watchSdoTargetTrail = new QPushButton(uiText("Watch", "监视"));
  watchSdoTargetTrail->setObjectName("watchSdoTargetTrail");
  watchSdoTargetTrail->setIcon(
      style()->standardIcon(QStyle::SP_FileDialogNewFolder));
  watchSdoTargetTrail->setToolTip(uiText(
      "Restore the selected trail target locally and add it to Watch without "
      "issuing an immediate SDO read.",
      "本地恢复所选轨迹目标并加入 Watch，不立即读取 SDO。"));
  auto *bookmarkSdoTargetTrail = new QPushButton(uiText("Bookmark", "书签"));
  bookmarkSdoTargetTrail->setObjectName("bookmarkSdoTargetTrail");
  bookmarkSdoTargetTrail->setIcon(
      style()->standardIcon(QStyle::SP_FileDialogInfoView));
  bookmarkSdoTargetTrail->setToolTip(uiText(
      "Save the selected trail target as a project Object Bookmark without "
      "bus access.",
      "把所选轨迹目标保存为工程对象书签，不访问总线。"));
  auto *startupSdoTargetTrail = new QPushButton(uiText("Startup", "启动"));
  startupSdoTargetTrail->setObjectName("startupSdoTargetTrail");
  startupSdoTargetTrail->setIcon(
      style()->standardIcon(QStyle::SP_DialogApplyButton));
  startupSdoTargetTrail->setToolTip(uiText(
      "Create a Startup SDO candidate from the selected trail write value, or "
      "from its last value if no write value is saved. This only edits the "
      "Startup SDO table.",
      "用所选轨迹写入值创建 Startup SDO 候选；没有写入值时使用最后值。"
      "此操作只编辑 Startup SDO 表。"));
  auto *removeSdoTargetTrail = new QPushButton(uiText("Remove", "移除"));
  removeSdoTargetTrail->setObjectName("removeSdoTargetTrail");
  removeSdoTargetTrail->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
  auto *clearSdoTargetTrailButton = new QPushButton(uiText("Clear", "清空"));
  clearSdoTargetTrailButton->setObjectName("clearSdoTargetTrail");
  clearSdoTargetTrailButton->setIcon(
      style()->standardIcon(QStyle::SP_TrashIcon));
  sdoTargetTrailHeader->addWidget(sdoTargetTrailHint, 1);
  sdoTargetTrailHeader->addWidget(restoreSdoTargetTrail);
  sdoTargetTrailHeader->addWidget(watchSdoTargetTrail);
  sdoTargetTrailHeader->addWidget(bookmarkSdoTargetTrail);
  sdoTargetTrailHeader->addWidget(startupSdoTargetTrail);
  sdoTargetTrailHeader->addWidget(removeSdoTargetTrail);
  sdoTargetTrailHeader->addWidget(clearSdoTargetTrailButton);
  sdoTrailLayout->addLayout(sdoTargetTrailHeader);
  ensureSdoTargetTrailTable();
  sdoTargetTrailTable_->setMinimumHeight(96);
  sdoTargetTrailTable_->setMaximumHeight(QWIDGETSIZE_MAX);
  sdoTargetTrailTable_->setToolTip(uiText(
      "Double-click or use Restore Target to refill the SDO fields from a "
      "recent local target. Use Watch, Bookmark, or Startup to reuse the same "
      "row locally. These actions do not read the bus, write SDOs, change "
      "state, toggle Free Run, or run Host Health.",
      "双击或点击恢复目标可从最近本地目标回填 SDO 字段；也可以用 Watch、"
      "书签或 Startup 在本地复用同一行。不会读取总线、写入 SDO、切换状态、"
      "改变 Free Run 或运行 Host Health。"));
  sdoTargetTrailDetailLabel_ = makeStatusSummaryLabel(
      uiText("Select an SDO target trail row to review reuse readiness and "
             "local execution boundary.",
             "选择一条 SDO 目标轨迹行，以复核复用就绪度和本地执行边界。"),
      uiText(
          "Selecting trail rows and reading this detail strip are local "
          "review actions. Restore, Watch, Bookmark, and Startup reuse "
          "local table evidence only.",
          "选择目标轨迹行和查看此详情条都是本地复核动作；恢复、Watch、书签和 "
          "Startup 只复用本地表格证据。"));
  sdoTrailLayout->addWidget(sdoTargetTrailDetailLabel_);
  sdoTrailLayout->addWidget(sdoTargetTrailTable_, 1);
  sdoDictionaryLayout->addWidget(sdoFilter_);
  auto *sdoFilterChips = new QHBoxLayout;
  sdoFilterChips->setSpacing(6);
  struct SdoFilterChip {
    QString label;
    QString query;
    QString tip;
  };
  const QVector<SdoFilterChip> sdoFilterPresets = {
      {uiText("All", "全部"), QString(),
       uiText("Clear Object Dictionary filtering", "清除对象字典过滤")},
      {uiText("Writable", "可写"), "tag:writable",
       uiText("Show writable Object Dictionary entries",
              "显示可写对象字典条目")},
      {uiText("Readable", "可读"), "tag:readable",
       uiText("Show readable Object Dictionary entries",
              "显示可读对象字典条目")},
      {"CiA 402", "tag:cia402",
       uiText("Show common CiA 402 drive objects",
              "显示常见 CiA 402 驱动对象")},
      {uiText("Identity", "身份"), "tag:identity",
       uiText("Show device identity and diagnostic identity objects",
              "显示设备身份和诊断身份对象")},
      {"PDO", "tag:pdo",
       uiText("Show PDO assignment and mapping objects",
              "显示 PDO 分配和映射对象")},
      {uiText("Errors", "错误"), "tag:error",
       uiText("Show error, emergency, and diagnostic objects",
              "显示错误、Emergency 和诊断对象")},
      {uiText("Evidence", "有证据"), "tag:evidence",
       uiText("Show Object Dictionary rows with Last Value or Last Status",
              "显示已有 Last Value 或 Last Status 的对象字典行")},
      {uiText("Failed", "失败"), "tag:failed",
       uiText("Show Object Dictionary rows whose latest SDO evidence failed",
              "显示最新 SDO 证据为失败的对象字典行")},
  };
  for (const auto &preset : sdoFilterPresets) {
    auto *button = new QPushButton(preset.label);
    button->setObjectName("sdoFilterChip");
    button->setProperty("filterQuery", preset.query);
    button->setToolTip(preset.tip);
    connect(button, &QPushButton::clicked, this,
            [this, query = preset.query] { setSdoFilterPreset(query); });
    sdoFilterChips->addWidget(button);
  }
  sdoFilterChips->addStretch(1);
  sdoSummaryLabel_ = new QLabel(uiText("No OD entries", "暂无 OD 条目"));
  sdoSummaryLabel_->setObjectName("diagnosticsSummary");
  sdoSummaryLabel_->setTextInteractionFlags(Qt::TextSelectableByMouse);
  auto *readVisibleDictionary = new QPushButton(uiText("Read", "读取"));
  readVisibleDictionary->setObjectName("readVisibleDictionary");
  readVisibleDictionary->setIcon(
      style()->standardIcon(QStyle::SP_BrowserReload));
  readVisibleDictionary->setToolTip(uiText(
      "Read every currently visible Object Dictionary row. Large batches ask "
      "for confirmation.",
      "读取当前过滤后可见的所有对象字典行；大批量读取前会要求确认。"));
  auto *readFailedDictionary = new QPushButton(uiText("Retry", "重试"));
  readFailedDictionary->setObjectName("readFailedDictionary");
  readFailedDictionary->setIcon(
      style()->standardIcon(QStyle::SP_MessageBoxWarning));
  readFailedDictionary->setToolTip(uiText(
      "Retry only Object Dictionary rows whose latest SDO evidence failed.",
      "只重试最新 SDO 证据为失败的对象字典行。"));
  auto *watchVisibleDictionary = new QPushButton(uiText("Watch", "监视"));
  watchVisibleDictionary->setObjectName("watchVisibleDictionary");
  watchVisibleDictionary->setIcon(
      style()->standardIcon(QStyle::SP_FileDialogNewFolder));
  watchVisibleDictionary->setToolTip(uiText(
      "Add currently visible Object Dictionary rows to Watch without issuing "
      "immediate reads.",
      "把当前过滤后可见的对象字典行加入 Watch，不立即批量读取。"));
  sdoFilterChips->addWidget(sdoSummaryLabel_);
  sdoFilterChips->addWidget(readVisibleDictionary);
  sdoFilterChips->addWidget(readFailedDictionary);
  sdoFilterChips->addWidget(watchVisibleDictionary);
  sdoDictionaryLayout->addLayout(sdoFilterChips);
  sdoTable_->setToolTip(
      uiText("Selecting a row fills Index/Sub/Type. Double-click reads the "
             "selected object.",
             "选中行会填充 Index/Sub/Type；双击读取选中对象。"));
  sdoDictionaryLayout->addWidget(sdoTable_, 1);

  auto *bookmarkHeader = new QHBoxLayout;
  bookmarkHeader->setSpacing(8);
  auto *bookmarkSelectedDictionary =
      new QPushButton(uiText("Bookmark Selected", "收藏所选"));
  bookmarkSelectedDictionary->setObjectName("bookmarkSelectedDictionary");
  bookmarkSelectedDictionary->setIcon(
      style()->standardIcon(QStyle::SP_FileDialogInfoView));
  bookmarkSelectedDictionary->setToolTip(uiText(
      "Save selected Object Dictionary rows as project bookmarks. This does "
      "not read or write the bus.",
      "把选中的对象字典行保存为工程书签，不读写总线。"));
  auto *fillBookmarkSdo = new QPushButton(uiText("Fill Bookmark", "回填书签"));
  fillBookmarkSdo->setObjectName("fillBookmarkSdo");
  fillBookmarkSdo->setIcon(style()->standardIcon(QStyle::SP_ArrowForward));
  fillBookmarkSdo->setToolTip(
      uiText("Fill the SDO fields from the selected object bookmark.",
             "用选中的对象书签回填 SDO 指令字段。"));
  auto *watchBookmarkSdo =
      new QPushButton(uiText("Watch Bookmark", "监视书签"));
  watchBookmarkSdo->setObjectName("watchBookmarkSdo");
  watchBookmarkSdo->setIcon(
      style()->standardIcon(QStyle::SP_FileDialogNewFolder));
  watchBookmarkSdo->setToolTip(
      uiText("Add selected object bookmarks to Watch without immediate reads.",
             "把选中的对象书签加入 Watch，不立即读取。"));
  auto *startupBookmarkSdo =
      new QPushButton(uiText("Create Startup", "创建启动项"));
  startupBookmarkSdo->setObjectName("startupBookmarkSdo");
  startupBookmarkSdo->setIcon(
      style()->standardIcon(QStyle::SP_DialogApplyButton));
  startupBookmarkSdo->setToolTip(uiText(
      "Create or update Startup SDO rows from selected bookmarks using their "
      "saved Last Value. This does not read or write the bus.",
      "使用所选书签保存的最后值创建或更新 Startup SDO 行；不会读写总线。"));
  auto *removeBookmarkSdo =
      new QPushButton(uiText("Remove Bookmark", "移除书签"));
  removeBookmarkSdo->setObjectName("removeBookmarkSdo");
  removeBookmarkSdo->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
  bookmarkHeader->addWidget(
      makeSectionTitle(uiText("Object Bookmarks", "对象书签")));
  bookmarkHeader->addStretch(1);
  bookmarkHeader->addWidget(bookmarkSelectedDictionary);
  bookmarkHeader->addWidget(fillBookmarkSdo);
  bookmarkHeader->addWidget(watchBookmarkSdo);
  bookmarkHeader->addWidget(startupBookmarkSdo);
  bookmarkHeader->addWidget(removeBookmarkSdo);
  sdoBookmarkLayout->addLayout(bookmarkHeader);
  objectBookmarkTable_ = new QTableWidget;
  objectBookmarkTable_->setObjectName("objectBookmarkTable");
  objectBookmarkTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
  objectBookmarkTable_->setSelectionMode(QAbstractItemView::ExtendedSelection);
  objectBookmarkTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  objectBookmarkTable_->setContextMenuPolicy(Qt::CustomContextMenu);
  objectBookmarkTable_->verticalHeader()->setVisible(false);
  objectBookmarkTable_->setMinimumHeight(120);
  objectBookmarkTable_->setToolTip(uiText(
      "Project-local SDO object bookmarks. Double-click fills the SDO target; "
      "reads and writes still require explicit user actions.",
      "工程内 SDO 对象书签。双击只回填 SDO "
      "目标；读取和写入仍需用户显式操作。"));
  ensureObjectBookmarkTable();
  objectBookmarkDetailLabel_ = makeStatusSummaryLabel(
      uiText("Select an object bookmark to review saved evidence, reuse "
             "options, and local execution boundary.",
             "选择一个对象书签，以复核保存证据、复用选项和本地执行边界。"),
      uiText("Selecting bookmarks and reading this detail strip are local "
             "review actions. Fill, Watch, and Startup reuse project data only "
             "until an explicit read or apply action is requested.",
             "选择书签和查看此详情条都是本地复核动作；回填、Watch 和 Startup "
             "只复用"
             "工程数据，直到用户显式请求读取或应用。"));
  sdoBookmarkLayout->addWidget(objectBookmarkDetailLabel_);
  sdoBookmarkLayout->addWidget(objectBookmarkTable_, 1);

  auto *sdoHistoryHeader = new QHBoxLayout;
  sdoHistoryHeader->setSpacing(8);
  auto *historyWatchSelected =
      new QPushButton(uiText("Watch Selected", "监视所选"));
  historyWatchSelected->setObjectName("watchSelectedHistory");
  historyWatchSelected->setIcon(
      style()->standardIcon(QStyle::SP_FileDialogNewFolder));
  historyWatchSelected->setToolTip(uiText(
      "Add selected SDO history rows to Watch without issuing immediate reads.",
      "把选中的 SDO 历史行加入 Watch，不立即批量读取。"));
  auto *startupFromSelectedHistory =
      new QPushButton(uiText("Create Startup", "创建启动项"));
  startupFromSelectedHistory->setObjectName("startupFromSelectedHistory");
  startupFromSelectedHistory->setIcon(
      style()->standardIcon(QStyle::SP_DialogApplyButton));
  startupFromSelectedHistory->setToolTip(uiText(
      "Create Startup SDO rows from selected SDO history rows that contain "
      "values.",
      "用选中且带值的 SDO 历史行创建 Startup SDO。"));
  auto *clearSdoHistory = new QPushButton(uiText("Clear History", "清空历史"));
  clearSdoHistory->setObjectName("clearSdoHistory");
  clearSdoHistory->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
  sdoHistoryHeader->addWidget(
      makeSectionTitle(uiText("SDO Operation History", "SDO 操作历史")));
  sdoHistoryHeader->addStretch(1);
  sdoHistoryHeader->addWidget(historyWatchSelected);
  sdoHistoryHeader->addWidget(startupFromSelectedHistory);
  sdoHistoryHeader->addWidget(clearSdoHistory);
  sdoHistoryLayout->addLayout(sdoHistoryHeader);
  ensureSdoHistoryTable();
  sdoHistoryTable_->setToolTip(
      uiText("Audit trail for SDO reads and writes in this session. Select "
             "history rows to reuse them as Watch items or Startup SDO rows.",
             "记录本次会话中的 SDO 读写操作；可选中历史行复用为 Watch 或 "
             "Startup SDO。"));
  sdoHistoryDetailLabel_ = makeStatusSummaryLabel(
      uiText("Select an SDO history row to review audit evidence.",
             "选择 SDO 历史行以复核审计证据。"),
      uiText("This preview is local. It summarizes the selected SDO operation, "
             "target, value, status, detail, reuse options, and operation "
             "boundary without reading or writing the bus.",
             "此预览仅在本地工作。它汇总选中的 SDO 操作、目标、值、状态、详情、"
             "复用选项和操作边界，不读写总线。"));
  sdoHistoryLayout->addWidget(sdoHistoryDetailLabel_);
  sdoHistoryLayout->addWidget(sdoHistoryTable_, 1);

  sdoModeTabs_->addTab(sdoDictionaryPage, uiText("Dictionary", "字典"));
  sdoModeTabs_->addTab(sdoSelectedPage, uiText("Selected", "选中对象"));
  sdoModeTabs_->addTab(sdoTrailPage, uiText("Trail", "目标轨迹"));
  sdoModeTabs_->addTab(sdoBookmarkPage, uiText("Bookmarks", "书签"));
  sdoModeTabs_->addTab(sdoHistoryPage, uiText("History", "历史"));
  sdoLayout->addWidget(sdoModeTabs_, 1);

  auto *pdoPage = new QWidget;
  pdoMapPage_ = pdoPage;
  auto *pdoLayout = new QVBoxLayout(pdoPage);
  pdoLayout->setContentsMargins(14, 14, 14, 14);
  pdoLayout->setSpacing(10);
  auto *pdoControls = new QHBoxLayout;
  pdoControls->setSpacing(8);
  pdoFilter_ = new QLineEdit;
  pdoFilter_->setPlaceholderText(
      uiText("Filter PDO map by SM, PDO, index, sub, bits, or name",
             "按 SM、PDO、索引、子项、位宽或名称过滤 PDO 映射"));
  auto *addSelectedPdoWatch =
      new QPushButton(uiText("Add Selected to Watch", "选中项加入监视"));
  addSelectedPdoWatch->setObjectName("addSelectedPdoWatch");
  addSelectedPdoWatch->setIcon(
      style()->standardIcon(QStyle::SP_FileDialogNewFolder));
  addSelectedPdoWatch->setToolTip(uiText(
      "Add the selected PDO map rows to Watch without duplicating existing "
      "items.",
      "把选中的 PDO 映射行加入 Watch，并自动复用已有监视项。"));
  pdoSummaryLabel_ = new QLabel(uiText("No PDO entries", "暂无 PDO 条目"));
  pdoSummaryLabel_->setObjectName("diagnosticsSummary");
  pdoControls->addWidget(pdoFilter_, 1);
  pdoControls->addWidget(addSelectedPdoWatch);
  pdoControls->addWidget(pdoSummaryLabel_);
  pdoLayout->addLayout(pdoControls);
  pdoDetailLabel_ = makeStatusSummaryLabel(
      uiText("Select a PDO entry to review process-data evidence.",
             "选择 PDO 条目以复核过程数据证据。"),
      uiText("This preview is local after PDO data is loaded. It summarizes "
             "the selected map row, direction, object address, bit width, "
             "name, inferred SDO type, and operation boundary without reading "
             "or writing SDOs.",
             "PDO 数据加载后，此预览仅在本地工作。它汇总选中映射行、方向、对象"
             "地址、位宽、名称、推断 SDO 类型和操作边界，不读写 SDO。"));
  pdoLayout->addWidget(pdoDetailLabel_);
  pdoLayout->addWidget(pdoTable_, 1);

  auto *startupPage = new QWidget;
  startupSdoPage_ = startupPage;
  auto *startupLayout = new QVBoxLayout(startupPage);
  startupLayout->setContentsMargins(14, 14, 14, 14);
  startupLayout->setSpacing(10);
  auto *startupControls = new QHBoxLayout;
  startupControls->setSpacing(8);
  auto *addStartup = new QPushButton(uiText("Add Startup SDO", "添加启动 SDO"));
  addStartup->setObjectName("addStartupSdo");
  auto *removeStartup = new QPushButton(uiText("Remove Selected", "移除所选"));
  removeStartup->setObjectName("removeStartupSdo");
  auto *moveStartupUp = new QPushButton(uiText("Move Up", "上移"));
  moveStartupUp->setObjectName("moveStartupSdoUp");
  moveStartupUp->setIcon(style()->standardIcon(QStyle::SP_ArrowUp));
  auto *moveStartupDown = new QPushButton(uiText("Move Down", "下移"));
  moveStartupDown->setObjectName("moveStartupSdoDown");
  moveStartupDown->setIcon(style()->standardIcon(QStyle::SP_ArrowDown));
  auto *preflightStartup = new QPushButton(uiText("Preflight", "预检查"));
  preflightStartup->setObjectName("preflightStartupSdo");
  preflightStartup->setIcon(
      style()->standardIcon(QStyle::SP_MessageBoxInformation));
  auto *verifyStartup = new QPushButton(uiText("Verify Startup", "校验启动项"));
  verifyStartup->setObjectName("verifyStartupSdo");
  verifyStartup->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
  auto *verifySelectedStartup =
      new QPushButton(uiText("Verify Selected", "校验所选"));
  verifySelectedStartup->setObjectName("verifySelectedStartupSdo");
  verifySelectedStartup->setIcon(
      style()->standardIcon(QStyle::SP_DialogApplyButton));
  startupWatchDiffsOnly_ = new QCheckBox(uiText("Diffs Only", "只看偏差"));
  startupWatchDiffsOnly_->setObjectName("startupWatchDiffsOnly");
  startupWatchDiffsOnly_->setToolTip(uiText(
      "Show only Startup SDO rows whose expected value differs from current "
      "Watch evidence.",
      "只显示期望启动值和当前 Watch 证据不一致的 Startup SDO 行。"));
  auto *focusWatchDiffStartup =
      new QPushButton(uiText("Review Diffs", "审阅偏差"));
  focusWatchDiffStartup->setObjectName("focusStartupSdoWatchDiffs");
  focusWatchDiffStartup->setIcon(
      style()->standardIcon(QStyle::SP_FileDialogDetailedView));
  focusWatchDiffStartup->setToolTip(
      uiText("Open Startup SDO, enable Diffs Only, and select the first Watch "
             "mismatch.",
             "打开 Startup SDO，启用只看偏差，并选中第一条 Watch 不一致项。"));
  auto *applyWatchDiffStartup =
      new QPushButton(uiText("Apply Diffs", "应用偏差"));
  applyWatchDiffStartup->setObjectName("applyStartupSdoWatchDiffs");
  applyWatchDiffStartup->setIcon(
      style()->standardIcon(QStyle::SP_DialogYesButton));
  applyWatchDiffStartup->setToolTip(uiText(
      "Apply only Startup SDO rows whose expected values differ from current "
      "Watch values.",
      "只应用和当前 Watch 值不一致的 Startup SDO 行。"));
  auto *applyStartup = new QPushButton(uiText("Apply Startup", "应用启动项"));
  applyStartup->setObjectName("applyStartupSdo");
  auto *applySelectedStartup =
      new QPushButton(uiText("Apply Selected", "应用所选"));
  applySelectedStartup->setObjectName("applySelectedStartupSdo");
  applySelectedStartup->setIcon(
      style()->standardIcon(QStyle::SP_DialogYesButton));
  startupControls->addWidget(addStartup);
  startupControls->addWidget(removeStartup);
  startupControls->addWidget(moveStartupUp);
  startupControls->addWidget(moveStartupDown);
  startupControls->addWidget(preflightStartup);
  startupControls->addWidget(verifyStartup);
  startupControls->addWidget(verifySelectedStartup);
  startupControls->addWidget(startupWatchDiffsOnly_);
  startupControls->addWidget(focusWatchDiffStartup);
  startupControls->addWidget(applyWatchDiffStartup);
  startupControls->addWidget(applyStartup);
  startupControls->addWidget(applySelectedStartup);
  startupControls->addStretch(1);
  startupLayout->addLayout(startupControls);
  startupWatchSummaryLabel_ =
      new QLabel(uiText("No Startup SDO rows", "暂无 Startup SDO 行"));
  startupWatchSummaryLabel_->setObjectName("diagnosticsSummary");
  startupWatchSummaryLabel_->setWordWrap(true);
  startupLayout->addWidget(startupWatchSummaryLabel_);
  startupSdoDetailLabel_ =
      new QLabel(uiText("Select a Startup SDO row to review apply evidence.",
                        "选择 Startup SDO 行以复核应用证据。"));
  startupSdoDetailLabel_->setObjectName("statusSummary");
  startupSdoDetailLabel_->setProperty("severity", "neutral");
  startupSdoDetailLabel_->setWordWrap(true);
  startupSdoDetailLabel_->setTextInteractionFlags(Qt::TextSelectableByMouse);
  startupSdoDetailLabel_->setToolTip(uiText(
      "This preview is local. It summarizes the selected Startup SDO target, "
      "expected value, Watch evidence, and apply boundary without writing the "
      "bus.",
      "此预览仅在本地工作。它汇总选中的 Startup SDO 目标、期望值、Watch 证据"
      "和应用边界，不写入总线。"));
  startupLayout->addWidget(startupSdoDetailLabel_);
  ensureStartupSdoTable();
  startupLayout->addWidget(startupSdoTable_);

  auto *notesPage = new QWidget;
  notesPage_ = notesPage;
  auto *notesLayout = new QVBoxLayout(notesPage);
  notesLayout->setContentsMargins(14, 14, 14, 14);
  projectNotes_ = new QPlainTextEdit;
  projectNotes_->setPlaceholderText(
      uiText("Project notes, commissioning steps, and handoff details",
             "工程备注、调试步骤和交接信息"));
  notesLayout->addWidget(projectNotes_);

  masterText_ = new QPlainTextEdit;
  infoText_ = new QPlainTextEdit;
  pdoText_ = new QPlainTextEdit;
  sdoText_ = new QPlainTextEdit;
  xmlText_ = new QPlainTextEdit;
  masterRawPage_ = masterText_;
  slaveRawPage_ = infoText_;
  pdoRawPage_ = pdoText_;
  sdoRawPage_ = sdoText_;
  esiXmlPage_ = xmlText_;
  for (auto *editor : {masterText_, infoText_, pdoText_, sdoText_, xmlText_}) {
    editor->setReadOnly(true);
    editor->setLineWrapMode(QPlainTextEdit::NoWrap);
  }

  auto *freeRunPage = new QWidget;
  freeRunPage_ = freeRunPage;
  auto *freeRunLayout = new QVBoxLayout(freeRunPage);
  freeRunLayout->setContentsMargins(14, 14, 14, 14);
  freeRunLayout->setSpacing(10);
  freeRunLayout->addWidget(
      makeSectionTitle(uiText("Runtime Status", "运行时状态")));
  freeRunLayout->addWidget(freeRunTable_, 1);
  freeRunLayout->addWidget(
      makeSectionTitle(uiText("Process Image", "过程映像")));
  freeRunFilter_ = new QLineEdit;
  freeRunFilter_->setPlaceholderText(
      uiText("Filter process image by name, index, PDO, direction, or value",
             "按名称、索引、PDO、方向或值过滤过程映像"));
  freeRunChangedOnly_ = new QCheckBox(uiText("Changed only", "仅变化项"));
  freeRunEntrySummaryLabel_ =
      new QLabel(uiText("No process image entries", "暂无过程映像条目"));
  freeRunEntrySummaryLabel_->setObjectName("diagnosticsSummary");
  auto *freeRunFilterLayout = new QHBoxLayout;
  freeRunFilterLayout->setSpacing(8);
  freeRunFilterLayout->addWidget(freeRunFilter_, 1);
  freeRunFilterLayout->addWidget(freeRunChangedOnly_);
  freeRunFilterLayout->addWidget(freeRunEntrySummaryLabel_);
  freeRunLayout->addLayout(freeRunFilterLayout);
  freeRunEntryDetailLabel_ =
      new QLabel(uiText("Select a process image entry to review map and name "
                        "evidence.",
                        "选择过程映像条目以复核映射和名称证据。"));
  freeRunEntryDetailLabel_->setObjectName("statusSummary");
  freeRunEntryDetailLabel_->setProperty("severity", "neutral");
  freeRunEntryDetailLabel_->setWordWrap(true);
  freeRunEntryDetailLabel_->setTextInteractionFlags(Qt::TextSelectableByMouse);
  freeRunEntryDetailLabel_->setToolTip(uiText(
      "This preview is local. It summarizes the selected Free Run row, name "
      "source, PDO map evidence, and output boundary without toggling Free "
      "Run.",
      "此预览仅在本地工作。它汇总选中的 Free Run 行、名称来源、PDO 映射证据"
      "和输出边界，不切换 Free Run。"));
  freeRunLayout->addWidget(freeRunEntryDetailLabel_);
  freeRunLayout->addWidget(freeRunEntryTable_, 3);

  auto *ioVariablePage = new QWidget;
  ioVariablePage_ = ioVariablePage;
  auto *ioVariableLayout = new QVBoxLayout(ioVariablePage);
  ioVariableLayout->setContentsMargins(14, 14, 14, 14);
  ioVariableLayout->setSpacing(10);
  auto *ioVariableControls = new QHBoxLayout;
  ioVariableControls->setSpacing(8);
  ioVar_->ioVariableFilter = new QLineEdit;
  ioVar_->ioVariableFilter->setPlaceholderText(uiText(
      "Filter variables by slave, symbol, index, PDO, value, meaning, or risk",
      "按从站、符号、索引、PDO、值、含义或风险过滤变量"));
  ioVar_->ioVariableScopeFilter = new QComboBox;
  ioVar_->ioVariableScopeFilter->setObjectName("ioVariableScopeFilter");
  ioVar_->ioVariableScopeFilter->setToolTip(
      uiText("Limit I/O variables to the current engineering review scope.",
             "按当前工程复核范围筛选 I/O 变量。"));
  ioVar_->ioVariableScopeFilter->addItem(uiText("All", "全部"), "all");
  ioVar_->ioVariableScopeFilter->addItem(uiText("Selected Slave", "当前从站"),
                                  "selected");
  ioVar_->ioVariableScopeFilter->addItem(uiText("Process Image", "过程映像"),
                                  "process");
  ioVar_->ioVariableScopeFilter->addItem(uiText("PDO Only", "仅 PDO"), "pdo");
  ioVar_->ioVariableScopeFilter->addItem(uiText("Watch Evidence", "Watch 证据"),
                                  "watch");
  ioVar_->ioVariableScopeFilter->addItem(uiText("Startup Diff", "启动不一致"),
                                  "startupDiff");
  ioVar_->ioVariableScopeFilter->addItem(uiText("Missing Value", "缺失值"),
                                  "missingValue");
  ioVar_->ioVariableScopeFilter->addItem(uiText("Rx Outputs", "Rx 输出"), "rx");
  ioVar_->ioVariableScopeFilter->addItem(uiText("Tx Inputs", "Tx 输入"), "tx");
  ioVar_->ioVariableScopeFilter->addItem("CiA 402", "cia402");
  ioVar_->ioVariableScopeFilter->addItem(uiText("Changed", "变化项"), "changed");
  ioVar_->ioVariableScopeFilter->addItem(uiText("PLC Issues", "PLC 交接问题"),
                                  "plcIssues");
  auto *refreshIoVariables =
      new QPushButton(uiText("Refresh View", "刷新视图"));
  refreshIoVariables->setObjectName("refreshIoVariables");
  refreshIoVariables->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
  auto *fillIoVariable = new QPushButton(uiText("Fill SDO", "填充 SDO"));
  fillIoVariable->setObjectName("fillIoVariableSdo");
  fillIoVariable->setIcon(style()->standardIcon(QStyle::SP_ArrowForward));
  auto *readIoVariable = new QPushButton(uiText("Read SDO", "读取 SDO"));
  readIoVariable->setObjectName("readIoVariableSdo");
  readIoVariable->setIcon(style()->standardIcon(QStyle::SP_ArrowDown));
  auto *watchSelectedIo = new QPushButton(uiText("Watch Selected", "监视所选"));
  watchSelectedIo->setObjectName("watchSelectedIoVariables");
  watchSelectedIo->setIcon(
      style()->standardIcon(QStyle::SP_FileDialogNewFolder));
  auto *watchVisibleIo = new QPushButton(uiText("Watch Visible", "监视可见"));
  watchVisibleIo->setObjectName("watchVisibleIoVariables");
  watchVisibleIo->setIcon(
      style()->standardIcon(QStyle::SP_FileDialogNewFolder));
  auto *startupSelectedIo =
      new QPushButton(uiText("Startup Selected", "启动所选"));
  startupSelectedIo->setObjectName("startupSelectedIoVariables");
  startupSelectedIo->setIcon(
      style()->standardIcon(QStyle::SP_DialogApplyButton));
  startupSelectedIo->setToolTip(uiText(
      "Create or update Startup SDO rows from selected I/O variable evidence. "
      "Watch value is preferred, Raw value is used as fallback, and no bus "
      "access is performed.",
      "用所选 I/O 变量证据创建或更新 Startup SDO；优先使用 Watch 值，缺失时"
      "使用 Raw 值，且不访问总线。"));
  auto *startupVisibleIo =
      new QPushButton(uiText("Startup Visible", "启动可见"));
  startupVisibleIo->setObjectName("startupVisibleIoVariables");
  startupVisibleIo->setIcon(
      style()->standardIcon(QStyle::SP_DialogApplyButton));
  startupVisibleIo->setToolTip(uiText(
      "Create or update Startup SDO rows from every visible I/O variable with "
      "Watch or Raw evidence. This does not read or write the bus.",
      "用所有可见且带 Watch 或 Raw 证据的 I/O 变量创建或更新 Startup SDO；不会"
      "读写总线。"));
  auto *editIoMetadata = new QPushButton(uiText("Alias", "别名"));
  editIoMetadata->setObjectName("editIoVariableMetadata");
  editIoMetadata->setIcon(style()->standardIcon(QStyle::SP_FileDialogInfoView));
  auto *bulkNameIoVariables = new QPushButton(uiText("Bulk Name", "批量命名"));
  bulkNameIoVariables->setObjectName("bulkNameIoVariables");
  bulkNameIoVariables->setIcon(
      style()->standardIcon(QStyle::SP_FileDialogDetailedView));
  auto *reviewPlcHandoff = new QPushButton(uiText("Review PLC", "审阅 PLC"));
  reviewPlcHandoff->setObjectName("reviewPlcHandoff");
  reviewPlcHandoff->setIcon(
      style()->standardIcon(QStyle::SP_FileDialogDetailedView));
  reviewPlcHandoff->setToolTip(
      uiText("Show visible I/O variables that are not ready for PLC handoff.",
             "只显示当前可见范围内尚未满足 PLC 交接质量的 I/O 变量。"));
  auto *exportIoVariables = new QPushButton(uiText("Export CSV", "导出 CSV"));
  exportIoVariables->setObjectName("exportIoVariablesCsv");
  exportIoVariables->setIcon(
      style()->standardIcon(QStyle::SP_DialogSaveButton));
  auto *exportIoPlc = new QPushButton(uiText("PLC CSV", "PLC CSV"));
  exportIoPlc->setObjectName("exportIoPlcSymbolsCsv");
  exportIoPlc->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
  ioVar_->ioVariableSummaryLabel =
      new QLabel(uiText("No I/O variables", "暂无 I/O 变量"));
  ioVar_->ioVariableSummaryLabel->setObjectName("diagnosticsSummary");
  ioVar_->ioVariableSummaryLabel->setWordWrap(true);
  ioVar_->ioVariableSummaryLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
  ioVariableControls->addWidget(ioVar_->ioVariableFilter, 1);
  ioVariableControls->addWidget(new QLabel(uiText("Scope", "范围")));
  ioVariableControls->addWidget(ioVar_->ioVariableScopeFilter);
  ioVariableControls->addWidget(refreshIoVariables);
  ioVariableControls->addWidget(fillIoVariable);
  ioVariableControls->addWidget(readIoVariable);
  ioVariableControls->addWidget(watchSelectedIo);
  ioVariableControls->addWidget(watchVisibleIo);
  ioVariableControls->addWidget(startupSelectedIo);
  ioVariableControls->addWidget(startupVisibleIo);
  ioVariableControls->addWidget(editIoMetadata);
  ioVariableControls->addWidget(bulkNameIoVariables);
  ioVariableControls->addWidget(reviewPlcHandoff);
  ioVariableControls->addWidget(exportIoVariables);
  ioVariableControls->addWidget(exportIoPlc);
  ioVariableControls->addWidget(ioVar_->ioVariableSummaryLabel);
  ioVariableLayout->addLayout(ioVariableControls);
  ioVar_->ioVariableDetailLabel =
      new QLabel(uiText("Select an I/O variable to review signal evidence.",
                        "选择 I/O 变量以复核信号证据。"));
  ioVar_->ioVariableDetailLabel->setObjectName("statusSummary");
  ioVar_->ioVariableDetailLabel->setProperty("severity", "neutral");
  ioVar_->ioVariableDetailLabel->setWordWrap(true);
  ioVar_->ioVariableDetailLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
  ioVar_->ioVariableDetailLabel->setToolTip(uiText(
      "This preview is local. It summarizes the selected signal, address, "
      "value evidence, Startup comparison, PDO map status, PLC quality, and "
      "operation boundary without reading or writing the bus.",
      "此预览仅在本地工作。它汇总选中信号、对象地址、值证据、Startup 对照、"
      "PDO 映射状态、PLC 质量和操作边界，不读写总线。"));
  ioVariableLayout->addWidget(ioVar_->ioVariableDetailLabel);
  ioVar_->ioVariableTable->setToolTip(uiText(
      "Engineering signal table merged from PDO Map, Free Run process image, "
      "Watch evidence, and Startup SDO expectations. Double-click fills and "
      "reads the selected SDO through the normal path.",
      "由 PDO 映射、Free Run 过程映像、Watch 证据和 Startup SDO 期望合并而"
      "成的工程信号表。双击会通过常规路径填充并读取所选 SDO。"));
  setTableRows(
      ioVar_->ioVariableTable,
      {uiText("Slave", "从站"), uiText("Dir", "方向"), uiText("Symbol", "符号"),
       uiText("Index", "索引"), uiText("Sub", "子项"), uiText("Bits", "位宽"),
       uiText("PDO", "PDO"), uiText("Source", "来源"), uiText("Raw", "原始值"),
       uiText("Decoded", "解码"), uiText("Meaning", "含义"),
       uiText("Watch", "Watch"), uiText("Startup", "启动"),
       uiText("Map", "映射"), uiText("Changed", "变化"), uiText("PLC", "PLC"),
       uiText("Alias", "别名"), uiText("Tags", "标签"), uiText("Note", "备注")},
      {});
  ioVariableLayout->addWidget(ioVar_->ioVariableTable, 1);

  auto *consistencyPage = new QWidget;
  consistencyPage_ = consistencyPage;
  auto *consistencyLayout = new QVBoxLayout(consistencyPage);
  consistencyLayout->setContentsMargins(14, 14, 14, 14);
  consistencyLayout->setSpacing(10);
  auto *consistencyControls = new QHBoxLayout;
  consistencyControls->setSpacing(8);
  consistency_->consistencyFilter = new QLineEdit;
  consistency_->consistencyFilter->setPlaceholderText(uiText(
      "Filter consistency by scope, object, variable, severity, or evidence",
      "按范围、对象、变量、级别或证据过滤一致性检查"));
  consistency_->consistencyScopeFilter = new QComboBox;
  consistency_->consistencyScopeFilter->setObjectName("consistencyScopeFilter");
  consistency_->consistencyScopeFilter->addItem(uiText("All", "全部"), "all");
  consistency_->consistencyScopeFilter->addItem(uiText("Errors", "错误"), "error");
  consistency_->consistencyScopeFilter->addItem(uiText("Warnings", "警告"), "warning");
  consistency_->consistencyScopeFilter->addItem(uiText("Topology", "拓扑"), "topology");
  consistency_->consistencyScopeFilter->addItem(uiText("Startup", "启动"), "startup");
  consistency_->consistencyScopeFilter->addItem(uiText("I/O Variables", "I/O 变量"), "io");
  consistency_->consistencyScopeFilter->addItem(uiText("Ready", "就绪"), "ready");
  auto *refreshConsistency =
      new QPushButton(uiText("Refresh Check", "刷新检查"));
  refreshConsistency->setObjectName("refreshConsistency");
  refreshConsistency->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
  auto *openIoFromConsistency =
      new QPushButton(uiText("Open Evidence", "打开证据"));
  openIoFromConsistency->setObjectName("openIoVariablesFromConsistency");
  openIoFromConsistency->setIcon(
      style()->standardIcon(QStyle::SP_FileDialogDetailedView));
  openIoFromConsistency->setToolTip(uiText(
      "Open the best local evidence table for the selected Consistency row. "
      "This only navigates loaded tables and does not access the bus.",
      "打开当前一致性行最相关的本地证据表；只导航已加载表格，不访问总线。"));
  consistency_->consistencySummaryLabel =
      new QLabel(uiText("No consistency check yet", "尚未运行一致性检查"));
  consistency_->consistencySummaryLabel->setObjectName("diagnosticsSummary");
  consistency_->consistencySummaryLabel->setWordWrap(true);
  consistency_->consistencySummaryLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
  consistencyControls->addWidget(consistency_->consistencyFilter, 1);
  consistencyControls->addWidget(new QLabel(uiText("Scope", "范围")));
  consistencyControls->addWidget(consistency_->consistencyScopeFilter);
  consistencyControls->addWidget(refreshConsistency);
  consistencyControls->addWidget(openIoFromConsistency);
  consistencyControls->addWidget(consistency_->consistencySummaryLabel);
  consistencyLayout->addLayout(consistencyControls);
  consistency_->consistencyDetailLabel =
      new QLabel(uiText("Select a Consistency row to review gate evidence.",
                        "选择一致性行以复核门禁证据。"));
  consistency_->consistencyDetailLabel->setObjectName("statusSummary");
  consistency_->consistencyDetailLabel->setProperty("severity", "neutral");
  consistency_->consistencyDetailLabel->setWordWrap(true);
  consistency_->consistencyDetailLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
  consistency_->consistencyDetailLabel->setToolTip(uiText(
      "This preview is local. It summarizes the selected consistency issue, "
      "expected/actual evidence, recommended action, evidence route, and "
      "read-only boundary without accessing the bus.",
      "此预览仅在本地工作。它汇总选中一致性问题、期望/实际证据、建议动作、"
      "证据路由和只读边界，不访问总线。"));
  consistencyLayout->addWidget(consistency_->consistencyDetailLabel);
  consistency_->consistencyTable->setToolTip(uiText(
      "Read-only online/offline consistency review. Double-click a row to "
      "open the best local evidence table without bus access.",
      "只读 Online/Offline 一致性审阅。双击行会打开最相关的本地证据表，不访"
      "问总线。"));
  setTableRows(consistency_->consistencyTable,
               {uiText("Level", "级别"), uiText("Scope", "范围"),
                uiText("Target", "目标"), uiText("Evidence", "证据"),
                uiText("Expected", "期望"), uiText("Actual", "实际"),
                uiText("Action", "建议动作")},
               {});
  consistencyLayout->addWidget(consistency_->consistencyTable, 1);

  auto *watchPage = new QWidget;
  watchPage_ = watchPage;
  auto *watchLayout = new QVBoxLayout(watchPage);
  watchLayout->setContentsMargins(14, 14, 14, 14);
  watchLayout->setSpacing(10);
  auto *watchControls = new QHBoxLayout;
  watchControls->setSpacing(8);
  auto *addWatch = new QPushButton(uiText("Add Current SDO", "添加当前 SDO"));
  addWatch->setObjectName("addWatchSdo");
  addWatch->setIcon(style()->standardIcon(QStyle::SP_FileDialogNewFolder));
  auto *addCia402Watch =
      new QPushButton(uiText("CiA 402 Preset", "CiA 402 预设"));
  addCia402Watch->setObjectName("addCia402WatchPreset");
  addCia402Watch->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
  addCia402Watch->setToolTip(
      uiText("Add common drive objects such as controlword, statusword, mode, "
             "error code, position, velocity, and torque to Watch.",
             "把控制字、状态字、模式、错误码、位置、速度和转矩等常见驱动对象加"
             "入监视。"));
  auto *refreshWatch = new QPushButton(uiText("Refresh Watch", "刷新监视"));
  refreshWatch->setObjectName("refreshWatch");
  refreshWatch->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
  auto *captureWatchBaselineButton =
      new QPushButton(uiText("Capture Baseline", "捕获基线"));
  captureWatchBaselineButton->setObjectName("captureWatchBaseline");
  captureWatchBaselineButton->setIcon(
      style()->standardIcon(QStyle::SP_DialogSaveButton));
  captureWatchBaselineButton->setToolTip(
      uiText("Store current Watch values as baseline for drift comparison.",
             "把当前 Watch 值保存为基线，用于偏差比较。"));
  auto *clearWatchBaselineButton =
      new QPushButton(uiText("Clear Baseline", "清除基线"));
  clearWatchBaselineButton->setObjectName("clearWatchBaseline");
  clearWatchBaselineButton->setIcon(
      style()->standardIcon(QStyle::SP_TrashIcon));
  clearWatchBaselineButton->setToolTip(
      uiText("Clear Watch baselines and drift results.",
             "清除 Watch 基线和偏差结果。"));
  auto *startupFromSelectedWatch =
      new QPushButton(uiText("Create Startup", "创建启动项"));
  startupFromSelectedWatch->setObjectName("startupFromSelectedWatch");
  startupFromSelectedWatch->setIcon(
      style()->standardIcon(QStyle::SP_FileDialogNewFolder));
  startupFromSelectedWatch->setToolTip(
      uiText("Create Startup SDO rows from selected Watch values.",
             "用选中 Watch 行的当前值创建 Startup SDO 行。"));
  auto *syncStartupFromWatch =
      new QPushButton(uiText("Sync Startup", "同步启动"));
  syncStartupFromWatch->setObjectName("syncStartupFromWatch");
  syncStartupFromWatch->setIcon(
      style()->standardIcon(QStyle::SP_DialogApplyButton));
  syncStartupFromWatch->setToolTip(uiText(
      "Update existing Startup SDO rows from selected Watch values, creating "
      "missing rows only when no matching Startup SDO exists.",
      "用选中 Watch 行的当前值更新已有 Startup SDO；没有匹配启动项时才创建新"
      "行。"));
  auto *clearWatch = new QPushButton(uiText("Clear Watch", "清空监视"));
  clearWatch->setObjectName("clearWatch");
  clearWatch->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
  watchAutoRefresh_ = new QCheckBox(uiText("Auto", "自动"));
  watchAutoRefresh_->setObjectName("watchAutoRefresh");
  watchRefreshInterval_ = new QComboBox;
  watchRefreshInterval_->setObjectName("watchRefreshInterval");
  watchRefreshInterval_->addItem("250 ms", 250);
  watchRefreshInterval_->addItem("500 ms", 500);
  watchRefreshInterval_->addItem("1 s", 1000);
  watchRefreshInterval_->addItem("2 s", 2000);
  watchRefreshInterval_->setCurrentIndex(2);
  watchFilter_ = new QLineEdit;
  watchFilter_->setPlaceholderText(
      uiText("Filter watch by slave, index, sub, value, type, or mode",
             "按从站、索引、子项、值、类型或模式过滤监视"));
  watchScopeFilter_ = new QComboBox;
  watchScopeFilter_->setObjectName("watchScopeFilter");
  watchScopeFilter_->setToolTip(
      uiText("Limit Watch rows to the active engineering scope.",
             "按当前工程关注范围筛选 Watch 行。"));
  watchScopeFilter_->addItem(uiText("All", "全部"), "all");
  watchScopeFilter_->addItem(uiText("Selected Slave", "当前从站"), "selected");
  watchScopeFilter_->addItem(uiText("Changed", "变化项"), "changed");
  watchScopeFilter_->addItem(uiText("Baseline Drift", "基线偏离"),
                             "baselineDrift");
  watchScopeFilter_->addItem(uiText("Startup Diff", "启动不一致"),
                             "startupDiff");
  watchScopeFilter_->addItem(uiText("Missing Value", "缺失值"), "missingValue");
  watchScopeFilter_->addItem("CiA 402", "cia402");
  watchChangedOnly_ = new QCheckBox(uiText("Changed only", "仅变化项"));
  watchSummaryLabel_ = new QLabel(uiText("No watch items", "暂无监视项"));
  watchSummaryLabel_->setObjectName("diagnosticsSummary");
  watchSummaryLabel_->setWordWrap(true);
  watchSummaryLabel_->setTextInteractionFlags(Qt::TextSelectableByMouse);
  watchControls->addWidget(addWatch);
  watchControls->addWidget(addCia402Watch);
  watchControls->addWidget(refreshWatch);
  watchControls->addWidget(captureWatchBaselineButton);
  watchControls->addWidget(clearWatchBaselineButton);
  watchControls->addWidget(startupFromSelectedWatch);
  watchControls->addWidget(syncStartupFromWatch);
  watchControls->addWidget(clearWatch);
  watchControls->addWidget(watchAutoRefresh_);
  watchControls->addWidget(watchRefreshInterval_);
  watchControls->addStretch(1);
  watchLayout->addLayout(watchControls);
  auto *watchFilterLayout = new QHBoxLayout;
  watchFilterLayout->setSpacing(8);
  watchFilterLayout->addWidget(watchFilter_, 1);
  watchFilterLayout->addWidget(new QLabel(uiText("Scope", "范围")));
  watchFilterLayout->addWidget(watchScopeFilter_);
  watchFilterLayout->addWidget(watchChangedOnly_);
  watchFilterLayout->addWidget(watchSummaryLabel_);
  watchLayout->addLayout(watchFilterLayout);
  watchDetailLabel_ =
      new QLabel(uiText("Select a Watch row to review value evidence.",
                        "选择 Watch 行以复核数值证据。"));
  watchDetailLabel_->setObjectName("statusSummary");
  watchDetailLabel_->setProperty("severity", "neutral");
  watchDetailLabel_->setWordWrap(true);
  watchDetailLabel_->setTextInteractionFlags(Qt::TextSelectableByMouse);
  watchDetailLabel_->setToolTip(uiText(
      "This preview is local. It summarizes the selected Watch value, baseline "
      "delta, Startup comparison, and refresh boundary without reading the "
      "bus.",
      "此预览仅在本地工作。它汇总选中 Watch 值、基线偏差、Startup 对照和刷新"
      "边界，不读取总线。"));
  watchLayout->addWidget(watchDetailLabel_);
  ensureWatchTable();
  watchLayout->addWidget(watchTable_, 1);

  auto *stateMachinePage = new QWidget;
  stateMachinePage_ = stateMachinePage;
  auto *stateMachineLayout = new QVBoxLayout(stateMachinePage);
  stateMachineLayout->setContentsMargins(14, 14, 14, 14);
  stateMachineLayout->setSpacing(10);
  auto *stateMachineHeader = new QHBoxLayout;
  stateMachineHeader->setSpacing(8);
  stateMachine_->stateMachineSummaryLabel =
      new QLabel(uiText("No state evidence", "暂无状态机证据"));
  stateMachine_->stateMachineSummaryLabel->setObjectName("diagnosticsSummary");
  stateMachine_->stateMachineSummaryLabel->setWordWrap(true);
  stateMachine_->stateMachineSummaryLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
  auto *stateSelectedNext =
      new QPushButton(uiText("Send Recommended", "发送推荐状态"));
  stateSelectedNext->setObjectName("stateSelectedNext");
  stateSelectedNext->setIcon(style()->standardIcon(QStyle::SP_CommandLink));
  stateSelectedNext->setToolTip(uiText(
      "Send the recommended EtherCAT state for the selected matrix row using "
      "the normal safety confirmation.",
      "对状态矩阵选中行发送推荐 EtherCAT 状态，并使用常规安全确认。"));
  auto *stateSelectedPreOp = new QPushButton("PREOP");
  stateSelectedPreOp->setObjectName("stateSelectedPreOp");
  auto *stateSelectedSafeOp = new QPushButton("SAFEOP");
  stateSelectedSafeOp->setObjectName("stateSelectedSafeOp");
  auto *stateSelectedOp = new QPushButton("OP");
  stateSelectedOp->setObjectName("stateSelectedOp");
  auto *stateAllPreOp = new QPushButton(uiText("All PREOP", "全部 PREOP"));
  stateAllPreOp->setObjectName("stateAllPreOp");
  auto *stateAllSafeOp = new QPushButton(uiText("All SAFEOP", "全部 SAFEOP"));
  stateAllSafeOp->setObjectName("stateAllSafeOp");
  stateMachineHeader->addWidget(
      makeSectionTitle(uiText("EtherCAT State Machine", "EtherCAT 状态机")));
  stateMachineHeader->addWidget(stateMachine_->stateMachineSummaryLabel, 1);
  stateMachineHeader->addWidget(stateSelectedNext);
  stateMachineHeader->addWidget(stateSelectedPreOp);
  stateMachineHeader->addWidget(stateSelectedSafeOp);
  stateMachineHeader->addWidget(stateSelectedOp);
  stateMachineHeader->addWidget(stateAllPreOp);
  stateMachineHeader->addWidget(stateAllSafeOp);
  stateMachineLayout->addLayout(stateMachineHeader);
  stateMachine_->stateMachineDetailLabel =
      new QLabel(uiText("Select a state row to review transition evidence.",
                        "选择状态机行以复核状态切换证据。"));
  stateMachine_->stateMachineDetailLabel->setObjectName("statusSummary");
  stateMachine_->stateMachineDetailLabel->setProperty("severity", "neutral");
  stateMachine_->stateMachineDetailLabel->setWordWrap(true);
  stateMachine_->stateMachineDetailLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
  stateMachine_->stateMachineDetailLabel->setToolTip(uiText(
      "Selecting a state-machine row only updates this local evidence preview. "
      "State requests still use the explicit buttons or row double-click and "
      "the normal confirmation dialog.",
      "选择状态机行只会更新此本地证据预览。状态请求仍必须通过显式按钮或双击行，"
      "并继续走常规确认对话框。"));
  stateMachineLayout->addWidget(stateMachine_->stateMachineDetailLabel);
  stateMachine_->stateMachineTable->setToolTip(uiText(
      "State matrix for each detected slave. Select a row to review evidence; "
      "double-click a row only when you intend to send the recommended state "
      "through the normal confirmation flow.",
      "按从站展示状态矩阵。选择行可复核证据；只有明确要通过常规确认流程发送"
      "推荐状态时，才双击该行。"));
  setTableRows(stateMachine_->stateMachineTable,
               {uiText("Slave", "从站"), uiText("Name", "名称"),
                uiText("Current", "当前"), uiText("Recommended", "推荐"),
                uiText("Evidence", "证据"), uiText("Drive", "驱动"),
                uiText("Startup", "启动"), uiText("PDO/Process", "PDO/过程"),
                uiText("Risk", "风险"), uiText("Action", "动作")},
               {});
  stateMachineLayout->addWidget(stateMachine_->stateMachineTable, 1);

  auto *diagnosticsPage = new QWidget;
  diagnosticsPage_ = diagnosticsPage;
  auto *diagnosticsLayout = new QVBoxLayout(diagnosticsPage);
  diagnosticsLayout->setContentsMargins(14, 14, 14, 14);
  diagnosticsLayout->setSpacing(10);
  auto *diagnosticsControls = new QHBoxLayout;
  diagnosticsControls->setSpacing(8);
  diagnostics_->diagnosticsFilter = new QLineEdit;
  diagnostics_->diagnosticsFilter->setPlaceholderText(uiText(
      "Filter diagnostics by source or message", "按来源或消息过滤诊断"));
  diagnostics_->diagnosticsLevelFilter = new QComboBox;
  diagnostics_->diagnosticsLevelFilter->addItem(uiText("All levels", "全部级别"), QString());
  diagnostics_->diagnosticsLevelFilter->addItem("Error", "Error");
  diagnostics_->diagnosticsLevelFilter->addItem("Warning", "Warning");
  diagnostics_->diagnosticsLevelFilter->addItem("Info", "Info");
  auto *hostCheck = new QPushButton(uiText("Run Host Check", "运行主机检查"));
  hostCheck->setObjectName("runHostCheck");
  hostCheck->setIcon(style()->standardIcon(QStyle::SP_ComputerIcon));
  auto *copyHostCommand = new QPushButton(uiText("Copy Command", "复制命令"));
  copyHostCommand->setObjectName("copyHostCommand");
  copyHostCommand->setIcon(
      style()->standardIcon(QStyle::SP_FileDialogContentsView));
  auto *clearDiagnostics = new QPushButton(uiText("Clear", "清空"));
  clearDiagnostics->setObjectName("clearDiagnostics");
  clearDiagnostics->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
  diagnostics_->hostHealthSummaryLabel = new QLabel(
      uiText("Host health has not been checked", "尚未运行主机健康检查"));
  diagnostics_->hostHealthSummaryLabel->setObjectName("diagnosticsSummary");
  diagnostics_->diagnosticsSummaryLabel = new QLabel(uiText("No diagnostics", "暂无诊断"));
  diagnostics_->diagnosticsSummaryLabel->setObjectName("diagnosticsSummary");
  diagnosticsControls->addWidget(diagnostics_->diagnosticsFilter, 1);
  diagnosticsControls->addWidget(diagnostics_->diagnosticsLevelFilter);
  diagnosticsControls->addWidget(hostCheck);
  diagnosticsControls->addWidget(copyHostCommand);
  diagnosticsControls->addWidget(clearDiagnostics);
  diagnosticsLayout->addLayout(diagnosticsControls);
  diagnosticsLayout->addWidget(
      makeSectionTitle(uiText("Host Health", "主机健康")));
  diagnosticsLayout->addWidget(diagnostics_->hostHealthSummaryLabel);
  setTableRows(hostHealthTable_,
               {uiText("Level", "级别"), uiText("Check", "检查项"),
                uiText("Result", "结果"), uiText("Action", "建议动作"),
                uiText("Command", "命令"), uiText("Detail", "细节")},
               {});
  diagnosticsLayout->addWidget(hostHealthTable_, 1);
  diagnosticsLayout->addWidget(
      makeSectionTitle(uiText("Event Stream", "事件流")));
  diagnosticsLayout->addWidget(diagnostics_->diagnosticsSummaryLabel);
  diagnosticsLayout->addWidget(diagnostics_->diagnosticsTable, 1);
  esiRepositoryPage_ = esiTable_;

  overviewTabIndex_ = tabs_->addTab(overview, uiText("Overview", "总览"));
  objectDictionaryTabIndex_ =
      tabs_->addTab(sdoPage, uiText("Object Dictionary", "对象字典"));
  pdoMapTabIndex_ = tabs_->addTab(pdoPage, uiText("PDO Map", "PDO 映射"));
  watchTabIndex_ = tabs_->addTab(watchPage, uiText("Watch", "监视"));
  startupSdoTabIndex_ =
      tabs_->addTab(startupPage, uiText("Startup SDO", "启动 SDO"));
  freeRunTabIndex_ = tabs_->addTab(freeRunPage, uiText("Free Run", "自由运行"));
  ioVariableTabIndex_ =
      tabs_->addTab(ioVariablePage, uiText("I/O Variables", "I/O 变量"));
  consistencyTabIndex_ =
      tabs_->addTab(consistencyPage, uiText("Consistency", "一致性"));
  stateMachineTabIndex_ =
      tabs_->addTab(stateMachinePage, uiText("State Machine", "状态机"));
  diagnosticsTabIndex_ =
      tabs_->addTab(diagnosticsPage, uiText("Diagnostics", "诊断"));
  esiRepositoryTabIndex_ =
      tabs_->addTab(esiTable_, uiText("ESI Repository", "ESI 仓库"));
  notesTabIndex_ = tabs_->addTab(notesPage, uiText("Notes", "备注"));
  rtTestPage_ = buildRtTestPage();
  rtTestTabIndex_ = tabs_->addTab(rtTestPage_, uiText("RT Test", "RT 测试"));
  esiXmlTabIndex_ = tabs_->addTab(xmlText_, uiText("ESI XML", "ESI XML"));
  masterRawTabIndex_ =
      tabs_->addTab(masterText_, uiText("Master Raw", "主站原始输出"));
  slaveRawTabIndex_ =
      tabs_->addTab(infoText_, uiText("Slave Raw", "从站原始输出"));
  pdoRawTabIndex_ = tabs_->addTab(pdoText_, uiText("PDO Raw", "PDO 原始输出"));
  sdoRawTabIndex_ = tabs_->addTab(sdoText_, uiText("SDO Raw", "SDO 原始输出"));
  tabs_->setCurrentIndex(0);
  rightLayout->addWidget(tabs_);
  root->addWidget(right);
  root->setSizes({360, 1080});

  logText_ = new QPlainTextEdit;
  logText_->setReadOnly(true);
  logText_->setMaximumBlockCount(1000);
  auto *logDock = new QDockWidget(uiText("Runtime Log", "运行日志"), this);
  logDock->setObjectName("runtimeLogDock");
  logDock->setWidget(logText_);
  addDockWidget(Qt::BottomDockWidgetArea, logDock);
  logDock->hide();

  statusSummaryLabel_ = new QLabel;
  statusSummaryLabel_->setObjectName("statusSummary");
  statusBar()->addPermanentWidget(statusSummaryLabel_, 1);
  workspaceBoundaryLabel_ = new QLabel;
  workspaceBoundaryLabel_->setObjectName("statusSummary");
  workspaceBoundaryLabel_->setProperty("severity", "neutral");
  workspaceBoundaryLabel_->setTextInteractionFlags(Qt::TextSelectableByMouse);
  statusBar()->addPermanentWidget(workspaceBoundaryLabel_);
  nextBestActionButton_ =
      new QPushButton(uiText("Next Best Action", "下一最佳动作"));
  nextBestActionButton_->setObjectName("nextBestActionButton");
  nextBestActionButton_->setIcon(style()->standardIcon(QStyle::SP_CommandLink));
  nextBestActionButton_->setProperty(
      "action", nextBestActionKey(NextBestActionKind::Connect));
  statusBar()->addPermanentWidget(nextBestActionButton_);
  refreshMasterSelector();
  refreshEsiRepository();
  updateSelectedSlavePanel();
  updateSdoInspector(uiText("Manual fields", "手动字段"));
  updateCommissioningWorkflow();
  updateIoVariableTable();
  updateStateMachineView();
  updateStatusBar();
  updateNextBestAction();
}


// — Tear down and reconstruct the UI while preserving table data and filter state
void MainWindow::rebuildUi() {
  const int previousSelectedPosition = selectedPosition();
  const QString masterSnapshot = lastMasterText_;
  const QString slaveInfoSnapshot = lastSlaveInfoText_;
  const QString pdoSnapshot = lastPdoText_;
  const QString sdoSnapshot = lastSdoText_;
  const QString xmlSnapshot = lastXmlText_;
  const int previousLoadedSlaveInfoPosition = loadedSlaveInfoPosition_;
  const int previousLoadedPdoPosition = loadedPdoPosition_;
  // Preserve loaded position only if the selection hasn't changed
  const int previousLoadedSdoPosition = loadedSdoPosition_;
  const int previousLoadedXmlPosition = loadedXmlPosition_;
  const QString notes =
      projectNotes_ ? projectNotes_->toPlainText() : QString();
  const QString runtimeLog = logText_ ? logText_->toPlainText() : QString();
  const QString sdoIndex = sdoIndex_ ? sdoIndex_->text() : QString("0x1000");
  const QString sdoSubIndex =
      sdoSubIndex_ ? sdoSubIndex_->text() : QString("0x00");
  const QString sdoWriteValue =
      sdoWriteValue_ ? sdoWriteValue_->text() : QString();
  const QString sdoValue = sdoValue_ ? sdoValue_->text() : QString();
  const QString sdoType = sdoType_ ? sdoType_->currentText() : QString();
  const QList<QStringList> sdoTargetTrailRows =
      copyTableRows(sdoTargetTrailTable_);
  const QList<QStringList> objectBookmarkRows =
      copyTableRows(objectBookmarkTable_);
  const QList<QStringList> watchRows = copyTableRows(watchTable_);
  const bool watchAutoRefresh =
      watchAutoRefresh_ ? watchAutoRefresh_->isChecked() : false;
  const int watchInterval = watchRefreshInterval_
                                ? watchRefreshInterval_->currentData().toInt()
                                : 1000;
  const QString watchFilter = watchFilter_ ? watchFilter_->text() : QString();
  const QString watchScope =
      watchScopeFilter_ ? watchScopeFilter_->currentData().toString() : "all";
  const bool watchChangedOnly =
      watchChangedOnly_ ? watchChangedOnly_->isChecked() : false;
  const QList<QStringList> startupRows = copyTableRows(startupSdoTable_);
  const QList<QStringList> diagnosticsRows = copyTableRows(diagnostics_->diagnosticsTable);
  const QString workflowFilter =
      workflowFilter_ ? workflowFilter_->text() : QString();
  const QString workflowScope =
      workflowScopeFilter_ ? workflowScopeFilter_->currentData().toString()
                           : QStringLiteral("all");
  const QString pdoFilter = pdoFilter_ ? pdoFilter_->text() : QString();
  const QString freeRunFilter =
      freeRunFilter_ ? freeRunFilter_->text() : QString();
  const bool freeRunChangedOnly =
      freeRunChangedOnly_ ? freeRunChangedOnly_->isChecked() : false;
  const QString diagnosticsFilter =
      diagnostics_->diagnosticsFilter ? diagnostics_->diagnosticsFilter->text() : QString();
  const QString diagnosticsLevel =
      diagnostics_->diagnosticsLevelFilter
          ? diagnostics_->diagnosticsLevelFilter->currentData().toString()
          : QString();
  const QVector<SlaveInfo> currentSlaves = slaves_;

  if (statusSummaryLabel_) {
    statusBar()->removeWidget(statusSummaryLabel_);
    delete statusSummaryLabel_;
    statusSummaryLabel_ = nullptr;
  }
  if (workspaceBoundaryLabel_) {
    statusBar()->removeWidget(workspaceBoundaryLabel_);
    delete workspaceBoundaryLabel_;
    workspaceBoundaryLabel_ = nullptr;
  }
  if (nextBestActionButton_) {
    statusBar()->removeWidget(nextBestActionButton_);
    delete nextBestActionButton_;
    nextBestActionButton_ = nullptr;
  }
  for (auto *dock : findChildren<QDockWidget *>()) {
    removeDockWidget(dock);
    delete dock;
  }
  for (auto *toolbar : findChildren<QToolBar *>()) {
    removeToolBar(toolbar);
    delete toolbar;
  }
  menuBar()->clear();
  delete takeCentralWidget();

  buildUi();
  wire();

  projectNotes_->setPlainText(notes);
  logText_->setPlainText(runtimeLog);
  sdoIndex_->setText(sdoIndex);
  sdoSubIndex_->setText(sdoSubIndex);
  sdoWriteValue_->setText(sdoWriteValue);
  sdoValue_->setText(sdoValue);
  sdoType_->setCurrentText(sdoType);
  updateSdoInspector(uiText("Restored session", "恢复会话"));
  if (!sdoTargetTrailRows.isEmpty()) {
    ensureSdoTargetTrailTable();
    sdoTargetTrailTable_->setRowCount(sdoTargetTrailRows.size());
    rememberedSdoTargetTrailKeys_.clear();
    for (int row = 0; row < sdoTargetTrailRows.size(); ++row) {
      const QStringList values = sdoTargetTrailRows.at(row);
      for (int column = 0; column < sdoTargetTrailTable_->columnCount();
           ++column) {
        sdoTargetTrailTable_->setItem(
            row, column, new QTableWidgetItem(values.value(column)));
      }
      rememberedSdoTargetTrailKeys_.insert(
          sdoTargetTrailRowKeyFromTable(sdoTargetTrailTable_, row));
    }
    sdoTargetTrailTable_->resizeColumnsToContents(); // auto-fit column widths
  }

  if (!currentSlaves.isEmpty()) {
    updateSlaves(currentSlaves);
    if (previousSelectedPosition >= 0 && topologyTree_) {
      bool restoredSelection = false;
      for (int top = 0;
           top < topologyTree_->topLevelItemCount() && !restoredSelection;
           ++top) {
        auto *masterItem = topologyTree_->topLevelItem(top);
        if (!masterItem) {
          continue;
        }
        for (int child = 0; child < masterItem->childCount(); ++child) {
          auto *slaveItem = masterItem->child(child);
          if (slaveItem && slaveItem->data(0, Qt::UserRole).toInt() ==
                               previousSelectedPosition) {
            topologyTree_->setCurrentItem(slaveItem);
            restoredSelection = true;
            break;
          }
        }
      }
    }
  }

  const int currentSelectedPosition = selectedPosition();
  lastMasterText_ = masterSnapshot;
  lastSlaveInfoText_ = slaveInfoSnapshot;
  lastPdoText_ = pdoSnapshot;
  lastSdoText_ = sdoSnapshot;
  lastXmlText_ = xmlSnapshot;
  loadedSlaveInfoPosition_ =
      !lastSlaveInfoText_.isEmpty() &&
              previousLoadedSlaveInfoPosition == currentSelectedPosition
          ? previousLoadedSlaveInfoPosition
          : -1;
  loadedPdoPosition_ = !lastPdoText_.isEmpty() && previousLoadedPdoPosition ==
                                                      currentSelectedPosition
                           ? previousLoadedPdoPosition
                           : -1;
  // Preserve loaded position only if the selection hasn't changed
  loadedSdoPosition_ = !lastSdoText_.isEmpty() && previousLoadedSdoPosition ==
                                                      currentSelectedPosition
                           // Preserve loaded position only if the selection hasn't changed
                           ? previousLoadedSdoPosition
                           : -1;
  loadedXmlPosition_ = !lastXmlText_.isEmpty() && previousLoadedXmlPosition ==
                                                      currentSelectedPosition
                           ? previousLoadedXmlPosition
                           : -1;
  masterText_->setPlainText(lastMasterText_);
  infoText_->setPlainText(lastSlaveInfoText_);
  pdoText_->setPlainText(lastPdoText_);
  sdoText_->setPlainText(lastSdoText_);
  xmlText_->setPlainText(lastXmlText_);
  updateMasterSummary(lastMasterText_);
  updateSlaveInfo(lastSlaveInfoText_);
  updatePdoTable(lastPdoText_);
  updateSdoTable(lastSdoText_);
  if (!objectBookmarkRows.isEmpty()) {
    ensureObjectBookmarkTable();
    objectBookmarkTable_->setRowCount(objectBookmarkRows.size());
    for (int row = 0; row < objectBookmarkRows.size(); ++row) {
      const QStringList values = objectBookmarkRows.at(row);
      for (int column = 0; column < objectBookmarkTable_->columnCount();
           ++column) {
        objectBookmarkTable_->setItem(
            row, column, new QTableWidgetItem(values.value(column)));
      }
    }
    objectBookmarkTable_->resizeColumnsToContents(); // auto-fit column widths
  }
  if (!watchRows.isEmpty()) {
    ensureWatchTable();
    watchTable_->setRowCount(watchRows.size());
    for (int row = 0; row < watchRows.size(); ++row) {
      const QStringList values = watchRows.at(row);
      // Migrate older 6/7-column watch formats to current 12-column layout
      const bool legacySixColumnWatch = values.size() == 6;
      // Migrate older watch formats to current layout
      const bool legacySevenColumnWatch = values.size() == 7;
      QStringList migrated;
      // Migrate older 6/7-column watch formats to current 12-column layout
      if (legacySixColumnWatch) {
        migrated = {values.value(0), values.value(1), values.value(2),
                    values.value(3), values.value(4), QString(),
                    QString(),       values.value(5), QString(),
                    QString()};
      // Migrate older watch formats to current layout
      } else if (legacySevenColumnWatch) {
        migrated = {values.value(0), values.value(1), values.value(2),
                    values.value(3), values.value(4), QString(),
                    values.value(5), values.value(6), QString(),
                    QString()};
      } else {
        migrated = values;
      }
      for (int column = 0; column < 12; ++column) {
        watchTable_->setItem(row, column,
                             new QTableWidgetItem(migrated.value(column)));
      }
    }
    updateWatchBaselineDeltas();
    updateWatchStartupDeltas();
    watchTable_->resizeColumnsToContents(); // auto-fit column widths
  }
  if (!startupRows.isEmpty()) {
    setTableRows(startupSdoTable_,
                 {uiText("Slave", "从站"), uiText("Index", "索引"),
                  uiText("Sub", "子项"), uiText("Value", "值"),
                  uiText("Type", "类型"), uiText("Status", "状态"),
                  uiText("Detail", "详情"), uiText("Watch Value", "Watch 值"),
                  uiText("Watch Delta", "Watch 偏差")},
                 startupRows);
    ensureStartupSdoTable();
    updateStartupSdoWatchEvidence();
  }
  if (!diagnosticsRows.isEmpty()) {
    setTableRows(diagnostics_->diagnosticsTable,
                 {uiText("Time", "时间"), uiText("Level", "级别"),
                  uiText("Source", "来源"), uiText("Message", "消息")},
                 diagnosticsRows);
    for (int row = 0; row < diagnostics_->diagnosticsTable->rowCount(); ++row) {
      styleDiagnosticsRow(row, diagnostics_->diagnosticsTable->item(row, 1)
                                   ? diagnostics_->diagnosticsTable->item(row, 1)->text()
                                   : QString());
    }
  }
  if (diagnostics_->diagnosticsFilter) {
    diagnostics_->diagnosticsFilter->setText(diagnosticsFilter);
  }
  if (workflowFilter_) {
    workflowFilter_->setText(workflowFilter);
  }
  if (workflowScopeFilter_) {
    const int index = workflowScopeFilter_->findData(workflowScope);
    workflowScopeFilter_->setCurrentIndex(index >= 0 ? index : 0);
  }
  if (pdoFilter_) {
    pdoFilter_->setText(pdoFilter);
  }
  if (freeRunFilter_) {
    freeRunFilter_->setText(freeRunFilter);
  }
  if (freeRunChangedOnly_) {
    freeRunChangedOnly_->setChecked(freeRunChangedOnly);
  }
  if (watchRefreshInterval_) {
    const int index = watchRefreshInterval_->findData(watchInterval);
    watchRefreshInterval_->setCurrentIndex(index >= 0 ? index : 2);
  }
  if (watchAutoRefresh_) {
    watchAutoRefresh_->setChecked(watchAutoRefresh);
  }
  if (watchFilter_) {
    watchFilter_->setText(watchFilter);
  }
  if (watchScopeFilter_) {
    const int index = watchScopeFilter_->findData(watchScope);
    watchScopeFilter_->setCurrentIndex(index >= 0 ? index : 0);
  }
  if (watchChangedOnly_) {
    watchChangedOnly_->setChecked(watchChangedOnly);
  }
  if (diagnostics_->diagnosticsLevelFilter) {
    const int index = diagnostics_->diagnosticsLevelFilter->findData(diagnosticsLevel);
    diagnostics_->diagnosticsLevelFilter->setCurrentIndex(index >= 0 ? index : 0);
  }
  filterPdoTable();
  filterFreeRunEntryTable();
  filterWatchTable();
  updateWatchAutoRefresh();
  filterDiagnosticsTable();
  updateDiagnosticsSummary();
  updateSelectedSlavePanel();
  updateCommissioningWorkflow();
  updateStatusBar();
}

