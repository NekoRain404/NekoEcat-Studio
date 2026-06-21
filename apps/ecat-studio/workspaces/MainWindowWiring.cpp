// MainWindowWiring.cpp — Signal/slot wiring for MainWindow.
// Extracted from MainWindow::wire() into logical sub-methods.
#include "MainWindowIncludes.h"

QAction *MainWindow::findAction(const char *name) const {
  for (auto *action : findChildren<QAction *>()) {
    if (action->objectName() == name) {
      return action;
    }
  }
  return static_cast<QAction *>(nullptr);
}

void MainWindow::wire() {
  disconnect(&client_, nullptr, this, nullptr);
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
  wireMenuActions();
  wireTopologyAndStateButtons();
  wireSdoInspector();
  wireStartupSdoButtons();
  wireWatchAndBookmarkButtons();
  wireSessionAndWorkflow();
  wireStateMachine();
  wireHostDiagnostics();
  wireIoVariableWorkspace();
  wireConsistencyWorkspace();
  wireDiagnosticsWorkspace();
  wireClearButtonsAndEventFilters();
  wireClientSignals();
  wireTimers();
}

void MainWindow::wireMenuActions() {
  // Wire service signals to EventBus
  connect(container_->sdo(), &SdoService::sdoValueReceived, eventBus_,
          [this](int p, const QString &i, const QString &si, const QString &v) {
            eventBus_->emitSdoValue(p, i, si, v);
          });
  connect(container_->topology(), &TopologyService::scanComplete, eventBus_,
          [this](const QVector<SlaveInfo> &s) {
            eventBus_->emitSlaveChanged(s);
          });

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
}

void MainWindow::wireTopologyAndStateButtons() {
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
}

void MainWindow::wireSdoInspector() {
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
}

void MainWindow::wireStartupSdoButtons() {
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
}

void MainWindow::wireWatchAndBookmarkButtons() {
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
}

void MainWindow::wireSessionAndWorkflow() {
  connect(session_->sessionBriefTable, &QTableWidget::itemSelectionChanged, this,
          &MainWindow::updateSessionBriefCopyButton);
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
  connect(slaveEvidence_->slaveEvidenceMatrixCopyButton, &QPushButton::clicked, this, [this] {
    copySlaveEvidenceMatrixRowDigest(
        slaveEvidence_->slaveEvidenceMatrixTable ? slaveEvidence_->slaveEvidenceMatrixTable->currentRow()
                                  : -1);
  });
  for (auto *button : slaveEvidence_->slaveEvidenceMatrixTriageButtons) {
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
}

void MainWindow::wireStateMachine() {
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
}

void MainWindow::wireHostDiagnostics() {
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
}

void MainWindow::wireIoVariableWorkspace() {
  connect(sdoInspector_->sdoIndex, &QLineEdit::editingFinished, this, [this] {
    restoreManualSdoWriteMode();
    rememberCurrentSdoTarget(uiText("Manual fields", "手动字段"),
                             uiText("Index edited", "已编辑索引"));
  });
  connect(sdoInspector_->sdoSubIndex, &QLineEdit::editingFinished, this, [this] {
    restoreManualSdoWriteMode();
    rememberCurrentSdoTarget(uiText("Manual fields", "手动字段"),
                             uiText("SubIndex edited", "已编辑子项"));
  });
  connect(sdoInspector_->sdoType, &QComboBox::currentTextChanged, this, [this] {
    restoreManualSdoWriteMode();
    rememberCurrentSdoTarget(uiText("Manual fields", "手动字段"),
                             uiText("Type changed", "已修改类型"));
  });
  connect(sdoInspector_->sdoWriteValue, &QLineEdit::textChanged, this,
          [this] { updateSdoInspector(uiText("Manual write", "手动写入")); });
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
}

void MainWindow::wireConsistencyWorkspace() {
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
}

void MainWindow::wireDiagnosticsWorkspace() {
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
}

void MainWindow::wireClearButtonsAndEventFilters() {
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
}

void MainWindow::wireClientSignals() {
  connect(&client_, &EcatClient::connected, this, [this] {
    connectionLabel_->setText(uiText("Runtime connected", "运行时已连接"));
    connectionLabel_->setProperty("state", "connected");
    repolish(connectionLabel_);
    log("Connected to ecatd");
    updateDiagnostics("Info", "Runtime", "Connected to ecatd");
    updateActionAvailability();
    updateWatchAutoRefresh();
    updateStatusBar();
    client_.getBackendMode();
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
  connect(&client_, &EcatClient::reconnecting, this,
          [this](int attempt, int intervalMs) {
            connectionLabel_->setText(
                uiText("Reconnecting (attempt %1, next in %2s)",
                       "重连中（第 %1 次，%2 秒后重试）")
                    .arg(attempt)
                    .arg(intervalMs / 1000));
            connectionLabel_->setProperty("state", "reconnecting");
            repolish(connectionLabel_);
            updateDiagnostics("Info", "Runtime",
                              QString("Reconnect attempt %1, next retry in %2s")
                                  .arg(attempt)
                                  .arg(intervalMs / 1000));
          });
  connect(&client_, &EcatClient::reconnected, this, [this] {
    updateDiagnostics("Info", "Runtime", "Reconnected to ecatd");
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
    rawText_->masterText->setPlainText(text);
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
        handleSdoValueResponse(position, index, subIndex, value);
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


}

void MainWindow::wireTimers() {
  refreshTimer_ = new QTimer(this);
  refreshTimer_->setInterval(3000);
  connect(refreshTimer_, &QTimer::timeout, this, &MainWindow::requestRefresh);
  refreshTimer_->start();

  watchRefreshTimer_ = new QTimer(this);
  connect(watchRefreshTimer_, &QTimer::timeout, this,
          [this] { refreshWatchList(true); });
  updateWatchAutoRefresh();
}
