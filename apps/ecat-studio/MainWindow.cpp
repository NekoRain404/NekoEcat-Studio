// Main application window: workspace tabs, toolbars, wiring, and all workspace methods.
#include "detail/RealtimeChartDialog.h"
#include "MainWindow.h"
#include "detail/RealtimeChartDialog.h"
#include "infra/TranslationRegistry.h"
#include "infra/LanguageManager.h"
#include "Cia402DriveModel.h"
#include "CommissioningWorkflowModel.h"
#include "detail/WorkflowStepDetail.h"
#include "WorkflowTableAdapter.h"
#include "detail/CommissioningWorkflowDetail.h"
#include "detail/ConsistencyDetail.h"
#include "ConsistencyModel.h"
#include "ConsistencyTableAdapter.h"
#include "detail/DiagnosticsEventDetail.h"
#include "EvidenceModel.h"
#include "detail/FreeRunEntryDetail.h"
#include "detail/HostHealthDetail.h"
#include "detail/IoVariableDetail.h"
#include "IoVariableModel.h"
#include "NextBestActionModel.h"
#include "detail/NextBestActionDetail.h"
#include "detail/ObjectBookmarkDetail.h"
#include "detail/PdoMapDetail.h"
#include "ProcessDataRowModel.h"
#include "ProcessDataTableAdapter.h"
#include "SdoDictionaryTableAdapter.h"
#include "SdoEvidenceModel.h"
#include "SdoEvidenceTableAdapter.h"
#include "detail/SdoHistoryRowDetail.h"
#include "detail/SdoTargetTrailDetail.h"
#include "detail/SelectedDriveSummaryDetail.h"
#include "detail/SlaveEvidenceSummaryDetail.h"
#include "SessionBriefModel.h"
#include "SessionBriefTableAdapter.h"
#include "detail/SessionBriefDetail.h"
#include "SlaveEvidenceModel.h"
#include "SlaveEvidenceTableAdapter.h"
#include "detail/SlaveEvidenceDetail.h"
#include "detail/StartupSdoRowDetail.h"
#include "detail/StateMachineRowDetail.h"
#include "StateMachineTableAdapter.h"
#include "utils/Documentation.h"
#include "utils/TableHelpers.h"
#include "utils/TextHelpers.h"
#include "utils/UiHelpers.h"
#include "TopologyModel.h"
#include "detail/WatchRowDetail.h"
#include "WatchStartupModel.h"
#include "WatchStartupTableAdapter.h"
#include "detail/WatchStartupDetail.h"
#include "detail/WorkspaceBoundaryDetail.h"
#include "WorkspaceTabBadgeTableAdapter.h"
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
// Locate the bundled ecatd binary next to the studio executable

namespace {

QString ecatdPath() {
  const QFileInfo app(QCoreApplication::applicationFilePath());
  const QStringList candidates = {
      app.dir().absoluteFilePath("ecatd"),
      app.dir().absoluteFilePath("../ecatd/ecatd"),
  };
// Map NextBestAction icon keys to Qt standard pixmaps
  for (const QString &candidate : candidates) {
    if (QFileInfo::exists(candidate)) {
      return QFileInfo(candidate).canonicalFilePath();
    }
  }
  return "ecatd";
}

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
// Semantic color for host-health severity badges
    return QStyle::SP_FileDialogNewFolder;
  case NextBestActionIconKey::MediaPlay:
    return QStyle::SP_MediaPlay;
  case NextBestActionIconKey::ContentsView:
    return QStyle::SP_FileDialogContentsView;
  }
  return QStyle::SP_FileDialogContentsView;
}

QColor hostHealthColorForKey(const QString &colorKey) {
    // Multi-branch condition check
  if (colorKey == QStringLiteral("error")) {
// Semantic color for diagnostics-event severity badges
    return QColor("#ef4444");
  }
    // Multi-branch condition check
  if (colorKey == QStringLiteral("warning")) {
    return QColor("#f59e0b");
  }
  return QColor("#22c55e");
}

QColor diagnosticsEventColorForKey(const QString &colorKey) {
  if (colorKey == QStringLiteral("error")) {
    return QColor("#ef4444");
// Bootstrap: load persisted settings, build UI, launch daemon
  }
  if (colorKey == QStringLiteral("warning")) {
    return QColor("#f59e0b");
  }
  return QColor("#60a5fa");
}

} // namespace

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  loadSettings();
  client_.setMasterTarget(settings_.activeMaster);
  eventBus_ = new EventBus(this);
  buildUi();
  applySettings();
  applyCustomShortcuts();
// Persist window geometry and gracefully shut down embedded daemon
  wire();
  setMinimumSize(1120, 720);
  QSettings settings("NekoEcatStudio", "NekoEcatStudio");
  const QByteArray geometry = settings.value("geometry").toByteArray();
    // Multi-branch condition check
  if (geometry.isEmpty() || !restoreGeometry(geometry)) {
    resize(1440, 900);
  }
  restoreState(settings.value("windowState").toByteArray());
  startEmbeddedDaemon();
}

// Destructor: saves settings and cleans up resources
MainWindow::~MainWindow() {
  saveSettings();
  QSettings settings("NekoEcatStudio", "NekoEcatStudio");
  settings.setValue("geometry", saveGeometry());
  settings.setValue("windowState", saveState());
    // Multi-branch condition check
  if (daemon_.state() != QProcess::NotRunning) {
    daemon_.terminate();
    // Multi-branch condition check
    if (!daemon_.waitForFinished(1200)) {
      daemon_.kill();
    }
  }
}

// Global key handler: Alt+Return triggers evidence action on focused table
bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
    // Multi-branch condition check
  if (event && event->type() == QEvent::KeyPress) {
    auto *keyEvent = static_cast<QKeyEvent *>(event);
    const bool isReturnKey =
        keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter;
    // Multi-branch condition check
    if (isReturnKey && (keyEvent->modifiers() & Qt::AltModifier)) {
    // Multi-branch condition check
      if (auto *table = qobject_cast<QTableWidget *>(watched)) {
        return runLocalEvidenceAction(table);
      }
    // Multi-branch condition check
      if (auto *viewport = qobject_cast<QWidget *>(watched)) {
        if (auto *table = qobject_cast<QTableWidget *>(viewport->parent())) {
          return runLocalEvidenceAction(table);
        }
      }
    }
  }
// Global event filter for keyboard shortcuts and focus management
  return QMainWindow::eventFilter(watched, event);
}

// Programmatic tab switch — maps logical page indices to their widget
bool MainWindow::activateWorkspaceTab(int index) {
  if (!tabs_ || index < 0) {
    return false;
  }
  const QVector<QPair<int, QWidget *>> pages = {
      {overviewTabIndex_, overviewPage_},
      {objectDictionaryTabIndex_, objectDictionaryPage_},
      {pdoMapTabIndex_, pdoMapPage_},
      {watchTabIndex_, watchPage_},
      {startupSdoTabIndex_, startupSdoPage_},
      {freeRunTabIndex_, freeRunPage_},
      {ioVariableTabIndex_, ioVariablePage_},
      {consistencyTabIndex_, consistencyPage_},
      {stateMachineTabIndex_, stateMachinePage_},
      {diagnosticsTabIndex_, diagnosticsPage_},
      {esiRepositoryTabIndex_, esiRepositoryPage_},
      {notesTabIndex_, notesPage_},
      {rtTestTabIndex_, rtTestPage_},
      {esiXmlTabIndex_, esiXmlPage_},
      {masterRawTabIndex_, masterRawPage_},
      {slaveRawTabIndex_, slaveRawPage_},
      {pdoRawTabIndex_, pdoRawPage_},
      {sdoRawTabIndex_, sdoRawPage_},
  };
  for (const auto &page : pages) {
    // Multi-branch condition check
    if (page.first == index && page.second) {
      return activateWorkspacePage(page.second);
    }
  }
  return index < tabs_->count() && activateWorkspacePage(tabs_->widget(index));
}

// Activate the workspace tab that contains the given page widget.
bool MainWindow::activateWorkspacePage(QWidget *page) {
// Activate a workspace page by its widget pointer
  if (!tabs_ || !page) {
    return false;
  }
  const int index = tabs_->indexOf(page);
    // Multi-branch condition check
  if (index < 0) {
    return false;
  }
  tabs_->setCurrentIndex(index);
  return true;
}

// Jump to the Object Dictionary pane, selecting the tab containing the target widget
bool MainWindow::activateObjectDictionaryPaneFor(QWidget *widget) {
  activateWorkspaceTab(objectDictionaryTabIndex_);
  return activateTabContainingWidget(sdoModeTabs_, widget);
}
// Classify a workspace page into its logical boundary category

WorkspaceBoundaryKind
MainWindow::workspaceBoundaryKindForPage(const QWidget *page) const {
    // Multi-branch condition check
  if (page == overviewPage_) {
    return WorkspaceBoundaryKind::Overview;
  }
    // Multi-branch condition check
  if (page == objectDictionaryPage_) {
    return WorkspaceBoundaryKind::ObjectDictionary;
  }
    // Multi-branch condition check
  if (page == pdoMapPage_) {
    return WorkspaceBoundaryKind::PdoMap;
  }
    // Multi-branch condition check
  if (page == watchPage_) {
    return WorkspaceBoundaryKind::Watch;
  }
    // Multi-branch condition check
  if (page == startupSdoPage_) {
    return WorkspaceBoundaryKind::StartupSdo;
  }
    // Multi-branch condition check
  if (page == freeRunPage_) {
    return WorkspaceBoundaryKind::FreeRun;
  }
    // Multi-branch condition check
  if (page == ioVariablePage_) {
    return WorkspaceBoundaryKind::IoVariables;
  }
    // Multi-branch condition check
  if (page == consistencyPage_) {
    return WorkspaceBoundaryKind::Consistency;
  }
    // Multi-branch condition check
  if (page == stateMachinePage_) {
    return WorkspaceBoundaryKind::StateMachine;
  }
    // Multi-branch condition check
  if (page == diagnosticsPage_) {
    return WorkspaceBoundaryKind::Diagnostics;
  }
    // Multi-branch condition check
  if (page == esiRepositoryPage_ || page == esiXmlPage_) {
    return WorkspaceBoundaryKind::Esi;
  }
    // Multi-branch condition check
  if (page == rtTestPage_) {
    return WorkspaceBoundaryKind::RtTest;
  }
    // Multi-branch condition check
  if (page == notesPage_) {
    return WorkspaceBoundaryKind::Notes;
  }
  return WorkspaceBoundaryKind::RawEvidence;
}

// Record a tab visit for back/forward navigation (capped at 80 entries)
void MainWindow::recordWorkspaceHistory(int index) {
    // Multi-branch condition check
  if (!tabs_ || suppressWorkspaceHistory_ || index < 0 ||
      index >= tabs_->count()) {
    return;
  }
  auto *page = tabs_->widget(index);
    // Multi-branch condition check
  if (!page) {
    return;
  }
    // Multi-branch condition check
  if (workspaceBackStack_.isEmpty() || workspaceBackStack_.last() != page) {
    workspaceBackStack_.append(page);
  }
  while (workspaceBackStack_.size() > 80) {
    workspaceBackStack_.removeFirst();
  }
  workspaceForwardStack_.clear();
  updateWorkspaceNavigationActions();
}

// Navigate to the previously visited workspace tab
void MainWindow::goWorkspaceBack() {
    // Multi-branch condition check
  if (!tabs_ || workspaceBackStack_.size() < 2) {
    updateWorkspaceNavigationActions();
    return;
  }
  QWidget *current = workspaceBackStack_.takeLast();
  if (current) {
    workspaceForwardStack_.append(current);
  }
  QWidget *target = workspaceBackStack_.last();
  QSignalBlocker blocker(tabs_);
  suppressWorkspaceHistory_ = true;
  activateWorkspacePage(target);
  suppressWorkspaceHistory_ = false;
  updateWorkspaceNavigationActions();
}

// Re-visit the next workspace tab from the forward stack
void MainWindow::goWorkspaceForward() {
    // Multi-branch condition check
  if (!tabs_ || workspaceForwardStack_.isEmpty()) {
    updateWorkspaceNavigationActions();
    return;
  }
  QWidget *target = workspaceForwardStack_.takeLast();
  QSignalBlocker blocker(tabs_);
  suppressWorkspaceHistory_ = true;
  activateWorkspacePage(target);
  suppressWorkspaceHistory_ = false;
    // Multi-branch condition check
  if (target &&
      (workspaceBackStack_.isEmpty() || workspaceBackStack_.last() != target)) {
    workspaceBackStack_.append(target);
  }
  updateWorkspaceNavigationActions();
}

// Enable/disable back/forward actions based on stack depth
void MainWindow::updateWorkspaceNavigationActions() {
    // Multi-branch condition check
  if (auto *action = findChild<QAction *>("workspaceBackAction")) {
    action->setEnabled(workspaceBackStack_.size() >= 2);
  }
    // Multi-branch condition check
  if (auto *action = findChild<QAction *>("workspaceForwardAction")) {
    action->setEnabled(!workspaceForwardStack_.isEmpty());
  }
}

