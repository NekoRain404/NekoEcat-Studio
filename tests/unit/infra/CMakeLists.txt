# Unit Tests - infra
# Auto-generated from monolithic tests/CMakeLists.txt


add_executable(native_backend_test
    native_backend_test.cpp
)

target_include_directories(native_backend_test PRIVATE
    ../../../src/igh
    ../../../src/core
)

target_link_libraries(native_backend_test PRIVATE
    Qt6::Core
    Qt6::Test
    ecat_igh
    ethercat
)

set_target_properties(native_backend_test PROPERTIES AUTOMOC ON)
add_test(NAME native_backend_test COMMAND native_backend_test)

add_executable(connection_lifecycle_test
    connection_lifecycle_test.cpp
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
    ../../../apps/ecatd/CommandDispatcher.cpp
    ../../../apps/ecatd/CommandDispatcher.h
    ../../../src/core/JsonProtocol.cpp
    ../../../src/core/JsonProtocol.h
    ../../../src/core/EthercatTypes.cpp
    ../../../src/core/EthercatTypes.h
)

target_include_directories(connection_lifecycle_test PRIVATE
    ../../../apps/ecatd
    ../../../apps/ecat-studio/infra
    ../../../src/core
)
target_link_libraries(connection_lifecycle_test PRIVATE Qt6::Core Qt6::Network)

set_target_properties(connection_lifecycle_test PROPERTIES AUTOMOC ON)

add_test(NAME connection_lifecycle_test COMMAND connection_lifecycle_test)

add_executable(ecat_client_methods_test
    ecat_client_methods_test.cpp
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
    ../../../src/core/JsonProtocol.cpp
    ../../../src/core/JsonProtocol.h
    ../../../src/core/EthercatTypes.cpp
    ../../../src/core/EthercatTypes.h
)
target_include_directories(ecat_client_methods_test PRIVATE
    ../../../apps/ecat-studio/infra
    ../../../src/core
)
target_link_libraries(ecat_client_methods_test PRIVATE Qt6::Core Qt6::Network Qt6::Test)
set_target_properties(ecat_client_methods_test PROPERTIES AUTOMOC ON)
add_test(NAME ecat_client_methods_test COMMAND ecat_client_methods_test)

add_executable(language_manager_test
    language_manager_test.cpp
    ../../../apps/ecat-studio/infra/LanguageManager.cpp
    ../../../apps/ecat-studio/infra/LanguageManager.h
)

target_include_directories(language_manager_test PRIVATE
    ../../../apps/ecat-studio/infra
)
target_link_libraries(language_manager_test PRIVATE Qt6::Core Qt6::Test)

set_target_properties(language_manager_test PROPERTIES AUTOMOC ON)

add_test(NAME language_manager_test COMMAND language_manager_test)

add_executable(settings_persistence_test
    settings_persistence_test.cpp
    ../../../apps/ecat-studio/infra/SettingsDialog.cpp
    ../../../apps/ecat-studio/infra/LanguageManager.cpp
    ../../../apps/ecat-studio/infra/LanguageManager.h
    ../../../apps/ecat-studio/infra/SettingsDialog.h
)

target_include_directories(settings_persistence_test PRIVATE
    ../../../apps/ecat-studio/infra
)
target_link_libraries(settings_persistence_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)

set_target_properties(settings_persistence_test PROPERTIES AUTOMOC ON)

