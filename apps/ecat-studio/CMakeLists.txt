# NekoEcat Studio — Qt6 Desktop GUI
#
# Directory layout:
#   models/      Pure data/logic types (no widget dependencies)
#   adapters/    Bridge between models and QTableWidget
#   ui_state/    Detail panel text builders for each workspace
#   helpers/     Reusable utilities (table, text, UI, docs)
#   infra/       Client, settings, shared types
#   workspaces/  MainWindow partial implementations per workspace
#   (root)       MainWindow core, main entry point

add_executable(ecat-studio

    # ── Models ──────────────────────────────────────────────────────────
    models/Cia402DriveModel.cpp                        models/Cia402DriveModel.h
    models/CommissioningWorkflowModel.cpp              models/CommissioningWorkflowModel.h
    models/ConsistencyEvidenceRouteModel.cpp           models/ConsistencyEvidenceRouteModel.h
    models/ConsistencyGateModel.cpp                    models/ConsistencyGateModel.h
    models/EvidenceStatusModel.cpp                     models/EvidenceStatusModel.h
    models/IoVariableBulkNamingModel.cpp               models/IoVariableBulkNamingModel.h
    models/IoVariableFilterModel.cpp                   models/IoVariableFilterModel.h
    models/IoVariableHandoffModel.cpp                  models/IoVariableHandoffModel.h
    models/NextBestActionModel.cpp                     models/NextBestActionModel.h
    models/ProcessDataRowModel.cpp                     models/ProcessDataRowModel.h
    models/SdoEvidenceModel.cpp                        models/SdoEvidenceModel.h
    models/SdoTargetPanelRouteModel.cpp                models/SdoTargetPanelRouteModel.h
    models/SessionBriefModel.cpp                       models/SessionBriefModel.h
    models/SlaveEvidenceModel.cpp                      models/SlaveEvidenceModel.h
    models/StateRecommendationModel.cpp                models/StateRecommendationModel.h
    models/TopologyBaselineModel.cpp                   models/TopologyBaselineModel.h
    models/TopologyChangeModel.cpp                     models/TopologyChangeModel.h
    models/WatchStartupModel.cpp                       models/WatchStartupModel.h

    # ── Table Adapters (model <-> QTableWidget) ─────────────────────────
    adapters/CommissioningWorkflowTableAdapter.cpp     adapters/CommissioningWorkflowTableAdapter.h
    adapters/ConsistencyTableAdapter.cpp               adapters/ConsistencyTableAdapter.h
    adapters/ProcessDataTableAdapter.cpp               adapters/ProcessDataTableAdapter.h
    adapters/SdoDictionaryTableAdapter.cpp             adapters/SdoDictionaryTableAdapter.h
    adapters/SdoEvidenceTableAdapter.cpp               adapters/SdoEvidenceTableAdapter.h
    adapters/SessionBriefTableAdapter.cpp              adapters/SessionBriefTableAdapter.h
    adapters/SlaveEvidenceTableAdapter.cpp             adapters/SlaveEvidenceTableAdapter.h
    adapters/StateMachineTableAdapter.cpp              adapters/StateMachineTableAdapter.h
    adapters/WatchStartupTableAdapter.cpp              adapters/WatchStartupTableAdapter.h
    adapters/WorkspaceTabBadgeTableAdapter.cpp         adapters/WorkspaceTabBadgeTableAdapter.h

    # ── UI State (detail panel text builders) ───────────────────────────
    ui_state/CommissioningWorkflowStepDetailUiState.cpp  ui_state/CommissioningWorkflowStepDetailUiState.h
    ui_state/CommissioningWorkflowUiState.cpp            ui_state/CommissioningWorkflowUiState.h
    ui_state/ConsistencyDetailUiState.cpp                ui_state/ConsistencyDetailUiState.h
    ui_state/DiagnosticsEventUiState.cpp                 ui_state/DiagnosticsEventUiState.h
    ui_state/FreeRunEntryDetailUiState.cpp               ui_state/FreeRunEntryDetailUiState.h
    ui_state/HostHealthUiState.cpp                       ui_state/HostHealthUiState.h
    ui_state/IoVariableDetailUiState.cpp                 ui_state/IoVariableDetailUiState.h
    ui_state/NextBestActionUiState.cpp                   ui_state/NextBestActionUiState.h
    ui_state/ObjectBookmarkDetailUiState.cpp             ui_state/ObjectBookmarkDetailUiState.h
    ui_state/PdoMapDetailUiState.cpp                     ui_state/PdoMapDetailUiState.h
    ui_state/SdoHistoryRowDetailUiState.cpp              ui_state/SdoHistoryRowDetailUiState.h
    ui_state/SdoTargetTrailDetailUiState.cpp             ui_state/SdoTargetTrailDetailUiState.h
    ui_state/SelectedDriveSummaryUiState.cpp             ui_state/SelectedDriveSummaryUiState.h
    ui_state/SelectedSlaveEvidenceSummaryUiState.cpp     ui_state/SelectedSlaveEvidenceSummaryUiState.h
    ui_state/SessionBriefUiState.cpp                     ui_state/SessionBriefUiState.h
    ui_state/SlaveEvidenceUiState.cpp                    ui_state/SlaveEvidenceUiState.h
    ui_state/StartupSdoRowDetailUiState.cpp              ui_state/StartupSdoRowDetailUiState.h
    ui_state/StateMachineRowDetailUiState.cpp            ui_state/StateMachineRowDetailUiState.h
    ui_state/WatchRowDetailUiState.cpp                   ui_state/WatchRowDetailUiState.h
    ui_state/WatchStartupUiState.cpp                     ui_state/WatchStartupUiState.h
    ui_state/WorkspaceBoundaryUiState.cpp                ui_state/WorkspaceBoundaryUiState.h
    ui_state/WorkspaceTabBadgeUiState.cpp                ui_state/WorkspaceTabBadgeUiState.h

    # ── Helpers ─────────────────────────────────────────────────────────
    helpers/StudioDocumentation.cpp                    helpers/StudioDocumentation.h
    helpers/StudioTableHelpers.cpp                     helpers/StudioTableHelpers.h
    helpers/StudioTextHelpers.cpp                      helpers/StudioTextHelpers.h
    helpers/StudioUiHelpers.cpp                        helpers/StudioUiHelpers.h

    # ── Infrastructure ──────────────────────────────────────────────────
    infra/EcatClient.cpp                               infra/EcatClient.h
    infra/LanguageManager.cpp                       infra/LanguageManager.h
    infra/TranslationRegistry.cpp                   infra/TranslationRegistry.h
    infra/ProcessDataTypes.h
    infra/SettingsDialog.cpp                           infra/SettingsDialog.h

    # ── MainWindow (core + partial per workspace) ───────────────────────
    MainWindow.h
    MainWindow.cpp
    workspaces/MainWindowStateMachine.cpp                workspaces/MainWindowTopologyUi.cpp
    workspaces/MainWindowCommandPalette.cpp
    workspaces/MainWindowConsistencyCommissioning.cpp
    workspaces/MainWindowCommissioning.cpp
    workspaces/MainWindowSlaveEvidence.cpp
    workspaces/MainWindowContextMenus.cpp
    workspaces/MainWindowDiagnosticsTopology.cpp
    workspaces/MainWindowIoVariableWorkspace.cpp
    workspaces/MainWindowIoVariableTable.cpp
    workspaces/MainWindowManual.cpp
    workspaces/MainWindowProjectIo.cpp
    workspaces/MainWindowRtTestWorkspace.cpp
    workspaces/MainWindowSdoWorkspace.cpp
    workspaces/MainWindowSdoWrite.cpp
    workspaces/MainWindowSdoSelection.cpp
    workspaces/MainWindowSdoTargetPanel.cpp
    workspaces/MainWindowSdoBookmarks.cpp
    workspaces/MainWindowSessionWorkspace.cpp
    workspaces/MainWindowStartupSdoWorkspace.cpp
    workspaces/MainWindowTexts.cpp
    workspaces/MainWindowTheme.cpp
    workspaces/MainWindowUiBuild.cpp
    workspaces/MainWindowWatchWorkspace.cpp
    workspaces/MainWindowWatchSync.cpp

    main.cpp
)

target_include_directories(ecat-studio PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/models
    ${CMAKE_CURRENT_SOURCE_DIR}/adapters
    ${CMAKE_CURRENT_SOURCE_DIR}/ui_state
    ${CMAKE_CURRENT_SOURCE_DIR}/helpers
    ${CMAKE_CURRENT_SOURCE_DIR}/infra
)

target_link_libraries(ecat-studio PRIVATE Qt6::Core Qt6::Network Qt6::Widgets ecat_core)