// Central guard: enable/disable all toolbar/menu actions based on current state
void MainWindow::updateActionAvailability() {
  auto setEnabled = [this](const char *name, bool enabled) {
    if (auto *action = findChild<QAction *>(name)) {
      action->setEnabled(enabled);
    }
    if (auto *button = findChild<QPushButton *>(name)) {
      button->setEnabled(enabled);
    }
  };

  const bool connected = client_.isConnected();
  const bool hasSlave = selectedPosition() >= 0;
  const bool hasAnySlave = !slaves_.isEmpty();
  const bool hasCurrentSdoTarget =
      hasSlave && sdoInspector_->sdoIndex && !sdoInspector_->sdoIndex->text().trimmed().isEmpty() &&
      sdoInspector_->sdoSubIndex && !sdoInspector_->sdoSubIndex->text().trimmed().isEmpty();
  const bool sdoLoadedForSelected = hasSlave &&
                                    loadedSdoPosition_ == selectedPosition() &&
                                    sdo_->sdoTable && sdo_->sdoTable->rowCount() > 0;
  const bool pdoLoadedForSelected = hasSlave &&
                                    loadedPdoPosition_ == selectedPosition() &&
                                    sdo_->pdoTable && sdo_->pdoTable->rowCount() > 0;
  const bool hasDictionarySelection =
      sdoLoadedForSelected && !selectedDictionaryRows().isEmpty();
  bool hasDictionaryValueSelection = false;
  if (sdoLoadedForSelected) {
    hasDictionaryValueSelection =
        sdoDictionaryRowsContainValue(sdo_->sdoTable, selectedDictionaryRows());
  }
  const bool hasObjectBookmarks =
      bookmark_->objectBookmarkTable && bookmark_->objectBookmarkTable->rowCount() > 0;
  const bool hasObjectBookmarkSelection =
      bookmark_->objectBookmarkTable && !selectedObjectBookmarkRows().isEmpty();
  const bool hasSdoTargetTrailSelection =
      sdoTargetTrailTable_ && sdoTargetTrailTable_->currentRow() >= 0;
  const bool hasSdoTargetTrailRows =
      sdoTargetTrailTable_ && sdoTargetTrailTable_->rowCount() > 0;
  const bool canCreateStartupFromSdoTargetTrail =
      hasSdoTargetTrailSelection &&
      sdoTargetTrailRowCanCreateStartup(sdoTargetTrailTable_->currentRow());
  bool hasVisibleDictionaryRows = false;
  if (sdoLoadedForSelected) {
    hasVisibleDictionaryRows = !visibleTableRows(sdo_->sdoTable).isEmpty();
  }
  const bool hasPdoSelection =
      pdoLoadedForSelected && !selectedTableRows(sdo_->pdoTable).isEmpty();
  bool hasIoVariableSelection = false;
  bool hasVisibleIoVariables = false;
  bool hasIoVariableValueSelection = false;
  bool hasVisibleIoVariableValues = false;
    // Multi-branch condition check
  if (ioVar_->ioVariableTable) {
    const QVector<int> selectedIoRows = selectedIoVariableRows(true);
    hasIoVariableSelection = !selectedIoRows.isEmpty();
    hasIoVariableValueSelection =
        ioVariableTableRowsContainValue(ioVar_->ioVariableTable, selectedIoRows);
    const QVector<int> visibleIoRows = visibleIoVariableRows();
    hasVisibleIoVariables = !visibleIoRows.isEmpty();
    hasVisibleIoVariableValues =
        ioVariableTableRowsContainValue(ioVar_->ioVariableTable, visibleIoRows);
  }
  const bool hasHistorySelection = sdoHistoryTable_ &&
                                   sdoHistoryTable_->rowCount() > 0 &&
                                   !selectedSdoHistoryRows().isEmpty();
  bool hasHistoryValueSelection = false;
    // Multi-branch condition check
  if (sdoHistoryTable_) {
    const QVector<int> rows = selectedSdoHistoryRows();
    for (const int row : rows) {
      if (row >= 0 && row < sdoHistoryTable_->rowCount() &&
          !sdoHistoryTable_->isRowHidden(row)) {
        const QString status = sdoHistoryTable_->item(row, 7)
                                   ? sdoHistoryTable_->item(row, 7)->text()
                                   : QString();
        const QString value = sdoHistoryTable_->item(row, 6)
                                  ? sdoHistoryTable_->item(row, 6)->text()
                                  : QString();
        if (!isSdoHistoryStartupSource(status, value)) {
          continue;
        }
        hasHistoryValueSelection = true;
        break;
      }
    }
  }
  const bool hasWatchValueSelection = selectedWatchRowsHaveValue();
  const bool hasHostCommand =
      hostHealthTable_ && hostHealthTable_->currentRow() >= 0 &&
      hostHealthTable_->item(hostHealthTable_->currentRow(), 4) &&
      !hostHealthTable_->item(hostHealthTable_->currentRow(), 4)
           ->text()
           .trimmed()
           .isEmpty();
  const bool hasSelectedObjectRow =
      sdoInspector_->sdoTargetTable && sdoInspector_->sdoTargetTable->currentRow() >= 0 &&
      sdoInspector_->sdoTargetTable->currentRow() < sdoInspector_->sdoTargetTable->rowCount();

  setEnabled("menuConnectAction", true);
  setEnabled("connectAction", true);
  setEnabled("overviewConnect", true);
  setEnabled("menuRefreshAction", connected);
  setEnabled("refreshAction", connected);
  setEnabled("overviewRefresh", connected);
  setEnabled("overviewRunNext", nextCommissioningWorkflowStep() >= 0);
  setEnabled("menuRescanAction", connected);
  setEnabled("rescanAction", connected);
  setEnabled("captureTopologyBaseline", connected && hasAnySlave);
  setEnabled("clearTopologyBaseline", !topologyBaseline_.isEmpty());
  setEnabled("readSdo", connected && hasSlave);
  setEnabled("readTargetSdo", connected && hasCurrentSdoTarget);
  setEnabled("readSelectedDictionary",
             connected && hasSlave && hasDictionarySelection);
  setEnabled("readVisibleDictionary",
             connected && hasSlave && hasVisibleDictionaryRows);
  setEnabled("readFailedDictionary",
             connected && hasSlave && hasFailedSdoEvidence());
  setEnabled("writeSdo", connected && hasSlave && selectedSdoWritable_);
  setEnabled("writeTargetSdo",
             connected && hasCurrentSdoTarget && selectedSdoWritable_);
  setEnabled("useSdoValue", selectedSdoWritable_ && sdoInspector_->sdoValue &&
                                !sdoInspector_->sdoValue->text().trimmed().isEmpty());
  setEnabled("useSdoEvidence",
             selectedSdoWritable_ &&
                 !currentSdoPreferredEvidenceValue().isEmpty());
  setEnabled("pickSdoEvidence",
             selectedSdoWritable_ && !currentSdoEvidenceCandidates().isEmpty());
  setEnabled("contextObjectDictionary", hasSlave);
  setEnabled("contextPdoMap", hasSlave);
  setEnabled("contextWatch", hasSlave);
  setEnabled("contextFreeRun", connected && hasSlave);
  setEnabled("contextDiagnostics", true);
  setEnabled("contextRefreshSlave", connected && hasSlave);
  setEnabled("contextPrepareSnapshot", connected && hasSlave);
  if (sdoInspector_->sdoWriteValue) {
    sdoInspector_->sdoWriteValue->setEnabled(selectedSdoWritable_);
  }
  setEnabled("addStartupSdo", connected && hasSlave);
  setEnabled("startupTargetSdo", connected && hasCurrentSdoTarget);
  updateStartupSdoControls();
  setEnabled("addWatchSdo", hasSlave);
  setEnabled("watchTargetSdo", hasCurrentSdoTarget);
  setEnabled("reviewSdoWriteDelta", currentSdoWriteDeltaReviewAvailable());
  setEnabled("runSdoTargetRowAction", hasSelectedObjectRow);
  setEnabled("copySdoTargetRowEvidence", hasSelectedObjectRow);
  setEnabled("copySdoEvidenceDigest", hasCurrentSdoTarget);
  setEnabled("openSdoWatchLink", currentSdoWatchRow() >= 0);
  setEnabled("openSdoStartupLink", currentSdoStartupRow() >= 0);
  setEnabled("openSdoBookmarkLink", currentSdoBookmarkRow() >= 0);
  setEnabled("openSdoTargetTrailLink", currentSdoTargetTrailRow() >= 0);
  setEnabled("watchSelectedDictionary", hasSlave && hasDictionarySelection);
  setEnabled("watchVisibleDictionary", hasSlave && hasVisibleDictionaryRows);
  setEnabled("startupSelectedEvidence",
             hasSlave && hasDictionaryValueSelection);
  setEnabled("bookmarkTargetSdo", hasSlave && hasCurrentSdoTarget);
  setEnabled("bookmarkSelectedDictionary", hasSlave && hasDictionarySelection);
  setEnabled("fillBookmarkSdo", hasObjectBookmarkSelection);
  setEnabled("watchBookmarkSdo", hasObjectBookmarkSelection);
  setEnabled("startupBookmarkSdo", hasObjectBookmarkSelection);
  setEnabled("removeBookmarkSdo", hasObjectBookmarkSelection);
  setEnabled("restoreSdoTargetTrail", hasSdoTargetTrailSelection);
  setEnabled("watchSdoTargetTrail", hasSdoTargetTrailSelection);
  setEnabled("bookmarkSdoTargetTrail", hasSdoTargetTrailSelection);
  setEnabled("startupSdoTargetTrail", canCreateStartupFromSdoTargetTrail);
  setEnabled("removeSdoTargetTrail", hasSdoTargetTrailSelection);
  setEnabled("clearSdoTargetTrail", hasSdoTargetTrailRows);
  updateSdoTargetRowActionButton();
  updateSdoTargetRowCopyButton();
  if (bookmark_->objectBookmarkTable) {
    bookmark_->objectBookmarkTable->setToolTip(
        uiText(
            "%1 project object bookmark(s). Double-click fills the SDO target; "
            "reads and writes still require explicit user actions.",
            "%1 个工程对象书签。双击只回填 SDO "
            "目标；读取和写入仍需用户显式操作。")
            .arg(hasObjectBookmarks ? bookmark_->objectBookmarkTable->rowCount() : 0));
  }
  setEnabled("addSelectedPdoWatch", hasSlave && hasPdoSelection);
  setEnabled("refreshIoVariables", ioVar_->ioVariableTable != nullptr);
  setEnabled("fillIoVariableSdo", hasIoVariableSelection);
  setEnabled("readIoVariableSdo", connected && hasIoVariableSelection);
  setEnabled("watchSelectedIoVariables", hasIoVariableSelection);
  setEnabled("watchVisibleIoVariables", hasVisibleIoVariables);
  setEnabled("startupSelectedIoVariables", hasIoVariableValueSelection);
  setEnabled("startupVisibleIoVariables", hasVisibleIoVariableValues);
  setEnabled("editIoVariableMetadata", hasIoVariableSelection);
  setEnabled("bulkNameIoVariables",
             hasIoVariableSelection || hasVisibleIoVariables);
  setEnabled("reviewPlcHandoffAction", hasVisibleIoVariables);
  setEnabled("reviewPlcHandoff", hasVisibleIoVariables);
  setEnabled("copySelectedPlcDeclarations", hasIoVariableSelection);
  setEnabled("copyVisiblePlcDeclarations", hasVisibleIoVariables);
  setEnabled("exportIoVariablesAction",
             ioVar_->ioVariableTable && ioVar_->ioVariableTable->rowCount() > 0);
  setEnabled("exportIoVariablesCsv",
             ioVar_->ioVariableTable && ioVar_->ioVariableTable->rowCount() > 0);
  setEnabled("exportIoPlcSymbolsAction",
             ioVar_->ioVariableTable && ioVar_->ioVariableTable->rowCount() > 0);
  setEnabled("exportIoPlcSymbolsCsv",
             ioVar_->ioVariableTable && ioVar_->ioVariableTable->rowCount() > 0);
  setEnabled("exportPlcDeclarationsAction", hasVisibleIoVariables);
  setEnabled("refreshConsistency", consistency_->consistencyTable != nullptr);
  setEnabled("openIoVariablesFromConsistency", ioVariableTabIndex_ >= 0);
  setEnabled("watchSelectedHistory", hasHistorySelection);
  setEnabled("startupFromSelectedHistory", hasHistoryValueSelection);
  setEnabled("addCia402WatchPreset", hasSlave);
  setEnabled("refreshWatch",
             connected && watch_->watchTable && watch_->watchTable->rowCount() > 0);
  setEnabled("captureWatchBaseline",
             watch_->watchTable && watch_->watchTable->rowCount() > 0);
  setEnabled("clearWatchBaseline", watch_->watchTable && watch_->watchTable->rowCount() > 0);
  setEnabled("startupFromSelectedWatch", hasWatchValueSelection);
  setEnabled("syncStartupFromWatch", hasWatchValueSelection);
  setEnabled("clearWatch", watch_->watchTable && watch_->watchTable->rowCount() > 0);
  setEnabled("runHostCheck", connected);
  setEnabled("copyHostCommand", hasHostCommand);
  setEnabled("initAction", connected && hasSlave);
  setEnabled("preOpAction", connected && hasSlave);
  setEnabled("safeOpAction", connected && hasSlave);
  setEnabled("opAction", connected && hasSlave);
  setEnabled("contextInit", connected && hasSlave);
  setEnabled("contextPreOp", connected && hasSlave);
  setEnabled("contextSafeOp", connected && hasSlave);
  setEnabled("contextOp", connected && hasSlave);
  updateDriveNextButton();
  setEnabled("allInitAction", connected && hasAnySlave);
  setEnabled("allPreOpAction", connected && hasAnySlave);
  setEnabled("allSafeOpAction", connected && hasAnySlave);
  setEnabled("allOpAction", connected && hasAnySlave);
  updateSelectedSlavePanel();
  updateCommissioningWorkflow();
  updateNextBestAction();
  updateTabBadges();
}

// Restore application preferences from QSettings (theme, language, scale, masters)
// Restore all persisted preferences from QSettings.
// Covers appearance, master profiles, timing, Free Run, display,
// notifications, and export settings.

// ── Recent Projects Management ────────────────────────────────────────
void MainWindow::addToRecentProjects(const QString &path)
{
    if (path.isEmpty()) return;
    recentProjectPaths_.removeAll(path);
    recentProjectPaths_.prepend(path);
    while (recentProjectPaths_.size() > kMaxRecentProjects)
        recentProjectPaths_.removeLast();
    updateRecentProjectsMenu();
    saveSettings();
}

void MainWindow::updateRecentProjectsMenu()
{
    if (!recentProjectsMenu_) return;
    recentProjectsMenu_->clear();
    recentProjectsMenu_->setEnabled(!recentProjectPaths_.isEmpty());
    for (int i = 0; i < recentProjectPaths_.size(); ++i) {
        const QString &p = recentProjectPaths_[i];
        const QString label = QString("&%1 %2").arg(i + 1).arg(QFileInfo(p).fileName());
        auto *action = recentProjectsMenu_->addAction(label);
        action->setToolTip(p);
        action->setData(p);
        connect(action, &QAction::triggered, this, [this, p]() {
            if (!readProjectFile(p)) {
                QMessageBox::warning(this, uiText("Open Project", "打开工程"),
                                     uiText("Failed to open project.", "工程打开失败。"));
            } else {
                addToRecentProjects(p);
            }
        });
    }
    if (!recentProjectPaths_.isEmpty()) {
        recentProjectsMenu_->addSeparator();
        auto *clearAction = recentProjectsMenu_->addAction(
            uiText("Clear Recent Projects", "清除最近工程"));
        connect(clearAction, &QAction::triggered, this, [this]() {
            recentProjectPaths_.clear();
            updateRecentProjectsMenu();
            saveSettings();
        });
    }
}

void MainWindow::applyCustomShortcuts()
{
    if (settings_.customShortcuts.isEmpty()) return;

    /* Map shortcut IDs → QAction objectName. */
    const QMap<QString, QString> idToObj = {
        {"newProject",       "newProjectAction"},
        {"openProject",      "openProjectAction"},
        {"saveProject",      "saveProjectAction"},
        {"saveProjectAs",    "saveProjectAsAction"},
        {"connect",          "menuConnectAction"},
        {"refresh",          "menuRefreshAction"},
        {"rescan",           "menuRescanAction"},
        {"commandPalette",   "commandPaletteAction"},
        {"settings",         "settingsAction"},
        {"manual",           "manualAction"},
        {"showLog",          "showLogAction"},
        {"workspaceBack",    "workspaceBackAction"},
        {"workspaceForward", "workspaceForwardAction"},
        {"filterFocus",      "filterFocusAction"},
    };
    for (auto it = settings_.customShortcuts.constBegin();
         it != settings_.customShortcuts.constEnd(); ++it) {
        const QString objName = idToObj.value(it.key());
        if (!objName.isEmpty()) {
            auto *action = findChild<QAction *>(objName);
            if (action) action->setShortcut(QKeySequence(it.value()));
            continue;
        }
        /* Tab shortcuts: tab1..tab9, nextTab, prevTab */
        if (it.key().startsWith("tab") || it.key() == "nextTab" || it.key() == "prevTab") {
            int idx = -1;
            if (it.key() == "nextTab") idx = tabSwitchShortcuts_.size() - 2;
            else if (it.key() == "prevTab") idx = tabSwitchShortcuts_.size() - 1;
            else {
                bool ok = false;
                int num = it.key().mid(3).toInt(&ok);
                if (ok && num >= 1 && num <= 9) idx = num - 1;
            }
            if (idx >= 0 && idx < tabSwitchShortcuts_.size()) {
                tabSwitchShortcuts_[idx]->setKey(QKeySequence(it.value()));
            }
        }
    }
}

void MainWindow::loadSettings() {
  QSettings s("NekoEcatStudio", "NekoEcatStudio");

  // ── Appearance ────────────────────────────────────────────────
  settings_.theme = s.value("preferences/theme", "Dark").toString();
  settings_.language = s.value("preferences/language", "English").toString();
  settings_.scale = s.value("preferences/scale", 1.0).toDouble();

  // ── Masters ───────────────────────────────────────────────────
  settings_.masters.clear();
  const int count = s.beginReadArray("preferences/masters");
  for (int i = 0; i < count; ++i) {
    s.setArrayIndex(i);
    MasterProfile profile;
    profile.name = s.value("name", QString("Master %1").arg(i)).toString();
    profile.target = s.value("target", QString::number(i)).toString().trimmed();
    if (!profile.target.isEmpty()) settings_.masters.append(profile);
  }
  s.endArray();
  if (settings_.masters.isEmpty()) settings_.masters.append(MasterProfile{});
  settings_.activeMaster = s.value("preferences/activeMaster", settings_.masters.first().target).toString().trimmed();
  if (settings_.activeMaster.isEmpty()) settings_.activeMaster = settings_.masters.first().target;
  bool known = false;
  for (const auto &p : settings_.masters) {
    if (p.target == settings_.activeMaster) { known = true; break; }
  }
  if (!known) settings_.masters.prepend(MasterProfile{QString("Master %1").arg(settings_.activeMaster), settings_.activeMaster});

  // ── Timing ────────────────────────────────────────────────────
  settings_.watchAutoRefreshMs = s.value("timing/watchAutoRefreshMs", 0).toInt();
  settings_.overviewAutoRefreshMs = s.value("timing/overviewAutoRefreshMs", 0).toInt();
  settings_.sdoReadTimeoutMs = s.value("timing/sdoReadTimeoutMs", 3000).toInt();
  settings_.sdoWriteTimeoutMs = s.value("timing/sdoWriteTimeoutMs", 5000).toInt();
  settings_.topologyPollIntervalMs = s.value("timing/topologyPollIntervalMs", 0).toInt();

  // ── Free Run ──────────────────────────────────────────────────
  settings_.freeRunCycleUs = s.value("freerun/cycleUs", 1000).toInt();
  settings_.freeRunAutoName = s.value("freerun/autoName", true).toBool();
  settings_.freeRunHighlightChanges = s.value("freerun/highlightChanges", true).toBool();

  // ── Display ───────────────────────────────────────────────────
  settings_.showRawTabs = s.value("display/showRawTabs", false).toBool();
  settings_.showColumnGrid = s.value("display/showColumnGrid", false).toBool();
  settings_.detailPanelWidth = s.value("display/detailPanelWidth", 360).toInt();
  settings_.tableRowHeight = s.value("display/tableRowHeight", 28).toInt();
  settings_.alternatingRowColors = s.value("display/alternatingRowColors", true).toBool();
  settings_.compactMode = s.value("display/compactMode", false).toBool();
  settings_.maxHistoryEntries = s.value("display/maxHistoryEntries", 200).toInt();

  // ── Notifications ─────────────────────────────────────────────
  settings_.notifyOnStateChange = s.value("notifications/onStateChange", true).toBool();
  settings_.notifyOnError = s.value("notifications/onError", true).toBool();
  settings_.notifyOnWatchDrift = s.value("notifications/onWatchDrift", false).toBool();
  settings_.soundEnabled = s.value("notifications/soundEnabled", false).toBool();
  settings_.toastDurationMs = s.value("notifications/toastDurationMs", 3000).toInt();

  // ── Export ────────────────────────────────────────────────────
  settings_.defaultExportDir = s.value("export/defaultDir", "").toString();
  settings_.esiRepositoryPath = s.value("export/esiPath", "").toString();
  settings_.exportIncludeTimestamp = s.value("export/includeTimestamp", true).toBool();
  settings_.exportIncludeMetadata = s.value("export/includeMetadata", true).toBool();
  settings_.csvDelimiter = s.value("export/csvDelimiter", ",").toString();

  // ── Recent Projects ──────────────────────────────────────────
  recentProjectPaths_ = s.value("recentProjects/paths").toStringList();

  // ── Custom Shortcuts ──────────────────────────────────────────
  settings_.customShortcuts.clear();
  const int scCount = s.beginReadArray("shortcuts/custom");
  for (int i = 0; i < scCount; ++i) {
      s.setArrayIndex(i);
      const QString id = s.value("id").toString();
      const QString seq = s.value("sequence").toString();
      if (!id.isEmpty() && !seq.isEmpty())
          settings_.customShortcuts[id] = seq;
  }
  s.endArray();
}

