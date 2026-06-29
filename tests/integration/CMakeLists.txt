# Integration Tests
# Auto-generated from monolithic tests/CMakeLists.txt

add_executable(native_backend_integration_test
    native_backend_integration_test.cpp
)

target_include_directories(native_backend_integration_test PRIVATE
    ../../src/igh
    ../../src/core
)

target_link_libraries(native_backend_integration_test PRIVATE
    Qt6::Core
    Qt6::Test
    ecat_igh
    ethercat
)

set_target_properties(native_backend_integration_test PROPERTIES AUTOMOC ON)
add_test(NAME native_backend_integration_test COMMAND native_backend_integration_test)

add_executable(protocol_integration_test
    protocol_integration_test.cpp
    ../../apps/ecat-studio/infra/EcatClient.cpp
    ../../apps/ecat-studio/infra/EcatClient.h
    ../../apps/ecatd/CommandDispatcher.cpp
    ../../apps/ecatd/CommandDispatcher.h
    ../../src/core/JsonProtocol.cpp
    ../../src/core/JsonProtocol.h
    ../../src/core/EthercatTypes.cpp
    ../../src/core/EthercatTypes.h
)

target_include_directories(protocol_integration_test PRIVATE
    ../../apps/ecatd
    ../../apps/ecat-studio/infra
    ../../src/core
)
target_link_libraries(protocol_integration_test PRIVATE Qt6::Core Qt6::Network)

set_target_properties(protocol_integration_test PROPERTIES AUTOMOC ON)

add_test(NAME protocol_integration_test COMMAND protocol_integration_test)

