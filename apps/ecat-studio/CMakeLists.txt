# NekoEcat Studio — Qt6 Desktop GUI
#
# Directory layout:
#   models/      Pure data/logic types (no widget dependencies)
#   adapters/    Bridge between models and QTableWidget
#   detail/    Detail panel text builders for each workspace
#   utils/     Reusable utilities (table, text, UI, docs)
#   infra/       Client, settings, shared types
#   workspaces/  MainWindow partial implementations per workspace
#   (root)       MainWindow core, main entry point

add_executable(ecat-studio
    # ── Plugins ───────────────────────────────────────────────────────
    plugins/PluginRegistry.h                 plugins/PluginRegistry.cpp
    plugins/notes/NotesPlugin.h              plugins/notes/NotesPlugin.cpp
    plugins/WorkspacePlugin.h

    # ── Services ──────────────────────────────────────────────────────
    services/EventBus.h                      services/EventBus.cpp

    # ── Models ──────────────────────────────────────────────────────────
    models/Cia402DriveModel.cpp                        models/Cia402DriveModel.h
    models/CommissioningWorkflowModel.cpp              models/CommissioningWorkflowModel.h
    models/EvidenceModel.cpp                     models/EvidenceModel.h
    models/NextBestActionModel.cpp                     models/NextBestActionModel.h
    models/ProcessDataRowModel.cpp                     models/ProcessDataRowModel.h
    models/SdoEvidenceModel.cpp                        models/SdoEvidenceModel.h
    models/SessionBriefModel.cpp                       models/SessionBriefModel.h
    models/SlaveEvidenceModel.cpp                      models/SlaveEvidenceModel.h
    models/TopologyModel.cpp                   models/TopologyModel.h
    models/ConsistencyModel.cpp                  models/ConsistencyModel.h
    models/IoVariableModel.cpp                         models/IoVariableModel.h
    models/WatchStartupModel.cpp                    models/WatchStartupModel.h
    adapters/WorkflowTableAdapter.cpp     adapters/WorkflowTableAdapter.h
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
    detail/WorkflowStepDetail.cpp  detail/WorkflowStepDetail.h
    detail/CommissioningWorkflowDetail.cpp            detail/CommissioningWorkflowDetail.h
    detail/ConsistencyDetail.cpp                detail/ConsistencyDetail.h
    detail/DiagnosticsEventDetail.cpp                 detail/DiagnosticsEventDetail.h
    detail/FreeRunEntryDetail.cpp               detail/FreeRunEntryDetail.h
    detail/HostHealthDetail.cpp                       detail/HostHealthDetail.h
    detail/IoVariableDetail.cpp                 detail/IoVariableDetail.h
    detail/NextBestActionDetail.cpp                   detail/NextBestActionDetail.h
    detail/ObjectBookmarkDetail.cpp             detail/ObjectBookmarkDetail.h
    detail/PdoMapDetail.cpp                     detail/PdoMapDetail.h
    detail/SdoHistoryRowDetail.cpp              detail/SdoHistoryRowDetail.h
    detail/SdoTargetTrailDetail.cpp             detail/SdoTargetTrailDetail.h
    detail/SelectedDriveSummaryDetail.cpp             detail/SelectedDriveSummaryDetail.h
    detail/SlaveEvidenceSummaryDetail.cpp     detail/SlaveEvidenceSummaryDetail.h
    detail/SessionBriefDetail.cpp                     detail/SessionBriefDetail.h
    detail/SlaveEvidenceDetail.cpp                    detail/SlaveEvidenceDetail.h
    detail/StartupSdoRowDetail.cpp              detail/StartupSdoRowDetail.h
    detail/StateMachineRowDetail.cpp            detail/StateMachineRowDetail.h
    detail/WatchRowDetail.cpp                   detail/WatchRowDetail.h
    detail/WatchStartupDetail.cpp                     detail/WatchStartupDetail.h
    detail/WorkspaceBoundaryDetail.cpp                detail/WorkspaceBoundaryDetail.h
    detail/WorkspaceTabBadgeDetail.cpp                detail/WorkspaceTabBadgeDetail.h
    detail/RealtimeChartDialog.cpp              detail/RealtimeChartDialog.h
    # ── Helpers ─────────────────────────────────────────────────────────
    utils/Documentation.cpp                    utils/Documentation.h
    utils/TableHelpers.cpp                     utils/TableHelpers.h
    utils/TextHelpers.cpp                      utils/TextHelpers.h
    utils/UiHelpers.cpp                        utils/UiHelpers.h
    # ── Infrastructure ──────────────────────────────────────────────────
    infra/EcatClient.cpp                               infra/EcatClient.h
    infra/LanguageManager.cpp                       infra/LanguageManager.h
    infra/TranslationRegistry.cpp                   infra/TranslationRegistry.h
    infra/ProcessDataTypes.h
    infra/SettingsDialog.cpp                           infra/SettingsDialog.h
    # ── MainWindow (core + partial per workspace) ───────────────────────
    MainWindow.h
    workspaces/MainWindowIncludes.h
    MainWindow.cpp
    workspaces/MainWindowStateMachine.cpp                workspaces/MainWindowTopologyUi.cpp
    workspaces/MainWindowCommandPalette.cpp
    workspaces/MainWindowConsistency.cpp
    workspaces/MainWindowCommissioning.cpp
    workspaces/MainWindowSlaveEvidence.cpp
    workspaces/MainWindowContextMenus.cpp
    workspaces/MainWindowExport.cpp
    workspaces/MainWindowFreeRunChart.cpp
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
    themes/ThemeManager.h             themes/ThemeManager.cpp
    workspaces/MainWindowUiBuild.cpp
    workspaces/MainWindowWatchWorkspace.cpp
    workspaces/MainWindowWatchSync.cpp
    themes/themes.qrc
    main.cpp
)
target_include_directories(ecat-studio PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/models
    ${CMAKE_CURRENT_SOURCE_DIR}/adapters
    ${CMAKE_CURRENT_SOURCE_DIR}/detail
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/infra
)
target_link_libraries(ecat-studio PRIVATE Qt6::Core Qt6::Network Qt6::Widgets ecat_core)

# ── Qt Translation Files ─────────────────────────────────────────────
qt_add_translations(ecat-studio
    TS_FILES
        ${CMAKE_SOURCE_DIR}/translations/nekoecat_zh.ts
        ${CMAKE_SOURCE_DIR}/translations/nekoecat_ja.ts
        ${CMAKE_SOURCE_DIR}/translations/nekoecat_de.ts
        ${CMAKE_SOURCE_DIR}/translations/nekoecat_ko.ts
        ${CMAKE_SOURCE_DIR}/translations/nekoecat_zh_TW.ts
        ${CMAKE_SOURCE_DIR}/translations/nekoecat_fr.ts
        ${CMAKE_SOURCE_DIR}/translations/nekoecat_es.ts
)