// Persist all current preferences to QSettings.
void MainWindow::saveSettings() {
  QSettings s("NekoEcatStudio", "NekoEcatStudio");

  // ── Appearance ────────────────────────────────────────────────
  s.setValue("preferences/theme", settings_.theme);
  s.setValue("preferences/language", settings_.language);
  s.setValue("preferences/scale", settings_.scale);
  s.setValue("preferences/activeMaster", settings_.activeMaster);
  s.beginWriteArray("preferences/masters");
  for (int i = 0; i < settings_.masters.size(); ++i) {
    s.setArrayIndex(i);
    s.setValue("name", settings_.masters[i].name);
    s.setValue("target", settings_.masters[i].target);
  }
  s.endArray();

  // ── Timing ────────────────────────────────────────────────────
  s.setValue("timing/watchAutoRefreshMs", settings_.watchAutoRefreshMs);
  s.setValue("timing/overviewAutoRefreshMs", settings_.overviewAutoRefreshMs);
  s.setValue("timing/sdoReadTimeoutMs", settings_.sdoReadTimeoutMs);
  s.setValue("timing/sdoWriteTimeoutMs", settings_.sdoWriteTimeoutMs);
  s.setValue("timing/topologyPollIntervalMs", settings_.topologyPollIntervalMs);

  // ── Free Run ──────────────────────────────────────────────────
  s.setValue("freerun/cycleUs", settings_.freeRunCycleUs);
  s.setValue("freerun/autoName", settings_.freeRunAutoName);
  s.setValue("freerun/highlightChanges", settings_.freeRunHighlightChanges);

  // ── Display ───────────────────────────────────────────────────
  s.setValue("display/showRawTabs", settings_.showRawTabs);
  s.setValue("display/showColumnGrid", settings_.showColumnGrid);
  s.setValue("display/detailPanelWidth", settings_.detailPanelWidth);
  s.setValue("display/tableRowHeight", settings_.tableRowHeight);
  s.setValue("display/alternatingRowColors", settings_.alternatingRowColors);
  s.setValue("display/compactMode", settings_.compactMode);
  s.setValue("display/maxHistoryEntries", settings_.maxHistoryEntries);

  // ── Notifications ─────────────────────────────────────────────
  s.setValue("notifications/onStateChange", settings_.notifyOnStateChange);
  s.setValue("notifications/onError", settings_.notifyOnError);
  s.setValue("notifications/onWatchDrift", settings_.notifyOnWatchDrift);
  s.setValue("notifications/soundEnabled", settings_.soundEnabled);
  s.setValue("notifications/toastDurationMs", settings_.toastDurationMs);

  // ── Export ────────────────────────────────────────────────────
  s.setValue("export/defaultDir", settings_.defaultExportDir);
  s.setValue("export/esiPath", settings_.esiRepositoryPath);
  s.setValue("export/includeTimestamp", settings_.exportIncludeTimestamp);
  s.setValue("export/includeMetadata", settings_.exportIncludeMetadata);
  s.setValue("export/csvDelimiter", settings_.csvDelimiter);

  // ── Recent Projects ──────────────────────────────────────────
  s.setValue("recentProjects/paths", recentProjectPaths_);

  // ── Custom Shortcuts ──────────────────────────────────────────
  s.remove("shortcuts/custom");
  s.beginWriteArray("shortcuts/custom");
  int scIdx = 0;
  for (auto it = settings_.customShortcuts.constBegin();
       it != settings_.customShortcuts.constEnd(); ++it, ++scIdx) {
      s.setArrayIndex(scIdx);
      s.setValue("id", it.key());
      s.setValue("sequence", it.value());
  }
  s.endArray();
}

// Open settings dialog; apply changes and rebuild UI if language changed
// Open settings dialog with live theme preview.
// When the user selects a theme in the combo, it is applied immediately
// as a temporary preview. On OK the new settings are persisted; on Cancel
// the original theme (and all other settings) are restored.
void MainWindow::openSettings() {
  const QString previousLanguage = settings_.language;
  const QString previousMaster = settings_.activeMaster;
  const QString previousTheme = settings_.theme;

  SettingsDialog dialog(settings_, this);

  /* Live theme preview: temporarily apply each theme as the user selects it. */
  connect(&dialog, &SettingsDialog::themePreviewRequested,
          this, [this](const QString &theme) {
    settings_.theme = theme;
    applyTheme();
  });

  if (dialog.exec() != QDialog::Accepted) {
    /* Revert to the original theme on cancel. */
    settings_.theme = previousTheme;
    applyTheme();
    return;
  }

  settings_ = dialog.settings();
  if (settings_.activeMaster != previousMaster) {
    clearOnlineViews();
  }
  client_.setMasterTarget(settings_.activeMaster);
  saveSettings();

  if (settings_.language != previousLanguage) {
    rebuildUi();
    applySettings();
    applyCustomShortcuts();
    QMessageBox::information(this, uiText("Settings", "设置"),
                             uiText("Language was applied.", "语言已应用。"));
    return;
  }
  applySettings();
  applyCustomShortcuts();
  QMessageBox::information(this, uiText("Settings", "设置"),
                           uiText("Settings were applied.", "设置已应用。"));
}

// Multi-language string selector — returns localized string based on active language.
// For Chinese Simplified, uses the inline zh parameter (preserving existing behavior).
// For other languages, delegates to TranslationRegistry for lookup by English key.
// Falls back to English if no translation is found.
// Translation now handled by Qt .ts/.qm system (QTranslator loaded in main.cpp).
// This function is a passthrough kept for incremental migration compatibility.
QString MainWindow::uiText(const QString &english, const QString & /*zh*/) const {
  return english;
}

// Display label for the active master (name + target)
QString MainWindow::activeMasterName() const {
  for (const auto &profile : settings_.masters) {
    if (profile.target == settings_.activeMaster) {
      return QString("%1  [%2]").arg(profile.name, profile.target);
    }
  }
  return QString("Master %1").arg(settings_.activeMaster);
}

// Rebuild the master combo box from the current settings profile list
void MainWindow::refreshMasterSelector() {
  if (!masterCombo_) {
    return;
  }
  QSignalBlocker blocker(masterCombo_);
  masterCombo_->clear();
  const QVector<MasterProfile> masters =
      settings_.masters.isEmpty() ? QVector<MasterProfile>{MasterProfile{}}
                                  : settings_.masters;
  int activeIndex = 0;
  for (int i = 0; i < masters.size(); ++i) {
    const QString label =
        QString("%1  [%2]").arg(masters[i].name, masters[i].target);
    masterCombo_->addItem(label, masters[i].target);
    // Multi-branch condition check
    if (masters[i].target == settings_.activeMaster) {
      activeIndex = i;
    }
  }
  masterCombo_->setCurrentIndex(activeIndex);
}

// Switch the active EtherCAT master — clears views if the target changed
void MainWindow::setActiveMaster(const QString &target) {
  const QString next = target.trimmed().isEmpty() ? "0" : target.trimmed();
    // Multi-branch condition check
  if (next == settings_.activeMaster) {
    return;
  }
    // Multi-branch condition check
  if (freeRun_ && client_.isConnected()) {
    client_.freeRunStop();
  }
  settings_.activeMaster = next;
  bool known = false;
  for (const auto &profile : settings_.masters) {
    // Multi-branch condition check
    if (profile.target == next) {
      known = true;
      break;
    }
  }
  if (!known) {
    settings_.masters.append(
        MasterProfile{QString("Master %1").arg(next), next});
  }
  client_.setMasterTarget(next);
  saveSettings();
  refreshMasterSelector();
  clearOnlineViews();
  log(QString("Active master changed to %1").arg(activeMasterName()));
  updateDiagnostics("Info", "Master",
                    QString("Active master: %1").arg(activeMasterName()));
  updateActionAvailability();
  if (client_.isConnected()) {
    requestRefresh();
  }
}

// Build and show a confirmation dialog with severity-categorized details
bool MainWindow::confirmDangerousOperation(const QString &title,
                                           const QString &summary,
                                           const QStringList &details,
                                           const QString &confirmText) {
  QDialog dialog(this);
  dialog.setObjectName("dangerConfirmDialog");
  dialog.setWindowTitle(title);
  dialog.setModal(true);
  dialog.resize(760, 560);

  auto *layout = new QVBoxLayout(&dialog);
  layout->setContentsMargins(18, 18, 18, 16);
  layout->setSpacing(12);

  auto *heading = new QLabel(title);
  heading->setObjectName("dialogTitle");
  heading->setWordWrap(true);

  auto *summaryLabel = new QLabel(summary);
  summaryLabel->setObjectName("diagnosticsSummary");
  summaryLabel->setWordWrap(true);
  summaryLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

  QStringList criticalItems;
  QStringList reviewItems;
  QStringList evidenceItems;
  QStringList contextItems;
  QStringList otherItems;

  auto containsAny = [](const QString &text,
                        std::initializer_list<const char *> words) {
    const QString lower = text.toLower();
    for (const char *word : words) {
    // Multi-branch condition check
      if (lower.contains(QString::fromUtf8(word).toLower())) {
        return true;
      }
    }
    return false;
  };
  auto startsWithAny = [](const QString &text,
                          std::initializer_list<const char *> words) {
    const QString lower = text.trimmed().toLower();
    for (const char *word : words) {
    // Multi-branch condition check
      if (lower.startsWith(QString::fromUtf8(word).toLower())) {
        return true;
      }
    }
    return false;
  };

  for (const QString &detail : details) {
    const QString trimmed = detail.trimmed();
    // Multi-branch condition check
    if (trimmed.isEmpty()) {
      continue;
    }
    if (startsWithAny(trimmed,
                      {"risk:", "风险：", "critical:", "critical："}) ||
        containsAny(trimmed,
                    {"drive command", "drive/mode", "actuator safety",
                     "persistent storage", "output behavior",
                     "outputs and drive behavior", "op can make outputs",
                     "can affect real hardware", "驱动命令", "驱动/模式",
                     "执行机构安全", "持久化", "输出行为", "op 可能"})) {
      criticalItems << trimmed;
    } else if (containsAny(trimmed, {"validation warning",
                                     "topology baseline",
                                     "consistency "
                                     "gate: not run",
                                     "consistency gate: stale",
                                     "consistency gate: ",
                                     "evidence set conflict",
                                     "write target: differs",
                                     "differs from",
                                     "mismatch",
                                     "missing",
                                     "no live",
                                     "no process",
                                     "no watch",
                                     "no local",
                                     "failed",
                                     "error",
                                     "warning",
                                     "stale",
                                     "校验警告",
                                     "拓扑基线",
                                     "一致性门禁",
                                     "证据集冲突",
                                     "写入目标：不同",
                                     "不同于",
                                     "不一致",
                                     "缺失",
                                     "没有",
                                     "无监视",
                                     "无可比较",
                                     "失败",
                                     "错误",
                                     "警告",
                                     "过期",
                                     "未运行"})) {
      reviewItems << trimmed;
    } else if (containsAny(trimmed, {"evidence",
                                     "watch",
                                     "startup",
                                     "pdo",
                                     "free run",
                                     "object class",
                                     "dictionary",
                                     "write target",
                                     "change preview",
                                     "current evidence",
                                     "current source",
                                     "drive evidence",
                                     "evidence score",
                                     "证据",
                                     "watch",
                                     "startup",
                                     "pdo",
                                     "free run",
                                     "对象类别",
                                     "对象字典",
                                     "写入目标",
                                     "变更预览",
                                     "当前证据",
                                     "当前值",
                                     "驱动证据"})) {
      evidenceItems << trimmed;
    } else if (startsWithAny(trimmed, {"master:",
                                       "slave:",
                                       "row:",
                                       "object:",
                                       "type:",
                                       "value:",
                                       "requested state:",
                                       "current state:",
                                       "detected slaves:",
                                       "current state mix:",
                                       "主站：",
                                       "从站：",
                                       "行：",
                                       "对象：",
                                       "类型：",
                                       "值：",
                                       "目标状态：",
                                       "当前状态：",
                                       "检测到从站：",
                                       "当前状态分布："}) ||
               containsAny(trimmed, {"this operation", "this sends",
                                     "confirm machine safety", "此操作",
                                     "继续前请确认"})) {
      contextItems << trimmed;
    } else {
      otherItems << trimmed;
    }
  }

  const QString severity =
      !criticalItems.isEmpty()
          ? uiText("Critical impact items require explicit review.",
                   "存在关键影响项，需要明确复核。")
          : (!reviewItems.isEmpty()
                 ? uiText("Review warnings before confirming.",
                          "确认前请复核警告项。")
                 : uiText("No high-risk evidence flags were detected, but this "
                          "is still an online operation.",
                          "未发现高风险证据标记，但这仍是在线操作。"));
  auto *severityLabel = new QLabel(severity);
  severityLabel->setObjectName("diagnosticsSummary");
  severityLabel->setWordWrap(true);
  severityLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
  severityLabel->setStyleSheet(
      !criticalItems.isEmpty()
          ? QStringLiteral("QLabel { color: #ef4444; font-weight: 700; }")
          : (!reviewItems.isEmpty()
                 ? QStringLiteral("QLabel { color: #f59e0b; font-weight: "
                                  "700; }")
                 : QStringLiteral("QLabel { color: #16a34a; font-weight: "
                                  "700; }")));

  auto sectionHtml = [this](const QString &title, const QString &subtitle,
                            const QString &color,
                            const QStringList &items) -> QString {
    if (items.isEmpty()) {
      return QString();
    }
    QString html =
        QString("<section style='border:1px solid %1; border-left:4px solid "
                "%1; border-radius:8px; padding:10px 12px; margin:10px "
                "0;'>")
            .arg(color);
    html += QString("<h3 style='margin:0 0 4px 0; color:%1; font-size:15px;'>"
                    "%2</h3>")
                .arg(color, title.toHtmlEscaped());
    if (!subtitle.trimmed().isEmpty()) {
      html += QString("<p style='margin:0 0 6px 0; color:%1;'>%2</p>")
                  .arg(settings_.theme == "Light" ? "#475569" : "#b9c6d6",
                       subtitle.toHtmlEscaped());
    }
    html += "<ul style='margin:6px 0 0 18px; padding:0;'>";
    for (const QString &item : items) {
      html += QString("<li style='margin:4px 0;'>%1</li>")
                  .arg(item.toHtmlEscaped());
    }
    html += "</ul></section>";
    return html;
  };

  const QString foreground = settings_.theme == "Light"
                                 ? QStringLiteral("#172033")
                                 : QStringLiteral("#e6edf5");
  const QString muted = settings_.theme == "Light" ? QStringLiteral("#475569")
                                                   : QStringLiteral("#b9c6d6");
  const QString background = settings_.theme == "Light"
                                 ? QStringLiteral("#ffffff")
                                 : QStringLiteral("#151b25");
  const QString border = settings_.theme == "Light" ? QStringLiteral("#d9e1ec")
                                                    : QStringLiteral("#2a3546");

  QString html = QStringLiteral("<!doctype html><html><body>");
  html += QString("<div style='font-family:Inter, Segoe UI, sans-serif; "
                  "font-size:13px; line-height:1.5; color:%1;'>")
              .arg(foreground);
  html += sectionHtml(uiText("Critical Impact", "关键影响"),
                      uiText("Items that can move outputs, drives, persistent "
                             "parameters, or machine behavior.",
                             "可能影响输出、驱动、持久参数或设备行为的项目。"),
                      "#ef4444", criticalItems);
  html += sectionHtml(uiText("Review Before Confirming", "确认前复核"),
                      uiText("Warnings, mismatches, stale gates, missing "
                             "evidence, or topology concerns.",
                             "警告、不一致、过期门禁、缺失证据或拓扑问题。"),
                      "#f59e0b", reviewItems);
  html += sectionHtml(uiText("Evidence", "证据"),
                      uiText("Loaded local or live evidence used to explain "
                             "the requested operation.",
                             "用于解释本次操作的已加载本地或实时证据。"),
                      "#2563eb", evidenceItems);
  html += sectionHtml(uiText("Target Context", "目标上下文"),
                      uiText("Master, slave, object, value, and operation "
                             "scope.",
                             "主站、从站、对象、数值和操作范围。"),
                      "#64748b", contextItems);
  html += sectionHtml(uiText("Other Details", "其他细节"), QString(), "#64748b",
                      otherItems);
  html +=
      QString("<p style='color:%1; margin:10px 0 0 0;'>%2</p>")
          .arg(muted,
               uiText("The confirm button only authorizes the operation; "
                      "the existing validation, runtime request, and "
                      "result logging paths remain unchanged.",
                      "确认按钮只授权本次操作；现有校验、运行时请求和结果记录"
                      "路径保持不变。")
                   .toHtmlEscaped());
  html += "</div></body></html>";

  auto *review = new QTextBrowser;
  review->setObjectName("dangerImpactReview");
  review->setReadOnly(true);
  review->setOpenExternalLinks(false);
  review->setFrameShape(QFrame::NoFrame);
  review->setStyleSheet(QString("QTextBrowser#dangerImpactReview { background: "
                                "%1; border: 1px solid %2; border-radius: 8px; "
                                "padding: 10px; }")
                            .arg(background, border));
  review->setHtml(html);

  auto *buttons = new QDialogButtonBox(QDialogButtonBox::Cancel);
  auto *confirm = buttons->addButton(confirmText, QDialogButtonBox::AcceptRole);
  if (auto *cancel = buttons->button(QDialogButtonBox::Cancel)) {
    cancel->setText(uiText("Cancel", "取消"));
    cancel->setDefault(true);
    cancel->setAutoDefault(true);
  }
  confirm->setAutoDefault(false);
  connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
  connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

  layout->addWidget(heading);
  layout->addWidget(summaryLabel);
  layout->addWidget(severityLabel);
  layout->addWidget(review, 1);
  layout->addWidget(buttons);

  const bool accepted = dialog.exec() == QDialog::Accepted;
  updateDiagnostics(
      accepted ? "Warning" : "Info", "Safety",
      QString("%1: %2").arg(title, accepted ? uiText("confirmed", "已确认")
                                            : uiText("cancelled", "已取消")));
  return accepted;
}

