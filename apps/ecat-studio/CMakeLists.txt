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
    plugins/dcsync/DcSyncPlugin.h                plugins/dcsync/DcSyncPlugin.cpp
    plugins/alevent/AlEventPlugin.h            plugins/alevent/AlEventPlugin.cpp
    plugins/signal/SignalPlugin.h              plugins/signal/SignalPlugin.cpp
    plugins/signal/SignalChartWidget.h         plugins/signal/SignalChartWidget.cpp
    plugins/WorkspacePlugin.h
    plugins/esi/EsiBrowserPlugin.h              plugins/esi/EsiBrowserPlugin.cpp
    plugins/esi/EsiParser.h                     plugins/esi/EsiParser.cpp
    plugins/esi/EsiDeviceMatcher.h              plugins/esi/EsiDeviceMatcher.cpp
    plugins/busstats/BusStatsPlugin.h           plugins/busstats/BusStatsPlugin.cpp
    plugins/oscilloscope/OscilloscopePlugin.h   plugins/oscilloscope/OscilloscopePlugin.cpp
    plugins/oscilloscope/OscilloscopeWidget.h   plugins/oscilloscope/OscilloscopeWidget.cpp
    plugins/oscilloscope/OscilloscopeService.h  plugins/oscilloscope/OscilloscopeService.cpp
    plugins/protocol/ProtocolAnalyzerPlugin.h   plugins/protocol/ProtocolAnalyzerPlugin.cpp
    plugins/protocol/ProtocolAnalyzerService.h  plugins/protocol/ProtocolAnalyzerService.cpp
    plugins/eoe/EoEPlugin.h                    plugins/eoe/EoEPlugin.cpp
    plugins/soe/SoEPlugin.h                    plugins/soe/SoEPlugin.cpp
    plugins/eni/EniExportPlugin.h              plugins/eni/EniExportPlugin.cpp
    plugins/project/ProjectPlugin.h             plugins/project/ProjectPlugin.cpp
    plugins/automation/AutomationPlugin.h       plugins/automation/AutomationPlugin.cpp
    plugins/chart/ChartPlugin.h                  plugins/chart/ChartPlugin.cpp
    plugins/chart/EcatChartWidget.h              plugins/chart/EcatChartWidget.cpp
    plugins/themecustomizer/ThemeCustomizerPlugin.h plugins/themecustomizer/ThemeCustomizerPlugin.cpp
    plugins/shortcuts/KeyboardShortcutsPlugin.h plugins/shortcuts/KeyboardShortcutsPlugin.cpp
    plugins/preferences/UserPreferencesPlugin.h plugins/preferences/UserPreferencesPlugin.cpp
    plugins/pdomapping/PdoMappingEditorPlugin.h plugins/pdomapping/PdoMappingEditorPlugin.cpp
    plugins/pdomapping/PdoMappingCanvas.h plugins/pdomapping/PdoMappingCanvas.cpp
    plugins/pdomapping/PdoMappingValidator.h plugins/pdomapping/PdoMappingValidator.cpp
    plugins/dcsyncprecision/DcSyncPrecisionPlugin.h plugins/dcsyncprecision/DcSyncPrecisionPlugin.cpp
    plugins/dcsyncprecision/DriftMonitorWidget.h plugins/dcsyncprecision/DriftMonitorWidget.cpp
    plugins/dcsyncprecision/JitterAnalysisWidget.h plugins/dcsyncprecision/JitterAnalysisWidget.cpp
    plugins/onlinediagnostics/OnlineDiagnosticsPlugin.h plugins/onlinediagnostics/OnlineDiagnosticsPlugin.cpp
    plugins/onlinediagnostics/BusMonitorWidget.h plugins/onlinediagnostics/BusMonitorWidget.cpp
    plugins/onlinediagnostics/ErrorAnalyzerWidget.h plugins/onlinediagnostics/ErrorAnalyzerWidget.cpp
    plugins/realtimeperf/RealtimePerformancePlugin.h plugins/realtimeperf/RealtimePerformancePlugin.cpp
    plugins/realtimeperf/LatencyMonitorWidget.h plugins/realtimeperf/LatencyMonitorWidget.cpp
    plugins/realtimeperf/ThroughputMonitorWidget.h plugins/realtimeperf/ThroughputMonitorWidget.cpp
    plugins/overview/OverviewPlugin.h              plugins/overview/OverviewPlugin.cpp
    plugins/od/OdPlugin.h                          plugins/od/OdPlugin.cpp
    plugins/watch/WatchPlugin.h                    plugins/watch/WatchPlugin.cpp
    plugins/freerun/FreeRunPlugin.h                plugins/freerun/FreeRunPlugin.cpp
    plugins/iovariable/IoVariablePlugin.h          plugins/iovariable/IoVariablePlugin.cpp
    plugins/startupsdo/StartupSdoPlugin.h          plugins/startupsdo/StartupSdoPlugin.cpp
    plugins/session/SessionPlugin.h                plugins/session/SessionPlugin.cpp
    plugins/consistency/ConsistencyPlugin.h        plugins/consistency/ConsistencyPlugin.cpp
    plugins/statemachine/StateMachinePlugin.h      plugins/statemachine/StateMachinePlugin.cpp
    plugins/topology/TopologyPlugin.h              plugins/topology/TopologyPlugin.cpp
    plugins/topology/SlaveNodeItem.h               plugins/topology/SlaveNodeItem.cpp
    plugins/topology/TopologyGraphWidget.h         plugins/topology/TopologyGraphWidget.cpp
    plugins/diagnostics/DiagnosticsPlugin.h        plugins/diagnostics/DiagnosticsPlugin.cpp
    plugins/rttest/RtTestPlugin.h                  plugins/rttest/RtTestPlugin.cpp
    plugins/export/ExportPlugin.h                  plugins/export/ExportPlugin.cpp

    # ── Services ──────────────────────────────────────────────────────
    services/EventBus.h                      services/EventBus.cpp
    services/SdoService.h                      services/SdoService.cpp
    services/WatchService.h                    services/WatchService.cpp
    services/TopologyService.h                 services/TopologyService.cpp
    services/DcSyncService.h                      services/DcSyncService.cpp
    services/AlEventService.h                  services/AlEventService.cpp
    services/SignalService.h                   services/SignalService.cpp
    services/ServiceContainer.h                services/ServiceContainer.cpp
    services/EsiService.h                      services/EsiService.cpp
    services/BusStatsService.h                 services/BusStatsService.cpp
    services/OnlineDiagnosticsService.h          services/OnlineDiagnosticsService.cpp
    services/RealtimePerformanceService.h        services/RealtimePerformanceService.cpp
    services/FreeRunConfigurationService.h      services/FreeRunConfigurationService.cpp
    services/FreeRunMonitoringService.h         services/FreeRunMonitoringService.cpp
    services/PdoConfigurationService.h         services/PdoConfigurationService.cpp
    services/OpStateService.h                  services/OpStateService.cpp
    services/ImpactAnalysisService.h           services/ImpactAnalysisService.cpp
    services/AsyncOperationManager.h           services/AsyncOperationManager.cpp
    services/PerformanceMonitorService.h       services/PerformanceMonitorService.cpp
    services/WatchdogService.h                 services/WatchdogService.cpp
    services/SafetyController.h                services/SafetyController.cpp
    services/DiagnosticReportService.h         services/DiagnosticReportService.cpp
    services/ProjectManagerService.h           services/ProjectManagerService.cpp
    services/ConfigurationService.h            services/ConfigurationService.cpp
    services/ChartService.h                    services/ChartService.cpp
    services/ScriptingService.h                services/ScriptingService.cpp
    services/BatchOperationService.h           services/BatchOperationService.cpp
    services/NetworkDiagnosticsService.h       services/NetworkDiagnosticsService.cpp
    services/EcatHealthService.h               services/EcatHealthService.cpp
    services/DcSyncPrecisionService.h         services/DcSyncPrecisionService.cpp
    services/SdoCacheService.h                 services/SdoCacheService.cpp
    services/PdoMappingService.h               services/PdoMappingService.cpp
    services/CoEService.h                      services/CoEService.cpp
    services/FoEService.h                      services/FoEService.cpp
    services/EoEService.h                      services/EoEService.cpp
    services/StateMachineService.h             services/StateMachineService.cpp
    services/ErrorHandlingService.h            services/ErrorHandlingService.cpp
    services/HotConnectService.h               services/HotConnectService.cpp
    services/RedundancyService.h               services/RedundancyService.cpp
    services/ExportService.h                         services/ExportService.cpp
    services/FirmwareUpdateService.h                 services/FirmwareUpdateService.cpp
    services/ReportGeneratorService.h                services/ReportGeneratorService.cpp

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
    utils/ConfirmDialogBuilder.h               utils/ConfirmDialogBuilder.cpp
    utils/Documentation.cpp                    utils/Documentation.h
    utils/TableHelpers.cpp                     utils/TableHelpers.h
    utils/TextHelpers.cpp                      utils/TextHelpers.h
    utils/UiHelpers.cpp                        utils/UiHelpers.h
    utils/MemoryPool.h
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
    workspaces/MainWindowAbout.cpp
    workspaces/MainWindowStateMachine.cpp                workspaces/MainWindowTopologyUi.cpp
    workspaces/MainWindowCommandPalette.cpp
    workspaces/MainWindowConsistency.cpp
    workspaces/MainWindowCommissioning.cpp
    workspaces/MainWindowSlaveEvidence.cpp
    workspaces/MainWindowContextMenus.cpp
    workspaces/MainWindowExport.cpp
    workspaces/MainWindowFreeRunChart.cpp
    workspaces/MainWindowFreeRunWorkspace.cpp
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
    workspaces/MainWindowWiring.cpp
    workspaces/MainWindowWorkspaceNav.cpp
    workspaces/MainWindowSettings.cpp
    themes/themes.qrc
    main.cpp
)
target_include_directories(ecat-studio PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/models
    ${CMAKE_CURRENT_SOURCE_DIR}/adapters
    ${open62541_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/detail
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/infra
    ${CMAKE_CURRENT_SOURCE_DIR}/services
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/esi
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/busstats
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/alarm
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/project
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/oscilloscope
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/protocol
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/chart
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/themecustomizer
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/shortcuts
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/preferences
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/pdomapping
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/onlinediagnostics
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/dcsyncprecision
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/realtimeperf
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/overview
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/od
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/watch
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/freerun
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/iovariable
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/startupsdo
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/session
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/consistency
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/statemachine
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/topology
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/diagnostics
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/alevent
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/dcsync
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/signal
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/rttest
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/export
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/notes
)
# ── Experimental Services ─────────────────────────────────────────────
option(ECAT_EXPERIMENTAL_SERVICES "Include experimental/stub services" OFF)
if(ECAT_EXPERIMENTAL_SERVICES)
    target_sources(ecat-studio PRIVATE
        services/WorkflowComplianceService.h services/WorkflowComplianceService.cpp
        services/WorkflowDeploymentService.h services/WorkflowDeploymentService.cpp
        plugins/alarm/AlarmPlugin.h                 plugins/alarm/AlarmPlugin.cpp
        services/AlarmService.h                    services/AlarmService.cpp
        services/LoggingService.h                  services/LoggingService.cpp
    )
    target_compile_definitions(ecat-studio PRIVATE ECAT_EXPERIMENTAL_SERVICES=1)
    message(STATUS "Experimental services: ENABLED")
else()
    message(STATUS "Experimental services: DISABLED")
endif()

find_package(Qt6 COMPONENTS Qml QUIET)
if(Qt6Qml_FOUND)
    target_link_libraries(ecat-studio PRIVATE Qt6::Qml)
    target_compile_definitions(ecat-studio PRIVATE ECAT_SCRIPTING_ENABLED)
endif()

target_link_libraries(ecat-studio PRIVATE Qt6::Core Qt6::Network Qt6::Widgets ecat_core open62541)

# ── Precompiled Headers ──────────────────────────────────────────────
target_precompile_headers(ecat-studio PRIVATE pch.h)

# ── Build Optimization Flags ─────────────────────────────────────────
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(ecat-studio PRIVATE -O2 -DNDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(ecat-studio PRIVATE -g -O0 -DDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Profile")
    target_compile_options(ecat-studio PRIVATE -O2 -pg -fprofile-generate)
    target_link_options(ecat-studio PRIVATE -pg -fprofile-generate)
endif()

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