add_test(NAME settings_persistence_test COMMAND settings_persistence_test)
set_tests_properties(settings_persistence_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── Concurrent Access Test ─────────────────────────────────────────
add_executable(concurrent_access_test
    concurrent_access_test.cpp
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
)
target_link_libraries(concurrent_access_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(concurrent_access_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/src/core
)
set_target_properties(concurrent_access_test PROPERTIES AUTOMOC ON)
add_test(NAME concurrent_access_test COMMAND concurrent_access_test)
set_tests_properties(concurrent_access_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── Error Recovery Test ─────────────────────────────────────────
add_executable(error_recovery_test
    error_recovery_test.cpp
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
)
target_link_libraries(error_recovery_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(error_recovery_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/src/core
)
set_target_properties(error_recovery_test PROPERTIES AUTOMOC ON)
add_test(NAME error_recovery_test COMMAND error_recovery_test)
set_tests_properties(error_recovery_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── UI Creation Test ─────────────────────────────────────────
add_executable(ui_creation_test
    ui_creation_test.cpp
    ../../../apps/ecat-studio/plugins/notes/NotesPlugin.cpp
    ../../../apps/ecat-studio/plugins/notes/NotesPlugin.h
    ../../../apps/ecat-studio/plugins/statemachine/StateMachinePlugin.cpp
    ../../../apps/ecat-studio/plugins/statemachine/StateMachinePlugin.h
    ../../../apps/ecat-studio/plugins/session/SessionPlugin.cpp
    ../../../apps/ecat-studio/plugins/session/SessionPlugin.h
    ../../../apps/ecat-studio/plugins/overview/OverviewPlugin.cpp
    ../../../apps/ecat-studio/plugins/overview/OverviewPlugin.h
    ../../../apps/ecat-studio/plugins/iovariable/IoVariablePlugin.cpp
    ../../../apps/ecat-studio/plugins/iovariable/IoVariablePlugin.h
    ../../../apps/ecat-studio/plugins/rttest/RtTestPlugin.cpp
    ../../../apps/ecat-studio/plugins/rttest/RtTestPlugin.h
    ../../../apps/ecat-studio/plugins/watch/WatchPlugin.cpp
    ../../../apps/ecat-studio/plugins/watch/WatchPlugin.h
    ../../../apps/ecat-studio/plugins/export/ExportPlugin.cpp
    ../../../apps/ecat-studio/plugins/export/ExportPlugin.h
    ../../../apps/ecat-studio/plugins/od/OdPlugin.cpp
    ../../../apps/ecat-studio/plugins/od/OdPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../../apps/ecat-studio/utils/TableHelpers.cpp
    ../../../apps/ecat-studio/utils/TableHelpers.h
    ../../../apps/ecat-studio/utils/TextHelpers.cpp
    ../../../apps/ecat-studio/utils/TextHelpers.h
    ../../../apps/ecat-studio/services/ServiceContainer.cpp
    ../../../apps/ecat-studio/services/SdoCacheService.cpp
    ../../../apps/ecat-studio/services/SdoCacheService.h
    ../../../apps/ecat-studio/services/SdoOptimizationService.cpp
    ../../../apps/ecat-studio/services/SdoOptimizationService.h
    ../../../apps/ecat-studio/services/PdoMappingOptimizationService.cpp
    ../../../apps/ecat-studio/services/PdoMappingOptimizationService.h
    ../../../apps/ecat-studio/services/HardwareVerificationService.cpp
    ../../../apps/ecat-studio/services/HardwareVerificationService.h
    ../../../apps/ecat-studio/services/AdvancedErrorAnalysisService.cpp
    ../../../apps/ecat-studio/services/AdvancedErrorAnalysisService.h
    ../../../apps/ecat-studio/services/FreeRunConfigurationService.cpp
    ../../../apps/ecat-studio/services/FreeRunConfigurationService.h
    ../../../apps/ecat-studio/services/FreeRunMonitoringService.cpp
    ../../../apps/ecat-studio/services/FreeRunMonitoringService.h
    ../../../apps/ecat-studio/services/PdoConfigurationService.cpp
    ../../../apps/ecat-studio/services/PdoConfigurationService.h
    ../../../apps/ecat-studio/services/OpStateService.cpp
    ../../../apps/ecat-studio/services/OpStateService.h
    ../../../apps/ecat-studio/plugins/oscilloscope/OscilloscopeService.cpp
    ../../../apps/ecat-studio/plugins/oscilloscope/OscilloscopeService.h
    ../../../apps/ecat-studio/plugins/protocol/ProtocolAnalyzerService.cpp
    ../../../apps/ecat-studio/plugins/protocol/ProtocolAnalyzerService.h
    ../../../apps/ecat-studio/services/WorkflowAnalyticsService.cpp
    ../../../apps/ecat-studio/services/WorkflowAnalyticsService.h
    ../../../apps/ecat-studio/services/FreeRunOptimizationService.cpp
    ../../../apps/ecat-studio/services/FreeRunOptimizationService.h
    ../../../apps/ecat-studio/services/RealtimePerformanceService.cpp
    ../../../apps/ecat-studio/services/RealtimePerformanceService.h
    ../../../apps/ecat-studio/services/RealtimeOptimizerService.cpp
    ../../../apps/ecat-studio/services/RealtimeOptimizerService.h
    ../../../apps/ecat-studio/services/ScriptingService.cpp
    ../../../apps/ecat-studio/services/ScriptingService.h
    ../../../apps/ecat-studio/services/TaskManagementService.cpp
    ../../../apps/ecat-studio/services/TaskManagementService.h
    ../../../apps/ecat-studio/services/ResourceManagementService.cpp
    ../../../apps/ecat-studio/services/ResourceManagementService.h
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
    ../../../apps/ecat-studio/services/SdoService.cpp
    ../../../apps/ecat-studio/services/SdoService.h
    ../../../apps/ecat-studio/services/WatchService.cpp
    ../../../apps/ecat-studio/services/WatchService.h
    ../../../apps/ecat-studio/services/TopologyService.cpp
    ../../../apps/ecat-studio/services/TopologyService.h
    ../../../apps/ecat-studio/services/DcSyncService.cpp
    ../../../apps/ecat-studio/services/DcSyncService.h
    ../../../apps/ecat-studio/services/AlEventService.cpp
    ../../../apps/ecat-studio/services/AlEventService.h
    ../../../apps/ecat-studio/services/SignalService.cpp
    ../../../apps/ecat-studio/services/SignalService.h
    ../../../apps/ecat-studio/services/PerformanceMonitorService.cpp
    ../../../apps/ecat-studio/services/PerformanceMonitorService.h
    ../../../apps/ecat-studio/services/EsiService.cpp
    ../../../apps/ecat-studio/services/EsiService.h
    ../../../apps/ecat-studio/services/BusStatsService.cpp
    ../../../apps/ecat-studio/services/BusStatsService.h
    ../../../apps/ecat-studio/services/WatchdogService.cpp
    ../../../apps/ecat-studio/services/WatchdogService.h
    ../../../apps/ecat-studio/services/SafetyController.cpp
    ../../../apps/ecat-studio/services/SafetyController.h
    ../../../apps/ecat-studio/services/DiagnosticReportService.cpp
    ../../../apps/ecat-studio/services/DiagnosticReportService.h
    ../../../apps/ecat-studio/services/ProjectManagerService.cpp
    ../../../apps/ecat-studio/services/ProjectManagerService.h
    ../../../apps/ecat-studio/services/ConfigurationService.cpp
    ../../../apps/ecat-studio/services/ConfigurationService.h
    ../../../apps/ecat-studio/services/AlarmService.cpp
    ../../../apps/ecat-studio/services/AlarmService.h
    ../../../apps/ecat-studio/services/LoggingService.cpp
    ../../../apps/ecat-studio/services/LoggingService.h
    ../../../apps/ecat-studio/services/ChartService.cpp
    ../../../apps/ecat-studio/services/ChartService.h
    ../../../apps/ecat-studio/services/BatchOperationService.cpp
    ../../../apps/ecat-studio/services/BatchOperationService.h
    ../../../apps/ecat-studio/services/NetworkDiagnosticsService.cpp
    ../../../apps/ecat-studio/services/NetworkDiagnosticsService.h
    ../../../apps/ecat-studio/services/EcatHealthService.cpp
    ../../../apps/ecat-studio/services/EcatHealthService.h
    ../../../apps/ecat-studio/services/ExportService.cpp
    ../../../apps/ecat-studio/services/ExportService.h
    ../../../apps/ecat-studio/services/DataPipelineService.cpp
    ../../../apps/ecat-studio/services/DataPipelineService.h
    ../../../apps/ecat-studio/services/DeviceManagerService.cpp
    ../../../apps/ecat-studio/services/DeviceManagerService.h
    ../../../apps/ecat-studio/services/FirmwareUpdateService.cpp
    ../../../apps/ecat-studio/services/FirmwareUpdateService.h
    ../../../apps/ecat-studio/services/ReportGeneratorService.cpp
    ../../../apps/ecat-studio/services/ReportGeneratorService.h
    ../../../apps/ecat-studio/services/MasterManagerService.cpp
    ../../../apps/ecat-studio/services/MasterManagerService.h
    ../../../apps/ecat-studio/services/DistributedClockService.cpp
    ../../../apps/ecat-studio/services/DistributedClockService.h
    ../../../apps/ecat-studio/services/DcSyncPrecisionService.cpp
    ../../../apps/ecat-studio/services/DcSyncPrecisionService.h
    ../../../apps/ecat-studio/services/DcSyncOptimizerService.cpp
    ../../../apps/ecat-studio/services/DcSyncOptimizerService.h
    ../../../apps/ecat-studio/services/OnlineDiagnosticsService.cpp
    ../../../apps/ecat-studio/services/OnlineDiagnosticsService.h
    ../../../apps/ecat-studio/services/MultiMasterService.cpp
    ../../../apps/ecat-studio/services/MultiMasterService.h
    ../../../apps/ecat-studio/services/PdoMappingService.cpp
    ../../../apps/ecat-studio/services/PdoMappingService.h
    ../../../apps/ecat-studio/services/CoEService.cpp
    ../../../apps/ecat-studio/services/CoEService.h
    ../../../apps/ecat-studio/services/FoEService.cpp
    ../../../apps/ecat-studio/services/FoEService.h
    ../../../apps/ecat-studio/services/EoEService.cpp
    ../../../apps/ecat-studio/services/EoEService.h
    ../../../apps/ecat-studio/services/TraceService.cpp
    ../../../apps/ecat-studio/services/TraceService.h
    ../../../apps/ecat-studio/services/DomainService.cpp
    ../../../apps/ecat-studio/services/DomainService.h
    ../../../apps/ecat-studio/services/SyncManagerService.cpp
    ../../../apps/ecat-studio/services/SyncManagerService.h
    ../../../apps/ecat-studio/services/StateMachineService.cpp
    ../../../apps/ecat-studio/services/StateMachineService.h
    ../../../apps/ecat-studio/services/ErrorHandlingService.cpp
    ../../../apps/ecat-studio/services/ErrorHandlingService.h
    ../../../apps/ecat-studio/services/HotConnectService.cpp
    ../../../apps/ecat-studio/services/HotConnectService.h
    ../../../apps/ecat-studio/services/RedundancyService.cpp
    ../../../apps/ecat-studio/services/RedundancyService.h
    ../../../apps/ecat-studio/services/EtherCATMonitorService.cpp
    ../../../apps/ecat-studio/services/EtherCATMonitorService.h
    ../../../apps/ecat-studio/services/EtherCATAnalyzerService.cpp
    ../../../apps/ecat-studio/services/EtherCATAnalyzerService.h
    ../../../apps/ecat-studio/services/EtherCATOptimizerService.cpp
    ../../../apps/ecat-studio/services/EtherCATOptimizerService.h
    ../../../apps/ecat-studio/services/EtherCATConfigService.cpp
    ../../../apps/ecat-studio/services/EtherCATConfigService.h
    ../../../apps/ecat-studio/services/EtherCATBackupService.cpp
    ../../../apps/ecat-studio/services/EtherCATBackupService.h
    ../../../apps/ecat-studio/services/EtherCATRecoveryService.cpp
    ../../../apps/ecat-studio/services/EtherCATRecoveryService.h
    ../../../apps/ecat-studio/services/EtherCATSimulationService.cpp
    ../../../apps/ecat-studio/services/EtherCATSimulationService.h
    ../../../apps/ecat-studio/services/EtherCATTestingService.cpp
    ../../../apps/ecat-studio/services/EtherCATTestingService.h
    ../../../apps/ecat-studio/services/EtherCATValidationService.cpp
    ../../../apps/ecat-studio/services/EtherCATValidationService.h
    ../../../apps/ecat-studio/services/EtherCATSecurityService.cpp
    ../../../apps/ecat-studio/services/EtherCATSecurityService.h
    ../../../apps/ecat-studio/services/EtherCATComplianceService.cpp
    ../../../apps/ecat-studio/services/EtherCATComplianceService.h
    ../../../apps/ecat-studio/services/EtherCATCertificationService.cpp
    ../../../apps/ecat-studio/services/EtherCATCertificationService.h
    ../../../apps/ecat-studio/services/EtherCATSecurityService.cpp
    ../../../apps/ecat-studio/services/EtherCATSecurityService.h
    ../../../apps/ecat-studio/services/EtherCATComplianceService.cpp
    ../../../apps/ecat-studio/services/EtherCATComplianceService.h
    ../../../apps/ecat-studio/services/EtherCATCertificationService.cpp
    ../../../apps/ecat-studio/services/EtherCATCertificationService.h
    ../../../apps/ecat-studio/services/EtherCATAnalyticsService.cpp
    ../../../apps/ecat-studio/services/EtherCATAnalyticsService.h
    ../../../apps/ecat-studio/services/EtherCATDeploymentService.cpp
    ../../../apps/ecat-studio/services/EtherCATDeploymentService.h
    ../../../apps/ecat-studio/services/EtherCATUpdateService.cpp
    ../../../apps/ecat-studio/services/EtherCATUpdateService.h
    ../../../apps/ecat-studio/services/EtherCATMaintenanceService.cpp
    ../../../apps/ecat-studio/services/EtherCATMaintenanceService.h
    ../../../apps/ecat-studio/services/EtherCATIntegrationService.cpp
    ../../../apps/ecat-studio/services/EtherCATIntegrationService.h
    ../../../apps/ecat-studio/services/EtherCATVisualizationService.cpp
    ../../../apps/ecat-studio/services/EtherCATVisualizationService.h
    ../../../apps/ecat-studio/services/EtherCATReportingService.cpp
    ../../../apps/ecat-studio/services/EtherCATReportingService.h
    ../../../apps/ecat-studio/services/EtherCATDocumentationService.cpp
    ../../../apps/ecat-studio/services/EtherCATDocumentationService.h
    ../../../apps/ecat-studio/services/TaskManagementService.cpp
    ../../../apps/ecat-studio/services/TaskManagementService.h
    ../../../apps/ecat-studio/services/ResourceManagementService.cpp
    ../../../apps/ecat-studio/services/ResourceManagementService.h
    ../../../apps/ecat-studio/services/WorkflowAutomationService.cpp
    ../../../apps/ecat-studio/services/WorkflowAutomationService.h
    ../../../apps/ecat-studio/services/WorkflowSchedulingService.cpp
    ../../../apps/ecat-studio/services/WorkflowSchedulingService.h
    ../../../apps/ecat-studio/services/WorkflowMonitoringService.cpp
    ../../../apps/ecat-studio/services/WorkflowMonitoringService.h
    ../../../apps/ecat-studio/services/WorkflowVisualizationService.cpp
    ../../../apps/ecat-studio/services/WorkflowVisualizationService.h
    ../../../apps/ecat-studio/services/WorkflowReportingService.cpp
    ../../../apps/ecat-studio/services/WorkflowReportingService.h
    ../../../apps/ecat-studio/services/WorkflowSecurityService.cpp
    ../../../apps/ecat-studio/services/WorkflowSecurityService.h
    ../../../apps/ecat-studio/services/WorkflowUpdateService.cpp
    ../../../apps/ecat-studio/services/WorkflowUpdateService.h
    ../../../apps/ecat-studio/services/WorkflowMaintenanceService.cpp
    ../../../apps/ecat-studio/services/WorkflowMaintenanceService.h
    ../../../apps/ecat-studio/services/WorkflowVisualizationStudioService.cpp
    ../../../apps/ecat-studio/services/WorkflowVisualizationStudioService.h
    ../../../apps/ecat-studio/services/WorkflowReportDesignerService.cpp
    ../../../apps/ecat-studio/services/WorkflowReportDesignerService.h
    ../../../apps/ecat-studio/services/WorkflowDocumentationBrowserService.cpp
    ../../../apps/ecat-studio/services/WorkflowDocumentationBrowserService.h
    ../../../apps/ecat-studio/services/WorkflowSecurityManagerService.cpp
    ../../../apps/ecat-studio/services/WorkflowSecurityManagerService.h
    ../../../apps/ecat-studio/services/WorkflowComplianceManagerService.cpp
    ../../../apps/ecat-studio/services/WorkflowComplianceManagerService.h
    ../../../apps/ecat-studio/services/WorkflowCertificationManagerService.cpp
    ../../../apps/ecat-studio/services/WorkflowCertificationManagerService.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_link_libraries(ui_creation_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(ui_creation_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/src/core
)
set_target_properties(ui_creation_test PROPERTIES AUTOMOC ON)
add_test(NAME ui_creation_test COMMAND ui_creation_test)
set_tests_properties(ui_creation_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── AsyncOperationManager ──────────────────────────────────────────
add_executable(async_operation_manager_test
    async_operation_manager_test.cpp
    ../../../apps/ecat-studio/services/AsyncOperationManager.cpp
    ../../../apps/ecat-studio/services/AsyncOperationManager.h
)

target_include_directories(async_operation_manager_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)

target_link_libraries(async_operation_manager_test PRIVATE Qt6::Core Qt6::Test)

set_target_properties(async_operation_manager_test PROPERTIES AUTOMOC ON)
add_test(NAME async_operation_manager_test COMMAND async_operation_manager_test)

# ── Project Persistence Test ─────────────────────────────────────────
add_executable(project_persistence_test
    project_persistence_test.cpp
    ../../../apps/ecat-studio/services/ProjectManagerService.cpp
    ../../../apps/ecat-studio/services/ProjectManagerService.h
)

target_include_directories(project_persistence_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)

target_link_libraries(project_persistence_test PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

set_target_properties(project_persistence_test PROPERTIES AUTOMOC ON)

add_test(NAME project_persistence_test COMMAND project_persistence_test)
set_tests_properties(project_persistence_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)
