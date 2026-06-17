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
  if (colorKey == QStringLiteral("error")) {
// Semantic color for diagnostics-event severity badges
    return QColor("#ef4444");
  }
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
  buildUi();
  applySettings();
// Persist window geometry and gracefully shut down embedded daemon
  wire();
  setMinimumSize(1120, 720);
  QSettings settings("NekoEcatStudio", "NekoEcatStudio");
  const QByteArray geometry = settings.value("geometry").toByteArray();
  if (geometry.isEmpty() || !restoreGeometry(geometry)) {
    resize(1440, 900);
  }
  restoreState(settings.value("windowState").toByteArray());
  startEmbeddedDaemon();
}

MainWindow::~MainWindow() {
  saveSettings();
  QSettings settings("NekoEcatStudio", "NekoEcatStudio");
  settings.setValue("geometry", saveGeometry());
  settings.setValue("windowState", saveState());
  if (daemon_.state() != QProcess::NotRunning) {
    daemon_.terminate();
    if (!daemon_.waitForFinished(1200)) {
      daemon_.kill();
    }
  }
}

// Global key handler: Alt+Return triggers evidence action on focused table
bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
  if (event && event->type() == QEvent::KeyPress) {
    auto *keyEvent = static_cast<QKeyEvent *>(event);
    const bool isReturnKey =
        keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter;
    if (isReturnKey && (keyEvent->modifiers() & Qt::AltModifier)) {
      if (auto *table = qobject_cast<QTableWidget *>(watched)) {
        return runLocalEvidenceAction(table);
      }
      if (auto *viewport = qobject_cast<QWidget *>(watched)) {
        if (auto *table = qobject_cast<QTableWidget *>(viewport->parent())) {
          return runLocalEvidenceAction(table);
        }
      }
    }
  }
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
  if (page == overviewPage_) {
    return WorkspaceBoundaryKind::Overview;
  }
  if (page == objectDictionaryPage_) {
    return WorkspaceBoundaryKind::ObjectDictionary;
  }
  if (page == pdoMapPage_) {
    return WorkspaceBoundaryKind::PdoMap;
  }
  if (page == watchPage_) {
    return WorkspaceBoundaryKind::Watch;
  }
  if (page == startupSdoPage_) {
    return WorkspaceBoundaryKind::StartupSdo;
  }
  if (page == freeRunPage_) {
    return WorkspaceBoundaryKind::FreeRun;
  }
  if (page == ioVariablePage_) {
    return WorkspaceBoundaryKind::IoVariables;
  }
  if (page == consistencyPage_) {
    return WorkspaceBoundaryKind::Consistency;
  }
  if (page == stateMachinePage_) {
    return WorkspaceBoundaryKind::StateMachine;
  }
  if (page == diagnosticsPage_) {
    return WorkspaceBoundaryKind::Diagnostics;
  }
  if (page == esiRepositoryPage_ || page == esiXmlPage_) {
    return WorkspaceBoundaryKind::Esi;
  }
  if (page == rtTestPage_) {
    return WorkspaceBoundaryKind::RtTest;
  }
  if (page == notesPage_) {
    return WorkspaceBoundaryKind::Notes;
  }
  return WorkspaceBoundaryKind::RawEvidence;
}

// Record a tab visit for back/forward navigation (capped at 80 entries)
void MainWindow::recordWorkspaceHistory(int index) {
  if (!tabs_ || suppressWorkspaceHistory_ || index < 0 ||
      index >= tabs_->count()) {
    return;
  }
  auto *page = tabs_->widget(index);
  if (!page) {
    return;
  }
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
  if (!tabs_ || workspaceForwardStack_.isEmpty()) {
    updateWorkspaceNavigationActions();
    return;
  }
  QWidget *target = workspaceForwardStack_.takeLast();
  QSignalBlocker blocker(tabs_);
  suppressWorkspaceHistory_ = true;
  activateWorkspacePage(target);
  suppressWorkspaceHistory_ = false;
  if (target &&
      (workspaceBackStack_.isEmpty() || workspaceBackStack_.last() != target)) {
    workspaceBackStack_.append(target);
  }
  updateWorkspaceNavigationActions();
}

