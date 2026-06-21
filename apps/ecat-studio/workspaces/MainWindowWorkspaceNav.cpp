// MainWindowWorkspaceNav.cpp — Workspace navigation, event filter, and action availability.
// Extracted from MainWindow.cpp to reduce its size.

#include "MainWindowIncludes.h"

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