# ── EventBus Integration Test ─────────────────────────────────────────
add_executable(event_bus_integration_test
    event_bus_integration_test.cpp
    ../../apps/ecat-studio/services/EventBus.cpp
    ../../apps/ecat-studio/services/EventBus.h
)
target_link_libraries(event_bus_integration_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(event_bus_integration_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/src/core
)
set_target_properties(event_bus_integration_test PROPERTIES AUTOMOC ON)
add_test(NAME event_bus_integration_test COMMAND event_bus_integration_test)
set_tests_properties(event_bus_integration_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── Plugin Integration Test ─────────────────────────────────────────
add_executable(plugin_integration_test
    plugin_integration_test.cpp
    ../../apps/ecat-studio/plugins/PluginRegistry.cpp
    ../../apps/ecat-studio/plugins/PluginRegistry.h
    ../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../apps/ecat-studio/plugins/notes/NotesPlugin.cpp
    ../../apps/ecat-studio/plugins/notes/NotesPlugin.h
    ../../apps/ecat-studio/plugins/statemachine/StateMachinePlugin.cpp
    ../../apps/ecat-studio/plugins/statemachine/StateMachinePlugin.h
    ../../apps/ecat-studio/services/ServiceContainer.cpp
    ../../apps/ecat-studio/services/SdoCacheService.cpp
    ../../apps/ecat-studio/services/SdoCacheService.h
    ../../apps/ecat-studio/services/SdoOptimizationService.cpp
    ../../apps/ecat-studio/services/SdoOptimizationService.h
    ../../apps/ecat-studio/services/PdoMappingOptimizationService.cpp
    ../../apps/ecat-studio/services/PdoMappingOptimizationService.h
    ../../apps/ecat-studio/services/HardwareVerificationService.cpp
    ../../apps/ecat-studio/services/HardwareVerificationService.h
    ../../apps/ecat-studio/services/AdvancedErrorAnalysisService.cpp
    ../../apps/ecat-studio/services/AdvancedErrorAnalysisService.h
    ../../apps/ecat-studio/services/FreeRunConfigurationService.cpp
    ../../apps/ecat-studio/services/FreeRunConfigurationService.h
    ../../apps/ecat-studio/services/FreeRunMonitoringService.cpp
    ../../apps/ecat-studio/services/FreeRunMonitoringService.h
    ../../apps/ecat-studio/services/PdoConfigurationService.cpp
    ../../apps/ecat-studio/services/PdoConfigurationService.h
    ../../apps/ecat-studio/services/OpStateService.cpp
    ../../apps/ecat-studio/services/OpStateService.h
    ../../apps/ecat-studio/plugins/oscilloscope/OscilloscopeService.cpp
    ../../apps/ecat-studio/plugins/oscilloscope/OscilloscopeService.h
    ../../apps/ecat-studio/plugins/protocol/ProtocolAnalyzerService.cpp
    ../../apps/ecat-studio/plugins/protocol/ProtocolAnalyzerService.h
    ../../apps/ecat-studio/services/WorkflowAnalyticsService.cpp
    ../../apps/ecat-studio/services/WorkflowAnalyticsService.h
    ../../apps/ecat-studio/services/FreeRunOptimizationService.cpp
    ../../apps/ecat-studio/services/FreeRunOptimizationService.h
    ../../apps/ecat-studio/services/RealtimePerformanceService.cpp
    ../../apps/ecat-studio/services/RealtimePerformanceService.h
    ../../apps/ecat-studio/services/RealtimeOptimizerService.cpp
    ../../apps/ecat-studio/services/RealtimeOptimizerService.h
    ../../apps/ecat-studio/services/ScriptingService.cpp
    ../../apps/ecat-studio/services/ScriptingService.h
    ../../apps/ecat-studio/services/TaskManagementService.cpp
    ../../apps/ecat-studio/services/TaskManagementService.h
    ../../apps/ecat-studio/services/ResourceManagementService.cpp
    ../../apps/ecat-studio/services/ResourceManagementService.h
    ../../apps/ecat-studio/services/WorkflowAutomationService.cpp
    ../../apps/ecat-studio/services/WorkflowAutomationService.h
    ../../apps/ecat-studio/services/WorkflowSchedulingService.cpp
    ../../apps/ecat-studio/services/WorkflowSchedulingService.h
    ../../apps/ecat-studio/services/WorkflowMonitoringService.cpp
    ../../apps/ecat-studio/services/WorkflowMonitoringService.h
    ../../apps/ecat-studio/services/WorkflowVisualizationService.cpp
    ../../apps/ecat-studio/services/WorkflowVisualizationService.h
    ../../apps/ecat-studio/services/WorkflowReportingService.cpp
    ../../apps/ecat-studio/services/WorkflowReportingService.h
    ../../apps/ecat-studio/services/WorkflowSecurityService.cpp
    ../../apps/ecat-studio/services/WorkflowSecurityService.h
    ../../apps/ecat-studio/services/WorkflowUpdateService.cpp
    ../../apps/ecat-studio/services/WorkflowUpdateService.h
    ../../apps/ecat-studio/services/WorkflowMaintenanceService.cpp
    ../../apps/ecat-studio/services/WorkflowMaintenanceService.h
    ../../apps/ecat-studio/services/WorkflowVisualizationStudioService.cpp
    ../../apps/ecat-studio/services/WorkflowVisualizationStudioService.h
    ../../apps/ecat-studio/services/WorkflowReportDesignerService.cpp
    ../../apps/ecat-studio/services/WorkflowReportDesignerService.h
    ../../apps/ecat-studio/services/WorkflowDocumentationBrowserService.cpp
    ../../apps/ecat-studio/services/WorkflowDocumentationBrowserService.h
    ../../apps/ecat-studio/services/WorkflowSecurityManagerService.cpp
    ../../apps/ecat-studio/services/WorkflowSecurityManagerService.h
    ../../apps/ecat-studio/services/WorkflowComplianceManagerService.cpp
    ../../apps/ecat-studio/services/WorkflowComplianceManagerService.h
    ../../apps/ecat-studio/services/WorkflowCertificationManagerService.cpp
    ../../apps/ecat-studio/services/WorkflowCertificationManagerService.h
    ../../apps/ecat-studio/services/EventBus.cpp
    ../../apps/ecat-studio/services/EventBus.h
    ../../apps/ecat-studio/services/SdoService.cpp
    ../../apps/ecat-studio/services/SdoService.h
    ../../apps/ecat-studio/services/WatchService.cpp
    ../../apps/ecat-studio/services/WatchService.h
    ../../apps/ecat-studio/services/TopologyService.cpp
    ../../apps/ecat-studio/services/TopologyService.h
    ../../apps/ecat-studio/services/DcSyncService.cpp
    ../../apps/ecat-studio/services/DcSyncService.h
    ../../apps/ecat-studio/services/AlEventService.cpp
    ../../apps/ecat-studio/services/AlEventService.h
    ../../apps/ecat-studio/services/SignalService.cpp
    ../../apps/ecat-studio/services/SignalService.h
    ../../apps/ecat-studio/services/PerformanceMonitorService.cpp
    ../../apps/ecat-studio/services/PerformanceMonitorService.h
    ../../apps/ecat-studio/services/EsiService.cpp
    ../../apps/ecat-studio/services/EsiService.h
    ../../apps/ecat-studio/services/BusStatsService.cpp
    ../../apps/ecat-studio/services/BusStatsService.h
    ../../apps/ecat-studio/services/WatchdogService.cpp
    ../../apps/ecat-studio/services/WatchdogService.h
    ../../apps/ecat-studio/services/SafetyController.cpp
    ../../apps/ecat-studio/services/SafetyController.h
    ../../apps/ecat-studio/services/DiagnosticReportService.cpp
    ../../apps/ecat-studio/services/DiagnosticReportService.h
    ../../apps/ecat-studio/services/ProjectManagerService.cpp
    ../../apps/ecat-studio/services/ProjectManagerService.h
    ../../apps/ecat-studio/services/ConfigurationService.cpp
    ../../apps/ecat-studio/services/ConfigurationService.h
    ../../apps/ecat-studio/services/AlarmService.cpp
    ../../apps/ecat-studio/services/AlarmService.h
    ../../apps/ecat-studio/services/LoggingService.cpp
    ../../apps/ecat-studio/services/LoggingService.h
    ../../apps/ecat-studio/services/ChartService.cpp
    ../../apps/ecat-studio/services/ChartService.h
    ../../apps/ecat-studio/services/BatchOperationService.cpp
    ../../apps/ecat-studio/services/BatchOperationService.h
    ../../apps/ecat-studio/services/NetworkDiagnosticsService.cpp
    ../../apps/ecat-studio/services/NetworkDiagnosticsService.h
    ../../apps/ecat-studio/services/EcatHealthService.cpp
    ../../apps/ecat-studio/services/EcatHealthService.h
    ../../apps/ecat-studio/services/ExportService.cpp
    ../../apps/ecat-studio/services/ExportService.h
    ../../apps/ecat-studio/services/DataPipelineService.cpp
    ../../apps/ecat-studio/services/DataPipelineService.h
    ../../apps/ecat-studio/services/DeviceManagerService.cpp
    ../../apps/ecat-studio/services/DeviceManagerService.h
    ../../apps/ecat-studio/services/FirmwareUpdateService.cpp
    ../../apps/ecat-studio/services/FirmwareUpdateService.h
    ../../apps/ecat-studio/services/ReportGeneratorService.cpp
    ../../apps/ecat-studio/services/ReportGeneratorService.h
    ../../apps/ecat-studio/services/MasterManagerService.cpp
    ../../apps/ecat-studio/services/MasterManagerService.h
    ../../apps/ecat-studio/services/DistributedClockService.cpp
    ../../apps/ecat-studio/services/DistributedClockService.h
    ../../apps/ecat-studio/services/DcSyncPrecisionService.cpp
    ../../apps/ecat-studio/services/DcSyncPrecisionService.h
    ../../apps/ecat-studio/services/DcSyncOptimizerService.cpp
    ../../apps/ecat-studio/services/DcSyncOptimizerService.h
    ../../apps/ecat-studio/services/OnlineDiagnosticsService.cpp
    ../../apps/ecat-studio/services/OnlineDiagnosticsService.h
    ../../apps/ecat-studio/services/MultiMasterService.cpp
    ../../apps/ecat-studio/services/MultiMasterService.h
    ../../apps/ecat-studio/services/PdoMappingService.cpp
    ../../apps/ecat-studio/services/PdoMappingService.h
    ../../apps/ecat-studio/services/CoEService.cpp
    ../../apps/ecat-studio/services/CoEService.h
    ../../apps/ecat-studio/services/FoEService.cpp
    ../../apps/ecat-studio/services/FoEService.h
    ../../apps/ecat-studio/services/EoEService.cpp
    ../../apps/ecat-studio/services/EoEService.h
    ../../apps/ecat-studio/services/TraceService.cpp
    ../../apps/ecat-studio/services/TraceService.h
    ../../apps/ecat-studio/services/DomainService.cpp
    ../../apps/ecat-studio/services/DomainService.h
    ../../apps/ecat-studio/services/SyncManagerService.cpp
    ../../apps/ecat-studio/services/SyncManagerService.h
    ../../apps/ecat-studio/services/StateMachineService.cpp
    ../../apps/ecat-studio/services/StateMachineService.h
    ../../apps/ecat-studio/services/ErrorHandlingService.cpp
    ../../apps/ecat-studio/services/ErrorHandlingService.h
    ../../apps/ecat-studio/services/HotConnectService.cpp
    ../../apps/ecat-studio/services/HotConnectService.h
    ../../apps/ecat-studio/services/RedundancyService.cpp
    ../../apps/ecat-studio/services/RedundancyService.h
    ../../apps/ecat-studio/services/EtherCATMonitorService.cpp
    ../../apps/ecat-studio/services/EtherCATMonitorService.h
    ../../apps/ecat-studio/services/EtherCATAnalyzerService.cpp
    ../../apps/ecat-studio/services/EtherCATAnalyzerService.h
    ../../apps/ecat-studio/services/EtherCATOptimizerService.cpp
    ../../apps/ecat-studio/services/EtherCATOptimizerService.h
    ../../apps/ecat-studio/services/EtherCATConfigService.cpp
    ../../apps/ecat-studio/services/EtherCATConfigService.h
    ../../apps/ecat-studio/services/EtherCATBackupService.cpp
    ../../apps/ecat-studio/services/EtherCATBackupService.h
    ../../apps/ecat-studio/services/EtherCATRecoveryService.cpp
    ../../apps/ecat-studio/services/EtherCATRecoveryService.h
    ../../apps/ecat-studio/services/EtherCATSimulationService.cpp
    ../../apps/ecat-studio/services/EtherCATSimulationService.h
    ../../apps/ecat-studio/services/EtherCATTestingService.cpp
    ../../apps/ecat-studio/services/EtherCATTestingService.h
    ../../apps/ecat-studio/services/EtherCATValidationService.cpp
    ../../apps/ecat-studio/services/EtherCATValidationService.h
    ../../apps/ecat-studio/services/EtherCATSecurityService.cpp
    ../../apps/ecat-studio/services/EtherCATSecurityService.h
    ../../apps/ecat-studio/services/EtherCATComplianceService.cpp
    ../../apps/ecat-studio/services/EtherCATComplianceService.h
    ../../apps/ecat-studio/services/EtherCATCertificationService.cpp
    ../../apps/ecat-studio/services/EtherCATCertificationService.h
    ../../apps/ecat-studio/services/EtherCATSecurityService.cpp
    ../../apps/ecat-studio/services/EtherCATSecurityService.h
    ../../apps/ecat-studio/services/EtherCATComplianceService.cpp
    ../../apps/ecat-studio/services/EtherCATComplianceService.h
    ../../apps/ecat-studio/services/EtherCATCertificationService.cpp
    ../../apps/ecat-studio/services/EtherCATCertificationService.h
    ../../apps/ecat-studio/services/EtherCATAnalyticsService.cpp
    ../../apps/ecat-studio/services/EtherCATAnalyticsService.h
    ../../apps/ecat-studio/services/EtherCATDeploymentService.cpp
    ../../apps/ecat-studio/services/EtherCATDeploymentService.h
    ../../apps/ecat-studio/services/EtherCATUpdateService.cpp
    ../../apps/ecat-studio/services/EtherCATUpdateService.h
    ../../apps/ecat-studio/services/EtherCATMaintenanceService.cpp
    ../../apps/ecat-studio/services/EtherCATMaintenanceService.h
    ../../apps/ecat-studio/services/EtherCATIntegrationService.cpp
    ../../apps/ecat-studio/services/EtherCATIntegrationService.h
    ../../apps/ecat-studio/services/EtherCATVisualizationService.cpp
    ../../apps/ecat-studio/services/EtherCATVisualizationService.h
    ../../apps/ecat-studio/services/EtherCATReportingService.cpp
    ../../apps/ecat-studio/services/EtherCATReportingService.h
    ../../apps/ecat-studio/services/EtherCATDocumentationService.cpp
    ../../apps/ecat-studio/services/EtherCATDocumentationService.h
    ../../apps/ecat-studio/services/TaskManagementService.cpp
    ../../apps/ecat-studio/services/TaskManagementService.h
    ../../apps/ecat-studio/services/ResourceManagementService.cpp
    ../../apps/ecat-studio/services/ResourceManagementService.h
    ../../apps/ecat-studio/infra/EcatClient.cpp
    ../../apps/ecat-studio/infra/EcatClient.h
)
target_link_libraries(plugin_integration_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(plugin_integration_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/src/core
)
set_target_properties(plugin_integration_test PROPERTIES AUTOMOC ON)
add_test(NAME plugin_integration_test COMMAND plugin_integration_test)
set_tests_properties(plugin_integration_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── Service Integration Test ─────────────────────────────────────────
add_executable(service_integration_test
    service_integration_test.cpp
    ../../apps/ecat-studio/services/ServiceContainer.cpp
    ../../apps/ecat-studio/services/SdoCacheService.cpp
    ../../apps/ecat-studio/services/SdoCacheService.h
    ../../apps/ecat-studio/services/SdoOptimizationService.cpp
    ../../apps/ecat-studio/services/SdoOptimizationService.h
    ../../apps/ecat-studio/services/PdoMappingOptimizationService.cpp
    ../../apps/ecat-studio/services/PdoMappingOptimizationService.h
    ../../apps/ecat-studio/services/HardwareVerificationService.cpp
    ../../apps/ecat-studio/services/HardwareVerificationService.h
    ../../apps/ecat-studio/services/AdvancedErrorAnalysisService.cpp
    ../../apps/ecat-studio/services/AdvancedErrorAnalysisService.h
    ../../apps/ecat-studio/services/FreeRunConfigurationService.cpp
    ../../apps/ecat-studio/services/FreeRunConfigurationService.h
    ../../apps/ecat-studio/services/FreeRunMonitoringService.cpp
    ../../apps/ecat-studio/services/FreeRunMonitoringService.h
    ../../apps/ecat-studio/services/PdoConfigurationService.cpp
    ../../apps/ecat-studio/services/PdoConfigurationService.h
    ../../apps/ecat-studio/services/OpStateService.cpp
    ../../apps/ecat-studio/services/OpStateService.h
    ../../apps/ecat-studio/plugins/oscilloscope/OscilloscopeService.cpp
    ../../apps/ecat-studio/plugins/oscilloscope/OscilloscopeService.h
    ../../apps/ecat-studio/plugins/protocol/ProtocolAnalyzerService.cpp
    ../../apps/ecat-studio/plugins/protocol/ProtocolAnalyzerService.h
    ../../apps/ecat-studio/services/WorkflowAnalyticsService.cpp
    ../../apps/ecat-studio/services/WorkflowAnalyticsService.h
    ../../apps/ecat-studio/services/FreeRunOptimizationService.cpp
    ../../apps/ecat-studio/services/FreeRunOptimizationService.h
    ../../apps/ecat-studio/services/RealtimePerformanceService.cpp
    ../../apps/ecat-studio/services/RealtimePerformanceService.h
    ../../apps/ecat-studio/services/RealtimeOptimizerService.cpp
    ../../apps/ecat-studio/services/RealtimeOptimizerService.h
    ../../apps/ecat-studio/services/ScriptingService.cpp
    ../../apps/ecat-studio/services/ScriptingService.h
    ../../apps/ecat-studio/services/TaskManagementService.cpp
    ../../apps/ecat-studio/services/TaskManagementService.h
    ../../apps/ecat-studio/services/ResourceManagementService.cpp
    ../../apps/ecat-studio/services/ResourceManagementService.h
    ../../apps/ecat-studio/services/EventBus.cpp
    ../../apps/ecat-studio/services/EventBus.h
    ../../apps/ecat-studio/services/SdoService.cpp
    ../../apps/ecat-studio/services/SdoService.h
    ../../apps/ecat-studio/services/WatchService.cpp
    ../../apps/ecat-studio/services/WatchService.h
    ../../apps/ecat-studio/services/TopologyService.cpp
    ../../apps/ecat-studio/services/TopologyService.h
    ../../apps/ecat-studio/services/DcSyncService.cpp
    ../../apps/ecat-studio/services/DcSyncService.h
    ../../apps/ecat-studio/services/AlEventService.cpp
    ../../apps/ecat-studio/services/AlEventService.h
    ../../apps/ecat-studio/services/SignalService.cpp
    ../../apps/ecat-studio/services/SignalService.h
    ../../apps/ecat-studio/services/PerformanceMonitorService.cpp
    ../../apps/ecat-studio/services/PerformanceMonitorService.h
    ../../apps/ecat-studio/services/EsiService.cpp
    ../../apps/ecat-studio/services/EsiService.h
    ../../apps/ecat-studio/services/BusStatsService.cpp
    ../../apps/ecat-studio/services/BusStatsService.h
    ../../apps/ecat-studio/services/WatchdogService.cpp
    ../../apps/ecat-studio/services/WatchdogService.h
    ../../apps/ecat-studio/services/SafetyController.cpp
    ../../apps/ecat-studio/services/SafetyController.h
    ../../apps/ecat-studio/services/DiagnosticReportService.cpp
    ../../apps/ecat-studio/services/DiagnosticReportService.h
    ../../apps/ecat-studio/services/ProjectManagerService.cpp
    ../../apps/ecat-studio/services/ProjectManagerService.h
    ../../apps/ecat-studio/services/ConfigurationService.cpp
    ../../apps/ecat-studio/services/ConfigurationService.h
    ../../apps/ecat-studio/services/AlarmService.cpp
    ../../apps/ecat-studio/services/AlarmService.h
    ../../apps/ecat-studio/services/LoggingService.cpp
    ../../apps/ecat-studio/services/LoggingService.h
    ../../apps/ecat-studio/services/ChartService.cpp
    ../../apps/ecat-studio/services/ChartService.h
    ../../apps/ecat-studio/services/BatchOperationService.cpp
    ../../apps/ecat-studio/services/BatchOperationService.h
    ../../apps/ecat-studio/services/NetworkDiagnosticsService.cpp
    ../../apps/ecat-studio/services/NetworkDiagnosticsService.h
    ../../apps/ecat-studio/services/EcatHealthService.cpp
    ../../apps/ecat-studio/services/EcatHealthService.h
    ../../apps/ecat-studio/services/ExportService.cpp
    ../../apps/ecat-studio/services/ExportService.h
    ../../apps/ecat-studio/services/DataPipelineService.cpp
    ../../apps/ecat-studio/services/DataPipelineService.h
    ../../apps/ecat-studio/services/DeviceManagerService.cpp
    ../../apps/ecat-studio/services/DeviceManagerService.h
    ../../apps/ecat-studio/services/FirmwareUpdateService.cpp
    ../../apps/ecat-studio/services/FirmwareUpdateService.h
    ../../apps/ecat-studio/services/ReportGeneratorService.cpp
    ../../apps/ecat-studio/services/ReportGeneratorService.h
    ../../apps/ecat-studio/services/MasterManagerService.cpp
    ../../apps/ecat-studio/services/MasterManagerService.h
    ../../apps/ecat-studio/services/DistributedClockService.cpp
    ../../apps/ecat-studio/services/DistributedClockService.h
    ../../apps/ecat-studio/services/DcSyncPrecisionService.cpp
    ../../apps/ecat-studio/services/DcSyncPrecisionService.h
    ../../apps/ecat-studio/services/DcSyncOptimizerService.cpp
    ../../apps/ecat-studio/services/DcSyncOptimizerService.h
    ../../apps/ecat-studio/services/OnlineDiagnosticsService.cpp
    ../../apps/ecat-studio/services/OnlineDiagnosticsService.h
    ../../apps/ecat-studio/services/MultiMasterService.cpp
    ../../apps/ecat-studio/services/MultiMasterService.h
    ../../apps/ecat-studio/services/PdoMappingService.cpp
    ../../apps/ecat-studio/services/PdoMappingService.h
    ../../apps/ecat-studio/services/CoEService.cpp
    ../../apps/ecat-studio/services/CoEService.h
    ../../apps/ecat-studio/services/FoEService.cpp
    ../../apps/ecat-studio/services/FoEService.h
    ../../apps/ecat-studio/services/EoEService.cpp
    ../../apps/ecat-studio/services/EoEService.h
    ../../apps/ecat-studio/services/TraceService.cpp
    ../../apps/ecat-studio/services/TraceService.h
    ../../apps/ecat-studio/services/DomainService.cpp
    ../../apps/ecat-studio/services/DomainService.h
    ../../apps/ecat-studio/services/SyncManagerService.cpp
    ../../apps/ecat-studio/services/SyncManagerService.h
    ../../apps/ecat-studio/services/StateMachineService.cpp
    ../../apps/ecat-studio/services/StateMachineService.h
    ../../apps/ecat-studio/services/ErrorHandlingService.cpp
    ../../apps/ecat-studio/services/ErrorHandlingService.h
    ../../apps/ecat-studio/services/HotConnectService.cpp
    ../../apps/ecat-studio/services/HotConnectService.h
    ../../apps/ecat-studio/services/RedundancyService.cpp
    ../../apps/ecat-studio/services/RedundancyService.h
    ../../apps/ecat-studio/services/EtherCATMonitorService.cpp
    ../../apps/ecat-studio/services/EtherCATMonitorService.h
    ../../apps/ecat-studio/services/EtherCATAnalyzerService.cpp
    ../../apps/ecat-studio/services/EtherCATAnalyzerService.h
    ../../apps/ecat-studio/services/EtherCATOptimizerService.cpp
    ../../apps/ecat-studio/services/EtherCATOptimizerService.h
    ../../apps/ecat-studio/services/EtherCATConfigService.cpp
    ../../apps/ecat-studio/services/EtherCATConfigService.h
    ../../apps/ecat-studio/services/EtherCATBackupService.cpp
    ../../apps/ecat-studio/services/EtherCATBackupService.h
    ../../apps/ecat-studio/services/EtherCATRecoveryService.cpp
    ../../apps/ecat-studio/services/EtherCATRecoveryService.h
    ../../apps/ecat-studio/services/EtherCATSimulationService.cpp
    ../../apps/ecat-studio/services/EtherCATSimulationService.h
    ../../apps/ecat-studio/services/EtherCATTestingService.cpp
    ../../apps/ecat-studio/services/EtherCATTestingService.h
    ../../apps/ecat-studio/services/EtherCATValidationService.cpp
    ../../apps/ecat-studio/services/EtherCATValidationService.h
    ../../apps/ecat-studio/services/EtherCATSecurityService.cpp
    ../../apps/ecat-studio/services/EtherCATSecurityService.h
    ../../apps/ecat-studio/services/EtherCATComplianceService.cpp
    ../../apps/ecat-studio/services/EtherCATComplianceService.h
    ../../apps/ecat-studio/services/EtherCATCertificationService.cpp
    ../../apps/ecat-studio/services/EtherCATCertificationService.h
    ../../apps/ecat-studio/services/EtherCATSecurityService.cpp
    ../../apps/ecat-studio/services/EtherCATSecurityService.h
    ../../apps/ecat-studio/services/EtherCATComplianceService.cpp
    ../../apps/ecat-studio/services/EtherCATComplianceService.h
    ../../apps/ecat-studio/services/EtherCATCertificationService.cpp
    ../../apps/ecat-studio/services/EtherCATCertificationService.h
    ../../apps/ecat-studio/services/EtherCATAnalyticsService.cpp
    ../../apps/ecat-studio/services/EtherCATAnalyticsService.h
    ../../apps/ecat-studio/services/EtherCATDeploymentService.cpp
    ../../apps/ecat-studio/services/EtherCATDeploymentService.h
    ../../apps/ecat-studio/services/EtherCATUpdateService.cpp
    ../../apps/ecat-studio/services/EtherCATUpdateService.h
    ../../apps/ecat-studio/services/EtherCATMaintenanceService.cpp
    ../../apps/ecat-studio/services/EtherCATMaintenanceService.h
    ../../apps/ecat-studio/services/EtherCATIntegrationService.cpp
    ../../apps/ecat-studio/services/EtherCATIntegrationService.h
    ../../apps/ecat-studio/services/EtherCATVisualizationService.cpp
    ../../apps/ecat-studio/services/EtherCATVisualizationService.h
    ../../apps/ecat-studio/services/EtherCATReportingService.cpp
    ../../apps/ecat-studio/services/EtherCATReportingService.h
    ../../apps/ecat-studio/services/EtherCATDocumentationService.cpp
    ../../apps/ecat-studio/services/EtherCATDocumentationService.h
    ../../apps/ecat-studio/services/TaskManagementService.cpp
    ../../apps/ecat-studio/services/TaskManagementService.h
    ../../apps/ecat-studio/services/ResourceManagementService.cpp
    ../../apps/ecat-studio/services/ResourceManagementService.h
    ../../apps/ecat-studio/services/WorkflowAutomationService.cpp
    ../../apps/ecat-studio/services/WorkflowAutomationService.h
    ../../apps/ecat-studio/services/WorkflowSchedulingService.cpp
    ../../apps/ecat-studio/services/WorkflowSchedulingService.h
    ../../apps/ecat-studio/services/WorkflowMonitoringService.cpp
    ../../apps/ecat-studio/services/WorkflowMonitoringService.h
    ../../apps/ecat-studio/services/WorkflowVisualizationService.cpp
    ../../apps/ecat-studio/services/WorkflowVisualizationService.h
    ../../apps/ecat-studio/services/WorkflowReportingService.cpp
    ../../apps/ecat-studio/services/WorkflowReportingService.h
    ../../apps/ecat-studio/services/WorkflowSecurityService.cpp
    ../../apps/ecat-studio/services/WorkflowSecurityService.h
    ../../apps/ecat-studio/services/WorkflowUpdateService.cpp
    ../../apps/ecat-studio/services/WorkflowUpdateService.h
    ../../apps/ecat-studio/services/WorkflowMaintenanceService.cpp
    ../../apps/ecat-studio/services/WorkflowMaintenanceService.h
    ../../apps/ecat-studio/services/WorkflowVisualizationStudioService.cpp
    ../../apps/ecat-studio/services/WorkflowVisualizationStudioService.h
    ../../apps/ecat-studio/services/WorkflowReportDesignerService.cpp
    ../../apps/ecat-studio/services/WorkflowReportDesignerService.h
    ../../apps/ecat-studio/services/WorkflowDocumentationBrowserService.cpp
    ../../apps/ecat-studio/services/WorkflowDocumentationBrowserService.h
    ../../apps/ecat-studio/services/WorkflowSecurityManagerService.cpp
    ../../apps/ecat-studio/services/WorkflowSecurityManagerService.h
    ../../apps/ecat-studio/services/WorkflowComplianceManagerService.cpp
    ../../apps/ecat-studio/services/WorkflowComplianceManagerService.h
    ../../apps/ecat-studio/services/WorkflowCertificationManagerService.cpp
    ../../apps/ecat-studio/services/WorkflowCertificationManagerService.h
    ../../apps/ecat-studio/infra/EcatClient.cpp
    ../../apps/ecat-studio/infra/EcatClient.h
)
target_link_libraries(service_integration_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(service_integration_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/src/core
)
set_target_properties(service_integration_test PROPERTIES AUTOMOC ON)
add_test(NAME service_integration_test COMMAND service_integration_test)
set_tests_properties(service_integration_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── Daemon Lifecycle Test (integration) ──────────────────────────────
add_executable(daemon_lifecycle_test
    daemon_lifecycle_test.cpp
    ../../apps/ecat-studio/infra/EcatClient.cpp
    ../../apps/ecat-studio/infra/EcatClient.h
    ../../apps/ecat-studio/services/EventBus.cpp
    ../../apps/ecat-studio/services/EventBus.h
    ../../src/core/JsonProtocol.cpp
    ../../src/core/JsonProtocol.h
    ../../src/core/EthercatTypes.cpp
    ../../src/core/EthercatTypes.h
)
target_include_directories(daemon_lifecycle_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(daemon_lifecycle_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test)
set_target_properties(daemon_lifecycle_test PROPERTIES AUTOMOC ON)
add_test(NAME daemon_lifecycle_test COMMAND daemon_lifecycle_test)

# ── Plugin Lifecycle Test (integration) ──────────────────────────────
add_executable(plugin_lifecycle_test
    plugin_lifecycle_test.cpp
    ../../apps/ecat-studio/plugins/PluginRegistry.cpp
    ../../apps/ecat-studio/plugins/PluginRegistry.h
    ../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../apps/ecat-studio/plugins/notes/NotesPlugin.cpp
    ../../apps/ecat-studio/plugins/notes/NotesPlugin.h
    ../../apps/ecat-studio/plugins/statemachine/StateMachinePlugin.cpp
    ../../apps/ecat-studio/plugins/statemachine/StateMachinePlugin.h
    ../../apps/ecat-studio/services/ServiceContainer.cpp
    ../../apps/ecat-studio/services/SdoCacheService.cpp
    ../../apps/ecat-studio/services/SdoCacheService.h
    ../../apps/ecat-studio/services/SdoOptimizationService.cpp
    ../../apps/ecat-studio/services/SdoOptimizationService.h
    ../../apps/ecat-studio/services/PdoMappingOptimizationService.cpp
    ../../apps/ecat-studio/services/PdoMappingOptimizationService.h
    ../../apps/ecat-studio/services/HardwareVerificationService.cpp
    ../../apps/ecat-studio/services/HardwareVerificationService.h
    ../../apps/ecat-studio/services/AdvancedErrorAnalysisService.cpp
    ../../apps/ecat-studio/services/AdvancedErrorAnalysisService.h
    ../../apps/ecat-studio/services/FreeRunConfigurationService.cpp
    ../../apps/ecat-studio/services/FreeRunConfigurationService.h
    ../../apps/ecat-studio/services/FreeRunMonitoringService.cpp
    ../../apps/ecat-studio/services/FreeRunMonitoringService.h
    ../../apps/ecat-studio/services/PdoConfigurationService.cpp
    ../../apps/ecat-studio/services/PdoConfigurationService.h
    ../../apps/ecat-studio/services/OpStateService.cpp
    ../../apps/ecat-studio/services/OpStateService.h
    ../../apps/ecat-studio/plugins/oscilloscope/OscilloscopeService.cpp
    ../../apps/ecat-studio/plugins/oscilloscope/OscilloscopeService.h
    ../../apps/ecat-studio/plugins/protocol/ProtocolAnalyzerService.cpp
    ../../apps/ecat-studio/plugins/protocol/ProtocolAnalyzerService.h
    ../../apps/ecat-studio/services/WorkflowAnalyticsService.cpp
    ../../apps/ecat-studio/services/WorkflowAnalyticsService.h
    ../../apps/ecat-studio/services/FreeRunOptimizationService.cpp
    ../../apps/ecat-studio/services/FreeRunOptimizationService.h
    ../../apps/ecat-studio/services/RealtimePerformanceService.cpp
    ../../apps/ecat-studio/services/RealtimePerformanceService.h
    ../../apps/ecat-studio/services/RealtimeOptimizerService.cpp
    ../../apps/ecat-studio/services/RealtimeOptimizerService.h
    ../../apps/ecat-studio/services/TaskManagementService.cpp
    ../../apps/ecat-studio/services/TaskManagementService.h
    ../../apps/ecat-studio/services/ResourceManagementService.cpp
    ../../apps/ecat-studio/services/ResourceManagementService.h
    ../../apps/ecat-studio/services/WorkflowAutomationService.cpp
    ../../apps/ecat-studio/services/WorkflowAutomationService.h
    ../../apps/ecat-studio/services/WorkflowSchedulingService.cpp
    ../../apps/ecat-studio/services/WorkflowSchedulingService.h
    ../../apps/ecat-studio/services/WorkflowMonitoringService.cpp
    ../../apps/ecat-studio/services/WorkflowMonitoringService.h
    ../../apps/ecat-studio/services/WorkflowVisualizationService.cpp
    ../../apps/ecat-studio/services/WorkflowVisualizationService.h
    ../../apps/ecat-studio/services/WorkflowReportingService.cpp
    ../../apps/ecat-studio/services/WorkflowReportingService.h
    ../../apps/ecat-studio/services/WorkflowSecurityService.cpp
    ../../apps/ecat-studio/services/WorkflowSecurityService.h
    ../../apps/ecat-studio/services/WorkflowUpdateService.cpp
    ../../apps/ecat-studio/services/WorkflowUpdateService.h
    ../../apps/ecat-studio/services/WorkflowMaintenanceService.cpp
    ../../apps/ecat-studio/services/WorkflowMaintenanceService.h
    ../../apps/ecat-studio/services/WorkflowVisualizationStudioService.cpp
    ../../apps/ecat-studio/services/WorkflowVisualizationStudioService.h
    ../../apps/ecat-studio/services/WorkflowReportDesignerService.cpp
    ../../apps/ecat-studio/services/WorkflowReportDesignerService.h
    ../../apps/ecat-studio/services/WorkflowDocumentationBrowserService.cpp
    ../../apps/ecat-studio/services/WorkflowDocumentationBrowserService.h
    ../../apps/ecat-studio/services/WorkflowSecurityManagerService.cpp
    ../../apps/ecat-studio/services/WorkflowSecurityManagerService.h
    ../../apps/ecat-studio/services/WorkflowComplianceManagerService.cpp
    ../../apps/ecat-studio/services/WorkflowComplianceManagerService.h
    ../../apps/ecat-studio/services/WorkflowCertificationManagerService.cpp
    ../../apps/ecat-studio/services/WorkflowCertificationManagerService.h
    ../../apps/ecat-studio/services/EventBus.cpp
    ../../apps/ecat-studio/services/EventBus.h
    ../../apps/ecat-studio/services/SdoService.cpp
    ../../apps/ecat-studio/services/SdoService.h
    ../../apps/ecat-studio/services/WatchService.cpp
    ../../apps/ecat-studio/services/WatchService.h
    ../../apps/ecat-studio/services/TopologyService.cpp
    ../../apps/ecat-studio/services/TopologyService.h
    ../../apps/ecat-studio/services/DcSyncService.cpp
    ../../apps/ecat-studio/services/DcSyncService.h
    ../../apps/ecat-studio/services/AlEventService.cpp
    ../../apps/ecat-studio/services/AlEventService.h
    ../../apps/ecat-studio/services/SignalService.cpp
    ../../apps/ecat-studio/services/SignalService.h
    ../../apps/ecat-studio/services/PerformanceMonitorService.cpp
    ../../apps/ecat-studio/services/PerformanceMonitorService.h
    ../../apps/ecat-studio/services/EsiService.cpp
    ../../apps/ecat-studio/services/EsiService.h
    ../../apps/ecat-studio/services/BusStatsService.cpp
    ../../apps/ecat-studio/services/BusStatsService.h
    ../../apps/ecat-studio/services/WatchdogService.cpp
    ../../apps/ecat-studio/services/WatchdogService.h
    ../../apps/ecat-studio/services/SafetyController.cpp
    ../../apps/ecat-studio/services/SafetyController.h
    ../../apps/ecat-studio/services/DiagnosticReportService.cpp
    ../../apps/ecat-studio/services/DiagnosticReportService.h
    ../../apps/ecat-studio/services/ProjectManagerService.cpp
    ../../apps/ecat-studio/services/ProjectManagerService.h
    ../../apps/ecat-studio/services/ConfigurationService.cpp
    ../../apps/ecat-studio/services/ConfigurationService.h
    ../../apps/ecat-studio/services/AlarmService.cpp
    ../../apps/ecat-studio/services/AlarmService.h
    ../../apps/ecat-studio/services/LoggingService.cpp
    ../../apps/ecat-studio/services/LoggingService.h
    ../../apps/ecat-studio/services/ChartService.cpp
    ../../apps/ecat-studio/services/ChartService.h
    ../../apps/ecat-studio/services/ScriptingService.cpp
    ../../apps/ecat-studio/services/ScriptingService.h
    ../../apps/ecat-studio/services/BatchOperationService.cpp
    ../../apps/ecat-studio/services/BatchOperationService.h
    ../../apps/ecat-studio/services/NetworkDiagnosticsService.cpp
    ../../apps/ecat-studio/services/NetworkDiagnosticsService.h
    ../../apps/ecat-studio/services/EcatHealthService.cpp
    ../../apps/ecat-studio/services/EcatHealthService.h
    ../../apps/ecat-studio/services/ExportService.cpp
    ../../apps/ecat-studio/services/ExportService.h
    ../../apps/ecat-studio/services/DataPipelineService.cpp
    ../../apps/ecat-studio/services/DataPipelineService.h
    ../../apps/ecat-studio/services/DeviceManagerService.cpp
    ../../apps/ecat-studio/services/DeviceManagerService.h
    ../../apps/ecat-studio/services/FirmwareUpdateService.cpp
    ../../apps/ecat-studio/services/FirmwareUpdateService.h
    ../../apps/ecat-studio/services/ReportGeneratorService.cpp
    ../../apps/ecat-studio/services/ReportGeneratorService.h
    ../../apps/ecat-studio/services/MasterManagerService.cpp
    ../../apps/ecat-studio/services/MasterManagerService.h
    ../../apps/ecat-studio/services/DistributedClockService.cpp
    ../../apps/ecat-studio/services/DistributedClockService.h
    ../../apps/ecat-studio/services/DcSyncPrecisionService.cpp
    ../../apps/ecat-studio/services/DcSyncPrecisionService.h
    ../../apps/ecat-studio/services/DcSyncOptimizerService.cpp
    ../../apps/ecat-studio/services/DcSyncOptimizerService.h
    ../../apps/ecat-studio/services/OnlineDiagnosticsService.cpp
    ../../apps/ecat-studio/services/OnlineDiagnosticsService.h
    ../../apps/ecat-studio/services/MultiMasterService.cpp
    ../../apps/ecat-studio/services/MultiMasterService.h
    ../../apps/ecat-studio/services/PdoMappingService.cpp
    ../../apps/ecat-studio/services/PdoMappingService.h
    ../../apps/ecat-studio/services/CoEService.cpp
    ../../apps/ecat-studio/services/CoEService.h
    ../../apps/ecat-studio/services/FoEService.cpp
    ../../apps/ecat-studio/services/FoEService.h
    ../../apps/ecat-studio/services/EoEService.cpp
    ../../apps/ecat-studio/services/EoEService.h
    ../../apps/ecat-studio/services/TraceService.cpp
    ../../apps/ecat-studio/services/TraceService.h
    ../../apps/ecat-studio/services/DomainService.cpp
    ../../apps/ecat-studio/services/DomainService.h
    ../../apps/ecat-studio/services/SyncManagerService.cpp
    ../../apps/ecat-studio/services/SyncManagerService.h
    ../../apps/ecat-studio/services/StateMachineService.cpp
    ../../apps/ecat-studio/services/StateMachineService.h
    ../../apps/ecat-studio/services/ErrorHandlingService.cpp
    ../../apps/ecat-studio/services/ErrorHandlingService.h
    ../../apps/ecat-studio/services/HotConnectService.cpp
    ../../apps/ecat-studio/services/HotConnectService.h
    ../../apps/ecat-studio/services/RedundancyService.cpp
    ../../apps/ecat-studio/services/RedundancyService.h
    ../../apps/ecat-studio/services/EtherCATMonitorService.cpp
    ../../apps/ecat-studio/services/EtherCATMonitorService.h
    ../../apps/ecat-studio/services/EtherCATAnalyzerService.cpp
    ../../apps/ecat-studio/services/EtherCATAnalyzerService.h
    ../../apps/ecat-studio/services/EtherCATOptimizerService.cpp
    ../../apps/ecat-studio/services/EtherCATOptimizerService.h
    ../../apps/ecat-studio/services/EtherCATConfigService.cpp
    ../../apps/ecat-studio/services/EtherCATConfigService.h
    ../../apps/ecat-studio/services/EtherCATBackupService.cpp
    ../../apps/ecat-studio/services/EtherCATBackupService.h
    ../../apps/ecat-studio/services/EtherCATRecoveryService.cpp
    ../../apps/ecat-studio/services/EtherCATRecoveryService.h
    ../../apps/ecat-studio/services/EtherCATSimulationService.cpp
    ../../apps/ecat-studio/services/EtherCATSimulationService.h
    ../../apps/ecat-studio/services/EtherCATTestingService.cpp
    ../../apps/ecat-studio/services/EtherCATTestingService.h
    ../../apps/ecat-studio/services/EtherCATValidationService.cpp
    ../../apps/ecat-studio/services/EtherCATValidationService.h
    ../../apps/ecat-studio/services/EtherCATSecurityService.cpp
    ../../apps/ecat-studio/services/EtherCATSecurityService.h
    ../../apps/ecat-studio/services/EtherCATComplianceService.cpp
    ../../apps/ecat-studio/services/EtherCATComplianceService.h
    ../../apps/ecat-studio/services/EtherCATCertificationService.cpp
    ../../apps/ecat-studio/services/EtherCATCertificationService.h
    ../../apps/ecat-studio/services/EtherCATSecurityService.cpp
    ../../apps/ecat-studio/services/EtherCATSecurityService.h
    ../../apps/ecat-studio/services/EtherCATComplianceService.cpp
    ../../apps/ecat-studio/services/EtherCATComplianceService.h
    ../../apps/ecat-studio/services/EtherCATCertificationService.cpp
    ../../apps/ecat-studio/services/EtherCATCertificationService.h
    ../../apps/ecat-studio/services/EtherCATAnalyticsService.cpp
    ../../apps/ecat-studio/services/EtherCATAnalyticsService.h
    ../../apps/ecat-studio/services/EtherCATDeploymentService.cpp
    ../../apps/ecat-studio/services/EtherCATDeploymentService.h
    ../../apps/ecat-studio/services/EtherCATUpdateService.cpp
    ../../apps/ecat-studio/services/EtherCATUpdateService.h
    ../../apps/ecat-studio/services/EtherCATMaintenanceService.cpp
    ../../apps/ecat-studio/services/EtherCATMaintenanceService.h
    ../../apps/ecat-studio/services/EtherCATIntegrationService.cpp
    ../../apps/ecat-studio/services/EtherCATIntegrationService.h
    ../../apps/ecat-studio/services/EtherCATVisualizationService.cpp
    ../../apps/ecat-studio/services/EtherCATVisualizationService.h
    ../../apps/ecat-studio/services/EtherCATReportingService.cpp
    ../../apps/ecat-studio/services/EtherCATReportingService.h
    ../../apps/ecat-studio/services/EtherCATDocumentationService.cpp
    ../../apps/ecat-studio/services/EtherCATDocumentationService.h
    ../../apps/ecat-studio/infra/EcatClient.cpp
    ../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(plugin_lifecycle_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(plugin_lifecycle_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
set_target_properties(plugin_lifecycle_test PROPERTIES AUTOMOC ON)
add_test(NAME plugin_lifecycle_test COMMAND plugin_lifecycle_test)
set_tests_properties(plugin_lifecycle_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── New Plugin Integration Test ─────────────────────────────────────
add_executable(plugin_new_integration_test
    plugin_new_integration_test.cpp
    ../../apps/ecat-studio/plugins/diagram/DiagramPlugin.cpp
    ../../apps/ecat-studio/plugins/diagram/DiagramPlugin.h
    ../../apps/ecat-studio/plugins/diagram/DiagramWidget.cpp
    ../../apps/ecat-studio/plugins/diagram/DiagramWidget.h
    ../../apps/ecat-studio/plugins/formula/FormulaPlugin.cpp
    ../../apps/ecat-studio/plugins/formula/FormulaPlugin.h
    ../../apps/ecat-studio/plugins/scriptlibrary/ScriptLibraryPlugin.cpp
    ../../apps/ecat-studio/plugins/scriptlibrary/ScriptLibraryPlugin.h
    ../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_include_directories(plugin_new_integration_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(plugin_new_integration_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
set_target_properties(plugin_new_integration_test PROPERTIES AUTOMOC ON)
add_test(NAME plugin_new_integration_test COMMAND plugin_new_integration_test)
set_tests_properties(plugin_new_integration_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── Workflow Optimizer Plugin Integration ────────────────────────────
add_executable(workflow_optimizer_plugin_integration_test
    workflow_optimizer_plugin_integration_test.cpp
    ../../apps/ecat-studio/plugins/workflowdesigner/WorkflowDesignerPlugin.cpp
    ../../apps/ecat-studio/plugins/workflowdesigner/WorkflowDesignerPlugin.h
    ../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../apps/ecat-studio/services/WorkflowOptimizationService.cpp
    ../../apps/ecat-studio/services/WorkflowOptimizationService.h
    ../../apps/ecat-studio/services/WorkflowSchedulingService.cpp
    ../../apps/ecat-studio/services/WorkflowSchedulingService.h
)
target_include_directories(workflow_optimizer_plugin_integration_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
)
target_link_libraries(workflow_optimizer_plugin_integration_test PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)
set_target_properties(workflow_optimizer_plugin_integration_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_optimizer_plugin_integration_test COMMAND workflow_optimizer_plugin_integration_test)
set_tests_properties(workflow_optimizer_plugin_integration_test PROPERTIES
    TIMEOUT 60
    LABELS "integration;workflow"
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── Workflow Dashboard Plugin Integration ────────────────────────────
add_executable(workflow_dashboard_plugin_integration_test
    workflow_dashboard_plugin_integration_test.cpp
    ../../apps/ecat-studio/plugins/optimizationdashboard/OptimizationDashboardPlugin.cpp
    ../../apps/ecat-studio/plugins/optimizationdashboard/OptimizationDashboardPlugin.h
    ../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../apps/ecat-studio/services/WorkflowOptimizationService.cpp
    ../../apps/ecat-studio/services/WorkflowOptimizationService.h
    ../../apps/ecat-studio/services/WorkflowMonitoringService.cpp
    ../../apps/ecat-studio/services/WorkflowMonitoringService.h
)
target_include_directories(workflow_dashboard_plugin_integration_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
)
target_link_libraries(workflow_dashboard_plugin_integration_test PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)
set_target_properties(workflow_dashboard_plugin_integration_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_dashboard_plugin_integration_test COMMAND workflow_dashboard_plugin_integration_test)
set_tests_properties(workflow_dashboard_plugin_integration_test PROPERTIES
    TIMEOUT 60
    LABELS "integration;workflow"
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── GUI-Daemon Integration Test ──────────────────────────────────────
add_executable(gui_daemon_integration_test
    gui_daemon_integration_test.cpp
    ../../apps/ecat-studio/infra/EcatClient.cpp
    ../../apps/ecat-studio/infra/EcatClient.h
    ../../apps/ecatd/CommandDispatcher.cpp
    ../../apps/ecatd/CommandDispatcher.h
    ../../src/core/JsonProtocol.cpp
    ../../src/core/JsonProtocol.h
    ../../src/core/EthercatTypes.cpp
    ../../src/core/EthercatTypes.h
)

target_include_directories(gui_daemon_integration_test PRIVATE
    ../../apps/ecatd
    ../../apps/ecat-studio/infra
    ../../src/core
)
target_link_libraries(gui_daemon_integration_test PRIVATE Qt6::Core Qt6::Network)
set_target_properties(gui_daemon_integration_test PROPERTIES AUTOMOC ON)
add_test(NAME gui_daemon_integration_test COMMAND gui_daemon_integration_test)