// Enable/disable back/forward actions based on stack depth
void MainWindow::updateWorkspaceNavigationActions() {
  if (auto *action = findChild<QAction *>("workspaceBackAction")) {
    action->setEnabled(workspaceBackStack_.size() >= 2);
  }
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
      hasSlave && sdoIndex_ && !sdoIndex_->text().trimmed().isEmpty() &&
      sdoSubIndex_ && !sdoSubIndex_->text().trimmed().isEmpty();
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
      objectBookmarkTable_ && objectBookmarkTable_->rowCount() > 0;
  const bool hasObjectBookmarkSelection =
      objectBookmarkTable_ && !selectedObjectBookmarkRows().isEmpty();
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
      sdoTargetTable_ && sdoTargetTable_->currentRow() >= 0 &&
      sdoTargetTable_->currentRow() < sdoTargetTable_->rowCount();

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
  setEnabled("useSdoValue", selectedSdoWritable_ && sdoValue_ &&
                                !sdoValue_->text().trimmed().isEmpty());
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
  if (sdoWriteValue_) {
    sdoWriteValue_->setEnabled(selectedSdoWritable_);
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
  if (objectBookmarkTable_) {
    objectBookmarkTable_->setToolTip(
        uiText(
            "%1 project object bookmark(s). Double-click fills the SDO target; "
            "reads and writes still require explicit user actions.",
            "%1 个工程对象书签。双击只回填 SDO "
            "目标；读取和写入仍需用户显式操作。")
            .arg(hasObjectBookmarks ? objectBookmarkTable_->rowCount() : 0));
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
void MainWindow::loadSettings() {
  QSettings settings("NekoEcatStudio", "NekoEcatStudio");
  settings_.theme = settings.value("preferences/theme", "Dark").toString();
  settings_.language =
      settings.value("preferences/language", "English").toString();
  settings_.scale = settings.value("preferences/scale", 1.0).toDouble();
  settings_.masters.clear();
  const int count = settings.beginReadArray("preferences/masters");
  for (int i = 0; i < count; ++i) {
    settings.setArrayIndex(i);
    MasterProfile profile;
    profile.name =
        settings.value("name", QString("Master %1").arg(i)).toString();
    profile.target =
        settings.value("target", QString::number(i)).toString().trimmed();
    if (!profile.target.isEmpty()) {
      settings_.masters.append(profile);
    }
  }
  settings.endArray();
  if (settings_.masters.isEmpty()) {
    settings_.masters.append(MasterProfile{});
  }
  settings_.activeMaster =
      settings
          .value("preferences/activeMaster", settings_.masters.first().target)
          .toString()
          .trimmed();
  if (settings_.activeMaster.isEmpty()) {
    settings_.activeMaster = settings_.masters.first().target;
  }
  bool known = false;
  for (const auto &profile : settings_.masters) {
    if (profile.target == settings_.activeMaster) {
      known = true;
      break;
    }
  }
  if (!known) {
    settings_.masters.prepend(
        MasterProfile{QString("Master %1").arg(settings_.activeMaster),
                      settings_.activeMaster});
  }
}

// Persist current preferences to QSettings
void MainWindow::saveSettings() {
  QSettings settings("NekoEcatStudio", "NekoEcatStudio");
  settings.setValue("preferences/theme", settings_.theme);
  settings.setValue("preferences/language", settings_.language);
  settings.setValue("preferences/scale", settings_.scale);
  settings.setValue("preferences/activeMaster", settings_.activeMaster);
  settings.beginWriteArray("preferences/masters");
  for (int i = 0; i < settings_.masters.size(); ++i) {
    settings.setArrayIndex(i);
    settings.setValue("name", settings_.masters[i].name);
    settings.setValue("target", settings_.masters[i].target);
  }
  settings.endArray();
}

// Open settings dialog; apply changes and rebuild UI if language changed
void MainWindow::openSettings() {
  const QString previousLanguage = settings_.language;
  const QString previousMaster = settings_.activeMaster;
  SettingsDialog dialog(settings_, this);
  if (dialog.exec() != QDialog::Accepted) {
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
    QMessageBox::information(this, uiText("Settings", "设置"),
                             uiText("Language was applied.", "语言已应用。"));
    return;
  }
  applySettings();
  QMessageBox::information(this, uiText("Settings", "设置"),
                           uiText("Settings were applied.", "设置已应用。"));
}

// Bilingual string selector — returns Chinese or English based on current language
QString MainWindow::uiText(const QString &english, const QString &zh) const {
  return settings_.language == "简体中文" ? zh : english;
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
    if (masters[i].target == settings_.activeMaster) {
      activeIndex = i;
    }
  }
  masterCombo_->setCurrentIndex(activeIndex);
}

// Switch the active EtherCAT master — clears views if the target changed
void MainWindow::setActiveMaster(const QString &target) {
  const QString next = target.trimmed().isEmpty() ? "0" : target.trimmed();
  if (next == settings_.activeMaster) {
    return;
  }
  if (freeRun_ && client_.isConnected()) {
    client_.freeRunStop();
  }
  settings_.activeMaster = next;
  bool known = false;
  for (const auto &profile : settings_.masters) {
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
      if (lower.startsWith(QString::fromUtf8(word).toLower())) {
        return true;
      }
    }
    return false;
  };

  for (const QString &detail : details) {
    const QString trimmed = detail.trimmed();
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
      const QString index = normalizeHexText(tableText(watch_->watchTable, row, 2), 4);
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
  if (freeRunEntryTable_) {
    for (int row = 0; row < freeRunEntryTable_->rowCount(); ++row) {
      if (tableText(freeRunEntryTable_, row, 0).toInt() != position) {
        continue;
      }
      ++freeRunRows;
      if (hasPdoMapIssueEvidence(tableText(freeRunEntryTable_, row, 13))) {
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

  if (!statusword.isEmpty() || !modeDisplay.isEmpty() || !errorCode.isEmpty()) {
    QStringList driveFacts;
    if (!statusword.isEmpty()) {
      driveFacts << uiText("statusword %1", "状态字 %1").arg(statusword);
    }
    if (!modeDisplay.isEmpty()) {
      driveFacts << uiText("mode %1", "模式 %1").arg(modeDisplay);
    }
    if (!errorCode.isEmpty()) {
      driveFacts << uiText("error %1", "错误 %1").arg(errorCode);
    }
    details << uiText("Drive evidence: %1", "驱动证据：%1")
                   .arg(driveFacts.join(" | "));
  } else {
    details << uiText("Drive evidence: no CiA 402 Watch values",
                      "驱动证据：没有 CiA 402 Watch 值");
  }

  if (startupRows > 0 || startupDiffs > 0) {
    details << uiText("Startup evidence: %1 row(s), %2 Watch mismatch(es)",
                      "Startup 证据：%1 行，%2 条 Watch 不一致")
                   .arg(startupRows)
                   .arg(startupDiffs);
  }
  if (mapIssues > 0) {
    details << uiText("PDO map evidence: %1 Free Run map issue(s)",
                      "PDO 映射证据：Free Run 有 %1 个映射问题")
                   .arg(mapIssues);
  }

  const QStringList topologyIssues = topologyBaselineIssues();
  if (!topologyIssues.isEmpty()) {
    details << uiText("Topology baseline: %1 issue(s); review before state "
                      "transition",
                      "拓扑基线：%1 个问题；切换状态前请复核")
                   .arg(topologyIssues.size());
  }
  details << consistencyGateDetails(uiText("state transition", "状态切换"));

  if (target == "OP" || target == "SAFEOP") {
    if (pdoRows <= 0) {
      details << uiText("Risk: PDO Map is not loaded for this slave",
                        "风险：当前从站尚未加载 PDO 映射");
    }
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
  }

  int freeRunRows = 0;
  int mapIssues = 0;
  if (freeRunEntryTable_) {
    for (int row = 0; row < freeRunEntryTable_->rowCount(); ++row) {
      if (tableText(freeRunEntryTable_, row, 0).toInt() != position) {
        continue;
      }
      ++freeRunRows;
      if (hasPdoMapIssueEvidence(tableText(freeRunEntryTable_, row, 13))) {
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
    if (freeRunEntryTable_) {
      for (int row = 0; row < freeRunEntryTable_->rowCount(); ++row) {
        if (tableText(freeRunEntryTable_, row, 0).toInt() != position) {
          continue;
        }
        ++freeRunRows;
        if (hasPdoMapIssueEvidence(tableText(freeRunEntryTable_, row, 13))) {
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
  if (infoText_) {
    infoText_->setPlainText(uiText("Loading selected slave identity for %1...",
                                   "正在加载 %1 的选中从站身份信息...")
                                .arg(target));
  }
  if (pdoText_) {
    pdoText_->setPlainText(
        uiText("Loading PDO Map for %1...", "正在加载 %1 的 PDO 映射...")
            .arg(target));
  }
  if (sdoText_) {
    sdoText_->setPlainText(uiText("Loading Object Dictionary for %1...",
                                  "正在加载 %1 的对象字典...")
                               .arg(target));
  }
  if (xmlText_) {
    xmlText_->setPlainText(
        uiText("Loading ESI XML for %1...", "正在加载 %1 的 ESI XML...")
            .arg(target));
  }

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
  if (sdoWriteValue_) {
    sdoWriteValue_->setEnabled(true);
    sdoWriteValue_->setPlaceholderText(uiText("Value to write", "写入值"));
  }
  if (topologyTree_) {
    topologyTree_->clear();
  }
  for (auto *table :
       {metricTable_, workflowTable_, stateMachine_->stateMachineTable, identityTable_,
        slaveEvidenceMatrixTable_, portTable_, mailboxTable_, sdo_->pdoTable,
        sdo_->sdoTable, sdoHistoryTable_, freeRunTable_, freeRunEntryTable_,
        ioVar_->ioVariableTable, watch_->watchTable}) {
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
  for (auto *editor : {masterText_, infoText_, pdoText_, sdoText_, xmlText_}) {
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
  if (connectRetryTimer_) {
    connectRetryTimer_->stop();
    disconnect(connectRetryTimer_, nullptr, this, nullptr);
    connectRetryTimer_->deleteLater();
    connectRetryTimer_ = nullptr;
  }
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
            if (client_.isConnected() && selectedPosition() >= 0) {
              client_.sdos(selectedPosition());
            }
          });
  connect(findChild<QPushButton *>("contextPdoMap"), &QPushButton::clicked,
          this, [this] {
            activateWorkspaceTab(pdoMapTabIndex_);
            if (client_.isConnected() && selectedPosition() >= 0) {
              client_.pdos(selectedPosition());
            }
          });
  connect(findChild<QPushButton *>("contextWatch"), &QPushButton::clicked, this,
          [this] {
            activateWorkspaceTab(watchTabIndex_);
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
            if (index >= 0) {
              setActiveMaster(masterCombo_->itemData(index).toString());
            }
          });

  connect(topologyTree_, &QTreeWidget::currentItemChanged, this,
          [this](QTreeWidgetItem *item) {
            if (item) {
              setSelectedSlave(item->data(0, Qt::UserRole).toInt());
            }
          });
  connect(topologyTree_, &QTreeWidget::customContextMenuRequested, this,
          &MainWindow::showTopologyContextMenu);

  connect(findChild<QPushButton *>("readSdo"), &QPushButton::clicked, this,
          [this] {
            if (selectedPosition() >= 0) {
              requestSdoRead(selectedPosition(), sdoIndex_->text(),
                             sdoSubIndex_->text(),
                             uiText("Manual SDO read", "手动 SDO 读取"));
            }
          });
  connect(findChild<QPushButton *>("readTargetSdo"), &QPushButton::clicked,
          this, [this] {
            if (selectedPosition() >= 0) {
              requestSdoRead(selectedPosition(), sdoIndex_->text(),
                             sdoSubIndex_->text(),
                             uiText("Selected Object panel", "选中对象面板"));
            }
          });
  connect(findChild<QPushButton *>("reviewSdoWriteDelta"),
          &QPushButton::clicked, this, &MainWindow::reviewCurrentSdoWriteDelta);
  connect(findChild<QPushButton *>("runSdoTargetRowAction"),
          &QPushButton::clicked, this, [this] {
            openSdoTargetPanelRow(
                sdoTargetTable_ ? sdoTargetTable_->currentRow() : -1);
          });
  connect(findChild<QPushButton *>("copySdoTargetRowEvidence"),
          &QPushButton::clicked, this, [this] {
            copySdoTargetPanelRowDigest(
                sdoTargetTable_ ? sdoTargetTable_->currentRow() : -1);
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
  connect(sdoTargetTable_, &QTableWidget::customContextMenuRequested, this,
          &MainWindow::showSdoTargetPanelContextMenu);
  connect(sdoTargetTable_, &QTableWidget::currentCellChanged, this,
          [this](int, int, int, int) { updateActionAvailability(); });
  connect(sdoTargetTable_, &QTableWidget::cellDoubleClicked, this,
          [this](int row, int) { openSdoTargetPanelRow(row); });
  sdoTargetTable_->installEventFilter(this);
  if (sdoTargetTable_->viewport()) {
    sdoTargetTable_->viewport()->installEventFilter(this);
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
                objectBookmarkTable_ ? objectBookmarkTable_->currentRow() : -1,
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
  connect(session_->sessionBriefCopyButton, &QPushButton::clicked, this, [this] {
    copySessionBriefRowDigest(
        session_->sessionBriefTable ? session_->sessionBriefTable->currentRow() : -1);
  });
  connect(session_->sessionBriefTable, &QTableWidget::cellDoubleClicked, this,
          [this](int row) { openSessionBriefRow(row); });
  connect(workflowTable_, &QTableWidget::itemSelectionChanged, this,
          &MainWindow::updateWorkflowStepCopyButton);
  connect(workflowTable_, &QTableWidget::itemSelectionChanged, this,
          &MainWindow::updateWorkflowStepDetail);
  connect(workflowStepCopyButton_, &QPushButton::clicked, this, [this] {
    copyWorkflowStepDigest(workflowTable_ ? workflowTable_->currentRow() : -1);
  });
  connect(workflowReviewButton_, &QPushButton::clicked, this,
          &MainWindow::reviewFirstCommissioningWorkflowIssue);
  connect(workflowReviewNextButton_, &QPushButton::clicked, this,
          &MainWindow::reviewNextCommissioningWorkflowIssue);
  connect(workflowFilter_, &QLineEdit::textChanged, this,
          &MainWindow::filterCommissioningWorkflow);
  connect(workflowScopeFilter_,
          QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          [this] { filterCommissioningWorkflow(); });
  connect(slaveEvidenceMatrixFilter_, &QLineEdit::textChanged, this,
          &MainWindow::filterSlaveEvidenceMatrix);
  connect(slaveEvidenceMatrixScopeFilter_,
          QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          [this] { filterSlaveEvidenceMatrix(); });
  connect(slaveEvidenceMatrixTable_, &QTableWidget::itemSelectionChanged, this,
          &MainWindow::filterSlaveEvidenceMatrix);
  connect(slaveEvidenceMatrixReviewButton_, &QPushButton::clicked, this,
          &MainWindow::reviewFirstSlaveEvidenceMatrixIssue);
  connect(slaveEvidenceMatrixReviewNextButton_, &QPushButton::clicked, this,
          &MainWindow::reviewNextSlaveEvidenceMatrixIssue);
  connect(slaveEvidenceMatrixCopyButton_, &QPushButton::clicked, this, [this] {
    copySlaveEvidenceMatrixRowDigest(
        slaveEvidenceMatrixTable_ ? slaveEvidenceMatrixTable_->currentRow()
                                  : -1);
  });
  for (auto *button : slaveEvidenceMatrixTriageButtons_) {
    connect(button, &QPushButton::clicked, this, [this, button] {
      if (!slaveEvidenceMatrixScopeFilter_) {
        return;
      }
      const QString scope = button->property("scope").toString();
      const int scopeIndex = slaveEvidenceMatrixScopeFilter_->findData(scope);
      if (scopeIndex >= 0) {
        slaveEvidenceMatrixScopeFilter_->setCurrentIndex(scopeIndex);
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
  connect(workflowTable_, &QTableWidget::cellDoubleClicked, this,
          [this](int row) { runCommissioningWorkflowStep(row); });
  connect(slaveEvidenceMatrixTable_, &QTableWidget::cellDoubleClicked, this,
          [this](int row) { openSlaveEvidenceMatrixRow(row); });
  auto stateMachinePositionForRow = [this](int row) -> int {
    if (stateMachine_->stateMachineTable && row >= 0 &&
        row < stateMachine_->stateMachineTable->rowCount()) {
      const int position =
          stateMachinePositionFromTable(stateMachine_->stateMachineTable, row);
      if (position >= 0) {
        return position;
      }
    }
    return selectedPosition();
  };
  auto requestRecommendedState = [this, stateMachinePositionForRow](int row) {
    if (!client_.isConnected()) {
      updateDiagnostics(
          "Warning", "State",
          uiText("State request skipped: runtime is not connected",
                 "状态请求已跳过：运行时尚未连接"));
      return;
    }
    if (!stateMachine_->stateMachineTable) {
      return;
    }
    int targetRow = row;
    if (targetRow < 0) {
      targetRow = stateMachine_->stateMachineTable->currentRow();
    }
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
  connect(sdoIndex_, &QLineEdit::editingFinished, this, [this] {
    restoreManualSdoWriteMode();
    rememberCurrentSdoTarget(uiText("Manual fields", "手动字段"),
                             uiText("Index edited", "已编辑索引"));
  });
  connect(sdoSubIndex_, &QLineEdit::editingFinished, this, [this] {
    restoreManualSdoWriteMode();
    rememberCurrentSdoTarget(uiText("Manual fields", "手动字段"),
                             uiText("SubIndex edited", "已编辑子项"));
  });
  connect(sdoType_, &QComboBox::currentTextChanged, this, [this] {
    restoreManualSdoWriteMode();
    rememberCurrentSdoTarget(uiText("Manual fields", "手动字段"),
                             uiText("Type changed", "已修改类型"));
  });
  connect(sdoWriteValue_, &QLineEdit::textChanged, this,
          [this] { updateSdoInspector(uiText("Manual write", "手动写入")); });
  connect(sdoWriteValue_, &QLineEdit::editingFinished, this, [this] {
    rememberCurrentSdoTarget(uiText("Manual write", "手动写入"),
                             uiText("Write value edited", "已编辑写入值"));
  });
  connect(sdoValue_, &QLineEdit::textChanged, this,
          [this] { updateSdoInspector(uiText("Read-back", "读回值")); });
  connect(sdo_->pdoFilter, &QLineEdit::textChanged, this,
          &MainWindow::filterPdoTable);
  connect(sdo_->pdoTable, &QTableWidget::itemSelectionChanged, this,
          &MainWindow::updateActionAvailability);
  connect(sdo_->pdoTable, &QTableWidget::itemSelectionChanged, this,
          &MainWindow::updatePdoRowDetail);
  connect(sdo_->sdoFilter, &QLineEdit::textChanged, this,
          &MainWindow::filterSdoTable);
  connect(freeRunFilter_, &QLineEdit::textChanged, this,
          &MainWindow::filterFreeRunEntryTable);
  connect(freeRunChangedOnly_, &QCheckBox::toggled, this,
          &MainWindow::filterFreeRunEntryTable);
  connect(freeRunEntryTable_, &QTableWidget::itemSelectionChanged, this,
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
  connect(objectBookmarkTable_, &QTableWidget::itemSelectionChanged, this,
          &MainWindow::updateActionAvailability);
  connect(objectBookmarkTable_, &QTableWidget::itemSelectionChanged, this,
          &MainWindow::updateObjectBookmarkRowDetail);
  connect(objectBookmarkTable_, &QTableWidget::cellDoubleClicked, this,
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
  connect(startupSdoTable_, &QTableWidget::currentCellChanged, this, [this] {
    updateStartupSdoControls();
    updateStartupSdoRowDetail();
  });
  connect(startupSdoTable_, &QTableWidget::itemSelectionChanged, this,
          &MainWindow::updateStartupSdoControls);
  connect(startupSdoTable_, &QTableWidget::itemSelectionChanged, this,
          &MainWindow::updateStartupSdoRowDetail);
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
                      workflowTable_,
                      slaveEvidenceMatrixTable_,
                      stateMachine_->stateMachineTable,
                      identityTable_,
                      portTable_,
                      mailboxTable_,
                      sdo_->pdoTable,
                      sdo_->sdoTable,
                      sdoTargetTrailTable_,
                      objectBookmarkTable_,
                      sdoHistoryTable_,
                      freeRunTable_,
                      freeRunEntryTable_,
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
  connect(&client_, &EcatClient::masterText, this, [this](const QString &text) {
    lastMasterText_ = text;
    masterText_->setPlainText(text);
    updateMasterSummary(text);
    updateStatusBar();
  });
  connect(&client_, &EcatClient::slavesChanged, this,
          &MainWindow::updateSlaves);
  connect(&client_, &EcatClient::slaveTextResult, this,
          [this](const QString &title, int position, const QString &text) {
            if (position != selectedPosition()) {
              log(QString("Ignored stale %1 response for slave #%2")
                      .arg(title)
                      .arg(position));
              return;
            }
            if (title == "Info") {
              loadedSlaveInfoPosition_ = position;
              lastSlaveInfoText_ = text;
              infoText_->setPlainText(text);
              updateSlaveInfo(text);
            } else if (title == "PDO") {
              loadedPdoPosition_ = position;
              lastPdoText_ = text;
              pdoText_->setPlainText(text);
              updatePdoTable(text);
            } else if (title == "SDO") {
              loadedSdoPosition_ = position;
              lastSdoText_ = text;
              sdoText_->setPlainText(text);
              updateSdoTable(text);
            } else if (title == "ESI XML") {
              loadedXmlPosition_ = position;
              lastXmlText_ = text;
              xmlText_->setPlainText(text);
            }
            updateSelectedSlaveEvidenceSummary();
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
        if (currentTarget && sdoValue_) {
          sdoValue_->setText(value);
          sdoValue_->setPlaceholderText(uiText(
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
        if (hasStartupCheck) {
          auto normalize = [](QString text) {
            return text.trimmed().remove(' ').toLower();
          };
          for (const int startupCheckRow : startupCheckRows) {
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
                                             : (currentTarget && sdoType_
                                                    ? sdoType_->currentText()
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
        if (!watch_->watchTable->item(row, 6) ||
            watch_->watchTable->item(row, 6)->text().trimmed().isEmpty()) {
          const bool watchRefreshSource =
              source.contains("Watch", Qt::CaseInsensitive) ||
              source.contains("监视", Qt::CaseInsensitive);
          if (!watchRefreshSource && !readType.isEmpty()) {
            setCell(6, readType);
          } else if (!watchRefreshSource && currentTarget && sdoType_) {
            setCell(6, sdoType_->currentText());
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
                               : (sdoIndex_ ? sdoIndex_->text() : QString());
          const QString subIndex =
              match.hasMatch()
                  ? match.captured(3)
                  : (sdoSubIndex_ ? sdoSubIndex_->text() : QString());
          QStringList pending = pendingSdoWrites_.take(
              sdoEvidenceKey(position.toInt(), index, subIndex));
          if (pending.isEmpty()) {
            pending =
                pendingSdoWrites_.take(QString("%1|%2").arg(index, subIndex));
          }
          const QString type =
              pending.value(3, sdoType_ ? sdoType_->currentText() : QString());
          const QString expectedValue = pending.value(
              4, sdoWriteValue_ ? sdoWriteValue_->text() : QString());
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
              if (row < 0 || row >= startupSdoTable_->rowCount()) {
                continue;
              }
              auto *status = startupSdoTable_->item(row, 5);
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
  connect(&daemon_, &QProcess::readyReadStandardError, this, [this] {
    const QString text =
        QString::fromLocal8Bit(daemon_.readAllStandardError()).trimmed();
    if (!text.isEmpty()) {
      log("ecatd: " + text);
    }
  });
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
  if (sdo_->pdoTable) {
    pdoRows = sdo_->pdoTable->rowCount();
    for (int row = 0; row < sdo_->pdoTable->rowCount(); ++row) {
      const QString pdo = tableText(sdo_->pdoTable, row, 1);
      const int bits = tableText(sdo_->pdoTable, row, 4).toInt();
      const QString name = tableText(sdo_->pdoTable, row, 5);
      const QString address = QString("%1:%2").arg(
          tableText(sdo_->pdoTable, row, 2), tableText(sdo_->pdoTable, row, 3));
      if (pdo.contains("RxPDO", Qt::CaseInsensitive)) {
        ++rxPdoRows;
        rxBits += bits;
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
  if (pdoRows <= 0) {
    details << uiText("PDO map warning: no PDO rows are loaded for review",
                      "PDO 映射警告：当前没有可复核的 PDO 行");
  } else if (rxPdoRows > 0) {
    details << uiText("Output risk: Free Run may exchange %1 RxPDO/output "
                      "entry row(s)",
                      "输出风险：Free Run 可能交换 %1 条 RxPDO/输出条目")
                   .arg(rxPdoRows);
    if (!rxPreview.isEmpty()) {
      details << uiText("Output preview: %1", "输出预览：%1")
                     .arg(rxPreview.join(" | "));
    }
  }

  if (freeRunEntryTable_ && freeRunEntryTable_->rowCount() > 0) {
    int outputEntries = 0;
    int inputEntries = 0;
    QStringList meaningPreview;
    for (int row = 0; row < freeRunEntryTable_->rowCount(); ++row) {
      const QString direction = tableText(freeRunEntryTable_, row, 2).toLower();
      if (direction.contains("rx") || direction.contains("out")) {
        ++outputEntries;
      } else if (direction.contains("tx") || direction.contains("in")) {
        ++inputEntries;
      }
      const QString meaning = tableText(freeRunEntryTable_, row, 12);
      if (!meaning.isEmpty() && meaningPreview.size() < 3) {
        meaningPreview << meaning;
      }
    }
    details << uiText("Previous Free Run cache: %1 entries, output-like %2, "
                      "input-like %3",
                      "上次 Free Run 缓存：%1 项，输出类 %2，输入类 %3")
                   .arg(freeRunEntryTable_->rowCount())
                   .arg(outputEntries)
                   .arg(inputEntries);
    if (!meaningPreview.isEmpty()) {
      details << uiText("Decoded evidence: %1", "解析证据：%1")
                     .arg(meaningPreview.join(" | "));
    }
  }

  QString statusword;
  QString errorCode;
  if (watch_->watchTable && selected >= 0) {
    for (int row = 0; row < watch_->watchTable->rowCount(); ++row) {
      const int rowPosition = tableText(watch_->watchTable, row, 1).toInt();
      if (rowPosition != selected) {
        continue;
      }
      const QString index = normalizeHexText(tableText(watch_->watchTable, row, 2), 4);
      const QString value = tableText(watch_->watchTable, row, 4);
      const QString decoded = tableText(watch_->watchTable, row, 5);
      if (index == "0x6041" && !decoded.isEmpty()) {
        statusword = decoded;
      } else if (index == "0x603f" && !decoded.isEmpty() && !value.isEmpty() &&
                 value != "0" && value.toLower() != "0x0000") {
        errorCode = decoded;
      }
    }
  }
  if (!statusword.isEmpty() || !errorCode.isEmpty()) {
    QStringList driveFacts;
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
  if (!freeRunEntryDetailLabel_) {
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

  auto applyState = [this](const FreeRunEntryDetailUiState &state) {
    freeRunEntryDetailLabel_->setText(state.text);
    freeRunEntryDetailLabel_->setProperty("severity", state.severityKey);
    freeRunEntryDetailLabel_->setToolTip(state.tooltip);
    repolish(freeRunEntryDetailLabel_);
  };

  if (!freeRunEntryTable_) {
    applyState(freeRunEntryDetailUnavailableState(texts));
    return;
  }

  const int row = freeRunEntryTable_->currentRow();
  if (row < 0 || row >= freeRunEntryTable_->rowCount() ||
      freeRunEntryTable_->isRowHidden(row)) {
    applyState(freeRunEntryDetailNoSelectionState(texts));
    return;
  }

  applyState(buildFreeRunEntryDetailUiState(
      freeRunEntryTableRowFromTable(freeRunEntryTable_, row), texts));
}

// Append a timestamped message to the diagnostics log
void MainWindow::log(const QString &message) {
  logText_->appendPlainText(QString("[%1] %2").arg(
      QDateTime::currentDateTime().toString("HH:mm:ss"), message));
}

// Return the currently selected slave position, or -1 if none
int MainWindow::selectedPosition() const {
  auto *item = topologyTree_->currentItem();
  return item ? item->data(0, Qt::UserRole).toInt() : -1;
}

// Return the currently selected Object Dictionary row indices
QVector<int> MainWindow::selectedDictionaryRows() const {
  if (!sdo_->sdoTable || selectedPosition() < 0 ||
      loadedSdoPosition_ != selectedPosition()) {
    return {};
  }
  return selectedTableRows(sdo_->sdoTable);
}

// Return the currently selected SDO history row indices
QVector<int> MainWindow::selectedSdoHistoryRows() const {
  return selectedTableRows(sdoHistoryTable_);
}

// Return the currently selected Startup SDO row indices
QVector<int> MainWindow::selectedStartupSdoRows() const {
  return selectedTableRows(startupSdoTable_, true);
}

// Check if a watch table row has a non-empty value
bool MainWindow::watchRowHasValue(int row) const {
  return watch_->watchTable && row >= 0 && row < watch_->watchTable->rowCount() &&
         !watch_->watchTable->isRowHidden(row) && watch_->watchTable->item(row, 4) &&
         !watch_->watchTable->item(row, 4)->text().trimmed().isEmpty();
}

// Check if any of the selected watch rows contain a value
bool MainWindow::selectedWatchRowsHaveValue() const {
  if (!watch_->watchTable) {
    return false;
  }
  for (const int row : selectedTableRows(watch_->watchTable, true)) {
    if (watchRowHasValue(row)) {
      return true;
    }
  }
  return false;
}

// Highlight a slave in the topology tree and update dependent panels
void MainWindow::setSelectedSlave(int position) {
  if (position < 0) {
    selectedLabel_->setText(activeMasterName());
    beginSelectedSlaveOnlineLoad(-1);
    filterWatchTable();
    updateSelectedSlavePanel();
    updateActionAvailability();
    updateCommissioningWorkflow();
    updateIoVariableTable();
    updateStateMachineView();
    return;
  }
  for (const auto &slave : slaves_) {
    if (slave.position == position) {
      selectedLabel_->setText(
          QString("#%1  %2").arg(slave.position).arg(slave.name));
      beginSelectedSlaveOnlineLoad(position);
      client_.slaveInfo(position);
      client_.pdos(position);
      client_.sdos(position);
      client_.xml(position);
      filterWatchTable();
      updateSelectedSlavePanel();
      updateActionAvailability();
      updateCommissioningWorkflow();
      updateIoVariableTable();
      updateStateMachineView();
      return;
    }
  }
}

// Compare previous and current slave lists; emit diagnostics for topology changes
void MainWindow::reportTopologyChanges(const QVector<SlaveInfo> &previous,
                                       const QVector<SlaveInfo> &current) {
  const QVector<TopologyChange> changes =
      detectTopologyChanges(previous, current);
  if (changes.isEmpty()) {
    return;
  }

  for (const auto &change : changes) {
    switch (change.kind) {
    case TopologyChangeKind::Added:
      updateDiagnostics("Warning", "Topology",
                        QString("Slave #%1 added: %2 (%3)")
                            .arg(change.position)
                            .arg(change.current.name, change.current.state));
      break;
    case TopologyChangeKind::NameChanged:
      updateDiagnostics("Warning", "Topology",
                        QString("Slave #%1 identity changed: %2 -> %3")
                            .arg(change.position)
                            .arg(change.previous.name, change.current.name));
      break;
    case TopologyChangeKind::StateChanged: {
      const QString level = change.current.state == "OP" ? "Info" : "Warning";
      updateDiagnostics(level, "Topology",
                        QString("Slave #%1 state changed: %2 -> %3 (%4)")
                            .arg(change.position)
                            .arg(change.previous.state, change.current.state,
                                 change.current.name));
      break;
    }
    case TopologyChangeKind::FlagsChanged:
      updateDiagnostics("Info", "Topology",
                        QString("Slave #%1 flags changed: %2 -> %3")
                            .arg(change.position)
                            .arg(change.previous.flags, change.current.flags));
      break;
    case TopologyChangeKind::Removed:
      updateDiagnostics("Error", "Topology",
                        QString("Slave #%1 removed: %2 (%3)")
                            .arg(change.position)
                            .arg(change.previous.name, change.previous.state));
      break;
    }
  }
  consistencyFresh_ = false;
}

// Process a fresh slave scan result — update topology tree and detect changes
void MainWindow::updateSlaves(const QVector<SlaveInfo> &slaves) {
  const int previous = selectedPosition();
  const QVector<SlaveInfo> previousSlaves = slaves_;
  reportTopologyChanges(previousSlaves, slaves);

  // Skip full tree rebuild if slave list is identical (avoids UI flicker)
  if (slaves == slaves_) {
    return;
  }
  slaves_ = slaves;
  topologyTree_->setUpdatesEnabled(false);
  topologyTree_->clear();

  auto *master = new QTreeWidgetItem(
      topologyTree_,
      {activeMasterName(),
       QString("%1 %2").arg(slaves.size()).arg(uiText("slaves", "从站"))});
  master->setData(0, Qt::UserRole, -1);
  master->setExpanded(true);

  for (const auto &slave : slaves) {
    auto *item = new QTreeWidgetItem(
        master,
        {QString("#%1  %2").arg(slave.position).arg(slave.name), slave.state});
    item->setData(0, Qt::UserRole, slave.position);
    if (slave.state == "OP") {
      item->setForeground(1, QColor("#8ff0c8"));
    } else if (slave.state == "PREOP") {
      item->setForeground(1, QColor("#ffd27b"));
    }
  }

  topologyTree_->expandAll();
  setMetricCard(slaveCountLabel_, uiText("Slaves", "从站"),
                QString::number(slaves.size()));
  log(QString("Scan complete: %1 slaves").arg(slaves.size()));
  updateDiagnostics("Info", "Scan",
                    QString("%1 slave(s) detected").arg(slaves.size()));
  updateSelectedSlavePanel();
  updateTopologyBaselineSummary();
  updateActionAvailability();
  updateCommissioningWorkflow();
  updateIoVariableTable();
  updateStateMachineView();
  updateStatusBar();

  if (!slaves.isEmpty()) {
    QTreeWidgetItem *target = master->child(0);
    for (int i = 0; i < master->childCount(); ++i) {
      if (master->child(i)->data(0, Qt::UserRole).toInt() == previous) {
        target = master->child(i);
        break;
      }
    }
    topologyTree_->setCurrentItem(target);
  }
  topologyTree_->setUpdatesEnabled(true);
}

// Parse the daemon's master status text and update the overview metric cards
void MainWindow::updateMasterSummary(const QString &text) {
  const QString phase = capture(text, R"(^\s*Phase:\s*(.+)$)");
  const QString slaves = capture(text, R"(^\s*Slaves:\s*(.+)$)");
  const QString link = capture(text, R"(^\s*Link:\s*(.+)$)");
  const QString loss = capture(text, R"(^\s*Lost frames:\s*(.+)$)");
  setMetricCard(masterStateLabel_, uiText("Master", "主站"),
                phase.isEmpty() ? uiText("Unknown", "未知") : phase);
  if (!slaves.isEmpty()) {
    setMetricCard(slaveCountLabel_, uiText("Slaves", "从站"), slaves);
  }
  setMetricCard(linkStateLabel_, uiText("Link", "链路"),
                link.isEmpty() ? uiText("Unknown", "未知") : link);
  setMetricCard(lossLabel_, uiText("Frame Loss", "丢帧"),
                loss.isEmpty() ? "0" : loss);

  QList<QStringList> rows;
  const auto lines = text.split('\n');
  for (const auto &line : lines) {
    const auto idx = line.indexOf(':');
    if (idx > 0) {
      rows.append({line.left(idx).trimmed(), line.mid(idx + 1).trimmed()});
    }
  }
  setTableRows(metricTable_, {"Metric", "Value"}, rows);
  updateCommissioningWorkflow();
}

// Parse identity text from the daemon and populate the identity/port/mailbox tables
void MainWindow::updateSlaveInfo(const QString &text) {
  QList<QStringList> identity;
  for (const QString &key :
       {"Vendor Id", "Product code", "Revision number", "Serial number",
        "Group", "Order number", "Device name"}) {
    const QString value = capture(
        text,
        QString(R"(^\s*%1:\s*(.+)$)").arg(QRegularExpression::escape(key)));
    if (!value.isEmpty()) {
      identity.append({key, value});
    }
  }
  setTableRows(identityTable_, {"Field", "Value"}, identity);

  QList<QStringList> ports;
  bool inPortTable = false;
  for (const auto &line : text.split('\n')) {
    if (line.startsWith("Port  Type")) {
      inPortTable = true;
      continue;
    }
    if (inPortTable) {
      const auto parts = line.simplified().split(' ');
      if (parts.size() >= 5 && parts.first().at(0).isDigit()) {
        ports.append({parts.value(0), parts.value(1), parts.value(2),
                      parts.value(3), parts.value(4)});
      } else if (!line.trimmed().isEmpty()) {
        inPortTable = false;
      }
    }
  }
  setTableRows(portTable_, {"Port", "Type", "Link", "Loop", "Signal"}, ports);

  QList<QStringList> mailboxes;
  for (const QString &prefix :
       {"Bootstrap", "Standard", "Supported protocols"}) {
    const QString value = capture(
        text,
        QString(R"(^\s*%1\s*(.*)$)").arg(QRegularExpression::escape(prefix)));
    if (!value.isEmpty()) {
      QString cleaned = value;
      cleaned.remove(':');
      mailboxes.append({prefix, cleaned.trimmed()});
    }
  }
  setTableRows(mailboxTable_, {"Mailbox", "Value"}, mailboxes);
  updateCommissioningWorkflow();
  updateStateMachineView();
}

// Parse PDO map text from the daemon and populate the PDO table with direction detection
void MainWindow::updatePdoTable(const QString &text) {
  QList<QStringList> rows;
  QString sm;
  QString pdo;
  const QRegularExpression smRe(R"(^SM(\d+):.*DefaultSize\s+(\d+).*)");
  const QRegularExpression pdoRe(
      R"(^\s+(RxPDO|TxPDO)\s+(0x[0-9a-fA-F]+)\s+\"(.+)\")");
  const QRegularExpression entryRe(
      R"(^\s+PDO entry\s+(0x[0-9a-fA-F]+):([0-9a-fA-F]+),\s+(\d+)\s+bit,\s+\"(.+)\")");
  for (const auto &line : text.split('\n')) {
    auto m = smRe.match(line);
    if (m.hasMatch()) {
      sm = QString("SM%1 (%2 bytes)").arg(m.captured(1), m.captured(2));
      continue;
    }
    m = pdoRe.match(line);
    if (m.hasMatch()) {
      pdo =
          QString("%1 %2 %3").arg(m.captured(1), m.captured(2), m.captured(3));
      continue;
    }
    m = entryRe.match(line);
    if (m.hasMatch()) {
      rows.append({sm, pdo, m.captured(1), m.captured(2), m.captured(3),
                   m.captured(4)});
    }
  }
  setTableRows(sdo_->pdoTable, {"SM", "PDO", "Index", "Sub", "Bits", "Name"}, rows);
  filterPdoTable();
  updateIoVariableTable();
  updateStateMachineView();
}

// Apply text filter to the PDO map table, hiding non-matching rows
void MainWindow::filterPdoTable() {
  if (!sdo_->pdoTable) {
    return;
  }
  const QString needle = sdo_->pdoFilter ? sdo_->pdoFilter->text().trimmed() : QString();
  int visible = 0;
  for (int row = 0; row < sdo_->pdoTable->rowCount(); ++row) {
    bool match = needle.isEmpty();
    for (int column = 0; column < sdo_->pdoTable->columnCount() && !match;
         ++column) {
      const auto *item = sdo_->pdoTable->item(row, column);
      match = item && item->text().contains(needle, Qt::CaseInsensitive);
    }
    sdo_->pdoTable->setRowHidden(row, !match);
    if (match) {
      ++visible;
    }
  }
  if (sdo_->pdoSummaryLabel) {
    const int total = sdo_->pdoTable->rowCount();
    sdo_->pdoSummaryLabel->setText(
        total > 0 ? uiText("%1/%2 PDO entries", "%1/%2 个 PDO 条目")
                        .arg(visible)
                        .arg(total)
                  : uiText("No PDO entries", "暂无 PDO 条目"));
  }
  updatePdoRowDetail();
  updateActionAvailability();
  updateCommissioningWorkflow();
}

// Update the detail strip below the PDO map table for the current row
void MainWindow::updatePdoRowDetail() {
  if (!sdo_->pdoDetailLabel) {
    return;
  }
  const PdoMapDetailTexts texts = {
      .unavailableText = uiText("PDO Map evidence is not available.",
                                "当前没有可用的 PDO 映射证据。"),
      .unavailableTip =
          uiText("This preview is local only and does not access the bus.",
                 "此预览仅在本地工作，不访问总线。"),
      .noSelectionText = uiText(
          "Select a visible PDO entry to review Sync Manager, PDO, object "
          "address, bit width, name, inferred type, and operation boundary.",
          "选择一条可见 PDO 条目，以复核 Sync "
          "Manager、PDO、对象地址、位宽、名称、"
          "推断类型和操作边界。"),
      .noSelectionTip = uiText(
          "Selecting rows, filtering, and reading this detail strip are local "
          "review actions after PDO data is loaded. Loading or refreshing PDO "
          "Map is the explicit online PDO evidence path; double-clicking a row "
          "fills and reads the SDO through the normal read path.",
          "PDO "
          "数据加载后，选择行、筛选和查看此详情条都是本地审阅动作；加载或刷新 "
          "PDO Map 才是显式在线 PDO "
          "证据路径；双击行会通过普通读取路径填充并读取 "
          "SDO。"),
      .directionRxOutput = uiText("Rx output", "Rx 输出"),
      .directionTxInput = uiText("Tx input", "Tx 输入"),
      .directionUnknown = uiText("PDO direction unknown", "PDO 方向未知"),
      .roleRxOutput =
          uiText("Output/process command candidate", "输出/过程命令候选"),
      .roleTxInput =
          uiText("Input/process feedback candidate", "输入/过程反馈候选"),
      .roleGeneric = uiText("Generic process-data entry", "通用过程数据条目"),
      .typeFallback = uiText("type?", "类型?"),
      .unnamed = uiText("Unnamed PDO entry", "未命名 PDO 条目"),
      .cia402Candidate = uiText("CiA 402 candidate", "CiA 402 候选"),
      .genericEntry = uiText("Generic PDO entry", "通用 PDO 条目"),
      .summaryPattern = uiText("%1 | %2:%3 | %4 bit %5 | %6 | %7",
                               "%1 | %2:%3 | %4 bit %5 | %6 | %7"),
      .selectedTitle = uiText("Selected PDO Map row", "选中的 PDO 映射行"),
      .slaveLabel = uiText("Slave", "从站"),
      .syncManagerLabel = uiText("Sync Manager", "Sync Manager"),
      .pdoLabel = uiText("PDO", "PDO"),
      .objectLabel = uiText("Object", "对象"),
      .bitsLabel = uiText("Bits", "位宽"),
      .inferredTypeLabel = uiText("Inferred SDO Type", "推断 SDO 类型"),
      .nameLabel = uiText("Name", "名称"),
      .directionLabel = uiText("Direction", "方向"),
      .roleLabel = uiText("Process Role", "过程角色"),
      .driveEvidenceLabel = uiText("Drive Evidence", "驱动证据"),
      .localBoundary = uiText(
          "Local preview boundary: after PDO Map data is loaded, selecting "
          "this row, filtering PDO entries, and reading this detail strip do "
          "not read SDOs, write SDOs, change state, toggle Free Run, "
          "rescan/connect, or run Host Health.",
          "本地预览边界：PDO Map 数据加载后，选择此行、筛选 PDO 条目和查看此"
          "详情条都不读取 SDO、不写 SDO、不切换状态、不改变 Free Run、不重扫/"
          "连接，也不运行 Host Health。"),
      .executionBoundary = uiText(
          "Execution boundary: Load/refresh PDO Map is explicit online PDO "
          "evidence loading; Fill SDO only prepares the local target; "
          "double-click or Fill and Read uses the normal explicit SDO read "
          "path; Add Selected to Watch creates Watch rows without immediate "
          "reads.",
          "执行边界：加载/刷新 PDO Map 是显式在线 PDO 证据加载；填充 SDO "
          "只准备本地目标；双击或填充并读取会走普通显式 SDO "
          "读取路径；选中项加入 Watch 只创建 Watch 行，不立即读取。"),
  };

  auto applyState = [this](const PdoMapDetailUiState &state) {
    sdo_->pdoDetailLabel->setText(state.text);
    sdo_->pdoDetailLabel->setProperty("severity", state.severityKey);
    sdo_->pdoDetailLabel->setToolTip(state.tooltip);
    repolish(sdo_->pdoDetailLabel);
  };

  if (!sdo_->pdoTable) {
    applyState(pdoMapDetailUnavailableState(texts));
    return;
  }

  const int row = sdo_->pdoTable->currentRow();
  if (row < 0 || row >= sdo_->pdoTable->rowCount() || sdo_->pdoTable->isRowHidden(row)) {
    applyState(pdoMapDetailNoSelectionState(texts));
    return;
  }

  applyState(buildPdoMapDetailUiState(pdoMapTableRowFromTable(sdo_->pdoTable, row),
                                      selectedPosition(), texts));
}

// Parse Object Dictionary text from the daemon, merge with evidence, and populate the SDO table
void MainWindow::updateSdoTable(const QString &text) {
  QList<QStringList> rows;
  QString object;
  auto normalizeHex = [](QString text, int minimumDigits) {
    text = text.trimmed();
    if (text.isEmpty()) {
      return text;
    }
    QString digits = text;
    if (digits.startsWith("0x", Qt::CaseInsensitive)) {
      digits = digits.mid(2);
    }
    bool ok = false;
    const quint64 parsed = digits.toULongLong(&ok, 16);
    if (!ok) {
      return text.toLower();
    }
    return QString("0x%1")
        .arg(parsed, minimumDigits, 16, QLatin1Char('0'))
        .toLower();
  };
  const QRegularExpression objRe(R"(^SDO\s+(0x[0-9a-fA-F]+),\s+\"(.+)\")");
  const QRegularExpression entryRe(
      R"(^\s+(0x[0-9a-fA-F]+):([0-9a-fA-F]+),\s+(\S+),\s+([^,]+),\s+(\d+)\s+bit,\s+\"(.+)\")");
  for (const auto &line : text.split('\n')) {
    auto m = objRe.match(line);
    if (m.hasMatch()) {
      object = QString("%1 %2").arg(m.captured(1), m.captured(2));
      continue;
    }
    m = entryRe.match(line);
    if (m.hasMatch()) {
      const QString index = m.captured(1);
      const QString subIndex = m.captured(2);
      const QString key =
          QString("%1|%2|%3")
              .arg(selectedPosition())
              .arg(normalizeHex(index, 4), normalizeHex(subIndex, 2));
      const QStringList evidence = sdoEvidence_.value(key);
      rows.append(
          {object, index, subIndex, m.captured(3), m.captured(4).trimmed(),
           m.captured(5), m.captured(6), evidence.value(0),
           evidence.isEmpty()
               ? QString()
               : QString("%1  %2").arg(evidence.value(1), evidence.value(3))});
    }
  }
  setTableRows(sdo_->sdoTable,
               {"Object", "Index", "Sub", "Access", "Type", "Bits", "Name",
                "Last Value", "Last Status"},
               rows);
  for (int row = 0; row < sdo_->sdoTable->rowCount(); ++row) {
    const QString lastStatus =
        sdo_->sdoTable->item(row, 8) ? sdo_->sdoTable->item(row, 8)->text() : QString();
    if (lastStatus.isEmpty()) {
      continue;
    }
    const QColor color =
        lastStatus.contains(uiText("Complete", "完成")) ||
                lastStatus.contains(uiText("OK", "成功")) ||
                lastStatus.contains(uiText("Write OK", "写入完成"))
            ? QColor("#22c55e")
            : (lastStatus.contains(uiText("Failed", "失败"))
                   ? QColor("#ef4444")
                   : QColor("#f59e0b"));
    if (auto *statusItem = sdo_->sdoTable->item(row, 8)) {
      statusItem->setForeground(color);
    }
    if (auto *valueItem = sdo_->sdoTable->item(row, 7)) {
      valueItem->setBackground(settings_.theme == "Light" ? QColor("#eef2ff")
                                                          : QColor("#172036"));
    }
  }
  filterSdoTable(sdo_->sdoFilter ? sdo_->sdoFilter->text() : QString());
  updateCommissioningWorkflow();
  updateStateMachineView();
}

// Process real-time Free Run telemetry — update signals table and entry table with PDO map cross-reference
void MainWindow::updateFreeRunTelemetry(const QJsonObject &telemetry) {
  const QList<QStringList> rows = {
      {"Running", telemetry.value("running").toBool() ? "Yes" : "No"},
      {"Status", telemetry.value("status").toString()},
      {"Cycle Count", telemetry.value("cycles").toString()},
      {"Configured Slaves",
       QString::number(telemetry.value("configuredSlaves").toInt())},
      {"PDO Entries", QString::number(telemetry.value("pdoEntries").toInt())},
      {"Slaves Responding",
       QString::number(telemetry.value("slavesResponding").toInt())},
      {"AL State", telemetry.value("alStateText").toString()},
      {"AL State Mask",
       QString("0x%1").arg(telemetry.value("alStates").toInt(), 0, 16)},
      {"Link Up", telemetry.value("linkUp").toBool() ? "Yes" : "No"},
      {"Working Counter",
       QString::number(telemetry.value("workingCounter").toInt())},
      {"WKC State", telemetry.value("wcStateText").toString()},
      {"WKC State Code", QString::number(telemetry.value("wcState").toInt())},
      {"Redundancy Active",
       QString::number(telemetry.value("redundancyActive").toInt())},
  };
  setTableRows(freeRunTable_, {"Signal", "Value"}, rows);

  QList<QStringList> entries;
  const auto array = telemetry.value("entries").toArray();
  auto directionClass = [](QString text) {
    text = text.trimmed().toLower();
    if (text.contains("rx") || text.contains("output") || text == "out") {
      return QStringLiteral("rx");
    }
    if (text.contains("tx") || text.contains("input") || text == "in") {
      return QStringLiteral("tx");
    }
    return QString();
  };
  auto pdoMapEvidence = [this, &directionClass](
                            int slave, const QString &direction,
                            const QString &index, const QString &subIndex,
                            int bits, QString *mappedName,
                            QString *mappedDetail) {
    if (mappedName) {
      mappedName->clear();
    }
    if (mappedDetail) {
      mappedDetail->clear();
    }
    if (!sdo_->pdoTable || sdo_->pdoTable->rowCount() <= 0) {
      return uiText("No PDO map", "无 PDO 映射");
    }
    if (slave != selectedPosition()) {
      return uiText("Map not loaded for slave", "未加载该从站映射");
    }

    const QString normalizedIndex = normalizeHexText(index, 4);
    const QString normalizedSubIndex = normalizeHexText(subIndex, 2);
    const QString runtimeDirection = directionClass(direction);
    for (int row = 0; row < sdo_->pdoTable->rowCount(); ++row) {
      const QString mapIndex =
          normalizeHexText(tableText(sdo_->pdoTable, row, 2), 4);
      const QString mapSubIndex =
          normalizeHexText(tableText(sdo_->pdoTable, row, 3), 2);
      if (mapIndex != normalizedIndex || mapSubIndex != normalizedSubIndex) {
        continue;
      }

      const QString mapPdo = tableText(sdo_->pdoTable, row, 1);
      const QString mapBitsText = tableText(sdo_->pdoTable, row, 4);
      const int mapBits = mapBitsText.toInt();
      const QString mapDirection = directionClass(mapPdo);
      if (mappedName) {
        *mappedName = tableText(sdo_->pdoTable, row, 5);
      }
      if (mappedDetail) {
        *mappedDetail = QString("%1 %2 bit").arg(mapPdo, mapBitsText);
      }
      QStringList issues;
      if (!runtimeDirection.isEmpty() && !mapDirection.isEmpty() &&
          runtimeDirection != mapDirection) {
        issues << uiText("direction mismatch", "方向不一致");
      }
      if (bits > 0 && mapBits > 0 && bits != mapBits) {
        issues << uiText("bit mismatch %1/%2", "位宽不一致 %1/%2")
                      .arg(bits)
                      .arg(mapBits);
      }
      return issues.isEmpty() ? uiText("Mapped", "已映射")
                              : uiText("Mapped warning: %1", "映射警告：%1")
                                    .arg(issues.join("; "));
    }
    return uiText("Missing in PDO map", "PDO 映射缺失");
  };
  for (const auto &value : array) {
    const auto object = value.toObject();
    const QString slave = QString::number(object.value("slave").toInt());
    const QString sync = QString::number(object.value("sync").toInt());
    const QString direction = object.value("direction").toString();
    const QString pdo = object.value("pdo").toString();
    const QString index = object.value("index").toString();
    const QString subIndex = object.value("subindex").toString();
    const int bits = object.value("bits").toInt();
    QString mappedName;
    QString mappedDetail;
    const QString mapEvidence =
        pdoMapEvidence(object.value("slave").toInt(), direction, index,
                       subIndex, bits, &mappedName, &mappedDetail);
    const QString nameKey =
        QString("%1|%2|%3|%4|%5|%6")
            .arg(slave, sync, direction, pdo, index, subIndex);
    const QString objectNameKey = ioVariableTableObjectKey(
        object.value("slave").toInt(), index, subIndex);
    const QString aliasName = ioVariableMetadata_.value(objectNameKey).value(0);
    QString displayName = aliasName.trimmed();
    QString displayNameSource =
        displayName.isEmpty() ? QString() : uiText("I/O alias", "I/O 别名");
    auto takeDisplayName = [&](const QString &candidate,
                               const QString &source) {
      if (!displayName.isEmpty() || candidate.trimmed().isEmpty()) {
        return;
      }
      displayName = candidate.trimmed();
      displayNameSource = source;
    };
    takeDisplayName(object.value("displayName").toString(),
                    uiText("runtime display name", "运行时显示名"));
    takeDisplayName(object.value("name").toString(),
                    uiText("runtime name", "运行时名称"));
    takeDisplayName(mappedName, uiText("PDO map", "PDO 映射"));
    takeDisplayName(freeRunObjectNames_.value(objectNameKey),
                    uiText("object cache", "对象缓存"));
    takeDisplayName(freeRunEntryNames_.value(nameKey),
                    uiText("entry cache", "条目缓存"));
    if (displayName.isEmpty()) {
      displayName = QString("%1 %2:%3").arg(direction, index, subIndex);
      displayNameSource = uiText("address fallback", "地址回退");
    } else {
      freeRunEntryNames_.insert(nameKey, displayName);
      if (!objectNameKey.isEmpty()) {
        freeRunObjectNames_.insert(objectNameKey, displayName);
      }
    }
    const QString rawValue = object.value("rawValue").toString();
    const QString decodedValue = object.value("decodedValue").toString();
    QString meaning = object.value("meaning").toString();
    if (meaning.trimmed().isEmpty()) {
      meaning = decodeWatchValue(index, subIndex, QString(),
                                 rawValue.isEmpty() ? decodedValue : rawValue,
                                 "CiA 402");
    }
    entries.append({
        slave,
        sync,
        direction,
        pdo,
        index,
        subIndex,
        QString::number(bits),
        QString::number(object.value("offset").toInt()),
        QString::number(object.value("bit").toInt()),
        displayName,
        rawValue,
        decodedValue,
        meaning,
        mapEvidence,
        mappedDetail.isEmpty()
            ? uiText("Name source: %1", "名称来源：%1").arg(displayNameSource)
            : QString("%1 | %2").arg(mappedDetail,
                                     uiText("Name source: %1", "名称来源：%1")
                                         .arg(displayNameSource)),
    });
  }
  updateFreeRunEntryTable(entries);
}

// Rebuild the Free Run entry table from the processed rows
void MainWindow::updateFreeRunEntryTable(const QList<QStringList> &rows) {
  if (!freeRunEntryTable_) {
    return;
  }

  const QStringList headers = {"Slave",   "SM",         "Dir",       "PDO",
                               "Index",   "Sub",        "Bits",      "Offset",
                               "Bit",     "Name",       "Raw",       "Decoded",
                               "Meaning", "Map Status", "Map Detail"};
  const int selectedRow = freeRunEntryTable_->currentRow();
  const int verticalScroll =
      freeRunEntryTable_->verticalScrollBar()
          ? freeRunEntryTable_->verticalScrollBar()->value()
          : 0;
  const int horizontalScroll =
      freeRunEntryTable_->horizontalScrollBar()
          ? freeRunEntryTable_->horizontalScrollBar()->value()
          : 0;

  if (freeRunEntryTable_->columnCount() != headers.size()) {
    freeRunEntryTable_->setColumnCount(headers.size());
    freeRunEntryTable_->setHorizontalHeaderLabels(headers);
  }
  if (freeRunEntryTable_->rowCount() != rows.size()) {
    freeRunEntryTable_->setRowCount(rows.size());
  }

  const QColor changedBackground =
      settings_.theme == "Light" ? QColor("#fff7cc") : QColor("#3a2f16");
  const QColor changedForeground =
      settings_.theme == "Light" ? QColor("#854d0e") : QColor("#fde68a");
  for (int row = 0; row < rows.size(); ++row) {
    const QStringList values = rows[row];
    const QString key =
        QString("%1|%2|%3|%4|%5|%6")
            .arg(values.value(0), values.value(1), values.value(2),
                 values.value(3), values.value(4), values.value(5));
    const QString valueSignature = values.value(10) + "|" + values.value(11);
    const bool changed = freeRunEntryValues_.contains(key) &&
                         freeRunEntryValues_.value(key) != valueSignature;
    freeRunEntryValues_.insert(key, valueSignature);

    for (int column = 0; column < headers.size(); ++column) {
      auto *item = freeRunEntryTable_->item(row, column);
      if (!item) {
        item = new QTableWidgetItem;
        freeRunEntryTable_->setItem(row, column, item);
      }
      item->setText(values.value(column));
      item->setData(Qt::UserRole, changed);
      if (changed && (column == 10 || column == 11)) {
        item->setBackground(changedBackground);
        item->setForeground(changedForeground);
      } else if (column == 13) {
        const QString status = values.value(column);
        const bool warning =
            status.contains(uiText("warning", "警告"), Qt::CaseInsensitive) ||
            status.contains(uiText("Missing", "缺失"), Qt::CaseInsensitive) ||
            status.contains("缺失") ||
            status.contains(uiText("No PDO map", "无 PDO 映射"),
                            Qt::CaseInsensitive);
        const bool mapped = status == uiText("Mapped", "已映射");
        item->setForeground(
            mapped ? QColor("#22c55e")
                   : (warning ? QColor("#f59e0b") : QColor("#64748b")));
      } else {
        item->setBackground(QBrush());
        item->setForeground(QBrush());
      }
    }
  }
  freeRunEntryTable_->resizeColumnsToContents();
  if (selectedRow >= 0 && selectedRow < freeRunEntryTable_->rowCount()) {
    freeRunEntryTable_->selectRow(selectedRow);
  }
  if (freeRunEntryTable_->verticalScrollBar()) {
    freeRunEntryTable_->verticalScrollBar()->setValue(verticalScroll);
  }
  if (freeRunEntryTable_->horizontalScrollBar()) {
    freeRunEntryTable_->horizontalScrollBar()->setValue(horizontalScroll);
  }
  filterFreeRunEntryTable();
  updateIoVariableTable();
  updateStateMachineView();
}

// Apply text/scope filter to the Free Run entry table
void MainWindow::filterFreeRunEntryTable() {
  if (!freeRunEntryTable_) {
    return;
  }
  const QString needle =
      freeRunFilter_ ? freeRunFilter_->text().trimmed() : QString();
  const bool changedOnly =
      freeRunChangedOnly_ && freeRunChangedOnly_->isChecked();
  int visibleRows = 0;
  int changedRows = 0;
  for (int row = 0; row < freeRunEntryTable_->rowCount(); ++row) {
    const auto *firstItem = freeRunEntryTable_->item(row, 0);
    const bool changed = firstItem && firstItem->data(Qt::UserRole).toBool();
    if (changed) {
      ++changedRows;
    }
    bool match = needle.isEmpty();
    for (int column = 0; column < freeRunEntryTable_->columnCount() && !match;
         ++column) {
      const auto *item = freeRunEntryTable_->item(row, column);
      match = item && item->text().contains(needle, Qt::CaseInsensitive);
    }
    match = match && (!changedOnly || changed);
    freeRunEntryTable_->setRowHidden(row, !match);
    if (match) {
      ++visibleRows;
    }
  }
  if (freeRunEntrySummaryLabel_) {
    freeRunEntrySummaryLabel_->setText(
        uiText("%1 visible / %2 entries, %3 changed",
               "%1 可见 / %2 条目，%3 变化")
            .arg(visibleRows)
            .arg(freeRunEntryTable_->rowCount())
            .arg(changedRows));
  }
  updateFreeRunEntryDetail();
  updateCommissioningWorkflow();
}

// Apply text/scope/changed-only filter to the Watch table
void MainWindow::filterWatchTable() {
  if (!watch_->watchTable) {
    return;
  }
  const QString needle =
      watch_->watchFilter ? watch_->watchFilter->text().trimmed() : QString();
  const QString scope =
      watch_->watchScopeFilter ? watch_->watchScopeFilter->currentData().toString() : "all";
  const bool changedOnly = watch_->watchChangedOnly && watch_->watchChangedOnly->isChecked();
  const int selected = selectedPosition();
  int visible = 0;
  int changedRows = 0;
  int driftRows = 0;
  int startupDriftRows = 0;
  int missingValueRows = 0;

  for (int row = 0; row < watch_->watchTable->rowCount(); ++row) {
    const int position = watch_->watchTable->item(row, 1)
                             ? watch_->watchTable->item(row, 1)->text().toInt()
                             : -1;
    const QString index = watch_->watchTable->item(row, 2)
                              ? watch_->watchTable->item(row, 2)->text().trimmed()
                              : QString();
    const QString subIndex = watch_->watchTable->item(row, 3)
                                 ? watch_->watchTable->item(row, 3)->text().trimmed()
                                 : QString();
    const QString key = QString("%1|%2|%3").arg(position).arg(index, subIndex);
    const bool changed = watchChangedKeys_.contains(key);
    if (changed) {
      ++changedRows;
    }
    const QString value = watch_->watchTable->item(row, 4)
                              ? watch_->watchTable->item(row, 4)->text().trimmed()
                              : QString();
    const bool missingValue = value.isEmpty();
    if (missingValue) {
      ++missingValueRows;
    }
    const QString delta = watch_->watchTable->item(row, 9)
                              ? watch_->watchTable->item(row, 9)->text().trimmed()
                              : QString();
    const QString normalizedDelta = delta.toLower();
    const bool baselineDrift = !delta.isEmpty() && normalizedDelta != "0" &&
                               normalizedDelta != "match" && delta != "匹配";
    if (baselineDrift) {
      ++driftRows;
    }
    const QString startupDelta =
        watch_->watchTable->item(row, 11)
            ? watch_->watchTable->item(row, 11)->text().trimmed()
            : QString();
    const QString normalizedStartupDelta = startupDelta.toLower();
    const bool startupDrift =
        !startupDelta.isEmpty() && normalizedStartupDelta != "match" &&
        startupDelta != "匹配" && normalizedStartupDelta != "pending" &&
        startupDelta != "待比较";
    if (startupDrift) {
      ++startupDriftRows;
    }
    const QString mode = watch_->watchTable->item(row, 7)
                             ? watch_->watchTable->item(row, 7)->text().trimmed()
                             : QString();
    const QString normalizedMode = mode.toLower();
    const QString normalizedIndex = normalizeHexText(index, 4);
    const bool cia402 =
        normalizedMode.contains("cia 402") ||
        normalizedMode.contains("cia402") || normalizedIndex == "0x6040" ||
        normalizedIndex == "0x6041" || normalizedIndex == "0x6060" ||
        normalizedIndex == "0x6061" || normalizedIndex == "0x603f" ||
        normalizedIndex == "0x6064" || normalizedIndex == "0x606c" ||
        normalizedIndex == "0x6077" || normalizedIndex == "0x607a" ||
        normalizedIndex == "0x60ff" || normalizedIndex == "0x6071";
    const bool selectedSlave = selected >= 0 && position == selected;

    bool match = needle.isEmpty();
    for (int column = 0; column < watch_->watchTable->columnCount() && !match;
         ++column) {
      const auto *item = watch_->watchTable->item(row, column);
      match = item && item->text().contains(needle, Qt::CaseInsensitive);
    }
    bool scopeMatch = true;
    if (scope == "selected") {
      scopeMatch = selectedSlave;
    } else if (scope == "changed") {
      scopeMatch = changed;
    } else if (scope == "baselineDrift") {
      scopeMatch = baselineDrift;
    } else if (scope == "startupDiff") {
      scopeMatch = startupDrift;
    } else if (scope == "missingValue") {
      scopeMatch = missingValue;
    } else if (scope == "cia402") {
      scopeMatch = cia402;
    }
    match = match && scopeMatch;
    match = match && (!changedOnly || changed);
    watch_->watchTable->setRowHidden(row, !match);
    if (match) {
      ++visible;
    }
  }

  if (watch_->watchSummaryLabel) {
    const bool autoEnabled =
        watch_->watchAutoRefresh && watch_->watchAutoRefresh->isChecked();
    const int interval = watch_->watchRefreshInterval
                             ? watch_->watchRefreshInterval->currentData().toInt()
                             : 1000;
    const QString mode = autoEnabled ? uiText("auto %1 ms", "自动 %1 ms")
                                           .arg(interval > 0 ? interval : 1000)
                                     : uiText("manual", "手动");
    const QString scopeLabel = watch_->watchScopeFilter
                                   ? watch_->watchScopeFilter->currentText()
                                   : uiText("All", "全部");
    const QString summary =
        uiText("%1/%2 | %3 | %4 | changed %5 | drift %6 | startup %7 | missing "
               "%8",
               "%1/%2 | %3 | %4 | 变化 %5 | 偏离 %6 | 启动 %7 | 缺失 %8")
            .arg(visible)
            .arg(watch_->watchTable->rowCount())
            .arg(scopeLabel, mode)
            .arg(changedRows)
            .arg(driftRows)
            .arg(startupDriftRows)
            .arg(missingValueRows);
    watch_->watchSummaryLabel->setText(summary);
    watch_->watchSummaryLabel->setToolTip(
        uiText(
            "Visible rows: %1/%2\nScope: %3\nRefresh: %4\nChanged rows: "
            "%5\nBaseline drift: %6\nStartup diff: %7\nMissing values: %8",
            "可见行：%1/%2\n范围：%3\n刷新：%4\n变化项：%5\n基线偏离：%6\n启动"
            "不一致：%7\n缺失值：%8")
            .arg(visible)
            .arg(watch_->watchTable->rowCount())
            .arg(scopeLabel, mode)
            .arg(changedRows)
            .arg(driftRows)
            .arg(startupDriftRows)
            .arg(missingValueRows));
  }
  updateWatchRowDetail();
  updateCommissioningWorkflow();
}

// Update the detail strip below the Watch table for the current row
void MainWindow::updateWatchRowDetail() {
  if (!watch_->watchDetailLabel) {
    return;
  }
  const WatchRowDetailTexts texts = {
      .unavailableText = uiText("Watch evidence is not available.",
                                "当前没有可用的 Watch 证据。"),
      .unavailableTip =
          uiText("This preview is local only and does not access the bus.",
                 "此预览仅在本地工作，不访问总线。"),
      .noSelectionText = uiText(
          "Select a visible Watch row to review value, decoded meaning, "
          "baseline drift, Startup comparison, and read boundary.",
          "选择一条可见 Watch 行，以复核数值、解析含义、基线偏离、Startup "
          "对照和读取边界。"),
      .noSelectionTip = uiText(
          "Selecting rows, filtering scopes, and reading this detail strip are "
          "local UI actions. Refresh Watch and Auto polling are the explicit "
          "SDO read paths.",
          "选择行、筛选范围和查看此详情条都是本地界面动作；刷新 Watch "
          "和自动轮询才是显式 SDO 读取路径。"),
      .emptyValue = uiText("Empty", "空"),
      .typeFallback = uiText("Type?", "类型?"),
      .noBaseline = uiText("No baseline", "无基线"),
      .noComparison = uiText("No comparison", "无对照"),
      .startupMismatch = uiText("Startup mismatch", "Startup 不一致"),
      .baselineDrift = uiText("Baseline drift", "基线偏离"),
      .changed = uiText("Changed", "已变化"),
      .stableEvidence = uiText("Stable evidence", "证据稳定"),
      .cia402Candidate = uiText("CiA 402 candidate", "CiA 402 候选"),
      .genericSdo = uiText("Generic SDO", "通用 SDO"),
      .matchText = uiText("Match", "匹配"),
      .pendingText = uiText("Pending", "待比较"),
      .summaryPattern =
          uiText("#%1 %2:%3 | %4 | Value: %5 | Baseline: %6 | Startup: %7 | %8",
                 "#%1 %2:%3 | %4 | 值：%5 | 基线：%6 | Startup：%7 | %8"),
      .selectedTitle = uiText("Selected Watch row", "选中的 Watch 行"),
      .timeLabel = uiText("Time", "时间"),
      .slaveLabel = uiText("Slave", "从站"),
      .objectLabel = uiText("Object", "对象"),
      .typeLabel = uiText("Type", "类型"),
      .modeLabel = uiText("Mode", "模式"),
      .valueLabel = uiText("Value", "值"),
      .decodedLabel = uiText("Decoded", "解析"),
      .baselineLabel = uiText("Baseline", "基线"),
      .baselineDeltaLabel = uiText("Baseline Delta", "基线偏差"),
      .startupLabel = uiText("Startup", "启动值"),
      .startupDeltaLabel = uiText("Startup Delta", "启动偏差"),
      .changedLabel = uiText("Changed", "是否变化"),
      .yesText = uiText("Yes", "是"),
      .noText = uiText("No", "否"),
      .driveEvidenceLabel = uiText("Drive Evidence", "驱动证据"),
      .localBoundary = uiText(
          "Local preview boundary: selecting this row, filtering Watch scopes, "
          "and reading this detail strip do not read SDOs, write SDOs, change "
          "state, toggle Free Run, or run Host Health.",
          "本地预览边界：选择此行、筛选 Watch 范围和查看此详情条都不读取 SDO、"
          "不写 SDO、不切换状态、不改变 Free Run，也不运行 Host Health。"),
      .executionBoundary = uiText(
          "Execution boundary: Refresh Watch and Auto polling read SDO "
          "objects; Create Startup and Sync Startup only edit the Startup "
          "table until Apply is used.",
          "执行边界：刷新 Watch 和自动轮询会读取 SDO 对象；创建 Startup "
          "和同步 Startup 只编辑 Startup 表，直到使用应用动作。"),
  };

  auto applyState = [this](const WatchRowDetailUiState &state) {
    watch_->watchDetailLabel->setText(state.text);
    watch_->watchDetailLabel->setProperty("severity", state.severityKey);
    watch_->watchDetailLabel->setToolTip(state.tooltip);
    repolish(watch_->watchDetailLabel);
  };

  if (!watch_->watchTable) {
    applyState(watchRowDetailUnavailableState(texts));
    return;
  }

  const int row = watch_->watchTable->currentRow();
  if (row < 0 || row >= watch_->watchTable->rowCount() ||
      watch_->watchTable->isRowHidden(row)) {
    applyState(watchRowDetailNoSelectionState(texts));
    return;
  }

  applyState(buildWatchRowDetailUiState(
      watchStartupWatchRow(watch_->watchTable, row, watchChangedKeys_), texts));
}

// Helper: bulk-set table rows with headers in a single operation
void MainWindow::setTableRows(QTableWidget *table, const QStringList &headers,
                              const QList<QStringList> &rows) {
  table->setUpdatesEnabled(false);
  table->clear();
  table->setColumnCount(headers.size());
  table->setHorizontalHeaderLabels(headers);
  table->setRowCount(rows.size());
  for (int row = 0; row < rows.size(); ++row) {
    for (int column = 0; column < headers.size(); ++column) {
      table->setItem(row, column,
                     new QTableWidgetItem(rows[row].value(column)));
    }
  }
  table->resizeColumnsToContents();
  table->setUpdatesEnabled(true);
}

// Helper: update a metric card label's title and value
void MainWindow::setMetricCard(QLabel *label, const QString &title,
                               const QString &value) {
  if (!label) {
    return;
  }
  if (auto *card = label->parentWidget()) {
    if (auto *titleLabel = card->findChild<QLabel *>("metricTitle")) {
      titleLabel->setText(title);
    }
  }
  label->setText(value);
  label->setToolTip(value);
}