QStringList
MainWindow::stateTransitionImpactDetails(int position,
                                         const QString &requestedState) const {
  QStringList details;

  const QString target = requestedState.trimmed().toUpper();
  const bool identityLoaded = loadedSlaveInfoPosition_ == position;
  const bool odLoaded = loadedSdoPosition_ == position;
  const bool pdoLoaded = loadedPdoPosition_ == position;
  const int identityRows =
      identityLoaded && identityTable_ ? identityTable_->rowCount() : 0;
  const int odRows = odLoaded && sdo_->sdoTable ? sdo_->sdoTable->rowCount() : 0;
  const int pdoRows = pdoLoaded && sdo_->pdoTable ? sdo_->pdoTable->rowCount() : 0;

  int watchRows = 0;
  int watchValueRows = 0;
  QString statusword;
  QString modeDisplay;
  QString errorCode;
    // Multi-branch condition check
  if (watch_->watchTable) {
    for (int row = 0; row < watch_->watchTable->rowCount(); ++row) {
    // Multi-branch condition check
      if (tableText(watch_->watchTable, row, 1).toInt() != position) {
        continue;
      }
      ++watchRows;
      const QString value = tableText(watch_->watchTable, row, 4);
      const QString decoded = tableText(watch_->watchTable, row, 5);
    // Multi-branch condition check
      if (!value.isEmpty()) {
        ++watchValueRows;
      }
      const QString index = normalizeHexText(tableText(watch_->watchTable, row, 2), 4);
    // Multi-branch condition check
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
    // Multi-branch condition check
  if (startupSdoTable_) {
    for (int row = 0; row < startupSdoTable_->rowCount(); ++row) {
    // Multi-branch condition check
      if (tableText(startupSdoTable_, row, 0).toInt() != position) {
        continue;
      }
      ++startupRows;
    // Multi-branch condition check
      if (hasStartupDiffEvidence(tableText(startupSdoTable_, row, 8))) {
        ++startupDiffs;
      }
    }
  }

  int freeRunRows = 0;
  int mapIssues = 0;
    // Multi-branch condition check
  if (freeRunWidgets_->freeRunEntryTable) {
    for (int row = 0; row < freeRunWidgets_->freeRunEntryTable->rowCount(); ++row) {
      if (tableText(freeRunWidgets_->freeRunEntryTable, row, 0).toInt() != position) {
        continue;
      }
      ++freeRunRows;
      if (hasPdoMapIssueEvidence(tableText(freeRunWidgets_->freeRunEntryTable, row, 13))) {
// Aggregate detailed evidence for a slave's state transition target
        ++mapIssues;
      }
    }
  }

  int evidenceScore = 0;
  evidenceScore += identityRows > 0 ? 1 : 0;
  evidenceScore += odRows > 0 ? 1 : 0;
  evidenceScore += pdoRows > 0 ? 1 : 0;
  evidenceScore += watchValueRows > 0 ? 1 : 0;
  evidenceScore += freeRunRows > 0 ? 1 : 0;

  details << uiText("Evidence score: %1/5 (ID %2, OD %3, PDO %4, Watch "
                    "%5/%6 values, Free Run %7)",
                    "证据完整度：%1/5（身份 %2，OD %3，PDO %4，Watch %5/%6 "
                    "有值，Free Run %7）")
                 .arg(evidenceScore)
                 .arg(identityRows > 0 ? uiText("ready", "就绪")
                                       : uiText("missing", "缺失"))
                 .arg(odRows)
                 .arg(pdoRows)
                 .arg(watchValueRows)
                 .arg(watchRows)
                 .arg(freeRunRows);

    // Multi-branch condition check
  if (!statusword.isEmpty() || !modeDisplay.isEmpty() || !errorCode.isEmpty()) {
    QStringList driveFacts;
    // Multi-branch condition check
    if (!statusword.isEmpty()) {
      driveFacts << uiText("statusword %1", "状态字 %1").arg(statusword);
    }
    // Multi-branch condition check
    if (!modeDisplay.isEmpty()) {
      driveFacts << uiText("mode %1", "模式 %1").arg(modeDisplay);
    }
    // Multi-branch condition check
    if (!errorCode.isEmpty()) {
      driveFacts << uiText("error %1", "错误 %1").arg(errorCode);
    }
    details << uiText("Drive evidence: %1", "驱动证据：%1")
                   .arg(driveFacts.join(" | "));
  } else {
    details << uiText("Drive evidence: no CiA 402 Watch values",
                      "驱动证据：没有 CiA 402 Watch 值");
  }

    // Multi-branch condition check
  if (startupRows > 0 || startupDiffs > 0) {
    details << uiText("Startup evidence: %1 row(s), %2 Watch mismatch(es)",
                      "Startup 证据：%1 行，%2 条 Watch 不一致")
                   .arg(startupRows)
                   .arg(startupDiffs);
  }
    // Multi-branch condition check
  if (mapIssues > 0) {
    details << uiText("PDO map evidence: %1 Free Run map issue(s)",
                      "PDO 映射证据：Free Run 有 %1 个映射问题")
                   .arg(mapIssues);
  }

  const QStringList topologyIssues = topologyBaselineIssues();
    // Multi-branch condition check
  if (!topologyIssues.isEmpty()) {
    details << uiText("Topology baseline: %1 issue(s); review before state "
                      "transition",
                      "拓扑基线：%1 个问题；切换状态前请复核")
                   .arg(topologyIssues.size());
  }
  details << consistencyGateDetails(uiText("state transition", "状态切换"));

    // Multi-branch condition check
  if (target == "OP" || target == "SAFEOP") {
    // Multi-branch condition check
    if (pdoRows <= 0) {
      details << uiText("Risk: PDO Map is not loaded for this slave",
                        "风险：当前从站尚未加载 PDO 映射");
    }
    // Multi-branch condition check
    if (watchValueRows <= 0) {
      details << uiText("Risk: no live Watch values for this slave",
                        "风险：当前从站没有实时 Watch 值");
    }
    if (target == "OP" && freeRunRows <= 0) {
      details << uiText("Risk: no process-image evidence before OP",
                        "风险：进入 OP 前没有过程映像证据");
    }
    if (startupDiffs > 0) {
      details << uiText("Risk: Startup SDO expectations differ from Watch",
                        "风险：Startup SDO 期望值和 Watch 不一致");
    }
  }

  return details;
}

// Heuristic: recommend the next safe EtherCAT state for a slave based on evidence
void MainWindow::clearOnlineViews() {
  slaves_.clear();
  lastMasterText_.clear();
  lastSlaveInfoText_.clear();
  lastPdoText_.clear();
  lastSdoText_.clear();
  lastXmlText_.clear();
  loadedSlaveInfoPosition_ = -1;
  loadedPdoPosition_ = -1;
  loadedSdoPosition_ = -1;
  loadedXmlPosition_ = -1;
  freeRunEntryNames_.clear();
  freeRunObjectNames_.clear();
  freeRunEntryValues_.clear();
  consistencyFresh_ = false;
  watchValues_.clear();
  sdoEvidence_.clear();
  watchChangedKeys_.clear();
  pendingSdoReads_.clear();
  pendingSdoReadTypes_.clear();
  pendingSdoWrites_.clear();
  pendingSdoVerifications_.clear();
  pendingStartupSdoChecks_.clear();
  topologyBaseline_.clear();
  selectedSdoWritable_ = true;
    // Multi-branch condition check
  if (sdoInspector_->sdoWriteValue) {
    sdoInspector_->sdoWriteValue->setEnabled(true);
    sdoInspector_->sdoWriteValue->setPlaceholderText(uiText("Value to write", "写入值"));
  }
  if (topologyTree_) {
    topologyTree_->clear();
  }
  for (auto *table :
       {metricTable_, workflow_->workflowTable, stateMachine_->stateMachineTable, identityTable_,
        slaveEvidence_->slaveEvidenceMatrixTable, portTable_, mailboxTable_, sdo_->pdoTable,
        sdo_->sdoTable, sdoHistoryTable_, freeRunWidgets_->freeRunTable, freeRunWidgets_->freeRunEntryTable,
        ioVar_->ioVariableTable, watch_->watchTable}) {
    // Multi-branch condition check
    if (table) {
      table->clear();
      table->setRowCount(0);
      table->setColumnCount(0);
    }
  }
  ensureWatchTable();
  ensureSdoHistoryTable();
  updateSdoHistoryRowDetail();
  updateSdoTargetTrailRowDetail();
  updateObjectBookmarkRowDetail();
  for (auto *editor : {rawText_->masterText, rawText_->infoText, rawText_->pdoText, rawText_->sdoText, rawText_->xmlText}) {
    if (editor) {
      editor->clear();
    }
  }
  if (selectedLabel_) {
    selectedLabel_->setText(activeMasterName());
  }
  setMetricCard(masterStateLabel_, uiText("Master", "主站"),
                uiText("Idle", "空闲"));
  setMetricCard(slaveCountLabel_, uiText("Slaves", "从站"), "0");
  setMetricCard(linkStateLabel_, uiText("Link", "链路"),
                uiText("Unknown", "未知"));
  setMetricCard(lossLabel_, uiText("Frame Loss", "丢帧"), "0");
  setMetricCard(freeRunLabel_, uiText("Free Run", "自由运行"),
                freeRun_ ? uiText("On", "开启") : uiText("Off", "关闭"));
  updateWatchAutoRefresh();
  updateIoVariableTable();
  updateSelectedSlavePanel();
  updateTopologyBaselineSummary();
  updateCommissioningWorkflow();
  updateStateMachineView();
  updateStatusBar();
}

// Connect all Qt signals/slots for the entire UI — called once after buildUi()
void MainWindow::wire() {
  disconnect(&client_, nullptr, this, nullptr);
    // Multi-branch condition check
  if (connectRetryTimer_) {
    connectRetryTimer_->stop();
    disconnect(connectRetryTimer_, nullptr, this, nullptr);
    connectRetryTimer_->deleteLater();
    connectRetryTimer_ = nullptr;
  }
    // Multi-branch condition check
  if (refreshTimer_) {
    refreshTimer_->stop();
    disconnect(refreshTimer_, nullptr, this, nullptr);
    refreshTimer_->deleteLater();
    refreshTimer_ = nullptr;
  }
  if (watchRefreshTimer_) {
    watchRefreshTimer_->stop();
    disconnect(watchRefreshTimer_, nullptr, this, nullptr);
    watchRefreshTimer_->deleteLater();
    watchRefreshTimer_ = nullptr;
  }

  auto findAction = [this](const char *name) {
    for (auto *action : findChildren<QAction *>()) {
      if (action->objectName() == name) {
        return action;
      }
    }
    return static_cast<QAction *>(nullptr);
  };

  connect(findAction("connectAction"), &QAction::triggered, &client_,
          &EcatClient::connectToDaemon);
  connect(findAction("refreshAction"), &QAction::triggered, this,
          &MainWindow::requestRefresh);
  connect(findAction("rescanAction"), &QAction::triggered, &client_,
          &EcatClient::rescan);
  connect(findAction("menuConnectAction"), &QAction::triggered, &client_,
          &EcatClient::connectToDaemon);
  connect(findAction("menuRefreshAction"), &QAction::triggered, this,
          &MainWindow::requestRefresh);
  connect(findAction("menuRescanAction"), &QAction::triggered, &client_,
          &EcatClient::rescan);
  connect(findAction("newProjectAction"), &QAction::triggered, this,
          &MainWindow::newProject);
  connect(findAction("openProjectAction"), &QAction::triggered, this,
          &MainWindow::openProject);
  connect(findAction("saveProjectAction"), &QAction::triggered, this,
          &MainWindow::saveProject);
  connect(findAction("saveProjectAsAction"), &QAction::triggered, this,
          &MainWindow::saveProjectAs);
  connect(findAction("exportReportAction"), &QAction::triggered, this,
          &MainWindow::exportDiagnosticsReport);
  connect(findAction("exportIoVariablesAction"), &QAction::triggered, this,
          &MainWindow::exportIoVariablesCsv);
  connect(findAction("exportIoPlcSymbolsAction"), &QAction::triggered, this,
          &MainWindow::exportIoVariablesPlcCsv);
  connect(findAction("exportPlcDeclarationsAction"), &QAction::triggered, this,
          &MainWindow::exportIoVariablesPlcDeclarationsSt);
  connect(findAction("reviewPlcHandoffAction"), &QAction::triggered, this,
          &MainWindow::reviewPlcHandoffIssues);
  connect(findAction("exportPdoMapAction"), &QAction::triggered, this,
          &MainWindow::exportPdoMapCsv);
  connect(findAction("exportSdoDictionaryAction"), &QAction::triggered, this,
          &MainWindow::exportSdoDictionaryCsv);
  connect(findAction("exportSdoHistoryAction"), &QAction::triggered, this,
          &MainWindow::exportSdoHistoryCsv);
  connect(findAction("exportWatchAction"), &QAction::triggered, this,
          &MainWindow::exportWatchCsv);
  connect(findAction("exportStartupSdoAction"), &QAction::triggered, this,
          &MainWindow::exportStartupSdoCsv);
  connect(findAction("exportEsiRepositoryAction"), &QAction::triggered, this,
          &MainWindow::exportEsiRepositoryCsv);
  connect(findAction("exportEsiXmlAction"), &QAction::triggered, this,
          &MainWindow::exportEsiXml);
  connect(findAction("exportTopologyAction"), &QAction::triggered, this,
          &MainWindow::exportTopologyCsv);
  connect(findAction("exportHostHealthAction"), &QAction::triggered, this,
          &MainWindow::exportHostHealthCsv);
  connect(findAction("exportPdoRawAction"), &QAction::triggered, this,
          &MainWindow::exportPdoRawText);
  connect(findAction("exportSdoRawAction"), &QAction::triggered, this,
          &MainWindow::exportSdoRawText);
  connect(findAction("exportMasterRawAction"), &QAction::triggered, this,
          &MainWindow::exportMasterRawText);
  connect(findAction("exportSlaveRawAction"), &QAction::triggered, this,
          &MainWindow::exportSlaveRawText);
  connect(findAction("importEsiAction"), &QAction::triggered, this,
          &MainWindow::importEsiFiles);
  connect(findAction("settingsAction"), &QAction::triggered, this,
          &MainWindow::openSettings);
  connect(findAction("manageMastersAction"), &QAction::triggered, this,
          &MainWindow::openSettings);
  connect(findAction("commandPaletteAction"), &QAction::triggered, this,
          &MainWindow::showCommandPalette);
  connect(findAction("commandPaletteToolbarAction"), &QAction::triggered, this,
          &MainWindow::showCommandPalette);
  connect(findAction("manualAction"), &QAction::triggered, this,
          &MainWindow::showManual);
  connect(findAction("aboutAction"), &QAction::triggered, this,
          &MainWindow::showAbout);
  connect(findAction("showLogAction"), &QAction::triggered, this, [this] {
    if (auto *dock = findChild<QDockWidget *>("runtimeLogDock")) {
      dock->show();
      dock->raise();
    }
  });
  connect(findAction("workspaceBackAction"), &QAction::triggered, this,
          &MainWindow::goWorkspaceBack);
  connect(findAction("workspaceForwardAction"), &QAction::triggered, this,
          &MainWindow::goWorkspaceForward);
  const QVector<QPair<const char *, QWidget *>> workspaceActions = {
      {"goOverviewAction", overviewPage_},
      {"goObjectDictionaryAction", objectDictionaryPage_},
      {"goPdoMapAction", pdoMapPage_},
      {"goWatchAction", watchPage_},
      {"goStartupSdoAction", startupSdoPage_},
      {"goFreeRunAction", freeRunPage_},
      {"goIoVariablesAction", ioVariablePage_},
      {"goConsistencyAction", consistencyPage_},
      {"goStateMachineAction", stateMachinePage_},
      {"goDiagnosticsAction", diagnosticsPage_},
      {"goEsiRepositoryAction", esiRepositoryPage_},
      {"goNotesAction", notesPage_},
      {"goRtTestAction", rtTestPage_},
      {"goEsiXmlAction", esiXmlPage_},
      {"goMasterRawAction", masterRawPage_},
      {"goSlaveRawAction", slaveRawPage_},
      {"goPdoRawAction", pdoRawPage_},
      {"goSdoRawAction", sdoRawPage_},
  };
  for (const auto &workspaceAction : workspaceActions) {
    if (auto *action = findAction(workspaceAction.first)) {
      connect(action, &QAction::triggered, this,
              [this, page = workspaceAction.second] {
                activateWorkspacePage(page);
              });
    }
  }
  if (tabs_) {
    connect(tabs_, &QTabWidget::currentChanged, this,
            &MainWindow::recordWorkspaceHistory);
    connect(tabs_, &QTabWidget::currentChanged, this,
            [this] { updateWorkspaceBoundary(); });
    recordWorkspaceHistory(tabs_->currentIndex());
  }
  connect(nextBestActionButton_, &QPushButton::clicked, this,
          &MainWindow::runNextBestAction);
  connect(findChild<QPushButton *>("captureTopologyBaseline"),
          &QPushButton::clicked, this, &MainWindow::captureTopologyBaseline);
  connect(findChild<QPushButton *>("clearTopologyBaseline"),
          &QPushButton::clicked, this, &MainWindow::clearTopologyBaseline);
  connect(findChild<QPushButton *>("overviewConnect"), &QPushButton::clicked,
          &client_, &EcatClient::connectToDaemon);
  connect(findChild<QPushButton *>("overviewRefresh"), &QPushButton::clicked,
          this, &MainWindow::requestRefresh);
  connect(findChild<QPushButton *>("overviewRunNext"), &QPushButton::clicked,
          this, &MainWindow::runNextCommissioningWorkflowStep);
  connect(findChild<QPushButton *>("contextObjectDictionary"),
          &QPushButton::clicked, this, [this] {
            activateObjectDictionaryPaneFor(sdo_->sdoTable);
    // Multi-branch condition check
            if (client_.isConnected() && selectedPosition() >= 0) {
              client_.sdos(selectedPosition());
            }
          });
  connect(findChild<QPushButton *>("contextPdoMap"), &QPushButton::clicked,
          this, [this] {
            activateWorkspaceTab(pdoMapTabIndex_);
    // Multi-branch condition check
            if (client_.isConnected() && selectedPosition() >= 0) {
              client_.pdos(selectedPosition());
            }
          });
  connect(findChild<QPushButton *>("contextWatch"), &QPushButton::clicked, this,
          [this] {
            activateWorkspaceTab(watchTabIndex_);
    // Multi-branch condition check
            if (selectedPosition() >= 0) {
              addCurrentSdoToWatch();
            }
          });
  connect(findChild<QPushButton *>("contextFreeRun"), &QPushButton::clicked,
          this, [this] {
            activateWorkspaceTab(freeRunTabIndex_);
            if (auto *action = findChild<QAction *>("freeRunAction")) {
              action->trigger();
            }
          });
  connect(findChild<QPushButton *>("contextDiagnostics"), &QPushButton::clicked,
          this, [this] { activateWorkspaceTab(diagnosticsTabIndex_); });
  connect(findChild<QPushButton *>("contextRefreshSlave"),
          &QPushButton::clicked, this, [this] {
            if (client_.isConnected() && selectedPosition() >= 0) {
              client_.slaveInfo(selectedPosition());
              client_.sdos(selectedPosition());
              client_.pdos(selectedPosition());
              client_.xml(selectedPosition());
            }
          });
  connect(findChild<QPushButton *>("contextPrepareSnapshot"),
          &QPushButton::clicked, this,
          &MainWindow::prepareSelectedSlaveSnapshot);
  connect(findChild<QPushButton *>("contextDriveNext"), &QPushButton::clicked,
          this, [this] {
            QString label;
            QString value;
            QString reason;
            if (client_.isConnected() &&
                recommendedCia402Controlword(&label, &value, &reason)) {
              prepareCia402Controlword(label, value);
            }
          });
  connect(findAction("freeRunAction"), &QAction::toggled, this,
          &MainWindow::setFreeRun);
  connect(findAction("opAction"), &QAction::triggered, this, [this] {
    requestSlaveStateWithConfirmation(selectedPosition(), "OP");
  });
  connect(findAction("safeOpAction"), &QAction::triggered, this, [this] {
    requestSlaveStateWithConfirmation(selectedPosition(), "SAFEOP");
  });
  connect(findAction("preOpAction"), &QAction::triggered, this, [this] {
    requestSlaveStateWithConfirmation(selectedPosition(), "PREOP");
  });
  connect(findAction("initAction"), &QAction::triggered, this, [this] {
    requestSlaveStateWithConfirmation(selectedPosition(), "INIT");
  });
  connect(findAction("allOpAction"), &QAction::triggered, this,
          [this] { requestAllSlaveState("OP"); });
  connect(findAction("allSafeOpAction"), &QAction::triggered, this,
          [this] { requestAllSlaveState("SAFEOP"); });
  connect(findAction("allPreOpAction"), &QAction::triggered, this,
          [this] { requestAllSlaveState("PREOP"); });
  connect(findAction("allInitAction"), &QAction::triggered, this,
          [this] { requestAllSlaveState("INIT"); });
  connect(
      findChild<QPushButton *>("contextOp"), &QPushButton::clicked, this,
      [this] { requestSlaveStateWithConfirmation(selectedPosition(), "OP"); });
  connect(findChild<QPushButton *>("contextSafeOp"), &QPushButton::clicked,
          this, [this] {
            requestSlaveStateWithConfirmation(selectedPosition(), "SAFEOP");
          });
  connect(findChild<QPushButton *>("contextPreOp"), &QPushButton::clicked, this,
          [this] {
            requestSlaveStateWithConfirmation(selectedPosition(), "PREOP");
          });
  connect(findChild<QPushButton *>("contextInit"), &QPushButton::clicked, this,
          [this] {
            requestSlaveStateWithConfirmation(selectedPosition(), "INIT");
          });
  connect(masterCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, [this](int index) {
    // Multi-branch condition check
            if (index >= 0) {
              setActiveMaster(masterCombo_->itemData(index).toString());
            }
          });

  connect(topologyTree_, &QTreeWidget::currentItemChanged, this,
          [this](QTreeWidgetItem *item) {
    // Multi-branch condition check
            if (item) {
              setSelectedSlave(item->data(0, Qt::UserRole).toInt());
            }
          });
  connect(topologyTree_, &QTreeWidget::customContextMenuRequested, this,
          &MainWindow::showTopologyContextMenu);

  connect(findChild<QPushButton *>("readSdo"), &QPushButton::clicked, this,
          [this] {
            if (selectedPosition() >= 0) {
              requestSdoRead(selectedPosition(), sdoInspector_->sdoIndex->text(),
                             sdoInspector_->sdoSubIndex->text(),
                             uiText("Manual SDO read", "手动 SDO 读取"));
            }
          });
  connect(findChild<QPushButton *>("readTargetSdo"), &QPushButton::clicked,
          this, [this] {
            if (selectedPosition() >= 0) {
              requestSdoRead(selectedPosition(), sdoInspector_->sdoIndex->text(),
                             sdoInspector_->sdoSubIndex->text(),
                             uiText("Selected Object panel", "选中对象面板"));
            }
          });
  connect(findChild<QPushButton *>("reviewSdoWriteDelta"),
          &QPushButton::clicked, this, &MainWindow::reviewCurrentSdoWriteDelta);
  connect(findChild<QPushButton *>("runSdoTargetRowAction"),
          &QPushButton::clicked, this, [this] {
            openSdoTargetPanelRow(
                sdoInspector_->sdoTargetTable ? sdoInspector_->sdoTargetTable->currentRow() : -1);
          });
  connect(findChild<QPushButton *>("copySdoTargetRowEvidence"),
          &QPushButton::clicked, this, [this] {
            copySdoTargetPanelRowDigest(
                sdoInspector_->sdoTargetTable ? sdoInspector_->sdoTargetTable->currentRow() : -1);
          });
  connect(findChild<QPushButton *>("copySdoEvidenceDigest"),
          &QPushButton::clicked, this,
          &MainWindow::copyCurrentSdoEvidenceDigest);
  connect(findChild<QPushButton *>("openSdoWatchLink"), &QPushButton::clicked,
          this, &MainWindow::openCurrentSdoWatchLink);
  connect(findChild<QPushButton *>("openSdoStartupLink"), &QPushButton::clicked,
          this, &MainWindow::openCurrentSdoStartupLink);
  connect(findChild<QPushButton *>("openSdoBookmarkLink"),
          &QPushButton::clicked, this, &MainWindow::openCurrentSdoBookmarkLink);
  connect(findChild<QPushButton *>("openSdoTargetTrailLink"),
          &QPushButton::clicked, this,
          &MainWindow::openCurrentSdoTargetTrailLink);
  connect(sdoInspector_->sdoTargetTable, &QTableWidget::customContextMenuRequested, this,
          &MainWindow::showSdoTargetPanelContextMenu);
  connect(sdoInspector_->sdoTargetTable, &QTableWidget::currentCellChanged, this,
          [this](int, int, int, int) { updateActionAvailability(); });
  connect(sdoInspector_->sdoTargetTable, &QTableWidget::cellDoubleClicked, this,
          [this](int row, int) { openSdoTargetPanelRow(row); });
  sdoInspector_->sdoTargetTable->installEventFilter(this);
  if (sdoInspector_->sdoTargetTable->viewport()) {
    sdoInspector_->sdoTargetTable->viewport()->installEventFilter(this);
  }
  connect(findChild<QPushButton *>("readSelectedDictionary"),
          &QPushButton::clicked, this, &MainWindow::readSelectedDictionaryRows);
  connect(findChild<QPushButton *>("readVisibleDictionary"),
          &QPushButton::clicked, this, &MainWindow::readVisibleDictionaryRows);
  connect(findChild<QPushButton *>("readFailedDictionary"),
          &QPushButton::clicked, this, &MainWindow::readFailedDictionaryRows);
  connect(findChild<QPushButton *>("useSdoValue"), &QPushButton::clicked, this,
          &MainWindow::useReadSdoValueForWrite);
  connect(findChild<QPushButton *>("useSdoEvidence"), &QPushButton::clicked,
          this, &MainWindow::usePreferredSdoEvidenceForWrite);
  connect(findChild<QPushButton *>("pickSdoEvidence"), &QPushButton::clicked,
          this, &MainWindow::pickSdoEvidenceForWrite);
  connect(findChild<QPushButton *>("writeSdo"), &QPushButton::clicked, this,
          &MainWindow::writeCurrentSdo);
  connect(findChild<QPushButton *>("writeTargetSdo"), &QPushButton::clicked,
          this, &MainWindow::writeCurrentSdo);
  connect(findChild<QPushButton *>("addStartupSdo"), &QPushButton::clicked,
          this, &MainWindow::addStartupSdo);
  connect(findChild<QPushButton *>("startupTargetSdo"), &QPushButton::clicked,
          this, &MainWindow::addStartupSdo);
  connect(findChild<QPushButton *>("addSelectedPdoWatch"),
          &QPushButton::clicked, this,
          &MainWindow::addSelectedPdoEntriesToWatch);
  connect(findChild<QPushButton *>("removeStartupSdo"), &QPushButton::clicked,
          this, &MainWindow::removeStartupSdo);
  connect(findChild<QPushButton *>("moveStartupSdoUp"), &QPushButton::clicked,
          this, [this] { moveStartupSdoRow(-1); });
  connect(findChild<QPushButton *>("moveStartupSdoDown"), &QPushButton::clicked,
          this, [this] { moveStartupSdoRow(1); });
  connect(findChild<QPushButton *>("preflightStartupSdo"),
          &QPushButton::clicked, this,
          [this] { preflightStartupSdoList(true); });
  connect(findChild<QPushButton *>("verifyStartupSdo"), &QPushButton::clicked,
          this, &MainWindow::verifyStartupSdoList);
  connect(findChild<QPushButton *>("verifySelectedStartupSdo"),
          &QPushButton::clicked, this,
          &MainWindow::verifySelectedStartupSdoRows);
    // Connect startupWatchDiffsOnly signal to handler
  connect(watch_->startupWatchDiffsOnly, &QCheckBox::toggled, this, [this] {
    filterStartupSdoTable();
    updateStartupSdoControls();
    updateStartupSdoRowDetail();
  });
  connect(findChild<QPushButton *>("focusStartupSdoWatchDiffs"),
          &QPushButton::clicked, this, &MainWindow::focusStartupSdoWatchDiffs);
  connect(findChild<QPushButton *>("applyStartupSdoWatchDiffs"),
          &QPushButton::clicked, this,
          &MainWindow::applyStartupSdoWatchDiffRows);
  connect(findChild<QPushButton *>("applyStartupSdo"), &QPushButton::clicked,
          this, &MainWindow::applyStartupSdoList);
  connect(findChild<QPushButton *>("applySelectedStartupSdo"),
          &QPushButton::clicked, this,
          &MainWindow::applySelectedStartupSdoRows);
  connect(findChild<QPushButton *>("startupFromSelectedWatch"),
          &QPushButton::clicked, this,
          &MainWindow::addStartupSdoFromSelectedWatchRows);
  connect(findChild<QPushButton *>("syncStartupFromWatch"),
          &QPushButton::clicked, this,
          &MainWindow::syncSelectedWatchRowsToStartupSdo);
  connect(findChild<QPushButton *>("addWatchSdo"), &QPushButton::clicked, this,
          &MainWindow::addCurrentSdoToWatch);
  connect(findChild<QPushButton *>("watchTargetSdo"), &QPushButton::clicked,
          this, &MainWindow::addCurrentSdoToWatch);
  connect(findChild<QPushButton *>("bookmarkTargetSdo"), &QPushButton::clicked,
          this, &MainWindow::addCurrentSdoBookmark);
  connect(findChild<QPushButton *>("watchSelectedDictionary"),
          &QPushButton::clicked, this,
          &MainWindow::addSelectedDictionaryRowsToWatch);
  connect(findChild<QPushButton *>("startupSelectedEvidence"),
          &QPushButton::clicked, this,
          &MainWindow::addSelectedDictionaryEvidenceToStartupSdo);
  connect(findChild<QPushButton *>("watchVisibleDictionary"),
          &QPushButton::clicked, this,
          &MainWindow::addVisibleDictionaryRowsToWatch);
  connect(findChild<QPushButton *>("bookmarkSelectedDictionary"),
          &QPushButton::clicked, this,
          &MainWindow::addSelectedDictionaryRowsToBookmarks);
  connect(findChild<QPushButton *>("fillBookmarkSdo"), &QPushButton::clicked,
          this, [this] {
            applySdoSelectionFromBookmark(
                bookmark_->objectBookmarkTable ? bookmark_->objectBookmarkTable->currentRow() : -1,
                false);
          });
  connect(findChild<QPushButton *>("watchBookmarkSdo"), &QPushButton::clicked,
          this, &MainWindow::addSelectedObjectBookmarksToWatch);
  connect(findChild<QPushButton *>("startupBookmarkSdo"), &QPushButton::clicked,
          this, &MainWindow::addSelectedObjectBookmarksToStartupSdo);
  connect(findChild<QPushButton *>("removeBookmarkSdo"), &QPushButton::clicked,
          this, &MainWindow::removeSelectedObjectBookmarks);
  connect(findChild<QPushButton *>("restoreSdoTargetTrail"),
          &QPushButton::clicked, this, [this] {
            restoreSdoTargetTrailRow(
                sdoTargetTrailTable_ ? sdoTargetTrailTable_->currentRow() : -1);
          });
  connect(findChild<QPushButton *>("watchSdoTargetTrail"),
          &QPushButton::clicked, this,
          &MainWindow::addSdoTargetTrailRowToWatch);
  connect(findChild<QPushButton *>("bookmarkSdoTargetTrail"),
          &QPushButton::clicked, this, &MainWindow::bookmarkSdoTargetTrailRow);
  connect(findChild<QPushButton *>("startupSdoTargetTrail"),
          &QPushButton::clicked, this,
          &MainWindow::addSdoTargetTrailRowToStartup);
  connect(findChild<QPushButton *>("removeSdoTargetTrail"),
          &QPushButton::clicked, this,
          &MainWindow::removeSelectedSdoTargetTrailRows);
  connect(findChild<QPushButton *>("clearSdoTargetTrail"),
          &QPushButton::clicked, this, &MainWindow::clearSdoTargetTrail);
  connect(sdoTargetTrailTable_, &QTableWidget::itemSelectionChanged, this,
          &MainWindow::updateActionAvailability);
  connect(sdoTargetTrailTable_, &QTableWidget::itemSelectionChanged, this,
          &MainWindow::updateSdoTargetTrailRowDetail);
  connect(sdoTargetTrailTable_, &QTableWidget::cellDoubleClicked, this,
          [this](int row) { restoreSdoTargetTrailRow(row); });
  connect(findChild<QPushButton *>("addCia402WatchPreset"),
          &QPushButton::clicked, this, &MainWindow::addCia402WatchPreset);
  connect(findChild<QPushButton *>("refreshWatch"), &QPushButton::clicked, this,
          [this] { refreshWatchList(false); });
  connect(findChild<QPushButton *>("captureWatchBaseline"),
          &QPushButton::clicked, this, &MainWindow::captureWatchBaseline);
  connect(findChild<QPushButton *>("clearWatchBaseline"), &QPushButton::clicked,
          this, &MainWindow::clearWatchBaseline);
  connect(findChild<QPushButton *>("clearWatch"), &QPushButton::clicked, this,
          &MainWindow::clearWatchList);
  connect(findChild<QPushButton *>("watchSelectedHistory"),
          &QPushButton::clicked, this,
          &MainWindow::addSelectedHistoryRowsToWatch);
  connect(findChild<QPushButton *>("startupFromSelectedHistory"),
          &QPushButton::clicked, this,
          &MainWindow::addSelectedHistoryRowsToStartupSdo);
  connect(watch_->watchAutoRefresh, &QCheckBox::toggled, this,
          &MainWindow::updateWatchAutoRefresh);
  connect(watch_->watchRefreshInterval,
          QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          &MainWindow::updateWatchAutoRefresh);
  connect(watch_->watchFilter, &QLineEdit::textChanged, this,
          &MainWindow::filterWatchTable);
  connect(watch_->watchScopeFilter,
          QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          [this] { filterWatchTable(); });
  connect(watch_->watchChangedOnly, &QCheckBox::toggled, this,
          &MainWindow::filterWatchTable);
  connect(watch_->watchTable, &QTableWidget::itemSelectionChanged, this,
          &MainWindow::updateActionAvailability);
  connect(watch_->watchTable, &QTableWidget::itemSelectionChanged, this,
          &MainWindow::updateWatchRowDetail);
  connect(watch_->watchTable, &QTableWidget::cellDoubleClicked, this,
          [this](int row) { applySdoSelectionFromWatch(row, true); });
  connect(session_->sessionBriefTable, &QTableWidget::itemSelectionChanged, this,
          &MainWindow::updateSessionBriefCopyButton);
    // Connect sessionBriefCopyButton signal to handler
  connect(session_->sessionBriefCopyButton, &QPushButton::clicked, this, [this] {
    copySessionBriefRowDigest(
        session_->sessionBriefTable ? session_->sessionBriefTable->currentRow() : -1);
  });
  connect(session_->sessionBriefTable, &QTableWidget::cellDoubleClicked, this,
          [this](int row) { openSessionBriefRow(row); });
  connect(workflow_->workflowTable, &QTableWidget::itemSelectionChanged, this,
          &MainWindow::updateWorkflowStepCopyButton);
  connect(workflow_->workflowTable, &QTableWidget::itemSelectionChanged, this,
          &MainWindow::updateWorkflowStepDetail);
    // Connect workflowStepCopyButton signal to handler
  connect(workflow_->workflowStepCopyButton, &QPushButton::clicked, this, [this] {
    copyWorkflowStepDigest(workflow_->workflowTable ? workflow_->workflowTable->currentRow() : -1);
  });
  connect(workflow_->workflowReviewButton, &QPushButton::clicked, this,
          &MainWindow::reviewFirstCommissioningWorkflowIssue);
  connect(workflow_->workflowReviewNextButton, &QPushButton::clicked, this,
          &MainWindow::reviewNextCommissioningWorkflowIssue);
  connect(workflow_->workflowFilter, &QLineEdit::textChanged, this,
          &MainWindow::filterCommissioningWorkflow);
  connect(workflow_->workflowScopeFilter,
          QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          [this] { filterCommissioningWorkflow(); });
  connect(slaveEvidence_->slaveEvidenceMatrixFilter, &QLineEdit::textChanged, this,
          &MainWindow::filterSlaveEvidenceMatrix);
  connect(slaveEvidence_->slaveEvidenceMatrixScopeFilter,
          QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          [this] { filterSlaveEvidenceMatrix(); });
  connect(slaveEvidence_->slaveEvidenceMatrixTable, &QTableWidget::itemSelectionChanged, this,
          &MainWindow::filterSlaveEvidenceMatrix);
  connect(slaveEvidence_->slaveEvidenceMatrixReviewButton, &QPushButton::clicked, this,
          &MainWindow::reviewFirstSlaveEvidenceMatrixIssue);
  connect(slaveEvidence_->slaveEvidenceMatrixReviewNextButton, &QPushButton::clicked, this,
          &MainWindow::reviewNextSlaveEvidenceMatrixIssue);
    // Connect slaveEvidenceMatrixCopyButton signal to handler
  connect(slaveEvidence_->slaveEvidenceMatrixCopyButton, &QPushButton::clicked, this, [this] {
    copySlaveEvidenceMatrixRowDigest(
        slaveEvidence_->slaveEvidenceMatrixTable ? slaveEvidence_->slaveEvidenceMatrixTable->currentRow()
                                  : -1);
  });
  for (auto *button : slaveEvidence_->slaveEvidenceMatrixTriageButtons) {
    // Connect button signal to handler
    connect(button, &QPushButton::clicked, this, [this, button] {
      if (!slaveEvidence_->slaveEvidenceMatrixScopeFilter) {
        return;
      }
      const QString scope = button->property("scope").toString();
      const int scopeIndex = slaveEvidence_->slaveEvidenceMatrixScopeFilter->findData(scope);
      if (scopeIndex >= 0) {
        slaveEvidence_->slaveEvidenceMatrixScopeFilter->setCurrentIndex(scopeIndex);
      }
      filterSlaveEvidenceMatrix();
    });
  }
  auto *sessionBriefEnter =
      new QShortcut(QKeySequence(Qt::Key_Return), session_->sessionBriefTable);
  connect(sessionBriefEnter, &QShortcut::activated, this,
          [this] { openSessionBriefRow(session_->sessionBriefTable->currentRow()); });
  auto *sessionBriefKeypadEnter =
      new QShortcut(QKeySequence(Qt::Key_Enter), session_->sessionBriefTable);
  connect(sessionBriefKeypadEnter, &QShortcut::activated, this,
          [this] { openSessionBriefRow(session_->sessionBriefTable->currentRow()); });
  connect(workflow_->workflowTable, &QTableWidget::cellDoubleClicked, this,
          [this](int row) { runCommissioningWorkflowStep(row); });
  connect(slaveEvidence_->slaveEvidenceMatrixTable, &QTableWidget::cellDoubleClicked, this,
          [this](int row) { openSlaveEvidenceMatrixRow(row); });
  auto stateMachinePositionForRow = [this](int row) -> int {
    // Multi-branch condition check
    if (stateMachine_->stateMachineTable && row >= 0 &&
        row < stateMachine_->stateMachineTable->rowCount()) {
      const int position =
          stateMachinePositionFromTable(stateMachine_->stateMachineTable, row);
    // Multi-branch condition check
      if (position >= 0) {
        return position;
      }
    }
    return selectedPosition();
  };
  auto requestRecommendedState = [this, stateMachinePositionForRow](int row) {
    // Multi-branch condition check
    if (!client_.isConnected()) {
      updateDiagnostics(
          "Warning", "State",
          uiText("State request skipped: runtime is not connected",
                 "状态请求已跳过：运行时尚未连接"));
      return;
    }
    // Multi-branch condition check
    if (!stateMachine_->stateMachineTable) {
      return;
    }
    int targetRow = row;
    // Multi-branch condition check
    if (targetRow < 0) {
      targetRow = stateMachine_->stateMachineTable->currentRow();
    }
    // Multi-branch condition check
    if (targetRow < 0 && selectedPosition() >= 0) {
      for (int candidate = 0; candidate < stateMachine_->stateMachineTable->rowCount();
           ++candidate) {
        if (stateMachinePositionForRow(candidate) == selectedPosition()) {
          targetRow = candidate;
          break;
        }
      }
    }
    const int position = stateMachinePositionForRow(targetRow);
    const QString target =
        stateMachineTableRowFromTable(stateMachine_->stateMachineTable, targetRow)
            .recommended;
    if (position < 0 || target.isEmpty()) {
      updateDiagnostics(
          "Info", "State",
          uiText("No recommended state is available for the selected row",
                 "选中行暂无可发送的推荐状态"));
      return;
    }
    requestSlaveStateWithConfirmation(position, target);
  };
  auto requestStateMachineRowState =
      [this, stateMachinePositionForRow](const QString &state) {
        const int position = stateMachinePositionForRow(
            stateMachine_->stateMachineTable ? stateMachine_->stateMachineTable->currentRow() : -1);
        if (position < 0) {
          updateDiagnostics(
              "Info", "State",
              uiText("Select a state-machine row first", "请先选择状态机行"));
          return;
        }
        requestSlaveStateWithConfirmation(position, state);
      };
  connect(findChild<QPushButton *>("stateSelectedNext"), &QPushButton::clicked,
          this, [requestRecommendedState] { requestRecommendedState(-1); });
  connect(findChild<QPushButton *>("stateSelectedPreOp"), &QPushButton::clicked,
          this, [requestStateMachineRowState] {
            requestStateMachineRowState("PREOP");
          });
  connect(findChild<QPushButton *>("stateSelectedSafeOp"),
          &QPushButton::clicked, this, [requestStateMachineRowState] {
            requestStateMachineRowState("SAFEOP");
          });
  connect(findChild<QPushButton *>("stateSelectedOp"), &QPushButton::clicked,
          this,
          [requestStateMachineRowState] { requestStateMachineRowState("OP"); });
  connect(findChild<QPushButton *>("stateAllPreOp"), &QPushButton::clicked,
          this, [this] { requestAllSlaveState("PREOP"); });
  connect(findChild<QPushButton *>("stateAllSafeOp"), &QPushButton::clicked,
          this, [this] { requestAllSlaveState("SAFEOP"); });
    // Connect stateMachineTable signal to handler
  connect(stateMachine_->stateMachineTable, &QTableWidget::currentCellChanged, this, [this] {
    updateActionAvailability();
    updateStateMachineRowDetail();
    const bool canSend = client_.isConnected() && stateMachine_->stateMachineTable &&
                         stateMachine_->stateMachineTable->currentRow() >= 0;
    const bool hasRecommendedState =
        canSend && stateMachineRowHasRecommendation(
                       stateMachine_->stateMachineTable, stateMachine_->stateMachineTable->currentRow());
    for (const char *name : {"stateSelectedNext", "stateSelectedPreOp",
                             "stateSelectedSafeOp", "stateSelectedOp"}) {
      if (auto *button = findChild<QPushButton *>(name)) {
        button->setEnabled(QString::fromLatin1(name) == "stateSelectedNext"
                               ? hasRecommendedState
                               : canSend);
      }
    }
  });
  connect(stateMachine_->stateMachineTable, &QTableWidget::cellDoubleClicked, this,
          [requestRecommendedState](int row, int) {
            requestRecommendedState(row);
          });
  connect(findChild<QPushButton *>("runHostCheck"), &QPushButton::clicked, this,
          &MainWindow::runHostDiagnostics);
  connect(findChild<QPushButton *>("copyHostCommand"), &QPushButton::clicked,
          this, &MainWindow::copySelectedHostCommand);
  connect(hostHealthTable_, &QTableWidget::currentCellChanged, this,
          [this] { updateActionAvailability(); });
  connect(hostHealthTable_, &QTableWidget::cellDoubleClicked, this,
          [this](int, int column) {
            if (column == 4) {
              copySelectedHostCommand();
            }
          });
    // Connect sdoIndex signal to handler
  connect(sdoInspector_->sdoIndex, &QLineEdit::editingFinished, this, [this] {
    restoreManualSdoWriteMode();
    rememberCurrentSdoTarget(uiText("Manual fields", "手动字段"),
                             uiText("Index edited", "已编辑索引"));
  });
    // Connect sdoSubIndex signal to handler
  connect(sdoInspector_->sdoSubIndex, &QLineEdit::editingFinished, this, [this] {
    restoreManualSdoWriteMode();
    rememberCurrentSdoTarget(uiText("Manual fields", "手动字段"),
                             uiText("SubIndex edited", "已编辑子项"));
  });
    // Connect sdoType signal to handler
  connect(sdoInspector_->sdoType, &QComboBox::currentTextChanged, this, [this] {
    restoreManualSdoWriteMode();
    rememberCurrentSdoTarget(uiText("Manual fields", "手动字段"),
                             uiText("Type changed", "已修改类型"));
  });
  connect(sdoInspector_->sdoWriteValue, &QLineEdit::textChanged, this,
          [this] { updateSdoInspector(uiText("Manual write", "手动写入")); });
    // Connect sdoWriteValue signal to handler
  connect(sdoInspector_->sdoWriteValue, &QLineEdit::editingFinished, this, [this] {
    rememberCurrentSdoTarget(uiText("Manual write", "手动写入"),
                             uiText("Write value edited", "已编辑写入值"));
  });
  connect(sdoInspector_->sdoValue, &QLineEdit::textChanged, this,
          [this] { updateSdoInspector(uiText("Read-back", "读回值")); });
  connect(sdo_->pdoFilter, &QLineEdit::textChanged, this,
          &MainWindow::filterPdoTable);
  connect(sdo_->pdoTable, &QTableWidget::itemSelectionChanged, this,
          &MainWindow::updateActionAvailability);
  connect(sdo_->pdoTable, &QTableWidget::itemSelectionChanged, this,
          &MainWindow::updatePdoRowDetail);
  connect(sdo_->sdoFilter, &QLineEdit::textChanged, this,
          &MainWindow::filterSdoTable);
  connect(freeRunWidgets_->freeRunFilter, &QLineEdit::textChanged, this,
          &MainWindow::filterFreeRunEntryTable);
  connect(freeRunWidgets_->freeRunChangedOnly, &QCheckBox::toggled, this,
          &MainWindow::filterFreeRunEntryTable);
  connect(freeRunWidgets_->freeRunEntryTable, &QTableWidget::itemSelectionChanged, this,
          &MainWindow::updateFreeRunEntryDetail);
  connect(ioVar_->ioVariableFilter, &QLineEdit::textChanged, this,
          &MainWindow::filterIoVariableTable);
  connect(ioVar_->ioVariableScopeFilter,
          QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          [this] { filterIoVariableTable(); });
  connect(findChild<QPushButton *>("refreshIoVariables"), &QPushButton::clicked,
          this, &MainWindow::updateIoVariableTable);
  connect(findChild<QPushButton *>("fillIoVariableSdo"), &QPushButton::clicked,
          this, [this] {
            applySdoSelectionFromIoVariable(
                ioVar_->ioVariableTable ? ioVar_->ioVariableTable->currentRow() : -1, false);
          });
  connect(findChild<QPushButton *>("readIoVariableSdo"), &QPushButton::clicked,
          this, [this] {
            applySdoSelectionFromIoVariable(
                ioVar_->ioVariableTable ? ioVar_->ioVariableTable->currentRow() : -1, true);
          });
  connect(findChild<QPushButton *>("watchSelectedIoVariables"),
          &QPushButton::clicked, this,
          &MainWindow::addSelectedIoVariablesToWatch);
  connect(findChild<QPushButton *>("watchVisibleIoVariables"),
          &QPushButton::clicked, this,
          &MainWindow::addVisibleIoVariablesToWatch);
  connect(findChild<QPushButton *>("startupSelectedIoVariables"),
          &QPushButton::clicked, this,
          &MainWindow::addSelectedIoVariablesToStartupSdo);
  connect(findChild<QPushButton *>("startupVisibleIoVariables"),
          &QPushButton::clicked, this,
          &MainWindow::addVisibleIoVariablesToStartupSdo);
  connect(findChild<QPushButton *>("editIoVariableMetadata"),
          &QPushButton::clicked, this,
          &MainWindow::editSelectedIoVariableMetadata);
  connect(findChild<QPushButton *>("bulkNameIoVariables"),
          &QPushButton::clicked, this, &MainWindow::bulkNameIoVariables);
  connect(findChild<QPushButton *>("reviewPlcHandoff"), &QPushButton::clicked,
          this, &MainWindow::reviewPlcHandoffIssues);
  connect(findChild<QPushButton *>("exportIoVariablesCsv"),
          &QPushButton::clicked, this, &MainWindow::exportIoVariablesCsv);
  connect(findChild<QPushButton *>("exportIoPlcSymbolsCsv"),
          &QPushButton::clicked, this, &MainWindow::exportIoVariablesPlcCsv);
  connect(consistency_->consistencyFilter, &QLineEdit::textChanged, this,
          &MainWindow::filterConsistencyTable);
  connect(consistency_->consistencyScopeFilter,
          QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          [this] { filterConsistencyTable(); });
  connect(findChild<QPushButton *>("refreshConsistency"), &QPushButton::clicked,
          this, &MainWindow::updateConsistencyView);
  connect(findChild<QPushButton *>("openIoVariablesFromConsistency"),
          &QPushButton::clicked, this,
          [this] { focusEvidenceFromConsistency(); });
  connect(consistency_->consistencyTable, &QTableWidget::itemSelectionChanged, this,
          &MainWindow::updateConsistencyRowDetail);
  connect(consistency_->consistencyTable, &QTableWidget::cellDoubleClicked, this,
          [this](int row) { focusEvidenceFromConsistency(row); });
  connect(sdo_->sdoTable, &QTableWidget::currentCellChanged, this,
          [this](int row) { applySdoSelectionFromDictionary(row, false); });
  connect(sdo_->sdoTable, &QTableWidget::itemSelectionChanged, this,
          &MainWindow::updateActionAvailability);
  connect(sdo_->sdoTable, &QTableWidget::cellDoubleClicked, this,
          [this](int row) { applySdoSelectionFromDictionary(row, true); });
  connect(bookmark_->objectBookmarkTable, &QTableWidget::itemSelectionChanged, this,
          &MainWindow::updateActionAvailability);
  connect(bookmark_->objectBookmarkTable, &QTableWidget::itemSelectionChanged, this,
          &MainWindow::updateObjectBookmarkRowDetail);
  connect(bookmark_->objectBookmarkTable, &QTableWidget::cellDoubleClicked, this,
          [this](int row) { applySdoSelectionFromBookmark(row, false); });
  connect(sdoHistoryTable_, &QTableWidget::itemSelectionChanged, this,
          &MainWindow::updateActionAvailability);
  connect(sdoHistoryTable_, &QTableWidget::itemSelectionChanged, this,
          &MainWindow::updateSdoHistoryRowDetail);
  connect(sdoHistoryTable_, &QTableWidget::cellDoubleClicked, this,
          [this](int row) { applySdoSelectionFromHistory(row, true); });
  connect(sdo_->pdoTable, &QTableWidget::cellDoubleClicked, this,
          [this](int row) { applySdoSelectionFromPdoMap(row, true); });
  connect(ioVar_->ioVariableTable, &QTableWidget::itemSelectionChanged, this,
          &MainWindow::updateActionAvailability);
  connect(ioVar_->ioVariableTable, &QTableWidget::itemSelectionChanged, this,
          &MainWindow::updateIoVariableRowDetail);
  connect(ioVar_->ioVariableTable, &QTableWidget::cellDoubleClicked, this,
          [this](int row) { applySdoSelectionFromIoVariable(row, true); });
  connect(diagnostics_->diagnosticsFilter, &QLineEdit::textChanged, this,
          &MainWindow::filterDiagnosticsTable);
  connect(diagnostics_->diagnosticsLevelFilter,
          QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          [this] { filterDiagnosticsTable(); });
    // Connect startupSdoTable_ signal to handler
  connect(startupSdoTable_, &QTableWidget::currentCellChanged, this, [this] {
    updateStartupSdoControls();
    updateStartupSdoRowDetail();
  });
  connect(startupSdoTable_, &QTableWidget::itemSelectionChanged, this,
          &MainWindow::updateStartupSdoControls);
  connect(startupSdoTable_, &QTableWidget::itemSelectionChanged, this,
          &MainWindow::updateStartupSdoRowDetail);
    // Connect startupSdoTable_ signal to handler
  connect(startupSdoTable_, &QTableWidget::itemChanged, this, [this] {
    QTimer::singleShot(0, this, [this] {
      updateWatchStartupDeltas();
      updateStartupSdoRowDetail();
    });
  });
  connect(startupSdoTable_, &QTableWidget::cellDoubleClicked, this,
          [this](int row) { applySdoSelectionFromStartup(row, true); });
  connect(findChild<QPushButton *>("clearDiagnostics"), &QPushButton::clicked,
          this, [this] {
            diagnostics_->diagnosticsTable->clearContents();
            diagnostics_->diagnosticsTable->setRowCount(0);
            updateDiagnosticsSummary();
            updateNextBestAction();
          });
  connect(findChild<QPushButton *>("clearSdoHistory"), &QPushButton::clicked,
          this, [this] {
            if (sdoHistoryTable_) {
              sdoHistoryTable_->clearContents();
              sdoHistoryTable_->setRowCount(0);
              ensureSdoHistoryTable();
              updateSdoHistoryRowDetail();
            }
            pendingSdoReads_.clear();
            pendingSdoReadTypes_.clear();
            pendingSdoWrites_.clear();
            pendingSdoVerifications_.clear();
            pendingStartupSdoChecks_.clear();
            updateActionAvailability();
          });
  for (auto *table : {metricTable_,
                      session_->sessionBriefTable,
                      workflow_->workflowTable,
                      slaveEvidence_->slaveEvidenceMatrixTable,
                      stateMachine_->stateMachineTable,
                      identityTable_,
                      portTable_,
                      mailboxTable_,
                      sdo_->pdoTable,
                      sdo_->sdoTable,
                      sdoTargetTrailTable_,
                      bookmark_->objectBookmarkTable,
                      sdoHistoryTable_,
                      freeRunWidgets_->freeRunTable,
                      freeRunWidgets_->freeRunEntryTable,
                      ioVar_->ioVariableTable,
                      hostHealthTable_,
                      diagnostics_->diagnosticsTable,
                      watch_->watchTable,
                      esiTable_,
                      startupSdoTable_}) {
    connect(table, &QTableWidget::customContextMenuRequested, this,
            [this, table](const QPoint &position) {
              showTableContextMenu(table, position);
            });
    table->installEventFilter(this);
    if (table->viewport()) {
      table->viewport()->installEventFilter(this);
    }
  }

    // Connect client_ signal to handler
  connect(&client_, &EcatClient::connected, this, [this] {
    connectionLabel_->setText(uiText("Runtime connected", "运行时已连接"));
    connectionLabel_->setProperty("state", "connected");
    repolish(connectionLabel_);
    log("Connected to ecatd");
    updateDiagnostics("Info", "Runtime", "Connected to ecatd");
    updateActionAvailability();
    updateWatchAutoRefresh();
    updateStatusBar();
    requestRefresh();
  });
    // Connect client_ signal to handler
  connect(&client_, &EcatClient::disconnected, this, [this] {
    connectionLabel_->setText(uiText("Runtime disconnected", "运行时已断开"));
    connectionLabel_->setProperty("state", "disconnected");
    repolish(connectionLabel_);
    log("Runtime disconnected");
    updateDiagnostics("Warning", "Runtime", "Disconnected");
    updateWatchAutoRefresh();
    updateActionAvailability();
    updateStatusBar();
  });
  connect(
      &client_, &EcatClient::errorMessage, this,
      [this](const QString &message) {
        log("ERROR: " + message);
        updateDiagnostics("Error", "Runtime", message);
        if (!pendingSdoReads_.isEmpty()) {
          for (auto it = pendingSdoReads_.cbegin();
               it != pendingSdoReads_.cend(); ++it) {
            const QStringList parts = it.key().split('|');
            if (parts.size() == 3) {
              updateSdoTableEvidence(
                  parts.value(0).toInt(), parts.value(1), parts.value(2),
                  QString(), uiText("Failed", "失败"),
                  uiText("Runtime error while SDO read was pending: %1",
                         "SDO 读取待返回时发生运行时错误：%1")
                      .arg(message));
            }
          }
          appendSdoHistory(
              uiText("Read", "读取"), -1, QString(), QString(), QString(),
              QString(), uiText("Failed", "失败"),
              uiText("Runtime error while SDO request(s) were pending: %1",
                     "SDO 请求待返回时发生运行时错误：%1")
                  .arg(message));
          pendingSdoReads_.clear();
          pendingSdoReadTypes_.clear();
        }
        if (!pendingSdoWrites_.isEmpty()) {
          for (const auto &pending : std::as_const(pendingSdoWrites_)) {
            appendSdoHistory(uiText("Write", "写入"), pending.value(0).toInt(),
                             pending.value(1), pending.value(2),
                             pending.value(3), pending.value(4),
                             uiText("Failed", "失败"), message);
            updateSdoTableEvidence(
                pending.value(0).toInt(), pending.value(1), pending.value(2),
                pending.value(4), uiText("Failed", "失败"),
                uiText("SDO write failed before read-back verification: %1",
                       "SDO 写入在读回校验前失败：%1")
                    .arg(message));
          }
          pendingSdoWrites_.clear();
        }
        if (!pendingSdoVerifications_.isEmpty()) {
          for (const auto &pending : std::as_const(pendingSdoVerifications_)) {
            appendSdoHistory(
                uiText("Verify", "校验"), pending.value(0).toInt(),
                pending.value(1), pending.value(2), pending.value(3),
                pending.value(4), uiText("Failed", "失败"),
                uiText("Read-back failed: %1", "读回失败：%1").arg(message));
            updateSdoTableEvidence(
                pending.value(0).toInt(), pending.value(1), pending.value(2),
                pending.value(4), uiText("Failed", "失败"),
                uiText("Automatic read-back failed after SDO write: %1",
                       "SDO 写入后的自动读回失败：%1")
                    .arg(message));
          }
          pendingSdoVerifications_.clear();
        }
      });
    // Connect client_ signal to handler
  connect(&client_, &EcatClient::daemonInfo, this, [this](const QString &text) {
    log("Runtime: " + text);
    updateStatusBar();
  });
  connect(&client_, &EcatClient::hostDiagnosticsReady, this,
          [this](const QJsonArray &checks) {
            updateHostHealth(checks);
            for (const auto &value : checks) {
              const auto check = value.toObject();
              QString message = check.value("message").toString();
              const QString hint = check.value("hint").toString();
    // Multi-branch condition check
              if (!hint.isEmpty()) {
                message += " | " + hint;
              }
              const QString command = check.value("command").toString();
              if (!command.isEmpty()) {
                message += " | " + command;
              }
              const QString detail = check.value("detail").toString();
              if (!detail.isEmpty()) {
                message += " | " + detail;
              }
              updateDiagnostics(check.value("level").toString("Info"),
                                check.value("source").toString("Host"),
                                message);
            }
            log(QString("Host check complete: %1 item(s)").arg(checks.size()));
          });
    // Connect client_ signal to handler
  connect(&client_, &EcatClient::masterText, this, [this](const QString &text) {
    lastMasterText_ = text;
    rawText_->masterText->setPlainText(text);
    updateMasterSummary(text);
    updateStatusBar();
  });
  connect(&client_, &EcatClient::slavesChanged, this,
          &MainWindow::updateSlaves);
  connect(&client_, &EcatClient::slaveTextResult, this,
          [this](const QString &title, int position, const QString &text) {
    // Multi-branch condition check
            if (position != selectedPosition()) {
              log(QString("Ignored stale %1 response for slave #%2")
                      .arg(title)
                      .arg(position));
              return;
            }
    // Multi-branch condition check
            if (title == "Info") {
              loadedSlaveInfoPosition_ = position;
              lastSlaveInfoText_ = text;
              rawText_->infoText->setPlainText(text);
              updateSlaveInfo(text);
            } else if (title == "PDO") {
              loadedPdoPosition_ = position;
              lastPdoText_ = text;
              rawText_->pdoText->setPlainText(text);
              updatePdoTable(text);
            } else if (title == "SDO") {
              loadedSdoPosition_ = position;
              lastSdoText_ = text;
              rawText_->sdoText->setPlainText(text);
              updateSdoTable(text);
            } else if (title == "ESI XML") {
              loadedXmlPosition_ = position;
              lastXmlText_ = text;
              rawText_->xmlText->setPlainText(text);
            }
            updateSlaveEvidenceSummary();
          });
  connect(
      &client_, &EcatClient::sdoValue, this,
      [this](int position, const QString &index, const QString &subIndex,
             const QString &value) {
        log(QString("SDO upload #%1 %2:%3 = %4")
                .arg(position)
                .arg(index, subIndex, value));
        const QString key = sdoEvidenceKey(position, index, subIndex);
        const bool currentTarget =
            isCurrentSdoTarget(position, index, subIndex);
        if (currentTarget && sdoInspector_->sdoValue) {
          sdoInspector_->sdoValue->setText(value);
          sdoInspector_->sdoValue->setPlaceholderText(uiText(
              "Read-back for current SDO target", "当前 SDO 目标的读回值"));
          updateActionAvailability();
        }
        const QString source = pendingSdoReads_.take(key);
        const QString readType = pendingSdoReadTypes_.take(key);
        updateSdoTableEvidence(
            position, index, subIndex, value, uiText("Complete", "完成"),
            source.isEmpty() ? uiText("Runtime response", "运行时返回")
                             : source);
        const QStringList verification = pendingSdoVerifications_.take(key);
        const bool hasStartupCheck = pendingStartupSdoChecks_.contains(key);
        const QVector<int> startupCheckRows =
            hasStartupCheck ? pendingStartupSdoChecks_.take(key)
                            : QVector<int>{};
        if (!verification.isEmpty()) {
          auto normalize = [](QString text) {
            return text.trimmed().remove(' ').toLower();
          };
          const QString expected = verification.value(4);
          const bool match = normalize(expected) == normalize(value);
          const QString verifyDetail =
              match ? uiText("Read-back matched expected value %1",
                             "读回值匹配期望值 %1")
                          .arg(expected)
                    : uiText("Read-back mismatch, expected %1 got %2",
                             "读回不匹配，期望 %1，实际 %2")
                          .arg(expected, value);
          appendSdoHistory(uiText("Verify", "校验"), position, index, subIndex,
                           verification.value(3), value,
                           match ? uiText("OK", "成功")
                                 : uiText("Failed", "失败"),
                           verifyDetail);
          updateSdoTableEvidence(position, index, subIndex, value,
                                 match ? uiText("OK", "成功")
                                       : uiText("Failed", "失败"),
                                 verifyDetail);
          updateDiagnostics(match ? "Info" : "Error", "SDO",
                            match ? QString("SDO write verified #%1 %2:%3 = %4")
                                        .arg(position)
                                        .arg(index, subIndex, value)
                                  : QString("SDO write verification failed #%1 "
                                            "%2:%3 expected %4 got %5")
                                        .arg(position)
                                        .arg(index, subIndex, expected, value));
        }
    // Multi-branch condition check
        if (hasStartupCheck) {
          auto normalize = [](QString text) {
            return text.trimmed().remove(' ').toLower();
          };
          for (const int startupCheckRow : startupCheckRows) {
    // Multi-branch condition check
            if (startupCheckRow < 0 ||
                startupCheckRow >= startupSdoTable_->rowCount()) {
              continue;
            }
            const QString expected =
                startupSdoTable_->item(startupCheckRow, 3)
                    ? startupSdoTable_->item(startupCheckRow, 3)->text()
                    : QString();
            const bool match = normalize(expected) == normalize(value);
            auto *status = startupSdoTable_->item(startupCheckRow, 5);
            if (!status) {
              status = new QTableWidgetItem;
              startupSdoTable_->setItem(startupCheckRow, 5, status);
            }
            auto *detail = startupSdoTable_->item(startupCheckRow, 6);
            if (!detail) {
              detail = new QTableWidgetItem;
              startupSdoTable_->setItem(startupCheckRow, 6, detail);
            }
            status->setText(match ? uiText("Verified", "已校验")
                                  : uiText("Mismatch", "不匹配"));
            status->setForeground(match ? QColor("#22c55e")
                                        : QColor("#ef4444"));
            status->setBackground(match ? QBrush()
                                        : (settings_.theme == "Light"
                                               ? QBrush(QColor("#fef2f2"))
                                               : QBrush(QColor("#3a1218"))));
            detail->setText(
                match ? uiText("Read-back matched %1", "读回值匹配 %1")
                            .arg(expected)
                      : uiText("Expected %1, got %2", "期望 %1，实际 %2")
                            .arg(expected, value));
            updateDiagnostics(
                match ? "Info" : "Error", "Startup SDO",
                match
                    ? QString("Startup SDO verified row %1")
                          .arg(startupCheckRow + 1)
                    : QString("Startup SDO mismatch row %1 expected %2 got %3")
                          .arg(startupCheckRow + 1)
                          .arg(expected, value));
          }
          startupSdoTable_->resizeColumnsToContents();
          updateWatchStartupDeltas();
        }
        appendSdoHistory(uiText("Read", "读取"), position, index, subIndex,
                         !readType.isEmpty() ? readType
                                             : (currentTarget && sdoInspector_->sdoType
                                                    ? sdoInspector_->sdoType->currentText()
                                                    : QString()),
                         value, uiText("Complete", "完成"),
                         source.isEmpty()
                             ? uiText("Runtime response", "运行时返回")
                             : source);
        ensureWatchTable();
        int row = -1;
        for (int i = 0; i < watch_->watchTable->rowCount(); ++i) {
          const bool match =
              (watch_->watchTable->item(i, 1) &&
               watch_->watchTable->item(i, 1)->text().toInt() == position) &&
              (watch_->watchTable->item(i, 2) &&
               watch_->watchTable->item(i, 2)->text().compare(
                   index, Qt::CaseInsensitive) == 0) &&
              (watch_->watchTable->item(i, 3) &&
               watch_->watchTable->item(i, 3)->text().compare(
                   subIndex, Qt::CaseInsensitive) == 0);
          if (match) {
            row = i;
            break;
          }
        }
        if (row < 0) {
          row = watch_->watchTable->rowCount();
          watch_->watchTable->insertRow(row);
          watch_->watchTable->setItem(row, 1,
                               new QTableWidgetItem(QString::number(position)));
          watch_->watchTable->setItem(row, 2, new QTableWidgetItem(index));
          watch_->watchTable->setItem(row, 3, new QTableWidgetItem(subIndex));
          watch_->watchTable->setItem(row, 8, new QTableWidgetItem);
          watch_->watchTable->setItem(row, 9, new QTableWidgetItem);
          watch_->watchTable->setItem(row, 10, new QTableWidgetItem);
          watch_->watchTable->setItem(row, 11, new QTableWidgetItem);
        }
        const bool changed =
            watchValues_.contains(key) && watchValues_.value(key) != value;
        watchValues_.insert(key, value);
        if (changed) {
          watchChangedKeys_.insert(key);
        }

        auto setCell = [this, row](int column, const QString &text) {
          auto *item = watch_->watchTable->item(row, column);
          if (!item) {
            item = new QTableWidgetItem;
            watch_->watchTable->setItem(row, column, item);
          }
          item->setText(text);
          return item;
        };

        setCell(0, QDateTime::currentDateTime().toString("HH:mm:ss"));
        auto *valueItem = setCell(4, value);
        const QString currentType = watch_->watchTable->item(row, 6)
                                        ? watch_->watchTable->item(row, 6)->text()
                                        : QString();
        const QString currentMode = watch_->watchTable->item(row, 7)
                                        ? watch_->watchTable->item(row, 7)->text()
                                        : QString();
    // Multi-branch condition check
        if (!watch_->watchTable->item(row, 6) ||
            watch_->watchTable->item(row, 6)->text().trimmed().isEmpty()) {
          const bool watchRefreshSource =
              source.contains("Watch", Qt::CaseInsensitive) ||
              source.contains("监视", Qt::CaseInsensitive);
    // Multi-branch condition check
          if (!watchRefreshSource && !readType.isEmpty()) {
            setCell(6, readType);
          } else if (!watchRefreshSource && currentTarget && sdoInspector_->sdoType) {
            setCell(6, sdoInspector_->sdoType->currentText());
          }
        }
        const QString effectiveType = watch_->watchTable->item(row, 6)
                                          ? watch_->watchTable->item(row, 6)->text()
                                          : currentType;
        setCell(5, decodeWatchValue(index, subIndex, effectiveType, value,
                                    currentMode));
        if (currentMode.trimmed().isEmpty()) {
          setCell(7, "Watch");
        }
        /* Feed OD-monitor charts that track this position:index:subIndex. */
        {
            const QString odPrefix = QString("od_%1_%2").arg(index, subIndex);
            for (auto *chart : openCharts_) {
                if (!chart) continue;
                if (chart->entryKey().startsWith(odPrefix)) {
                    bool numOk = false;
                    double v = value.toDouble(&numOk);
                    if (!numOk) v = value.toULongLong(&numOk, 16);
                    if (numOk) chart->feedValue(v);
                }
            }
        }

        updateWatchBaselineDelta(row);
        updateWatchStartupDelta(row);
        if (changed) {
          valueItem->setBackground(settings_.theme == "Light"
                                       ? QColor("#fff7cc")
                                       : QColor("#3a2f16"));
          valueItem->setForeground(settings_.theme == "Light"
                                       ? QColor("#854d0e")
                                       : QColor("#fde68a"));
        } else {
          valueItem->setBackground(QBrush());
          valueItem->setForeground(QBrush());
        }
        watch_->watchTable->resizeColumnsToContents();
        updateWatchAutoRefresh();
        updateSelectedDriveSummary();
      });
  connect(
      &client_, &EcatClient::commandSucceeded, this,
      [this](const QString &message) {
        log(message);
        if (message.startsWith("SDO download complete")) {
          const auto match =
              QRegularExpression(
                  R"(SDO download complete(?:\s+#(\d+))?:\s*(\S+):(\S+))")
                  .match(message);
          const QString position =
              match.hasMatch() && !match.captured(1).isEmpty()
                  ? match.captured(1)
                  : QString::number(selectedPosition());
          const QString index =
              match.hasMatch() ? match.captured(2)
                               : (sdoInspector_->sdoIndex ? sdoInspector_->sdoIndex->text() : QString());
          const QString subIndex =
              match.hasMatch()
                  ? match.captured(3)
                  : (sdoInspector_->sdoSubIndex ? sdoInspector_->sdoSubIndex->text() : QString());
          QStringList pending = pendingSdoWrites_.take(
              sdoEvidenceKey(position.toInt(), index, subIndex));
          if (pending.isEmpty()) {
            pending =
                pendingSdoWrites_.take(QString("%1|%2").arg(index, subIndex));
          }
          const QString type =
              pending.value(3, sdoInspector_->sdoType ? sdoInspector_->sdoType->currentText() : QString());
          const QString expectedValue = pending.value(
              4, sdoInspector_->sdoWriteValue ? sdoInspector_->sdoWriteValue->text() : QString());
          appendSdoHistory(uiText("Write", "写入"), position.toInt(),
                           pending.value(1, index), pending.value(2, subIndex),
                           type, expectedValue, uiText("Complete", "完成"),
                           uiText("%1; waiting for read-back verification",
                                  "%1；等待读回校验")
                               .arg(message));
          updateSdoTableEvidence(
              position.toInt(), pending.value(1, index),
              pending.value(2, subIndex), expectedValue,
              uiText("Write OK", "写入完成"),
              uiText("%1; waiting for read-back verification",
                     "%1；等待读回校验")
                  .arg(message));
          pendingSdoVerifications_.insert(
              sdoEvidenceKey(position.toInt(), index, subIndex),
              {position, index, subIndex, type, expectedValue});
          requestSdoRead(
              position.toInt(), index, subIndex,
              uiText("SDO write read-back verification", "SDO 写入读回校验"),
              type);
        }
        requestRefresh();
      });
  connect(&client_, &EcatClient::startupSdoResults, this,
          [this](const QJsonArray &results) {
            ensureStartupSdoTable();
            for (const auto &value : results) {
              const auto object = value.toObject();
              const int row = object.value("row").toInt(-1);
    // Multi-branch condition check
              if (row < 0 || row >= startupSdoTable_->rowCount()) {
                continue;
              }
              auto *status = startupSdoTable_->item(row, 5);
    // Multi-branch condition check
              if (!status) {
                status = new QTableWidgetItem;
                startupSdoTable_->setItem(row, 5, status);
              }
              auto *detail = startupSdoTable_->item(row, 6);
              if (!detail) {
                detail = new QTableWidgetItem;
                startupSdoTable_->setItem(row, 6, detail);
              }
              if (object.value("ok").toBool()) {
                status->setText(uiText("Applied", "已应用"));
                status->setForeground(QColor("#22c55e"));
                status->setBackground(QBrush());
                detail->setText(QString());
              } else {
                status->setText(uiText("Failed", "失败"));
                status->setForeground(QColor("#ef4444"));
                status->setBackground(settings_.theme == "Light"
                                          ? QColor("#fef2f2")
                                          : QColor("#3a1218"));
                detail->setText(object.value("error").toString());
              }
            }
            startupSdoTable_->resizeColumnsToContents();
            updateWatchStartupDeltas();
          });
  connect(&client_, &EcatClient::freeRunChanged, this,
          [this](bool running, const QString &status) {
            freeRun_ = running;
            lastFreeRunStatus_ = status;
            refreshTimer_->setInterval(running ? 500 : 3000);
            setMetricCard(freeRunLabel_, uiText("Free Run", "自由运行"),
                          running ? uiText("On", "开启")
                                  : uiText("Off", "关闭"));
            log("Free Run status: " + status);
            updateDiagnostics(running ? "Info" : "Info", "Free Run", status);
            updateStatusBar();
          });
  connect(&client_, &EcatClient::freeRunTelemetry, this,
          &MainWindow::updateFreeRunTelemetry);

  // RT stability test telemetry updates.
  connect(&client_, &EcatClient::rtTestTelemetry, this,
          [this](const QJsonObject &telemetry) {
            rtTestRunning_ = telemetry.value("running").toBool();
            updateRtTestTelemetry(telemetry);
            updateActionAvailability();
          });


  // ── EventBus wiring ──────────────────────────────────────────────
  connect(&client_, &EcatClient::slavesChanged, eventBus_, [this](const QVector<SlaveInfo> &s) {
    eventBus_->emitSlaveChanged(s);
  });
  connect(&client_, &EcatClient::sdoValue, eventBus_, [this](int p, const QString &i, const QString &si, const QString &v) {
    eventBus_->emitSdoValue(p, i, si, v);
  });
  connect(&client_, &EcatClient::connectionStateChanged, eventBus_, [this](ConnectionState state) {
    eventBus_->emitConnectionStateChanged(state == ConnectionState::Connected);
  });
  connect(&client_, &EcatClient::freeRunTelemetry, eventBus_, [this](const QJsonObject &tel) {
    eventBus_->emitFreeRunTelemetry(tel);
  });
  connectRetryTimer_ = new QTimer(this);
  connectRetryTimer_->setInterval(600);
  connect(connectRetryTimer_, &QTimer::timeout, &client_,
          &EcatClient::connectToDaemon);

  refreshTimer_ = new QTimer(this);
  refreshTimer_->setInterval(3000);
  connect(refreshTimer_, &QTimer::timeout, this, &MainWindow::requestRefresh);
  refreshTimer_->start();

  watchRefreshTimer_ = new QTimer(this);
  connect(watchRefreshTimer_, &QTimer::timeout, this,
          [this] { refreshWatchList(true); });
  updateWatchAutoRefresh();
}

// Launch the embedded ecatd process and start the connection-retry timer
void MainWindow::startEmbeddedDaemon() {
    // Connect daemon_ signal to handler
  connect(&daemon_, &QProcess::readyReadStandardError, this, [this] {
    const QString text =
        QString::fromLocal8Bit(daemon_.readAllStandardError()).trimmed();
    if (!text.isEmpty()) {
      log("ecatd: " + text);
    }
  });
    // Connect daemon_ signal to handler
  connect(&daemon_, &QProcess::readyReadStandardOutput, this, [this] {
    const QString text =
        QString::fromLocal8Bit(daemon_.readAllStandardOutput()).trimmed();
    if (!text.isEmpty()) {
      log("ecatd: " + text);
    }
  });
  connect(&daemon_, &QProcess::errorOccurred, this,
          [this](QProcess::ProcessError) {
            log("Failed to launch embedded ecatd; trying to connect to an "
                "external runtime");
          });

  daemon_.setProgram(ecatdPath());
  daemon_.start();
  connectRetryTimer_->start();
}

// Periodic refresh: ping daemon, poll master state, scan slaves
void MainWindow::requestRefresh() {
    // Multi-branch condition check
  if (!client_.isConnected()) {
    return;
  }
  client_.ping();
  client_.freeRunStatus();
  if (!freeRun_) {
    client_.master();
    client_.scan();
  }
  // Poll RT test status if it's running.
  if (rtTestRunning_) {
    client_.rtTestStatus();
  }
}

// Build a human-readable summary of Free Run impact on the selected slave
QStringList MainWindow::freeRunImpactDetails() const {
  QStringList details;
  const int selected = selectedPosition();

  QString slaveName = uiText("unknown slave", "未知从站");
  QString slaveState = uiText("unknown", "未知");
  int opSlaves = 0;
  int safeOpSlaves = 0;
  int preOpSlaves = 0;
  int initSlaves = 0;
  for (const auto &slave : slaves_) {
    const QString state = slave.state.trimmed().toUpper();
    // Multi-branch condition check
    if (state == "OP") {
      ++opSlaves;
    } else if (state == "SAFEOP") {
      ++safeOpSlaves;
    } else if (state == "PREOP") {
      ++preOpSlaves;
    } else if (state == "INIT") {
      ++initSlaves;
    }
    if (slave.position == selected) {
      slaveName =
          slave.name.trimmed().isEmpty() ? slaveName : slave.name.trimmed();
      slaveState =
          slave.state.trimmed().isEmpty() ? slaveState : slave.state.trimmed();
    }
  }

  details << uiText("Selected slave context: #%1 %2, state %3",
                    "选中从站上下文：#%1 %2，状态 %3")
                 .arg(selected)
                 .arg(slaveName, slaveState);
  details << uiText("Bus state mix: OP %1, SAFEOP %2, PREOP %3, INIT %4",
                    "总线状态分布：OP %1，SAFEOP %2，PREOP %3，INIT %4")
                 .arg(opSlaves)
                 .arg(safeOpSlaves)
                 .arg(preOpSlaves)
                 .arg(initSlaves);

  int pdoRows = 0;
  int rxPdoRows = 0;
  int txPdoRows = 0;
  int rxBits = 0;
  int txBits = 0;
  QStringList rxPreview;
    // Multi-branch condition check
  if (sdo_->pdoTable) {
    pdoRows = sdo_->pdoTable->rowCount();
    for (int row = 0; row < sdo_->pdoTable->rowCount(); ++row) {
      const QString pdo = tableText(sdo_->pdoTable, row, 1);
      const int bits = tableText(sdo_->pdoTable, row, 4).toInt();
      const QString name = tableText(sdo_->pdoTable, row, 5);
      const QString address = QString("%1:%2").arg(
          tableText(sdo_->pdoTable, row, 2), tableText(sdo_->pdoTable, row, 3));
    // Multi-branch condition check
      if (pdo.contains("RxPDO", Qt::CaseInsensitive)) {
        ++rxPdoRows;
        rxBits += bits;
    // Multi-branch condition check
        if (rxPreview.size() < 4) {
          rxPreview << QString("%1 %2").arg(address, name);
        }
      } else if (pdo.contains("TxPDO", Qt::CaseInsensitive)) {
        ++txPdoRows;
        txBits += bits;
      }
    }
  }
  details << uiText("PDO map evidence: %1 row(s), RxPDO/output %2 (%3 bit), "
                    "TxPDO/input %4 (%5 bit)",
                    "PDO 映射证据：%1 行，RxPDO/输出 %2（%3 bit），TxPDO/输入 "
                    "%4（%5 bit）")
                 .arg(pdoRows)
                 .arg(rxPdoRows)
                 .arg(rxBits)
                 .arg(txPdoRows)
                 .arg(txBits);
    // Multi-branch condition check
  if (pdoRows <= 0) {
    details << uiText("PDO map warning: no PDO rows are loaded for review",
                      "PDO 映射警告：当前没有可复核的 PDO 行");
  } else if (rxPdoRows > 0) {
    details << uiText("Output risk: Free Run may exchange %1 RxPDO/output "
                      "entry row(s)",
                      "输出风险：Free Run 可能交换 %1 条 RxPDO/输出条目")
                   .arg(rxPdoRows);
    // Multi-branch condition check
    if (!rxPreview.isEmpty()) {
      details << uiText("Output preview: %1", "输出预览：%1")
                     .arg(rxPreview.join(" | "));
    }
  }

    // Multi-branch condition check
  if (freeRunWidgets_->freeRunEntryTable && freeRunWidgets_->freeRunEntryTable->rowCount() > 0) {
    int outputEntries = 0;
    int inputEntries = 0;
    QStringList meaningPreview;
    for (int row = 0; row < freeRunWidgets_->freeRunEntryTable->rowCount(); ++row) {
      const QString direction = tableText(freeRunWidgets_->freeRunEntryTable, row, 2).toLower();
    // Multi-branch condition check
      if (direction.contains("rx") || direction.contains("out")) {
        ++outputEntries;
      } else if (direction.contains("tx") || direction.contains("in")) {
        ++inputEntries;
      }
      const QString meaning = tableText(freeRunWidgets_->freeRunEntryTable, row, 12);
    // Multi-branch condition check
      if (!meaning.isEmpty() && meaningPreview.size() < 3) {
        meaningPreview << meaning;
      }
    }
    details << uiText("Previous Free Run cache: %1 entries, output-like %2, "
                      "input-like %3",
                      "上次 Free Run 缓存：%1 项，输出类 %2，输入类 %3")
                   .arg(freeRunWidgets_->freeRunEntryTable->rowCount())
                   .arg(outputEntries)
                   .arg(inputEntries);
    // Multi-branch condition check
    if (!meaningPreview.isEmpty()) {
      details << uiText("Decoded evidence: %1", "解析证据：%1")
                     .arg(meaningPreview.join(" | "));
    }
  }

  QString statusword;
  QString errorCode;
    // Multi-branch condition check
  if (watch_->watchTable && selected >= 0) {
    for (int row = 0; row < watch_->watchTable->rowCount(); ++row) {
      const int rowPosition = tableText(watch_->watchTable, row, 1).toInt();
    // Multi-branch condition check
      if (rowPosition != selected) {
        continue;
      }
      const QString index = normalizeHexText(tableText(watch_->watchTable, row, 2), 4);
      const QString value = tableText(watch_->watchTable, row, 4);
      const QString decoded = tableText(watch_->watchTable, row, 5);
    // Multi-branch condition check
      if (index == "0x6041" && !decoded.isEmpty()) {
        statusword = decoded;
      } else if (index == "0x603f" && !decoded.isEmpty() && !value.isEmpty() &&
                 value != "0" && value.toLower() != "0x0000") {
        errorCode = decoded;
      }
    }
  }
    // Multi-branch condition check
  if (!statusword.isEmpty() || !errorCode.isEmpty()) {
    QStringList driveFacts;
    // Multi-branch condition check
    if (!statusword.isEmpty()) {
      driveFacts << uiText("statusword %1", "状态字 %1").arg(statusword);
    }
    if (!errorCode.isEmpty()) {
      driveFacts << uiText("error %1", "错误 %1").arg(errorCode);
    }
    details << uiText("Drive Watch evidence: %1", "驱动 Watch 证据：%1")
                   .arg(driveFacts.join(" | "));
  } else {
    details << uiText(
        "Drive Watch evidence: no CiA 402 status/error watch rows",
        "驱动 Watch 证据：没有 CiA 402 状态/错误监视行");
  }

  const QStringList topologyIssues = topologyBaselineIssues();
  if (!topologyIssues.isEmpty()) {
    details << uiText("Topology baseline: %1 issue(s); review before Free Run",
                      "拓扑基线：%1 个问题；启动 Free Run 前请复核")
                   .arg(topologyIssues.size());
  }
  details << consistencyGateDetails(uiText("Free Run start", "启动 Free Run"));
  details << uiText(
      "GUI refresh will switch to 500 ms while Free Run is active.",
      "Free Run 运行时 GUI 刷新周期会切换到 500 ms。");
  return details;
}

// Start or stop Free Run — includes a safety confirmation dialog
void MainWindow::setFreeRun(bool enabled) {
    // Multi-branch condition check
  if (enabled && client_.isConnected()) {
    QStringList details = {
        uiText("Master: %1", "主站：%1").arg(activeMasterName()),
        uiText("Selected slave: #%1", "选中从站：#%1").arg(selectedPosition()),
        uiText("Detected slaves: %1", "检测到从站：%1").arg(slaves_.size()),
        uiText("Free Run exchanges cyclic process-image telemetry without a "
               "PLC project.",
               "自由运行会在没有 PLC 工程的情况下交换周期过程映像遥测。"),
        uiText("Review PDO outputs and actuator safety before continuing.",
               "继续前请复核 PDO 输出和执行机构安全。"),
    };
    details << freeRunImpactDetails();
    if (!confirmDangerousOperation(
            uiText("Confirm Free Run Start", "确认启动自由运行"),
            uiText("This operation starts cyclic Free Run telemetry.",
                   "此操作会启动周期自由运行遥测。"),
            details, uiText("Start Free Run", "启动自由运行"))) {
      if (auto *action = findChild<QAction *>("freeRunAction")) {
        QSignalBlocker blocker(action);
        action->setChecked(false);
      }
      freeRun_ = false;
      setMetricCard(freeRunLabel_, uiText("Free Run", "自由运行"),
                    uiText("Off", "关闭"));
      updateStatusBar();
      return;
    }
  }
  freeRun_ = enabled;
  refreshTimer_->setInterval(enabled ? 500 : 3000);
  setMetricCard(freeRunLabel_, uiText("Free Run", "自由运行"),
                enabled ? uiText("On", "开启") : uiText("Off", "关闭"));
    // Multi-branch condition check
  if (!client_.isConnected()) {
    log(enabled ? "Free Run armed; waiting for runtime" : "Free Run disabled");
    return;
  }

  if (enabled) {
    log("Free Run enabled: starting runtime PDO cycle and using 500 ms online "
        "refresh");
    client_.freeRunStart();
  } else {
    log("Free Run disabled: stopping runtime PDO cycle and restoring slow "
        "refresh");
    client_.freeRunStop();
  }
}

// Update the detail strip below the Free Run entry table
void MainWindow::updateFreeRunEntryDetail() {
  if (!freeRunWidgets_->freeRunEntryDetailLabel) {
    return;
  }
  const FreeRunEntryDetailTexts texts = {
      .unavailableText = uiText("Process-image evidence is not available.",
                                "当前没有可用的过程映像证据。"),
      .unavailableTip =
          uiText("This preview is local only and does not access the bus.",
                 "此预览仅在本地工作，不访问总线。"),
      .noSelectionText = uiText(
          "Select a visible process-image row to review name source, value, "
          "PDO map evidence, and output boundary.",
          "选择一条可见过程映像行，以复核名称来源、数值、PDO "
          "映射证据和输出边界。"),
      .noSelectionTip =
          uiText("Filtering, row selection, and this detail preview are local "
                 "UI actions. Toggling Free Run remains the explicit online "
                 "action.",
                 "过滤、选择行和此详情预览都是本地界面动作；切换 Free Run "
                 "仍是显式在线动作。"),
      .unknown = uiText("Unknown", "未知"),
      .directionFallback = uiText("Dir?", "方向?"),
      .unnamed = uiText("Unnamed", "未命名"),
      .emptyValue = uiText("Empty", "空"),
      .noMapEvidence = uiText("No map evidence", "无映射证据"),
      .outputBoundary = uiText("Output-like process data", "输出类过程数据"),
      .inputBoundary =
          uiText("Input/telemetry process data", "输入/遥测过程数据"),
      .mappedText = uiText("Mapped", "已映射"),
      .summaryPattern =
          uiText("#%1 %2 %3:%4 | %5 | %6 bit @ %7.%8 | %9 | Value: "
                 "%10 | Map: %11",
                 "#%1 %2 %3:%4 | %5 | %6 bit @ %7.%8 | %9 | 值：%10 | "
                 "映射：%11"),
      .nameSourceMarkers = {uiText("Name source:", "名称来源："),
                            QStringLiteral("name source:")},
      .selectedTitle = uiText("Selected Free Run process-image entry",
                              "选中的 Free Run 过程映像条目"),
      .slaveLabel = uiText("Slave", "从站"),
      .syncManagerLabel = uiText("Sync Manager", "同步管理器"),
      .directionLabel = uiText("Direction", "方向"),
      .pdoLabel = uiText("PDO", "PDO"),
      .objectLabel = uiText("Object", "对象"),
      .nameLabel = uiText("Name", "名称"),
      .nameSourceLabel = uiText("Name Source", "名称来源"),
      .locationLabel = uiText("Location", "位置"),
      .rawLabel = uiText("Raw", "原始值"),
      .decodedLabel = uiText("Decoded", "解码值"),
      .meaningLabel = uiText("Meaning", "含义"),
      .mapStatusLabel = uiText("Map Status", "映射状态"),
      .mapDetailLabel = uiText("Map Detail", "映射详情"),
      .changedLabel = uiText("Changed", "是否变化"),
      .yesText = uiText("Yes", "是"),
      .noText = uiText("No", "否"),
      .boundaryLabel = uiText("Boundary", "边界"),
      .localBoundary = uiText(
          "Local preview boundary: selecting this row, filtering rows, and "
          "reading this detail strip do not read SDOs, write SDOs, change "
          "state, toggle Free Run, or run Host Health.",
          "本地预览边界：选择此行、过滤行和查看此详情条都不读取 SDO、不写 SDO、"
          "不切换状态、不改变 Free Run，也不运行 Host Health。"),
      .executionBoundary = uiText(
          "Execution boundary: Toggle Free Run remains the explicit online "
          "action; Fill SDO Fields and Alt+Enter only refill local SDO target "
          "fields unless you explicitly choose Fill and Read.",
          "执行边界：切换 Free Run 仍是显式在线动作；填充 SDO 字段和 Alt+Enter "
          "只会本地回填 SDO 目标，除非明确选择填充并读取。"),
  };

  auto applyState = [this](const FreeRunEntryDetailState &state) {
    freeRunWidgets_->freeRunEntryDetailLabel->setText(state.text);
    freeRunWidgets_->freeRunEntryDetailLabel->setProperty("severity", state.severityKey);
    freeRunWidgets_->freeRunEntryDetailLabel->setToolTip(state.tooltip);
    repolish(freeRunWidgets_->freeRunEntryDetailLabel);
  };

  if (!freeRunWidgets_->freeRunEntryTable) {
    applyState(freeRunEntryDetailUnavailableState(texts));
    return;
  }

  const int row = freeRunWidgets_->freeRunEntryTable->currentRow();
  if (row < 0 || row >= freeRunWidgets_->freeRunEntryTable->rowCount() ||
      freeRunWidgets_->freeRunEntryTable->isRowHidden(row)) {
    applyState(freeRunEntryDetailNoSelectionState(texts));
    return;
  }

  applyState(buildFreeRunEntryDetailState(
      freeRunEntryTableRowFromTable(freeRunWidgets_->freeRunEntryTable, row), texts));
}

// Append a timestamped message to the diagnostics log