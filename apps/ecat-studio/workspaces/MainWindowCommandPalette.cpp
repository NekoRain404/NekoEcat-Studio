// Command palette: fuzzy search dialog for quick action access.

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
#include "helpers/StudioDocumentation.h"
#include "helpers/StudioTableHelpers.h"
#include "helpers/StudioTextHelpers.h"
#include "helpers/StudioUiHelpers.h"
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
    // Search input field
#include <QLineEdit>
#include <QListWidget>
    // Create list item for command
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


// — Open the modal command palette dialog for quick action search and execution
void MainWindow::showCommandPalette() {
// Command palette: fuzzy search dialog for quick action access.
  QDialog dialog(this);
  dialog.setObjectName("commandPalette");
  dialog.setWindowTitle(uiText("Command Palette", "命令面板"));
  dialog.setModal(true);
  dialog.resize(760, 560);

// ── Dialog Layout ─────────────────────────────────────────────────────
  auto *layout = new QVBoxLayout(&dialog);
  layout->setContentsMargins(18, 18, 18, 16);
  layout->setSpacing(12);

  auto *title = new QLabel(uiText("Command Palette", "命令面板"));
  title->setObjectName("dialogTitle");
// ── Search Input + Safety Filter ──────────────────────────────────────
  auto *search = new QLineEdit;
  search->setPlaceholderText(uiText("Search commands, workspaces, or masters",
                                    "搜索命令、工作区或主站"));
  auto *commandFilterRow = new QHBoxLayout;
  commandFilterRow->setSpacing(8);
  auto *safetyFilter = new QComboBox;
  safetyFilter->setObjectName("commandSafetyFilter");
  safetyFilter->setMinimumWidth(180);
    // Add command to palette list
  safetyFilter->addItem(
      uiText("All action types  Alt+A", "全部操作类型  Alt+A"), QString());
    // Add command to palette list
  safetyFilter->addItem(
      uiText("Local navigation/data  Alt+L", "本地导航/数据  Alt+L"),
      QStringLiteral("local"));
    // Add command to palette list
  safetyFilter->addItem(
      uiText("Online read/runtime  Alt+O", "在线读取/运行  Alt+O"),
      QStringLiteral("online"));
    // Add command to palette list
  safetyFilter->addItem(
      uiText("Writes/state changes  Alt+D", "写入/状态切换  Alt+D"),
      QStringLiteral("danger"));
    // Add command to palette list
  safetyFilter->addItem(uiText("Host checks  Alt+H", "主机检查  Alt+H"),
                        QStringLiteral("host"));
    // Add command to palette list
  safetyFilter->addItem(uiText("Project/file  Alt+F", "工程/文件  Alt+F"),
                        QStringLiteral("file"));
  safetyFilter->setToolTip(
      uiText("Filter command types with Alt+A/L/O/D/H/F inside the palette",
             "在命令面板内用 Alt+A/L/O/D/H/F 切换操作类型过滤"));
// ── Command List Widget ───────────────────────────────────────────────
  auto *list = new QListWidget;
  list->setObjectName("commandList");
  list->setUniformItemSizes(true);
  list->setSelectionMode(QAbstractItemView::SingleSelection);
  list->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  list->setContextMenuPolicy(Qt::CustomContextMenu);
  list->setToolTip(uiText("Enter runs the selected command. Alt+P pins or "
                          "unpins it. Right-click for pin actions.",
                          "Enter 执行选中命令。Alt+P 固定或取消固定。右键可"
                          "管理固定状态。"));
// ── Preview + Stats Labels ────────────────────────────────────────────
  auto *commandPreview = new QLabel(
      uiText("Select a command to review its action type and safety boundary.",
             "选择命令以查看操作类型和安全边界。"));
  commandPreview->setObjectName("commandPreview");
  commandPreview->setWordWrap(true);
  commandPreview->setTextInteractionFlags(Qt::TextSelectableByMouse);
  auto *commandStats =
      new QLabel(uiText("Type counts update with search and filter.",
                        "类型计数会随搜索和过滤更新。"));
  commandStats->setObjectName("commandStats");
  commandStats->setWordWrap(true);
  commandStats->setTextInteractionFlags(Qt::TextSelectableByMouse);

  layout->addWidget(title);
    // Add widget to layout
  commandFilterRow->addWidget(search, 1);
    // Add widget to layout
  commandFilterRow->addWidget(safetyFilter);
  layout->addLayout(commandFilterRow);
  layout->addWidget(list, 1);
  layout->addWidget(commandStats);
  layout->addWidget(commandPreview);

// ── Command Registry ──────────────────────────────────────────────────
  struct CommandItem {
    QString title;
// Each CommandItem defines a searchable action: title, subtitle, icon, run callback, and enabled check.
    QString subtitle;
    QIcon icon;
    std::function<void()> run;
    std::function<bool()> enabled;
  };
// Safety categories: local (no I/O), online (reads), danger (writes/state), host (diagnostics), file (project).
  struct CommandSafety {
    QString key;
    QString label;
    QString hint;
    QColor color;
  };

  auto actionByName = [this](const char *name) -> QAction * {
    return findChild<QAction *>(name);
  };
  auto actionCommand = [&](const char *name, const QString &title,
                           const QString &subtitle) -> CommandItem {
    QAction *action = actionByName(name);
    return CommandItem{
        title,
        subtitle,
        action ? action->icon() : QIcon(),
        [action] {
          if (action && action->isEnabled()) {
// ── Build Command List ────────────────────────────────────────────────
            action->trigger();
          }
        },
        [action] { return action && action->isEnabled(); },
    };
  };
  auto buttonCommand = [&](const char *name, const QString &title,
                           const QString &subtitle,
                           const QIcon &fallbackIcon = QIcon()) -> CommandItem {
    auto *button = findChild<QPushButton *>(name);
    return CommandItem{
        title,
        subtitle,
        button && !button->icon().isNull() ? button->icon() : fallbackIcon,
        [button] {
          if (button && button->isEnabled()) {
            button->click();
          }
        },
        [button] { return button && button->isEnabled(); },
    };
  };
  auto workspaceCommand = [&](QWidget *page, const QString &name,
                              const QString &shortcut) -> CommandItem {
    const QString shortcutText =
        shortcut.isEmpty() ? QString() : QKeySequence(shortcut).toString();
    return CommandItem{
        uiText("Go to Workspace: ", "切换工作区：") + name,
        shortcutText.isEmpty()
            ? uiText("Activate workspace page without running online commands",
                     "只切换工作区，不执行在线命令")
            : uiText("Activate workspace page (%1) without running online "
                     "commands",
                     "只切换工作区，不执行在线命令（%1）")
                  .arg(shortcutText),
        style()->standardIcon(QStyle::SP_FileDialogListView),
        [this, page] { activateWorkspacePage(page); },
        [this, page] { return tabs_ && page && tabs_->indexOf(page) >= 0; },
    };
  };
  auto currentSdoLabel = [this] {
    const int position = selectedPosition();
    // Get current search text
    const QString index = sdoInspector_->sdoIndex ? sdoInspector_->sdoIndex->text().trimmed() : QString();
    const QString subIndex =
    // Get current search text
        sdoInspector_->sdoSubIndex ? sdoInspector_->sdoSubIndex->text().trimmed() : QString();
    const QString object = !index.isEmpty() && !subIndex.isEmpty()
                               ? QString("%1:%2").arg(index, subIndex)
                               : uiText("no object selected", "尚未选择对象");
    return position >= 0 ? QString("#%1 %2").arg(position).arg(object) : object;
  };
  auto currentSdoValueLabel = [this, currentSdoLabel] {
    QString line = currentSdoLabel();
    const QString type =
        sdoInspector_->sdoType ? sdoInspector_->sdoType->currentText().trimmed() : QString();
    // Get current search text
    QString value = sdoInspector_->sdoValue ? sdoInspector_->sdoValue->text().trimmed() : QString();
    if (value.isEmpty() && sdoInspector_->sdoWriteValue) {
    // Get current search text
      value = sdoInspector_->sdoWriteValue->text().trimmed();
    }
    if (!type.isEmpty()) {
      line += " " + type;
    }
    if (!value.isEmpty()) {
      line += " = " + value;
    }
    return line;
  };
  auto hasCurrentSdo = [this] {
    return selectedPosition() >= 0 && sdoInspector_->sdoIndex &&
    // Get current search text
           !sdoInspector_->sdoIndex->text().trimmed().isEmpty() && sdoInspector_->sdoSubIndex &&
    // Get current search text
           !sdoInspector_->sdoSubIndex->text().trimmed().isEmpty();
  };
  auto hasCurrentSdoValue = [this, hasCurrentSdo] {
    const bool hasReadValue =
    // Get current search text
        sdoInspector_->sdoValue && !sdoInspector_->sdoValue->text().trimmed().isEmpty();
    const bool hasWriteValue =
    // Get current search text
        sdoInspector_->sdoWriteValue && !sdoInspector_->sdoWriteValue->text().trimmed().isEmpty();
    return hasCurrentSdo() && (hasReadValue || hasWriteValue);
  };
  auto hasSelectedObjectPanelRow = [this] {
    return sdoInspector_->sdoTargetTable && sdoInspector_->sdoTargetTable->currentRow() >= 0 &&
// ── Navigation Commands ───────────────────────────────────────────────
           sdoInspector_->sdoTargetTable->currentRow() < sdoInspector_->sdoTargetTable->rowCount();
  };
  auto hasSdoTargetTrailRow = [this] {
    return sdoTargetTrailTable_ && sdoTargetTrailTable_->currentRow() >= 0;
  };
  auto hasWatchValueSelection = [this] { return selectedWatchRowsHaveValue(); };
  auto hasStartupWatchDiffs = [this] {
    return client_.isConnected() && !startupSdoRowsWithWatchDiffs().isEmpty();
  };
  auto canReviewStartupWatchDiffs = [this] {
    return !startupSdoRowsWithWatchDiffs().isEmpty();
  };
  auto canReviewFailedSdoEvidence = [this] { return hasFailedSdoEvidence(); };
  auto canRetryFailedSdoEvidence = [this] {
    return client_.isConnected() && selectedPosition() >= 0 &&
           hasFailedSdoEvidence();
  };

  updateIoVariableTable();

  QVector<CommandItem> commands;
  commands.append(actionCommand("connectAction",
                                uiText("Connect Runtime", "连接运行时"),
                                uiText("Connect to ecatd", "连接到 ecatd")));
  commands.append(actionCommand(
      "refreshAction", uiText("Refresh Online Data", "刷新在线数据"),
      uiText("Update master, slave, and telemetry data",
             "更新主站、从站和遥测数据")));
  commands.append(
      actionCommand("rescanAction", uiText("Rescan Bus", "重新扫描总线"),
// ── Commissioning Workflow Commands ────────────────────────────────────
                    uiText("Request a bus rescan on the active master",
                           "请求当前主站重新扫描总线")));
  commands.append(CommandItem{
      uiText("Run Next Commissioning Step", "执行调试下一步"),
      uiText("Run the first actionable row in the Overview workflow",
             "执行总览工作流中第一条可执行步骤"),
      style()->standardIcon(QStyle::SP_CommandLink),
      [this] { runNextCommissioningWorkflowStep(); },
      [this] { return nextCommissioningWorkflowStep() >= 0; },
  });
  commands.append(CommandItem{
      uiText("Copy Workflow Step Evidence", "复制工作流步骤证据"),
      uiText(
          "Copy the selected Overview workflow step phase, status, risk, "
          "evidence, next action, and local boundary to the clipboard "
          "without bus access",
          "把总览工作流当前步骤的阶段、状态、风险、依据、下一步和本地边界复制"
          "到剪贴板；不访问总线"),
      style()->standardIcon(QStyle::SP_FileDialogContentsView),
      [this] {
        activateWorkspaceTab(overviewTabIndex_);
        copyWorkflowStepDigest(workflow_->workflowTable ? workflow_->workflowTable->currentRow()
                                              : -1);
      },
      [this] {
        const int row = workflow_->workflowTable ? workflow_->workflowTable->currentRow() : -1;
        return workflow_->workflowTable && row >= 0 && !workflow_->workflowTable->isRowHidden(row);
      },
  });
// ── Workflow Scope Commands (per-workspace actions) ────────────────────
  struct WorkflowScopeCommand {
    QString label;
    QString scope;
    QString description;
  };
  const QVector<WorkflowScopeCommand> workflowScopeCommands = {
      {uiText("All", "全部"), "all",
       uiText("Show every commissioning workflow step",
              "显示全部调试工作流步骤")},
      {uiText("Open", "未完成"), "open",
       uiText("Show workflow steps that are not ready",
              "只显示尚未就绪的工作流步骤")},
      {uiText("Blocked", "受阻"), "blocked",
       uiText("Show blocked workflow steps", "只显示受阻工作流步骤")},
      {uiText("Action", "待执行"), "action",
       uiText("Show actionable workflow steps", "只显示待执行工作流步骤")},
      {uiText("Risk", "风险"), "risk",
       uiText("Show workflow steps with risk text",
              "只显示有风险文字的工作流步骤")},
      {uiText("Evidence Gap", "证据缺口"), "gap",
       uiText("Show workflow steps with missing or stale evidence",
              "只显示存在缺失或过期证据的工作流步骤")},
      {uiText("Ready", "就绪"), "ready",
       uiText("Show ready workflow steps", "只显示已就绪工作流步骤")},
  };
  for (const auto &scopeCommand : workflowScopeCommands) {
    const QString scope = scopeCommand.scope;
    commands.append(CommandItem{
        uiText("Workflow Scope: ", "工作流范围：") + scopeCommand.label,
        scopeCommand.description +
            uiText(" without bus access", "；不访问总线"),
        style()->standardIcon(QStyle::SP_FileDialogDetailedView),
        [this, scope] {
          if (!workflow_->workflowScopeFilter) {
            return;
          }
          activateWorkspaceTab(overviewTabIndex_);
          const int index = workflow_->workflowScopeFilter->findData(scope);
          if (index >= 0) {
            workflow_->workflowScopeFilter->setCurrentIndex(index);
          }
          filterCommissioningWorkflow();
        },
        [this] { return workflow_->workflowTable && workflow_->workflowTable->rowCount() > 0; },
    });
  }
  commands.append(CommandItem{
      uiText("Review First Workflow Issue", "审阅首个工作流问题"),
      uiText("Select the first visible non-ready workflow row without running "
             "the step or accessing the bus",
             "选择首个可见的未就绪工作流行；不执行步骤也不访问总线"),
      style()->standardIcon(QStyle::SP_FileDialogDetailedView),
      [this] {
        activateWorkspaceTab(overviewTabIndex_);
        reviewFirstCommissioningWorkflowIssue();
      },
      [this] {
        return workflow_->workflowReviewButton && workflow_->workflowReviewButton->isEnabled();
      },
  });
  commands.append(CommandItem{
      uiText("Review Next Workflow Issue", "审阅下个工作流问题"),
      uiText("Select the next visible non-ready workflow row with wraparound "
             "without bus access",
             "选择下一个可见的未就绪工作流行并在末尾回绕；不访问总线"),
      style()->standardIcon(QStyle::SP_ArrowForward),
      [this] {
        activateWorkspaceTab(overviewTabIndex_);
        reviewNextCommissioningWorkflowIssue();
      },
      [this] {
        return workflow_->workflowReviewNextButton &&
               workflow_->workflowReviewNextButton->isEnabled();
      },
  });
  commands.append(CommandItem{
      uiText("Run Next Best Action", "执行下一最佳动作"),
      nextBestActionButton_
          ? nextBestActionButton_->toolTip()
          : uiText("Execute the current global navigator action",
                   "执行当前全局导航推荐动作"),
      style()->standardIcon(QStyle::SP_CommandLink),
      [this] { runNextBestAction(); },
      [this] {
        return nextBestActionButton_ && nextBestActionButton_->isEnabled();
      },
  });
  commands.append(CommandItem{
      uiText("Prepare Selected Slave Snapshot", "准备选中从站快照"),
      uiText("Read identity, Object Dictionary, PDO Map, ESI XML, and CiA 402 "
             "Watch evidence without state changes or writes",
// ── State Machine Commands ────────────────────────────────────────────
             "读取身份、对象字典、PDO 映射、ESI XML 和 CiA 402 Watch "
             "证据，不切换状态也不写入"),
      style()->standardIcon(QStyle::SP_FileDialogDetailedView),
      [this] { prepareSelectedSlaveSnapshot(); },
      [this] { return client_.isConnected() && selectedPosition() >= 0; },
  });
  commands.append(
      actionCommand("freeRunAction", uiText("Toggle Free Run", "切换自由运行"),
                    uiText("Start or stop cyclic process image telemetry",
                           "启动或停止周期过程映像遥测")));
  commands.append(actionCommand(
      "initAction", uiText("Set Selected Slave: INIT", "选中从站切换到 INIT"),
      uiText("Move the selected slave to INIT", "将选中从站切换到 INIT")));
  commands.append(actionCommand(
      "preOpAction",
      uiText("Set Selected Slave: PREOP", "选中从站切换到 PREOP"),
      uiText("Move the selected slave to PREOP", "将选中从站切换到 PREOP")));
  commands.append(actionCommand(
      "safeOpAction",
      uiText("Set Selected Slave: SAFEOP", "选中从站切换到 SAFEOP"),
      uiText("Move the selected slave to SAFEOP", "将选中从站切换到 SAFEOP")));
  commands.append(actionCommand(
      "opAction", uiText("Set Selected Slave: OP", "选中从站切换到 OP"),
      uiText("Move the selected slave to OP", "将选中从站切换到 OP")));
  commands.append(actionCommand(
      "allInitAction", uiText("Set All Slaves: INIT", "全部从站切换到 INIT"),
      uiText("Request INIT on every detected slave",
             "请求全部已检测从站进入 INIT")));
  commands.append(actionCommand(
      "allPreOpAction", uiText("Set All Slaves: PREOP", "全部从站切换到 PREOP"),
      uiText("Request PREOP on every detected slave",
             "请求全部已检测从站进入 PREOP")));
  commands.append(
      actionCommand("allSafeOpAction",
                    uiText("Set All Slaves: SAFEOP", "全部从站切换到 SAFEOP"),
                    uiText("Request SAFEOP on every detected slave",
                           "请求全部已检测从站进入 SAFEOP")));
// ── Command Filtering ───────────────────────────────────────────────
// Filter commands by search text and update the results list
  commands.append(actionCommand(
      "allOpAction", uiText("Set All Slaves: OP", "全部从站切换到 OP"),
      uiText("Request OP on every detected slave",
             "请求全部已检测从站进入 OP")));
  commands.append(CommandItem{
      uiText("Read Current SDO", "读取当前 SDO"),
      uiText("Upload %1 using the current SDO fields",
             "使用当前 SDO 字段读取 %1")
          .arg(currentSdoLabel()),
      style()->standardIcon(QStyle::SP_ArrowDown),
      [this] {
        if (auto *button = findChild<QPushButton *>("readSdo");
            button && button->isEnabled()) {
// ── SDO Commands ──────────────────────────────────────────────────────
          button->click();
        }
      },
      [this, hasCurrentSdo] {
        auto *button = findChild<QPushButton *>("readSdo");
        return button && button->isEnabled() && hasCurrentSdo();
      },
  });
  commands.append(buttonCommand(
      "writeSdo", uiText("Write Current SDO", "写入当前 SDO"),
      uiText(
          "Download the current write value with validation and confirmation",
          "通过校验和确认后写入当前数值"),
      style()->standardIcon(QStyle::SP_ArrowUp)));
  commands.append(CommandItem{
      uiText("Use Read Value for Write", "使用读回值作为写入值"),
      uiText("Copy the current read-back value into the write field",
             "把当前读回值复制到写入框"),
      style()->standardIcon(QStyle::SP_ArrowForward),
      [this] { useReadSdoValueForWrite(); },
      [this] {
        return sdoInspector_->useSdoValueButton && sdoInspector_->useSdoValueButton->isEnabled() &&
    // Get current search text
               sdoInspector_->sdoValue && !sdoInspector_->sdoValue->text().trimmed().isEmpty();
      },
  });
  commands.append(CommandItem{
      uiText("Use Best Evidence for Write", "使用最佳证据作为写入值"),
      uiText("Copy the best local Read/Watch/OD/Startup/Bookmark/Target Trail "
             "evidence into the write field without bus access",
             "把最佳本地读回、Watch、OD、Startup、书签或目标轨迹证据复制到写入"
             "框，不访问总线"),
      style()->standardIcon(QStyle::SP_ArrowForward),
      [this] { usePreferredSdoEvidenceForWrite(); },
      [this] {
        return selectedSdoWritable_ &&
               !currentSdoPreferredEvidenceValue().isEmpty();
      },
  });
  commands.append(CommandItem{
      uiText("Pick Evidence for Write", "选择证据作为写入值"),
      uiText("Choose one local evidence value for the write field without bus "
             "access",
             "选择一个本地证据值填入写入框，不访问总线"),
      style()->standardIcon(QStyle::SP_FileDialogContentsView),
      [this] { pickSdoEvidenceForWrite(); },
      [this] {
        return selectedSdoWritable_ &&
               !currentSdoEvidenceCandidates().isEmpty();
      },
  });
  commands.append(CommandItem{
      uiText("Add Current SDO to Watch", "添加当前 SDO 到监视"),
      uiText("Track the selected slave/index/subindex",
             "监视当前选中的从站、索引和子项"),
      style()->standardIcon(QStyle::SP_FileDialogNewFolder),
      [this] { addCurrentSdoToWatch(); },
      [hasCurrentSdo] { return hasCurrentSdo(); },
  });
  commands.append(CommandItem{
      uiText("Review Current SDO Evidence Delta", "审阅当前 SDO 证据差异"),
      uiText("Open the local evidence behind the current Evidence Set conflict "
             "or Write Delta without bus access",
             "打开当前证据集冲突或写入差异背后的本地证据，不访问总线"),
      style()->standardIcon(QStyle::SP_MessageBoxWarning),
      [this] { reviewCurrentSdoWriteDelta(); },
      [this] { return currentSdoWriteDeltaReviewAvailable(); },
  });
  commands.append(CommandItem{
      uiText("Copy Current SDO Evidence Digest", "复制当前 SDO 证据摘要"),
      uiText("Copy target, Evidence Set, Write Delta, and local evidence links "
             "without bus access",
             "复制目标、证据集、写入差异和本地证据链接，不访问总线"),
      style()->standardIcon(QStyle::SP_FileDialogContentsView),
      [this] { copyCurrentSdoEvidenceDigest(); },
      [hasCurrentSdo] { return hasCurrentSdo(); },
  });
  commands.append(CommandItem{
      uiText("Open Selected Object Row Evidence", "打开选中对象本行证据"),
      uiText("Run the same local action as double-click or Alt+Enter on the "
             "Selected Object panel row",
             "执行与双击或 Alt+Enter 选中对象面板行相同的本地动作"),
      style()->standardIcon(QStyle::SP_FileDialogDetailedView),
      [this] {
        openSdoTargetPanelRow(sdoInspector_->sdoTargetTable ? sdoInspector_->sdoTargetTable->currentRow()
                                              : -1);
      },
      hasSelectedObjectPanelRow,
  });
  commands.append(CommandItem{
      uiText("Copy Selected Object Row Evidence", "复制选中对象本行证据"),
      uiText("Copy the selected review row, current SDO target, recommended "
             "local action, and safety boundary without bus access",
             "复制当前复核行、SDO 目标、推荐本地动作和安全边界，不访问总线"),
      style()->standardIcon(QStyle::SP_FileDialogContentsView),
      [this] {
        copySdoTargetPanelRowDigest(
            sdoInspector_->sdoTargetTable ? sdoInspector_->sdoTargetTable->currentRow() : -1);
      },
      hasSelectedObjectPanelRow,
  });
// ── Watch Commands ────────────────────────────────────────────────────
  commands.append(CommandItem{
      uiText("Restore SDO Target Trail Row", "恢复 SDO 目标轨迹行"),
      uiText("Refill the SDO fields from the selected local target trail row",
             "从所选本地目标轨迹行回填 SDO 字段"),
      style()->standardIcon(QStyle::SP_ArrowForward),
      [this] {
        restoreSdoTargetTrailRow(
            sdoTargetTrailTable_ ? sdoTargetTrailTable_->currentRow() : -1);
      },
      [hasSdoTargetTrailRow] { return hasSdoTargetTrailRow(); },
  });
  commands.append(CommandItem{
      uiText("Add SDO Target Trail Row to Watch",
             "将 SDO 目标轨迹行加入 Watch"),
      uiText("Restore the selected target trail row locally and add it to "
             "Watch without immediate reads",
             "本地恢复所选目标轨迹行并加入 Watch，不立即读取"),
      style()->standardIcon(QStyle::SP_FileDialogNewFolder),
      [this] { addSdoTargetTrailRowToWatch(); },
      [hasSdoTargetTrailRow] { return hasSdoTargetTrailRow(); },
  });
  commands.append(CommandItem{
      uiText("Bookmark SDO Target Trail Row", "收藏 SDO 目标轨迹行"),
      uiText("Save the selected target trail row as a project Object Bookmark "
             "without bus access",
             "把所选目标轨迹行保存为工程对象书签，不访问总线"),
      style()->standardIcon(QStyle::SP_FileDialogInfoView),
      [this] { bookmarkSdoTargetTrailRow(); },
      [hasSdoTargetTrailRow] { return hasSdoTargetTrailRow(); },
  });
  commands.append(CommandItem{
      uiText("Create Startup SDO from Target Trail Row",
             "从目标轨迹行创建 Startup SDO"),
      uiText("Use the selected trail write value, or its last value fallback, "
             "to create a local Startup SDO candidate",
             "使用所选轨迹写入值，或最后值兜底，创建本地 Startup SDO 候选"),
      style()->standardIcon(QStyle::SP_DialogApplyButton),
      [this] { addSdoTargetTrailRowToStartup(); },
      [this, hasSdoTargetTrailRow] {
        return hasSdoTargetTrailRow() &&
               sdoTargetTrailRowCanCreateStartup(
                   sdoTargetTrailTable_->currentRow());
      },
  });
  commands.append(CommandItem{
      uiText("Open Current SDO Watch Evidence", "打开当前 SDO Watch 证据"),
      uiText("Open the matching Watch row without reading the bus",
             "打开匹配的 Watch 行，不读取总线"),
      style()->standardIcon(QStyle::SP_FileDialogDetailedView),
      [this] { openCurrentSdoWatchLink(); },
      [this] { return currentSdoWatchRow() >= 0; },
  });
  commands.append(CommandItem{
      uiText("Open Current SDO Startup Evidence", "打开当前 SDO Startup 证据"),
      uiText("Open the matching Startup SDO row without writing the bus",
             "打开匹配的 Startup SDO 行，不写入总线"),
      style()->standardIcon(QStyle::SP_FileDialogDetailedView),
      [this] { openCurrentSdoStartupLink(); },
      [this] { return currentSdoStartupRow() >= 0; },
  });
  commands.append(CommandItem{
      uiText("Open Current SDO Bookmark", "打开当前 SDO 书签"),
      uiText("Open the matching Object Bookmark without bus access",
             "打开匹配的对象书签，不访问总线"),
      style()->standardIcon(QStyle::SP_FileDialogDetailedView),
      [this] { openCurrentSdoBookmarkLink(); },
      [this] { return currentSdoBookmarkRow() >= 0; },
  });
  commands.append(CommandItem{
      uiText("Open Current SDO Target Trail", "打开当前 SDO 目标轨迹"),
      uiText("Open the matching Target Trail row without bus access",
             "打开匹配的目标轨迹行，不访问总线"),
      style()->standardIcon(QStyle::SP_FileDialogDetailedView),
      [this] { openCurrentSdoTargetTrailLink(); },
      [this] { return currentSdoTargetTrailRow() >= 0; },
  });
  commands.append(CommandItem{
      uiText("Bookmark Current SDO", "收藏当前 SDO"),
      uiText("Save the current SDO target as a project object bookmark without "
             "bus access",
             "把当前 SDO 目标保存为工程对象书签，不访问总线"),
      style()->standardIcon(QStyle::SP_FileDialogInfoView),
      [this] { addCurrentSdoBookmark(); },
      [hasCurrentSdo] { return hasCurrentSdo(); },
  });
  commands.append(CommandItem{
      uiText("Bookmark Selected OD Rows", "收藏所选 OD 行"),
      uiText("Save selected Object Dictionary rows as project bookmarks",
             "把选中的对象字典行保存为工程对象书签"),
      style()->standardIcon(QStyle::SP_FileDialogInfoView),
      [this] { addSelectedDictionaryRowsToBookmarks(); },
      [this] {
        return selectedPosition() >= 0 &&
               loadedSdoPosition_ == selectedPosition() && sdo_->sdoTable &&
               !selectedDictionaryRows().isEmpty();
      },
  });
  commands.append(CommandItem{
      uiText("Create Startup SDO from OD Evidence",
             "从 OD 证据创建 Startup SDO"),
// ── Free Run Commands ─────────────────────────────────────────────────
      uiText(
          "Create or update Startup SDO rows from selected Object "
          "Dictionary rows that have Last Value evidence without bus "
          "access",
          "使用选中且已有 Last Value 证据的对象字典行创建或更新 Startup SDO，"
          "不访问总线"),
      style()->standardIcon(QStyle::SP_DialogApplyButton),
      [this] { addSelectedDictionaryEvidenceToStartupSdo(); },
      [this] {
        if (selectedPosition() < 0 ||
            loadedSdoPosition_ != selectedPosition() || !sdo_->sdoTable) {
          return false;
        }
        return sdoDictionaryRowsContainValue(sdo_->sdoTable,
                                             selectedDictionaryRows());
      },
  });
  commands.append(CommandItem{
      uiText("Watch Selected Object Bookmarks", "监视所选对象书签"),
      uiText("Add selected project object bookmarks to Watch without immediate "
             "reads",
             "把选中的工程对象书签加入 Watch，不立即读取"),
      style()->standardIcon(QStyle::SP_FileDialogNewFolder),
      [this] { addSelectedObjectBookmarksToWatch(); },
      [this] {
        return bookmark_->objectBookmarkTable && !selectedObjectBookmarkRows().isEmpty();
      },
  });
  commands.append(CommandItem{
      uiText("Create Startup SDO from Object Bookmarks",
             "从对象书签创建 Startup SDO"),
      uiText("Create or update Startup SDO rows from selected bookmarks using "
             "saved values without bus access",
             "使用所选书签保存的值创建或更新 Startup SDO 行，不访问总线"),
      style()->standardIcon(QStyle::SP_DialogApplyButton),
      [this] { addSelectedObjectBookmarksToStartupSdo(); },
      [this] {
        return bookmark_->objectBookmarkTable && !selectedObjectBookmarkRows().isEmpty();
      },
  });
  commands.append(CommandItem{
      uiText("Add Visible OD Rows to Watch", "可见对象加入监视"),
      uiText("Add the currently filtered Object Dictionary rows to Watch "
             "without immediate reads",
             "把当前过滤后可见的对象字典行加入 Watch，不立即读取"),
      style()->standardIcon(QStyle::SP_FileDialogNewFolder),
      [this] { addVisibleDictionaryRowsToWatch(); },
      [this] {
        if (selectedPosition() < 0 ||
            loadedSdoPosition_ != selectedPosition() || !sdo_->sdoTable) {
          return false;
        }
        for (int row = 0; row < sdo_->sdoTable->rowCount(); ++row) {
          if (!sdo_->sdoTable->isRowHidden(row)) {
            return true;
          }
        }
        return false;
      },
  });
  commands.append(CommandItem{
      uiText("Read Visible Object Dictionary Rows", "读取可见对象字典行"),
      uiText("Read every currently visible Object Dictionary row; large "
             "batches ask for confirmation",
             "读取当前过滤后可见的所有对象字典行；大批量读取前会要求确认"),
      style()->standardIcon(QStyle::SP_BrowserReload),
      [this] { readVisibleDictionaryRows(); },
      [this] {
        if (!client_.isConnected() || selectedPosition() < 0 ||
            loadedSdoPosition_ != selectedPosition() || !sdo_->sdoTable) {
          return false;
        }
        for (int row = 0; row < sdo_->sdoTable->rowCount(); ++row) {
          if (!sdo_->sdoTable->isRowHidden(row)) {
            return true;
          }
        }
        return false;
      },
  });
  commands.append(CommandItem{
      uiText("Review Failed Object Evidence", "审阅失败对象证据"),
      uiText("Open Object Dictionary, filter failed SDO evidence, and select "
// ── Command Execution ───────────────────────────────────────────────
             "the first failed object",
// Execute the selected command and close the palette
             "打开对象字典，过滤失败 SDO 证据，并选中第一条失败对象"),
      style()->standardIcon(QStyle::SP_MessageBoxWarning),
      [this] { focusFailedSdoEvidence(); },
      canReviewFailedSdoEvidence,
  });
  commands.append(CommandItem{
      uiText("Retry Failed Object Evidence", "重试失败对象证据"),
      uiText("Read only Object Dictionary rows whose latest SDO evidence "
             "failed",
             "只读取最新 SDO 证据失败的对象字典行"),
      style()->standardIcon(QStyle::SP_BrowserReload),
      [this] { readFailedDictionaryRows(); },
      canRetryFailedSdoEvidence,
  });
  commands.append(CommandItem{
      uiText("Add CiA 402 Watch Preset", "添加 CiA 402 监视预设"),
// ── I/O Variable Commands ─────────────────────────────────────────────
      uiText("Add common drive objects to Watch for the selected slave",
             "把常见驱动对象加入选中从站的监视列表"),
      style()->standardIcon(QStyle::SP_DialogApplyButton),
      [this] { addCia402WatchPreset(); },
      [this] { return selectedPosition() >= 0; },
  });
  const QVector<QPair<QString, QString>> sdoFilterCommands = {
      {uiText("All Objects", "全部对象"), QString()},
      {uiText("Writable Objects", "可写对象"), "tag:writable"},
      {uiText("Readable Objects", "可读对象"), "tag:readable"},
      {uiText("CiA 402 Objects", "CiA 402 对象"), "tag:cia402"},
      {uiText("Identity Objects", "身份对象"), "tag:identity"},
      {uiText("PDO Objects", "PDO 对象"), "tag:pdo"},
      {uiText("Error Objects", "错误对象"), "tag:error"},
      {uiText("Objects With Evidence", "有证据对象"), "tag:evidence"},
      {uiText("Failed Evidence Objects", "失败证据对象"), "tag:failed"},
  };
  for (const auto &filterCommand : sdoFilterCommands) {
    commands.append(CommandItem{
        uiText("Filter Object Dictionary: ", "过滤对象字典：") +
            filterCommand.first,
        filterCommand.second.isEmpty()
            ? uiText("Clear Object Dictionary filter", "清除对象字典过滤")
            : uiText("Apply semantic Object Dictionary filter %1",
                     "应用对象字典语义过滤 %1")
                  .arg(filterCommand.second),
        style()->standardIcon(QStyle::SP_FileDialogDetailedView),
        [this, query = filterCommand.second] { setSdoFilterPreset(query); },
        [this] {
          return selectedPosition() >= 0 &&
                 loadedSdoPosition_ == selectedPosition() && sdo_->sdoTable &&
                 sdo_->sdoTable->rowCount() > 0;
        },
    });
  }
  const QVector<QPair<QString, QString>> cia402Controlwords = {
      {uiText("Shutdown", "Shutdown"), "0x0006"},
      {uiText("Switch On", "Switch On"), "0x0007"},
      {uiText("Enable Operation", "Enable Operation"), "0x000f"},
      {uiText("Quick Stop", "Quick Stop"), "0x0002"},
      {uiText("Fault Reset", "Fault Reset"), "0x0080"},
  };
  QString recommendedLabel;
  QString recommendedValue;
  QString recommendedReason;
  const bool hasRecommendedCia402 = recommendedCia402Controlword(
      &recommendedLabel, &recommendedValue, &recommendedReason);
  commands.append(CommandItem{
      hasRecommendedCia402
          ? uiText("Recommended CiA 402: ", "推荐 CiA 402：") + recommendedLabel
          : uiText("Recommended CiA 402 Controlword", "推荐 CiA 402 控制字"),
      hasRecommendedCia402
          ? uiText("Based on statusword: %1. Write 0x6040:0x00 = %2",
                   "基于状态字：%1。写入 0x6040:0x00 = %2")
                .arg(recommendedReason, recommendedValue)
          : uiText("Add the CiA 402 Watch preset and refresh statusword first",
                   "先添加 CiA 402 监视预设并刷新状态字"),
      style()->standardIcon(QStyle::SP_DialogApplyButton),
      [this, recommendedLabel, recommendedValue] {
        prepareCia402Controlword(recommendedLabel, recommendedValue);
      },
      [this, hasRecommendedCia402] {
        return hasRecommendedCia402 && client_.isConnected() &&
               selectedPosition() >= 0;
      },
  });
  for (const auto &command : cia402Controlwords) {
    commands.append(CommandItem{
        uiText("CiA 402 Controlword: ", "CiA 402 控制字：") + command.first,
        uiText("Prepare and write 0x6040:0x00 uint16 = %1 with confirmation",
               "准备并确认写入 0x6040:0x00 uint16 = %1")
            .arg(command.second),
        style()->standardIcon(QStyle::SP_CommandLink),
        [this, command] {
          prepareCia402Controlword(command.first, command.second);
        },
        [this] { return client_.isConnected() && selectedPosition() >= 0; },
    });
  }
  commands.append(CommandItem{
      uiText("Add Current SDO to Startup", "添加当前 SDO 到启动项"),
      uiText("Create a Startup SDO row from the current object and write value",
             "用当前对象和写入值创建 Startup SDO 行"),
      style()->standardIcon(QStyle::SP_FileDialogNewFolder),
      [this] { addStartupSdo(); },
      [this, hasCurrentSdo] {
        auto *button = findChild<QPushButton *>("addStartupSdo");
        return button && button->isEnabled() && hasCurrentSdo();
      },
  });
  commands.append(CommandItem{
      uiText("Copy Current SDO Address", "复制当前 SDO 地址"),
      uiText("Copy %1 to the clipboard", "复制 %1 到剪贴板")
          .arg(currentSdoLabel()),
      style()->standardIcon(QStyle::SP_FileDialogContentsView),
      [this, currentSdoLabel] {
        QApplication::clipboard()->setText(currentSdoLabel()); // copy to system clipboard
        updateDiagnostics("Info", "SDO",
                          "Copied current SDO address: " + currentSdoLabel());
      },
// ── Consistency Commands ──────────────────────────────────────────────
      [hasCurrentSdo] { return hasCurrentSdo(); },
  });
  commands.append(CommandItem{
      uiText("Copy Current SDO and Value", "复制当前 SDO 和值"),
      uiText("Copy the current object with type and read/write value",
             "复制当前对象、类型和读回/写入值"),
      style()->standardIcon(QStyle::SP_FileDialogContentsView),
      [this, currentSdoValueLabel] {
        QApplication::clipboard()->setText(currentSdoValueLabel()); // copy to system clipboard
        updateDiagnostics("Info", "SDO",
                          "Copied current SDO value: " +
                              currentSdoValueLabel());
      },
      [hasCurrentSdoValue] { return hasCurrentSdoValue(); },
  });
  commands.append(CommandItem{
      uiText("Refresh Watch List", "刷新监视列表"),
      uiText("Read every SDO item in the Watch tab",
             "读取监视页中的每个 SDO 项"),
      style()->standardIcon(QStyle::SP_BrowserReload),
      [this] { refreshWatchList(); },
      [this] {
        return client_.isConnected() && watch_->watchTable &&
               watch_->watchTable->rowCount() > 0;
      },
  });
  commands.append(CommandItem{
      uiText("Capture Watch Baseline", "捕获 Watch 基线"),
      uiText("Store current Watch values for drift comparison",
             "保存当前 Watch 值用于偏差比较"),
      style()->standardIcon(QStyle::SP_DialogSaveButton),
      [this] { captureWatchBaseline(); },
      [this] { return watch_->watchTable && watch_->watchTable->rowCount() > 0; },
  });
  commands.append(CommandItem{
      uiText("Clear Watch Baseline", "清除 Watch 基线"),
      uiText("Clear saved baselines and delta results",
             "清除已保存基线和偏差结果"),
      style()->standardIcon(QStyle::SP_TrashIcon),
      [this] { clearWatchBaseline(); },
      [this] { return watch_->watchTable && watch_->watchTable->rowCount() > 0; },
  });
  struct WatchScopeCommand {
    QString label;
    QString scope;
    QString description;
  };
  const QVector<WatchScopeCommand> watchScopeCommands = {
      {uiText("All", "全部"), "all",
       uiText("Show every Watch row", "显示全部 Watch 行")},
      {uiText("Selected Slave", "当前从站"), "selected",
       uiText("Show only Watch rows for the selected slave",
              "只显示当前选中从站的 Watch 行")},
      {uiText("Changed", "变化项"), "changed",
       uiText("Show Watch rows whose value changed during the session",
              "只显示本会话中值发生变化的 Watch 行")},
      {uiText("Baseline Drift", "基线偏离"), "baselineDrift",
       uiText("Show Watch rows whose value differs from captured baseline",
              "只显示和已捕获基线不一致的 Watch 行")},
      {uiText("Startup Diff", "启动不一致"), "startupDiff",
       uiText("Show Watch rows whose value differs from Startup SDO",
              "只显示和 Startup SDO 期望值不一致的 Watch 行")},
      {uiText("Missing Value", "缺失值"), "missingValue",
       uiText("Show Watch rows that have no live value yet",
              "只显示还没有实时值的 Watch 行")},
      {"CiA 402", "cia402",
       uiText("Show common CiA 402 drive Watch rows",
              "只显示常见 CiA 402 驱动 Watch 行")},
  };
  for (const auto &scopeCommand : watchScopeCommands) {
    commands.append(CommandItem{
        uiText("Watch Scope: ", "Watch 范围：") + scopeCommand.label,
        scopeCommand.description,
        style()->standardIcon(QStyle::SP_FileDialogDetailedView),
        [this, scope = scopeCommand.scope, label = scopeCommand.label] {
          if (!watch_->watchScopeFilter) {
            return;
          }
          const int index = watch_->watchScopeFilter->findData(scope);
          if (index >= 0) {
            watch_->watchScopeFilter->setCurrentIndex(index);
          }
          activateWorkspaceTab(watchTabIndex_);
          filterWatchTable();
          updateDiagnostics(
              "Info", "Watch",
              uiText("Watch scope set to %1", "Watch 范围已切换为 %1")
                  .arg(label));
        },
        [this, scope = scopeCommand.scope] {
          if (!watch_->watchTable || watch_->watchTable->rowCount() <= 0) {
            return false;
          }
          return scope != "selected" || selectedPosition() >= 0;
        },
    });
  }
  commands.append(CommandItem{
      uiText("Open I/O Variables", "打开 I/O 变量"),
      uiText("Review PDO, process-image, Watch, and Startup evidence in one "
// ── Diagnostics Commands ──────────────────────────────────────────────
             "signal table",
             "在一张信号表中复核 PDO、过程映像、Watch 和 Startup 证据"),
      style()->standardIcon(QStyle::SP_FileDialogDetailedView),
      [this] {
        updateIoVariableTable();
        activateWorkspaceTab(ioVariableTabIndex_);
      },
      [this] { return ioVariableTabIndex_ >= 0; },
  });
  struct IoScopeCommand {
    QString label;
    QString scope;
    QString description;
  };
  const QVector<IoScopeCommand> ioScopeCommands = {
      {uiText("All", "全部"), "all",
       uiText("Show every I/O variable row", "显示全部 I/O 变量")},
      {uiText("Selected Slave", "当前从站"), "selected",
       uiText("Show variables for the selected slave", "只显示当前从站变量")},
      {uiText("Process Image", "过程映像"), "process",
       uiText("Show variables with Free Run process-image evidence",
              "显示已有 Free Run 过程映像证据的变量")},
      {uiText("Startup Diff", "启动不一致"), "startupDiff",
       uiText("Show variables whose Startup expectation differs from live "
              "evidence",
              "显示 Startup 期望和实时证据不一致的变量")},
      {uiText("Missing Value", "缺失值"), "missingValue",
       uiText("Show variables without live or Watch value evidence",
              "显示缺少实时或 Watch 值证据的变量")},
      {"CiA 402", "cia402",
       uiText("Show common CiA 402 drive variables",
              "显示常见 CiA 402 驱动变量")},
      {uiText("Changed", "变化项"), "changed",
       uiText("Show variables whose Free Run or Watch evidence changed",
              "显示 Free Run 或 Watch 证据发生变化的变量")},
      {uiText("PLC Issues", "PLC 交接问题"), "plcIssues",
       uiText("Show variables whose PLC handoff symbol is not ready",
              "显示 PLC 交接符号尚未就绪的变量")},
  };
  for (const auto &scopeCommand : ioScopeCommands) {
    commands.append(CommandItem{
        uiText("I/O Variables Scope: ", "I/O 变量范围：") + scopeCommand.label,
        scopeCommand.description,
        style()->standardIcon(QStyle::SP_FileDialogDetailedView),
        [this, scope = scopeCommand.scope, label = scopeCommand.label] {
          if (!ioVar_->ioVariableScopeFilter) {
            return;
          }
          const int index = ioVar_->ioVariableScopeFilter->findData(scope);
          if (index >= 0) {
            ioVar_->ioVariableScopeFilter->setCurrentIndex(index);
          }
          updateIoVariableTable();
          activateWorkspaceTab(ioVariableTabIndex_);
          updateDiagnostics(
              "Info", "I/O Variables",
              uiText("I/O Variables scope set to %1", "I/O 变量范围已切换为 %1")
                  .arg(label));
        },
        [this, scope = scopeCommand.scope] {
          if (!ioVar_->ioVariableTable || ioVar_->ioVariableTable->rowCount() <= 0) {
            return false;
          }
          return scope != "selected" || selectedPosition() >= 0;
        },
    });
  }
  commands.append(CommandItem{
      uiText("Watch Selected I/O Variables", "监视所选 I/O 变量"),
      uiText("Add selected I/O variable rows to Watch without immediate reads",
             "把选中的 I/O 变量加入 Watch，不立即读取"),
      style()->standardIcon(QStyle::SP_FileDialogNewFolder),
      [this] { addSelectedIoVariablesToWatch(); },
      [this] {
        return ioVar_->ioVariableTable && ioVar_->ioVariableTable->currentRow() >= 0;
      },
  });
  commands.append(CommandItem{
      uiText("Watch Visible I/O Variables", "监视可见 I/O 变量"),
      uiText("Add every visible I/O variable row to Watch without immediate "
             "reads",
             "把所有可见 I/O 变量加入 Watch，不立即读取"),
      style()->standardIcon(QStyle::SP_FileDialogNewFolder),
      [this] { addVisibleIoVariablesToWatch(); },
      [this] {
        if (!ioVar_->ioVariableTable) {
          return false;
        }
        // Iterate all rows and apply active filter predicates
        for (int row = 0; row < ioVar_->ioVariableTable->rowCount(); ++row) {
          if (!ioVar_->ioVariableTable->isRowHidden(row)) {
            return true;
          }
        }
        return false;
      },
  });
  commands.append(CommandItem{
      uiText("Create Startup SDO from Selected I/O Variables",
             "从所选 I/O 变量创建 Startup SDO"),
// ── Project Commands ──────────────────────────────────────────────────
      uiText("Create or update Startup SDO rows from selected I/O variable "
             "Watch or Raw evidence without bus access",
             "使用所选 I/O 变量的 Watch 或 Raw 证据创建或更新 Startup SDO，"
             "不访问总线"),
      style()->standardIcon(QStyle::SP_DialogApplyButton),
      [this] { addSelectedIoVariablesToStartupSdo(); },
      [this] {
        const QVector<int> rows = selectedIoVariableRows(true);
        return ioVariableTableRowsContainValue(ioVar_->ioVariableTable, rows);
      },
  });
  commands.append(CommandItem{
      uiText("Create Startup SDO from Visible I/O Variables",
             "从可见 I/O 变量创建 Startup SDO"),
      uiText("Create or update Startup SDO rows from every visible I/O "
             "variable with Watch or Raw evidence without bus access",
             "使用所有可见且带 Watch 或 Raw 证据的 I/O 变量创建或更新 Startup "
             "SDO，不访问总线"),
      style()->standardIcon(QStyle::SP_DialogApplyButton),
      [this] { addVisibleIoVariablesToStartupSdo(); },
      [this] {
        const QVector<int> rows = visibleIoVariableRows();
        return ioVariableTableRowsContainValue(ioVar_->ioVariableTable, rows);
      },
  });
  commands.append(CommandItem{
      uiText("Edit I/O Variable Alias", "编辑 I/O 变量别名"),
      uiText("Assign project-local alias, tags, and notes to the selected I/O "
             "variable",
             "给选中的 I/O 变量设置工程内别名、标签和备注"),
      style()->standardIcon(QStyle::SP_FileDialogInfoView),
      [this] { editSelectedIoVariableMetadata(); },
      [this] {
        return ioVar_->ioVariableTable && ioVar_->ioVariableTable->currentRow() >= 0;
      },
  });
  commands.append(CommandItem{
      uiText("Bulk Name I/O Variables", "批量命名 I/O 变量"),
      uiText("Generate project-local aliases and tags for selected or visible "
             "signals without bus access",
             "为所选或可见信号批量生成工程别名和标签，不访问总线"),
      style()->standardIcon(QStyle::SP_FileDialogDetailedView),
      [this] { bulkNameIoVariables(); },
      [this] { return ioVar_->ioVariableTable && ioVar_->ioVariableTable->rowCount() > 0; },
  });
  commands.append(CommandItem{
      uiText("Review PLC Handoff Issues", "审阅 PLC 交接问题"),
      uiText("Show visible I/O variables with missing aliases, automatic "
             "names, missing tags, or duplicate PLC symbols",
             "显示可见 I/O 变量中缺少 Alias、自动命名、缺少 Tags 或 PLC "
             "符号重复的行"),
      style()->standardIcon(QStyle::SP_FileDialogDetailedView),
      [this] { reviewPlcHandoffIssues(); },
      [this] { return ioVar_->ioVariableTable && !visibleIoVariableRows().isEmpty(); },
  });
  commands.append(CommandItem{
      uiText("Copy Selected PLC Declarations", "复制所选 PLC 声明"),
      uiText("Copy selected visible I/O variables as an IEC VAR_GLOBAL "
             "declaration block",
             "把选中的可见 I/O 变量复制为 IEC VAR_GLOBAL 声明块"),
      style()->standardIcon(QStyle::SP_FileDialogContentsView),
      [this] { copyIoVariablePlcDeclarations(true); },
      [this] {
        return ioVar_->ioVariableTable && !selectedIoVariableRows(true).isEmpty();
      },
  });
  commands.append(CommandItem{
      uiText("Copy Visible PLC Declarations", "复制可见 PLC 声明"),
      uiText("Copy every visible I/O variable as an IEC VAR_GLOBAL declaration "
             "block",
             "把所有可见 I/O 变量复制为 IEC VAR_GLOBAL 声明块"),
      style()->standardIcon(QStyle::SP_FileDialogContentsView),
      [this] { copyIoVariablePlcDeclarations(false); },
      [this] { return ioVar_->ioVariableTable && !visibleIoVariableRows().isEmpty(); },
  });
  commands.append(CommandItem{
      uiText("Export PLC Declarations ST", "导出 PLC 声明 ST"),
      uiText("Write visible I/O variables as an IEC Structured Text "
// ── Command Categories ──────────────────────────────────────────────
             "VAR_GLOBAL declaration file",
             "把可见 I/O 变量写成 IEC Structured Text VAR_GLOBAL 声明文件"),
      style()->standardIcon(QStyle::SP_DialogSaveButton),
      [this] { exportIoVariablesPlcDeclarationsSt(); },
      [this] { return ioVar_->ioVariableTable && !visibleIoVariableRows().isEmpty(); },
  });
  commands.append(CommandItem{
      uiText("Open Consistency Check", "打开一致性检查"),
      uiText("Review online/offline differences across topology, Startup SDO, "
             "Watch, PDO/Free Run, and I/O variable metadata",
             "审阅拓扑、Startup SDO、Watch、PDO/Free Run 和 I/O "
             "变量元数据的 Online/Offline 差异"),
      style()->standardIcon(QStyle::SP_FileDialogDetailedView),
      [this] { openConsistencyView(); },
      [this] { return consistencyTabIndex_ >= 0; },
  });
  commands.append(CommandItem{
      uiText("Open Selected Consistency Evidence", "打开所选一致性证据"),
      uiText("Navigate from the selected Consistency row to Startup, Watch, "
             "I/O Variables, or State Machine without bus access",
             "从所选一致性行跳到 Startup、Watch、I/O 变量或状态机；不访问总线"),
      style()->standardIcon(QStyle::SP_FileDialogDetailedView),
// ── Command Filtering + Search Logic ──────────────────────────────────
      [this] { focusEvidenceFromConsistency(); },
      [this] {
        return consistency_->consistencyTable && consistency_->consistencyTable->currentRow() >= 0;
      },
  });
  commands.append(CommandItem{
      uiText("Open Session Brief Evidence", "打开会话简报证据"),
      uiText("Open the local evidence target for the selected Overview "
             "Session Brief row without bus access",
             "打开总览会话简报当前行对应的本地证据；不访问总线"),
      style()->standardIcon(QStyle::SP_FileDialogDetailedView),
      [this] {
        activateWorkspaceTab(overviewTabIndex_);
        openSessionBriefRow(
            session_->sessionBriefTable ? session_->sessionBriefTable->currentRow() : -1);
      },
      [this] {
        return session_->sessionBriefTable && session_->sessionBriefTable->currentRow() >= 0;
      },
  });
  commands.append(CommandItem{
      uiText("Copy Session Brief Row Evidence", "复制会话简报本行证据"),
      uiText("Copy the selected Overview Session Brief row summary and local "
             "boundary to the clipboard without bus access",
             "把总览会话简报当前行摘要和本地边界复制到剪贴板；不访问总线"),
      style()->standardIcon(QStyle::SP_FileDialogContentsView),
      [this] {
        activateWorkspaceTab(overviewTabIndex_);
        copySessionBriefRowDigest(
            session_->sessionBriefTable ? session_->sessionBriefTable->currentRow() : -1);
      },
      [this] {
        return session_->sessionBriefTable && session_->sessionBriefTable->currentRow() >= 0;
      },
  });
  commands.append(CommandItem{
      uiText("Open Slave Matrix Evidence", "打开从站矩阵证据"),
      uiText("Route the selected Overview matrix row to its best loaded local "
             "evidence table without bus access",
             "把总览矩阵当前行路由到最相关的已加载本地证据表；不访问总线"),
      style()->standardIcon(QStyle::SP_FileDialogDetailedView),
      [this] {
        activateWorkspaceTab(overviewTabIndex_);
        openSlaveEvidenceMatrixRow(slaveEvidence_->slaveEvidenceMatrixTable
                                       ? slaveEvidence_->slaveEvidenceMatrixTable->currentRow()
                                       : -1);
      },
      [this] {
        return slaveEvidence_->slaveEvidenceMatrixTable &&
               slaveEvidence_->slaveEvidenceMatrixTable->currentRow() >= 0;
      },
  });
  commands.append(CommandItem{
      uiText("Copy Slave Matrix Row Evidence", "复制从站矩阵本行证据"),
      uiText("Copy the selected Overview matrix row summary and local boundary "
             "to the clipboard without bus access",
             "把总览矩阵当前行摘要和本地边界复制到剪贴板；不访问总线"),
      style()->standardIcon(QStyle::SP_FileDialogContentsView),
      [this] {
        activateWorkspaceTab(overviewTabIndex_);
        copySlaveEvidenceMatrixRowDigest(
            slaveEvidence_->slaveEvidenceMatrixTable ? slaveEvidence_->slaveEvidenceMatrixTable->currentRow()
                                      : -1);
      },
      [this] {
        return slaveEvidence_->slaveEvidenceMatrixTable &&
               slaveEvidence_->slaveEvidenceMatrixTable->currentRow() >= 0;
      },
  });
  commands.append(CommandItem{
      uiText("Review First Slave Matrix Issue", "审阅首个从站矩阵问题"),
      uiText("Open the first visible Risk or Action row from the Overview "
             "matrix using local loaded evidence only",
             "从总览矩阵打开首个可见风险或待执行行，只使用已加载本地证据"),
      style()->standardIcon(QStyle::SP_FileDialogDetailedView),
      [this] {
        activateWorkspaceTab(overviewTabIndex_);
        reviewFirstSlaveEvidenceMatrixIssue();
      },
      [this] {
        return slaveEvidence_->slaveEvidenceMatrixTable &&
               slaveEvidence_->slaveEvidenceMatrixTable->rowCount() > 0 &&
               (!slaveEvidence_->slaveEvidenceMatrixReviewButton ||
                slaveEvidence_->slaveEvidenceMatrixReviewButton->isEnabled());
      },
  });
  commands.append(CommandItem{
      uiText("Review Next Slave Matrix Issue", "审阅下个从站矩阵问题"),
      uiText("Open the next visible Risk or Action row after the current "
             "Overview matrix row using local loaded evidence only",
             "从总览矩阵当前行之后打开下一个可见风险或待执行行，只使用已加载本"
             "地证据"),
      style()->standardIcon(QStyle::SP_ArrowForward),
      [this] {
        activateWorkspaceTab(overviewTabIndex_);
        reviewNextSlaveEvidenceMatrixIssue();
      },
      [this] {
        return slaveEvidence_->slaveEvidenceMatrixTable &&
               slaveEvidence_->slaveEvidenceMatrixTable->rowCount() > 0 &&
               (!slaveEvidence_->slaveEvidenceMatrixReviewNextButton ||
                slaveEvidence_->slaveEvidenceMatrixReviewNextButton->isEnabled());
      },
  });
  commands.append(CommandItem{
      uiText("Sync Watch Values to Startup SDO", "同步 Watch 值到 Startup SDO"),
      uiText("Update matching Startup SDO rows from selected Watch values; "
             "create missing rows only when needed",
             "用选中 Watch 值更新匹配的 Startup SDO；仅在缺失时创建新行"),
      style()->standardIcon(QStyle::SP_DialogApplyButton),
      [this] { syncSelectedWatchRowsToStartupSdo(); },
      [hasWatchValueSelection] { return hasWatchValueSelection(); },
  });
  commands.append(CommandItem{
      uiText("Review Startup SDO Watch Diffs", "审阅 Startup SDO Watch 偏差"),
      uiText("Open Startup SDO, show only rows whose expected values differ "
             "from current Watch values, and select the first mismatch",
             "打开 Startup SDO，只显示和当前 Watch "
             "值不一致的行，并选中第一条偏差"),
      style()->standardIcon(QStyle::SP_FileDialogDetailedView),
      [this] { focusStartupSdoWatchDiffs(); },
      [canReviewStartupWatchDiffs] { return canReviewStartupWatchDiffs(); },
  });
  commands.append(CommandItem{
      uiText("Apply Startup SDO Watch Diffs", "应用 Startup SDO Watch 偏差"),
      uiText("Write only Startup SDO rows whose expected values differ from "
             "current Watch values",
             "只写入和当前 Watch 值不一致的 Startup SDO 行"),
      style()->standardIcon(QStyle::SP_DialogYesButton),
      [this] { applyStartupSdoWatchDiffRows(); },
      [hasStartupWatchDiffs] { return hasStartupWatchDiffs(); },
  });
  commands.append(CommandItem{
      uiText("Clear Watch List", "清空监视列表"),
      uiText("Remove all SDO watch rows", "移除所有 SDO 监视行"),
      style()->standardIcon(QStyle::SP_TrashIcon),
      [this] { clearWatchList(); },
      [this] { return watch_->watchTable && watch_->watchTable->rowCount() > 0; },
  });
  QString selectedRecommendedState;
  const int stateCommandPosition = selectedPosition();
  if (stateCommandPosition >= 0) {
    for (const auto &slave : slaves_) {
      if (slave.position == stateCommandPosition) {
        selectedRecommendedState = recommendedEthercatState(slave);
        break;
      }
    }
  }
  commands.append(CommandItem{
      uiText("Open State Machine", "打开状态机"),
      uiText("Review EtherCAT state, evidence, risk, and recommended next "
             "state for every slave",
             "复核每个从站的 EtherCAT 状态、证据、风险和推荐下一状态"),
      style()->standardIcon(QStyle::SP_FileDialogDetailedView),
      [this] {
        activateWorkspaceTab(stateMachineTabIndex_);
        updateStateMachineView();
      },
      [this] { return stateMachineTabIndex_ >= 0; },
  });
  commands.append(CommandItem{
      selectedRecommendedState.isEmpty()
          ? uiText("Send Recommended EtherCAT State", "发送推荐 EtherCAT 状态")
          : uiText("Send Recommended EtherCAT State: %1",
                   "发送推荐 EtherCAT 状态：%1")
                .arg(selectedRecommendedState),
      selectedRecommendedState.isEmpty()
          ? uiText("No state recommendation is currently available for the "
                   "selected slave",
                   "当前选中从站暂无可发送的状态推荐")
          : uiText("Use the normal confirmed state-transition flow for slave "
                   "#%1",
                   "对从站 #%1 使用常规确认状态切换流程")
                .arg(stateCommandPosition),
      style()->standardIcon(QStyle::SP_CommandLink),
      [this, stateCommandPosition, selectedRecommendedState] {
        requestSlaveStateWithConfirmation(stateCommandPosition,
                                          selectedRecommendedState);
      },
      [this, stateCommandPosition, selectedRecommendedState] {
        return client_.isConnected() && stateCommandPosition >= 0 &&
               !selectedRecommendedState.isEmpty();
      },
  });
  commands.append(CommandItem{
      uiText("Run Host Check", "运行主机检查"),
      uiText(
          "Validate kernel modules, EtherCAT config, NICs, blacklist, and DKMS",
          "检查内核模块、EtherCAT 配置、网卡、黑名单和 DKMS"),
      style()->standardIcon(QStyle::SP_ComputerIcon),
      [this] { runHostDiagnostics(); },
      [this] { return client_.isConnected(); },
  });
  commands.append(CommandItem{
      uiText("Back Workspace", "后退工作区"),
      uiText("Return to the previous workspace without running online commands",
             "返回上一个工作区，不执行在线命令"),
      style()->standardIcon(QStyle::SP_ArrowBack),
      [this] { goWorkspaceBack(); },
// ── Keyboard Shortcuts ────────────────────────────────────────────────
      [this] { return workspaceBackStack_.size() >= 2; },
  });
  commands.append(CommandItem{
      uiText("Forward Workspace", "前进工作区"),
      uiText("Move forward in workspace history without running online "
             "commands",
             "前进到下一个工作区，不执行在线命令"),
      style()->standardIcon(QStyle::SP_ArrowForward),
      [this] { goWorkspaceForward(); },
      [this] { return !workspaceForwardStack_.isEmpty(); },
  });
  commands.append(workspaceCommand(overviewPage_, uiText("Overview", "总览"),
                                   "Ctrl+Alt+1"));
  commands.append(workspaceCommand(objectDictionaryPage_,
                                   uiText("Object Dictionary", "对象字典"),
                                   "Ctrl+Alt+2"));
  commands.append(workspaceCommand(pdoMapPage_, uiText("PDO Map", "PDO 映射"),
                                   "Ctrl+Alt+3"));
  commands.append(
      workspaceCommand(watchPage_, uiText("Watch", "监视"), "Ctrl+Alt+4"));
  commands.append(workspaceCommand(
      startupSdoPage_, uiText("Startup SDO", "启动 SDO"), "Ctrl+Alt+5"));
  commands.append(workspaceCommand(freeRunPage_, uiText("Free Run", "自由运行"),
                                   "Ctrl+Alt+6"));
  commands.append(workspaceCommand(
      ioVariablePage_, uiText("I/O Variables", "I/O 变量"), "Ctrl+Alt+7"));
  commands.append(workspaceCommand(
      consistencyPage_, uiText("Consistency", "一致性"), "Ctrl+Alt+8"));
  commands.append(workspaceCommand(
      stateMachinePage_, uiText("State Machine", "状态机"), "Ctrl+Alt+9"));
  commands.append(workspaceCommand(
      diagnosticsPage_, uiText("Diagnostics", "诊断"), "Ctrl+Alt+0"));
  commands.append(workspaceCommand(esiRepositoryPage_,
                                   uiText("ESI Repository", "ESI 仓库"), {}));
  commands.append(workspaceCommand(notesPage_, uiText("Notes", "备注"), {}));
  commands.append(
      workspaceCommand(esiXmlPage_, uiText("ESI XML", "ESI XML"), {}));
  commands.append(workspaceCommand(masterRawPage_,
                                   uiText("Master Raw", "主站原始输出"), {}));
  commands.append(
      workspaceCommand(slaveRawPage_, uiText("Slave Raw", "从站原始输出"), {}));
  commands.append(
      workspaceCommand(pdoRawPage_, uiText("PDO Raw", "PDO 原始输出"), {}));
  commands.append(
      workspaceCommand(sdoRawPage_, uiText("SDO Raw", "SDO 原始输出"), {}));
  commands.append(actionCommand(
      "newProjectAction", uiText("New Project", "新建工程"),
      uiText("Create a clean project workspace", "创建干净工程工作区")));
  commands.append(actionCommand(
      "openProjectAction", uiText("Open Project", "打开工程"),
      uiText("Load a NekoEcat Studio project", "加载 NekoEcat Studio 工程")));
  commands.append(
      actionCommand("saveProjectAction", uiText("Save Project", "保存工程"),
                    uiText("Save the active project", "保存当前工程")));
  commands.append(actionCommand("saveProjectAsAction",
                                uiText("Save Project As", "工程另存为"),
                                uiText("Save the active project to a new file",
                                       "将当前工程保存为新文件")));
  commands.append(actionCommand(
      "exportReportAction", uiText("Export Diagnostics Report", "导出诊断报告"),
      uiText("Write runtime diagnostics as Markdown",
             "将运行诊断导出为 Markdown")));
  commands.append(
      actionCommand("exportIoVariablesAction",
                    uiText("Export I/O Variables CSV", "导出 I/O 变量 CSV"),
                    uiText("Write the current engineering signal table as CSV",
                           "将当前工程信号表导出为 CSV")));
  commands.append(
      actionCommand("exportIoPlcSymbolsAction",
                    uiText("Export PLC Symbols CSV", "导出 PLC 符号 CSV"),
                    uiText("Write visible I/O variables as PLC handoff records",
                           "将可见 I/O 变量导出为 PLC 交接记录")));
  commands.append(actionCommand(
      "reviewPlcHandoffAction",
      uiText("Review PLC Handoff Issues", "审阅 PLC 交接问题"),
      uiText("Open I/O Variables with the PLC Issues scope for handoff cleanup",
             "打开 I/O 变量并切到 PLC 交接问题范围，整理交接质量")));
  commands.append(
      actionCommand("importEsiAction", uiText("Import ESI XML", "导入 ESI XML"),
                    uiText("Add device descriptions to the repository",
                           "将设备描述加入仓库")));
  commands.append(
      actionCommand("settingsAction", uiText("Open Settings", "打开设置"),
                    uiText("Configure theme, language, scale, and masters",
                           "配置主题、语言、缩放和主站")));
  commands.append(actionCommand(
      "manualAction", uiText("Open User Manual", "打开使用说明书"),
      uiText("Read detailed NekoEcat Studio operating instructions",
             "查看 NekoEcat Studio 的详细操作说明")));
  commands.append(actionCommand(
      "aboutAction", uiText("About NekoEcat Studio", "关于 NekoEcat Studio"),
      uiText("Read version, architecture, capabilities, and safety information",
             "查看版本、架构、功能能力和安全说明")));
  commands.append(
      actionCommand("showLogAction", uiText("Show Runtime Log", "显示运行日志"),
                    uiText("Raise the runtime log dock", "显示运行日志面板")));

  for (const auto &profile : settings_.masters) {
    const QString label = QString("%1  [%2]").arg(profile.name, profile.target);
    commands.append(CommandItem{
        uiText("Switch Master: ", "切换主站：") + label,
        uiText("Make this master the active runtime target",
               "将此主站设为当前运行目标"),
        style()->standardIcon(QStyle::SP_ComputerIcon),
        [this, target = profile.target] { setActiveMaster(target); },
        [] { return true; },
    });
  }

  QSettings commandSettings("NekoEcatStudio", "NekoEcatStudio");
  QStringList recentCommandKeys =
      commandSettings.value("preferences/recentCommandKeys").toStringList();
  QStringList pinnedCommandKeys =
      commandSettings.value("preferences/pinnedCommandKeys").toStringList();
  auto commandRecentKey = [](const CommandItem &command) {
    QString key = command.title.trimmed();
    key.replace(QRegularExpression("\\s+"), " ");
    return key.toLower();
  };
  auto rememberCommand = [&](const CommandItem &command) {
    const QString key = commandRecentKey(command);
    if (key.isEmpty()) {
      return;
    }
    recentCommandKeys.removeAll(key);
    recentCommandKeys.prepend(key);
    while (recentCommandKeys.size() > 24) {
      recentCommandKeys.removeLast();
    }
    commandSettings.setValue("preferences/recentCommandKeys",
                             recentCommandKeys);
  };
  auto persistPinnedCommands = [&] {
    while (pinnedCommandKeys.size() > 24) {
      pinnedCommandKeys.removeLast();
    }
    commandSettings.setValue("preferences/pinnedCommandKeys",
                             pinnedCommandKeys);
  };
  auto togglePinnedCommand = [&](const CommandItem &command) {
    const QString key = commandRecentKey(command);
    if (key.isEmpty()) {
      return;
    }
    const bool wasPinned = pinnedCommandKeys.contains(key);
    pinnedCommandKeys.removeAll(key);
    if (!wasPinned) {
      pinnedCommandKeys.prepend(key);
    }
    persistPinnedCommands();
  };

  auto commandSafety = [this](const CommandItem &command) -> CommandSafety {
    const QString combined =
        QString("%1 %2").arg(command.title, command.subtitle).toLower();
    auto containsAny = [&combined](std::initializer_list<const char *> words) {
      for (const char *word : words) {
        if (combined.contains(QString::fromUtf8(word).toLower())) {
          return true;
        }
      }
      return false;
    };

    if (containsAny({"host check", "host health", "kernel modules", "dkms",
                     "blacklist", "主机检查", "主机健康", "内核模块",
                     "黑名单"})) {
      return {QStringLiteral("host"), uiText("Host", "主机"),
              uiText("Queries host environment diagnostics only from explicit "
                     "host-check commands",
                     "仅在显式主机检查命令中查询主机环境诊断"),
    // Define color for visual feedback
              QColor("#2563eb")};
    }
    if (containsAny({"write current sdo", "download the current write",
                     "set selected slave", "set all slaves",
                     "send recommended ethercat state", "cia 402 controlword",
                     "recommended cia 402", "apply startup sdo watch diffs",
                     "写入当前 sdo", "选中从站切换", "全部从站切换",
                     "发送推荐 ethercat 状态", "cia 402 控制字", "推荐 cia 402",
                     "应用 startup sdo watch 偏差"})) {
      return {QStringLiteral("danger"), uiText("Danger", "危险"),
              uiText("Uses the existing confirmation path before writing or "
                     "changing EtherCAT state",
                     "写入或切换 EtherCAT 状态前会继续走现有确认流程"),
    // Define color for visual feedback
              QColor("#dc2626")};
    }
    if (containsAny({"new project", "open project", "save project",
                     "export diagnostics report", "export i/o", "export plc",
                     "export report", "import esi", "open user manual",
                     "about ethercat studio", "新建工程", "打开工程",
                     "保存工程", "导出诊断报告", "导出 i/o", "导出 plc",
                     "导入 esi", "打开使用说明书", "关于 ethercat studio"})) {
      return {QStringLiteral("file"), uiText("File", "文件"),
              uiText("Changes project files, exports data, imports resources, "
                     "or opens documentation",
                     "修改工程文件、导出数据、导入资源或打开文档"),
    // Define color for visual feedback
              QColor("#64748b")};
    }
    if (containsAny({"connect runtime",
                     "refresh online data",
// ── Pin/Unpin Logic ───────────────────────────────────────────────────
                     "rescan bus",
                     "read current sdo",
                     "read visible object dictionary",
                     "retry failed object evidence",
                     "refresh watch list",
                     "prepare selected slave snapshot",
                     "toggle free run",
                     "run next commissioning step",
                     "run next best action",
                     "add current sdo to watch",
                     "upload",
                     "telemetry",
                     "连接运行时",
                     "刷新在线数据",
                     "重新扫描总线",
                     "读取当前 sdo",
                     "读取可见对象字典",
                     "重试失败对象证据",
                     "刷新监视列表",
                     "准备选中从站快照",
                     "切换自由运行",
                     "执行调试下一步",
                     "执行下一最佳动作",
                     "添加当前 sdo 到监视",
                     "遥测"})) {
      return {QStringLiteral("online"), uiText("Online", "在线"),
              uiText("May talk to the EtherCAT runtime or read live evidence",
                     "可能访问 EtherCAT 运行时或读取实时证据"),
    // Define color for visual feedback
              QColor("#d97706")};
    }
    return {QStringLiteral("local"), uiText("Local", "本地"),
            uiText("Uses loaded UI/project evidence or navigates without bus "
                   "access",
                   "只使用已加载界面/工程证据，或进行不访问总线的导航"),
    // Define color for visual feedback
            QColor("#16a34a")};
  };

  struct CommandDisplayRow {
    int index = -1;
    CommandSafety safety;
    bool enabled = false;
    int pinnedRank = -1;
    int recentRank = -1;
    int originalOrder = -1;
  };

  // Rebuild the command list from the current search and filter state
  auto refill = [&] {
    list->clear();
    // Get current search text
    const QString needle = search->text().trimmed();
    const QString safetyKey = safetyFilter->currentData().toString();
    QHash<QString, int> visibleCounts;
    QHash<QString, int> enabledCounts;
    QHash<QString, QString> safetyLabels;
    QVector<CommandDisplayRow> displayRows;
    int visibleTotal = 0;
    int enabledTotal = 0;
    int pinnedVisible = 0;
    int recentVisible = 0;
    for (int i = 0; i < commands.size(); ++i) {
      const auto &command = commands[i];
      const CommandSafety safety = commandSafety(command);
      if (!safetyKey.isEmpty() && safety.key != safetyKey) {
        continue;
      }
      const bool matches =
          needle.isEmpty() ||
          command.title.contains(needle, Qt::CaseInsensitive) ||
          command.subtitle.contains(needle, Qt::CaseInsensitive) ||
          safety.label.contains(needle, Qt::CaseInsensitive) ||
          safety.hint.contains(needle, Qt::CaseInsensitive);
      if (!matches) {
        continue;
      }
      const bool isEnabled = command.enabled();
      ++visibleTotal;
      ++visibleCounts[safety.key];
      safetyLabels[safety.key] = safety.label;
      if (isEnabled) {
        ++enabledTotal;
        ++enabledCounts[safety.key];
      }
      const int pinnedRank =
          pinnedCommandKeys.indexOf(commandRecentKey(command));
      if (pinnedRank >= 0) {
        ++pinnedVisible;
      }
      const int recentRank =
          recentCommandKeys.indexOf(commandRecentKey(command));
      if (recentRank >= 0) {
        ++recentVisible;
      }
      displayRows.append(
          CommandDisplayRow{i, safety, isEnabled, pinnedRank, recentRank, i});
    }
    // Sort commands by relevance
    std::stable_sort(
        displayRows.begin(), displayRows.end(),
        [](const CommandDisplayRow &left, const CommandDisplayRow &right) {
          const bool leftPinned = left.pinnedRank >= 0;
          const bool rightPinned = right.pinnedRank >= 0;
          if (leftPinned != rightPinned) {
            return leftPinned;
          }
          if (leftPinned && rightPinned &&
              left.pinnedRank != right.pinnedRank) {
            return left.pinnedRank < right.pinnedRank;
          }
          const bool leftRecent = left.recentRank >= 0;
          const bool rightRecent = right.recentRank >= 0;
          if (leftRecent != rightRecent) {
            return leftRecent;
          }
          if (leftRecent && rightRecent &&
              left.recentRank != right.recentRank) {
            return left.recentRank < right.recentRank;
          }
          return left.originalOrder < right.originalOrder;
        });
    for (const auto &row : displayRows) {
      const auto &command = commands[row.index];
      const CommandSafety safety = row.safety;
      const bool isEnabled = row.enabled;
    // Create list item for command
      auto *item = new QListWidgetItem(command.icon, command.title);
    // Store command identifier
      item->setData(Qt::UserRole, row.index);
    // Store command identifier
      item->setData(Qt::UserRole + 1, safety.key);
    // Store command identifier
      item->setData(Qt::UserRole + 2, safety.label);
    // Store command identifier
      item->setData(Qt::UserRole + 3, safety.hint);
    // Store command identifier
      item->setData(Qt::UserRole + 4, row.recentRank);
    // Store command identifier
      item->setData(Qt::UserRole + 5, row.pinnedRank);
      item->setToolTip(QString("%1: %2\n%3\n%4")
                           .arg(safety.label, safety.hint, command.subtitle,
                                row.pinnedRank >= 0
                                    ? uiText("Pinned command. Alt+P unpins.",
                                             "已固定命令。Alt+P 取消固定。")
                                    : uiText("Alt+P pins this command.",
                                             "Alt+P 固定此命令。")));
      item->setFlags(isEnabled ? item->flags()
                               : (item->flags() & ~Qt::ItemIsEnabled));
      if (isEnabled) {
    // Define color for visual feedback
        item->setForeground(QBrush(safety.color));
      }
      const QString pinnedPrefix =
          row.pinnedRank >= 0 ? uiText("[Pinned] ", "[固定] ") : QString();
      const QString recentPrefix =
          row.recentRank >= 0 ? uiText("[Recent] ", "[最近] ") : QString();
      item->setText(QString("%1%2[%3] %4\n%5")
                        .arg(pinnedPrefix, recentPrefix, safety.label,
                             command.title, command.subtitle));
      item->setSizeHint(QSize(0, 54));
    // Add command to palette list
      list->addItem(item);
    }
    if (list->count() > 0) {
      list->setCurrentRow(0);
    }
    QStringList parts;
    const QStringList order = {QStringLiteral("local"),
                               QStringLiteral("online"),
                               QStringLiteral("danger"), QStringLiteral("host"),
                               QStringLiteral("file")};
    for (const QString &key : order) {
      if (visibleCounts.value(key) <= 0) {
        continue;
      }
      const QString label = safetyLabels.value(key);
      parts << QString("%1 %2/%3")
                   .arg(label)
                   .arg(enabledCounts.value(key))
                   .arg(visibleCounts.value(key));
    }
    commandStats->setText(
        parts.isEmpty()
            ? uiText("No commands match the current search/filter.",
                     "当前搜索/过滤没有匹配命令。")
            : uiText("Showing %1 command(s), %2 executable, %3 "
                     "pinned, %4 recent. %5",
                     "显示 %1 条命令，%2 条可执行，%3 条固定，%4 条最近"
                     "使用。%5")
                  .arg(visibleTotal)
                  .arg(enabledTotal)
                  .arg(pinnedVisible)
                  .arg(recentVisible)
                  .arg(parts.join("  |  ")));
  };

  // Refresh the command detail preview panel
  auto updatePreview = [&] {
    auto *item = list->currentItem();
    if (!item) {
      commandPreview->setText(uiText(
          "No matching command. Adjust search text or action type filter.",
          "没有匹配命令。请调整搜索内容或操作类型过滤。"));
    // Set safety property for styling/theming
      commandPreview->setProperty("safety", "empty");
      repolish(commandPreview); // force QSS re-evaluation after property change
      return;
    }
    const int index = item->data(Qt::UserRole).toInt();
    if (index < 0 || index >= commands.size()) {
      return;
    }
    const auto &command = commands[index];
// ── Dialog Execution ──────────────────────────────────────────────────
    const QString safetyKey = item->data(Qt::UserRole + 1).toString();
    const QString safetyLabel = item->data(Qt::UserRole + 2).toString();
    const QString safetyHint = item->data(Qt::UserRole + 3).toString();
    const int recentRank = item->data(Qt::UserRole + 4).toInt();
    const int pinnedRank = item->data(Qt::UserRole + 5).toInt();
    const bool isEnabled = item->flags() & Qt::ItemIsEnabled;
    // Set safety property for styling/theming
    commandPreview->setProperty("safety", safetyKey);
    commandPreview->setText(
        QString("%1: %2\n%3\n%4: %5")
            .arg(uiText("Type", "类型"), safetyLabel, safetyHint,
                 uiText("Command", "命令"), command.subtitle) +
        (pinnedRank >= 0
             ? uiText("\nPinned command. Alt+P or the row menu can unpin it.",
                      "\n已固定命令。可用 Alt+P 或行菜单取消固定。")
             : uiText("\nAlt+P pins this command above recent commands.",
                      "\nAlt+P 可把此命令固定在最近使用命令之前。")) +
        (recentRank >= 0
             ? uiText("\nRecently used command.", "\n最近使用过的命令。")
             : QString()) +
        (isEnabled ? QString()
                   : uiText("\nDisabled in the current context.",
                            "\n当前上下文不可执行。")));
    repolish(commandPreview); // force QSS re-evaluation after property change
  };

  // Execute the currently selected command and close the palette
  auto runCurrent = [&] {
    auto *item = list->currentItem();
    if (!item || !(item->flags() & Qt::ItemIsEnabled)) {
      return;
    }
    const int index = item->data(Qt::UserRole).toInt();
    if (index >= 0 && index < commands.size()) {
      dialog.accept();
      rememberCommand(commands[index]);
      const CommandSafety safety = commandSafety(commands[index]);
      updateDiagnostics((safety.key == "danger") ? "Warning" : "Info",
                        "Command Palette",
                        uiText("Activated [%1] %2", "已触发 [%1] %2")
                            .arg(safety.label, commands[index].title));
      commands[index].run();
    }
  };

  // Move the list selection up or down by delta rows
  auto moveSelection = [&list](int delta) {
    if (list->count() <= 0) {
      return;
    }
    int row = list->currentRow();
    if (row < 0) {
      row = 0;
    } else {
      row += delta;
    }
    if (row < 0) {
      row = 0;
    } else if (row >= list->count()) {
      row = list->count() - 1;
    }
    list->setCurrentRow(row);
    if (auto *item = list->currentItem()) {
      list->scrollToItem(item);
    }
  };
  // Programmatically select a safety filter tab
  auto setSafetyFilter = [safetyFilter](const QString &key) {
    const int index = safetyFilter->findData(key);
    if (index >= 0) {
      safetyFilter->setCurrentIndex(index);
    }
  };
  // Toggle pin state for the currently selected command
  auto toggleCurrentPinned = [&] {
    auto *item = list->currentItem();
    if (!item) {
      return;
    }
    const int index = item->data(Qt::UserRole).toInt();
    if (index < 0 || index >= commands.size()) {
      return;
    }
    togglePinnedCommand(commands[index]);
    refill();
    for (int row = 0; row < list->count(); ++row) {
      auto *candidate = list->item(row);
      if (candidate && candidate->data(Qt::UserRole).toInt() == index) {
        list->setCurrentRow(row);
        list->scrollToItem(candidate);
        break;
      }
    }
    updatePreview();
  };

    // Connect QLineEdit::textChanged signal to handler
  connect(search, &QLineEdit::textChanged, &dialog, refill); // wire signal to slot
    // Connect QComboBox::currentIndexChanged signal to handler
  connect(safetyFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
          &dialog, refill);
    // Connect QLineEdit::returnPressed signal to handler
  connect(search, &QLineEdit::returnPressed, &dialog, runCurrent); // wire signal to slot
    // Connect QListWidget::currentRowChanged signal to handler
  connect(list, &QListWidget::currentRowChanged, &dialog,
          [&](int) { updatePreview(); });
    // Connect QListWidget::itemActivated signal to handler
  connect(list, &QListWidget::itemActivated, &dialog,
    // Create list item for command
          [&](QListWidgetItem *) { runCurrent(); });
    // Connect QListWidget::itemDoubleClicked signal to handler
  connect(list, &QListWidget::itemDoubleClicked, &dialog,
    // Create list item for command
          [&](QListWidgetItem *) { runCurrent(); });
    // Connect QListWidget::customContextMenuRequested signal to handler
  connect(list, &QListWidget::customContextMenuRequested, &dialog,
          [&](const QPoint &position) {
            auto *item = list->itemAt(position);
            if (!item) {
              return;
            }
            list->setCurrentItem(item);
            const int index = item->data(Qt::UserRole).toInt();
            if (index < 0 || index >= commands.size()) {
              return;
            }
            const bool pinned =
                pinnedCommandKeys.contains(commandRecentKey(commands[index]));
            QMenu menu(&dialog);
            auto *pinAction =
                menu.addAction(pinned ? uiText("Unpin Command", "取消固定命令")
                                      : uiText("Pin Command", "固定命令"));
            pinAction->setShortcut(QKeySequence("Alt+P"));
            auto *chosen = menu.exec(list->viewport()->mapToGlobal(position));
            if (chosen == pinAction) {
              toggleCurrentPinned();
            }
          });
  auto *downShortcut = new QShortcut(QKeySequence(Qt::Key_Down), &dialog);
  auto *upShortcut = new QShortcut(QKeySequence(Qt::Key_Up), &dialog);
  auto *pinShortcut = new QShortcut(QKeySequence("Alt+P"), &dialog);
  auto *allFilterShortcut = new QShortcut(QKeySequence("Alt+A"), &dialog);
  auto *localFilterShortcut = new QShortcut(QKeySequence("Alt+L"), &dialog);
  auto *onlineFilterShortcut = new QShortcut(QKeySequence("Alt+O"), &dialog);
  auto *dangerFilterShortcut = new QShortcut(QKeySequence("Alt+D"), &dialog);
  auto *hostFilterShortcut = new QShortcut(QKeySequence("Alt+H"), &dialog);
  auto *fileFilterShortcut = new QShortcut(QKeySequence("Alt+F"), &dialog);
    // Connect QShortcut::activated signal to handler
  connect(downShortcut, &QShortcut::activated, &dialog,
          [&] { moveSelection(1); });
    // Connect QShortcut::activated signal to handler
  connect(upShortcut, &QShortcut::activated, &dialog,
          [&] { moveSelection(-1); });
    // Connect QShortcut::activated signal to handler
  connect(pinShortcut, &QShortcut::activated, &dialog, toggleCurrentPinned); // wire signal to slot
    // Connect QShortcut::activated signal to handler
  connect(allFilterShortcut, &QShortcut::activated, &dialog,
          [setSafetyFilter] { setSafetyFilter(QString()); });
    // Connect QShortcut::activated signal to handler
  connect(localFilterShortcut, &QShortcut::activated, &dialog,
          [setSafetyFilter] { setSafetyFilter(QStringLiteral("local")); });
    // Connect QShortcut::activated signal to handler
  connect(onlineFilterShortcut, &QShortcut::activated, &dialog,
          [setSafetyFilter] { setSafetyFilter(QStringLiteral("online")); });
    // Connect QShortcut::activated signal to handler
  connect(dangerFilterShortcut, &QShortcut::activated, &dialog,
          [setSafetyFilter] { setSafetyFilter(QStringLiteral("danger")); });
    // Connect QShortcut::activated signal to handler
  connect(hostFilterShortcut, &QShortcut::activated, &dialog,
          [setSafetyFilter] { setSafetyFilter(QStringLiteral("host")); });
    // Connect QShortcut::activated signal to handler
  connect(fileFilterShortcut, &QShortcut::activated, &dialog,
          [setSafetyFilter] { setSafetyFilter(QStringLiteral("file")); });

  refill();
  updatePreview();
  search->setFocus();
  dialog.exec();
}

