# Unit Tests - utils
# Auto-generated from monolithic tests/CMakeLists.txt

add_executable(warnings_log_test
    warnings_log_test.cpp
)
target_link_libraries(warnings_log_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(warnings_log_test PROPERTIES AUTOMOC ON)
add_test(NAME warnings_log_test COMMAND warnings_log_test)

add_executable(table_helpers_test
    table_helpers_test.cpp
    ../../../apps/ecat-studio/utils/TableHelpers.cpp
    ../../../apps/ecat-studio/utils/TableHelpers.h
    ../../../apps/ecat-studio/utils/TextHelpers.cpp
    ../../../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(table_helpers_test PRIVATE
    ../../../apps/ecat-studio
)
target_link_libraries(table_helpers_test PRIVATE
    Qt6::Core
    Qt6::Widgets
)

add_test(NAME table_helpers_test COMMAND table_helpers_test)
set_tests_properties(table_helpers_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── Advanced Test Utilities ───────────────────────────────────────────

# TestAutomation
add_executable(test_automation_test
    test_automation_test.cpp
    ../../utils/TestAutomation.cpp
    ../../utils/TestAutomation.h
)
target_include_directories(test_automation_test PRIVATE
    ${CMAKE_SOURCE_DIR}/tests
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(test_automation_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
set_target_properties(test_automation_test PROPERTIES AUTOMOC ON)
add_test(NAME test_automation_test COMMAND test_automation_test)

# TestDataGenerator

add_executable(test_data_generator_test
    test_data_generator_test.cpp
    ../../utils/TestDataGenerator.cpp
    ../../utils/TestDataGenerator.h
    ../../../src/core/EthercatTypes.cpp
    ../../../src/core/EthercatTypes.h
)
target_include_directories(test_data_generator_test PRIVATE
    ${CMAKE_SOURCE_DIR}/tests
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(test_data_generator_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(test_data_generator_test PROPERTIES AUTOMOC ON)
add_test(NAME test_data_generator_test COMMAND test_data_generator_test)

# PluginTestFixture

add_executable(plugin_fixture_test
    plugin_fixture_test.cpp
    ../../fixtures/PluginTestFixture.cpp
    ../../fixtures/PluginTestFixture.h
    ../../../apps/ecat-studio/plugins/PluginRegistry.cpp
    ../../../apps/ecat-studio/plugins/PluginRegistry.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../../apps/ecat-studio/services/ServiceContainer.cpp
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
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(plugin_fixture_test PRIVATE
    ${CMAKE_SOURCE_DIR}/tests
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(plugin_fixture_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
set_target_properties(plugin_fixture_test PROPERTIES AUTOMOC ON)
add_test(NAME plugin_fixture_test COMMAND plugin_fixture_test)
set_tests_properties(plugin_fixture_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ServiceTestFixture

add_executable(service_fixture_test
    service_fixture_test.cpp
    ../../fixtures/ServiceTestFixture.cpp
    ../../fixtures/ServiceTestFixture.h
    ../../../apps/ecat-studio/services/ServiceContainer.cpp
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
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(service_fixture_test PRIVATE
    ${CMAKE_SOURCE_DIR}/tests
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(service_fixture_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
set_target_properties(service_fixture_test PROPERTIES AUTOMOC ON)
add_test(NAME service_fixture_test COMMAND service_fixture_test)
set_tests_properties(service_fixture_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# UITestFixture

add_executable(ui_fixture_test
    ui_fixture_test.cpp
    ../../fixtures/UITestFixture.cpp
    ../../fixtures/UITestFixture.h
)
target_include_directories(ui_fixture_test PRIVATE
    ${CMAKE_SOURCE_DIR}/tests
)
target_link_libraries(ui_fixture_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
set_target_properties(ui_fixture_test PROPERTIES AUTOMOC ON)
add_test(NAME ui_fixture_test COMMAND ui_fixture_test)
set_tests_properties(ui_fixture_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
