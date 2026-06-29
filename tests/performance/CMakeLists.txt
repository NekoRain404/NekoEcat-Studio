# Performance Benchmarks
# Auto-generated from monolithic tests/CMakeLists.txt

# ── EventBus Performance Test ────────────────────────────────────────
add_executable(eventbus_performance_test
    eventbus_performance_test.cpp
    ../../apps/ecat-studio/services/EventBus.cpp
    ../../apps/ecat-studio/services/EventBus.h
)
target_include_directories(eventbus_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(eventbus_performance_test PRIVATE Qt6::Core Qt6::Test ecat_core)
set_target_properties(eventbus_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME eventbus_performance_test COMMAND eventbus_performance_test)

# ── ExportService Performance Test ──────────────────────────────────
add_executable(export_service_performance_test
    export_service_performance_test.cpp
    ../../apps/ecat-studio/services/ExportService.cpp
    ../../apps/ecat-studio/services/ExportService.h
)
target_include_directories(export_service_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(export_service_performance_test PRIVATE
    Qt6::Core Qt6::Widgets Qt6::Test
)
set_target_properties(export_service_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME export_service_performance_test COMMAND export_service_performance_test)
set_tests_properties(export_service_performance_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── DataPipeline Performance Test ───────────────────────────────────
add_executable(data_pipeline_performance_test
    data_pipeline_performance_test.cpp
    ../../apps/ecat-studio/services/DataPipelineService.cpp
    ../../apps/ecat-studio/services/DataPipelineService.h
)
target_include_directories(data_pipeline_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(data_pipeline_performance_test PRIVATE
    Qt6::Core Qt6::Test
)
set_target_properties(data_pipeline_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME data_pipeline_performance_test COMMAND data_pipeline_performance_test)

# ── DeviceManager Performance Test ──────────────────────────────────
add_executable(device_manager_performance_test
    device_manager_performance_test.cpp
    ../../apps/ecat-studio/services/DeviceManagerService.cpp
    ../../apps/ecat-studio/services/DeviceManagerService.h
    ../../apps/ecat-studio/infra/EcatClient.cpp
    ../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(device_manager_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(device_manager_performance_test PRIVATE
    Qt6::Core Qt6::Network Qt6::Test ecat_core
)
set_target_properties(device_manager_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME device_manager_performance_test COMMAND device_manager_performance_test)

# ── MasterApiService Performance Test ──────────────────────────────
add_executable(master_api_performance_test
    master_api_performance_test.cpp
    ../../apps/ecat-studio/services/MasterApiService.cpp
    ../../apps/ecat-studio/services/MasterApiService.h
    ../../apps/ecat-studio/infra/EcatClient.cpp
    ../../apps/ecat-studio/infra/EcatClient.h
    ../../src/core/EthercatTypes.cpp
    ../../src/core/EthercatTypes.h
)
target_include_directories(master_api_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(master_api_performance_test PRIVATE Qt6::Core Qt6::Network Qt6::Test ecat_core)
set_target_properties(master_api_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME master_api_performance_test COMMAND master_api_performance_test)

# ── DomainService Performance Test ─────────────────────────────────
add_executable(domain_service_performance_test
    domain_service_performance_test.cpp
    ../../apps/ecat-studio/services/DomainService.cpp
    ../../apps/ecat-studio/services/DomainService.h
)
target_include_directories(domain_service_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(domain_service_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(domain_service_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME domain_service_performance_test COMMAND domain_service_performance_test)

# ── SyncManagerService Performance Test ────────────────────────────
add_executable(sync_manager_performance_test
    sync_manager_performance_test.cpp
    ../../apps/ecat-studio/services/SyncManagerService.cpp
    ../../apps/ecat-studio/services/SyncManagerService.h
)
target_include_directories(sync_manager_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(sync_manager_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(sync_manager_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME sync_manager_performance_test COMMAND sync_manager_performance_test)

# ── StateMachineService Performance Test ────────────────────────────
add_executable(state_machine_performance_test
    state_machine_performance_test.cpp
    ../../apps/ecat-studio/services/StateMachineService.cpp
    ../../apps/ecat-studio/services/StateMachineService.h
)
target_include_directories(state_machine_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(state_machine_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(state_machine_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME state_machine_performance_test COMMAND state_machine_performance_test)

# ── ErrorHandlingService Performance Test ───────────────────────────
add_executable(error_handling_performance_test
    error_handling_performance_test.cpp
    ../../apps/ecat-studio/services/ErrorHandlingService.cpp
    ../../apps/ecat-studio/services/ErrorHandlingService.h
)
target_include_directories(error_handling_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(error_handling_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(error_handling_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME error_handling_performance_test COMMAND error_handling_performance_test)

# ── DiagnosticReportService Performance Test ────────────────────────
add_executable(diagnostic_report_performance_test
    diagnostic_report_performance_test.cpp
    ../../apps/ecat-studio/services/DiagnosticReportService.cpp
    ../../apps/ecat-studio/services/DiagnosticReportService.h
    ../../apps/ecat-studio/services/EventBus.cpp
    ../../apps/ecat-studio/services/EventBus.h
    ../../apps/ecat-studio/services/TopologyService.cpp
    ../../apps/ecat-studio/services/TopologyService.h
    ../../apps/ecat-studio/services/DcSyncService.cpp
    ../../apps/ecat-studio/services/DcSyncService.h
    ../../apps/ecat-studio/services/PerformanceMonitorService.cpp
    ../../apps/ecat-studio/services/PerformanceMonitorService.h
    ../../apps/ecat-studio/services/WatchdogService.cpp
    ../../apps/ecat-studio/services/WatchdogService.h
    ../../apps/ecat-studio/infra/EcatClient.cpp
    ../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(diagnostic_report_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(diagnostic_report_performance_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::Test
    ecat_core
)
set_target_properties(diagnostic_report_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME diagnostic_report_performance_test COMMAND diagnostic_report_performance_test)

# ── HotConnectService Performance Test ──────────────────────────────
add_executable(hot_connect_performance_test
    hot_connect_performance_test.cpp
    ../../apps/ecat-studio/services/HotConnectService.cpp
    ../../apps/ecat-studio/services/HotConnectService.h
)
target_include_directories(hot_connect_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(hot_connect_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(hot_connect_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME hot_connect_performance_test COMMAND hot_connect_performance_test)

# ── RedundancyService Performance Test ──────────────────────────────
add_executable(redundancy_performance_test
    redundancy_performance_test.cpp
    ../../apps/ecat-studio/services/RedundancyService.cpp
    ../../apps/ecat-studio/services/RedundancyService.h
)
target_include_directories(redundancy_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(redundancy_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(redundancy_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME redundancy_performance_test COMMAND redundancy_performance_test)

# ── CableDiagnosticsService Performance Test ────────────────────────
add_executable(cable_diagnostics_performance_test
    cable_diagnostics_performance_test.cpp
    ../../apps/ecat-studio/services/CableDiagnosticsService.cpp
    ../../apps/ecat-studio/services/CableDiagnosticsService.h
)
target_include_directories(cable_diagnostics_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(cable_diagnostics_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(cable_diagnostics_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME cable_diagnostics_performance_test COMMAND cable_diagnostics_performance_test)

# ── EtherCATMonitorService Performance Test ────────────────────────
add_executable(ethercat_monitor_performance_test
    ethercat_monitor_performance_test.cpp
    ../../apps/ecat-studio/services/EtherCATMonitorService.cpp
    ../../apps/ecat-studio/services/EtherCATMonitorService.h
    ../../apps/ecat-studio/services/EventBus.cpp
    ../../apps/ecat-studio/services/EventBus.h
    ../../apps/ecat-studio/infra/EcatClient.cpp
    ../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(ethercat_monitor_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(ethercat_monitor_performance_test PRIVATE
    Qt6::Core Qt6::Network Qt6::Test ecat_core
)
set_target_properties(ethercat_monitor_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_monitor_performance_test COMMAND ethercat_monitor_performance_test)

# ── EtherCATAnalyzerService Performance Test ───────────────────────
add_executable(ethercat_analyzer_performance_test
    ethercat_analyzer_performance_test.cpp
    ../../apps/ecat-studio/services/EtherCATAnalyzerService.cpp
    ../../apps/ecat-studio/services/EtherCATAnalyzerService.h
    ../../apps/ecat-studio/services/EventBus.cpp
    ../../apps/ecat-studio/services/EventBus.h
    ../../apps/ecat-studio/infra/EcatClient.cpp
    ../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(ethercat_analyzer_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(ethercat_analyzer_performance_test PRIVATE
    Qt6::Core Qt6::Network Qt6::Test ecat_core
)
set_target_properties(ethercat_analyzer_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_analyzer_performance_test COMMAND ethercat_analyzer_performance_test)

# ── EtherCATOptimizerService Performance Test ──────────────────────
add_executable(ethercat_optimizer_performance_test
    ethercat_optimizer_performance_test.cpp
    ../../apps/ecat-studio/services/EtherCATOptimizerService.cpp
    ../../apps/ecat-studio/services/EtherCATOptimizerService.h
    ../../apps/ecat-studio/services/EventBus.cpp
    ../../apps/ecat-studio/services/EventBus.h
    ../../apps/ecat-studio/infra/EcatClient.cpp
    ../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(ethercat_optimizer_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(ethercat_optimizer_performance_test PRIVATE
    Qt6::Core Qt6::Network Qt6::Test ecat_core
)
set_target_properties(ethercat_optimizer_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_optimizer_performance_test COMMAND ethercat_optimizer_performance_test)

# ── EtherCATConfigService Performance Test ─────────────────────────
add_executable(ethercat_config_performance_test
    ethercat_config_performance_test.cpp
    ../../apps/ecat-studio/services/EtherCATConfigService.cpp
    ../../apps/ecat-studio/services/EtherCATConfigService.h
)
target_include_directories(ethercat_config_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(ethercat_config_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_config_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_config_performance_test COMMAND ethercat_config_performance_test)

# ── EtherCATBackupService Performance Test ─────────────────────────
add_executable(ethercat_backup_performance_test
    ethercat_backup_performance_test.cpp
    ../../apps/ecat-studio/services/EtherCATBackupService.cpp
    ../../apps/ecat-studio/services/EtherCATBackupService.h
)
target_include_directories(ethercat_backup_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(ethercat_backup_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_backup_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_backup_performance_test COMMAND ethercat_backup_performance_test)

# ── EtherCATRecoveryService Performance Test ───────────────────────
add_executable(ethercat_recovery_performance_test
    ethercat_recovery_performance_test.cpp
    ../../apps/ecat-studio/services/EtherCATRecoveryService.cpp
    ../../apps/ecat-studio/services/EtherCATRecoveryService.h
)
target_include_directories(ethercat_recovery_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(ethercat_recovery_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_recovery_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_recovery_performance_test COMMAND ethercat_recovery_performance_test)

# ── EtherCATSimulationService Performance Test ─────────────────────
add_executable(ethercat_simulation_performance_test
    ethercat_simulation_performance_test.cpp
    ../../apps/ecat-studio/services/EtherCATSimulationService.cpp
    ../../apps/ecat-studio/services/EtherCATSimulationService.h
)
target_include_directories(ethercat_simulation_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(ethercat_simulation_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_simulation_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_simulation_performance_test COMMAND ethercat_simulation_performance_test)

# ── EtherCATTestingService Performance Test ────────────────────────
add_executable(ethercat_testing_performance_test
    ethercat_testing_performance_test.cpp
    ../../apps/ecat-studio/services/EtherCATTestingService.cpp
    ../../apps/ecat-studio/services/EtherCATTestingService.h
)
target_include_directories(ethercat_testing_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(ethercat_testing_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_testing_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_testing_performance_test COMMAND ethercat_testing_performance_test)

# ── EtherCATValidationService Performance Test ─────────────────────
add_executable(ethercat_validation_performance_test
    ethercat_validation_performance_test.cpp
    ../../apps/ecat-studio/services/EtherCATValidationService.cpp
    ../../apps/ecat-studio/services/EtherCATValidationService.h
)
target_include_directories(ethercat_validation_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(ethercat_validation_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_validation_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_validation_performance_test COMMAND ethercat_validation_performance_test)

# ── EtherCATSecurityService Performance Test ────────────────────────
add_executable(ethercat_security_performance_test
    ethercat_security_performance_test.cpp
    ../../apps/ecat-studio/services/EtherCATSecurityService.cpp
    ../../apps/ecat-studio/services/EtherCATSecurityService.h
)
target_include_directories(ethercat_security_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(ethercat_security_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_security_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_security_performance_test COMMAND ethercat_security_performance_test)

# ── EtherCATComplianceService Performance Test ──────────────────────
add_executable(ethercat_compliance_performance_test
    ethercat_compliance_performance_test.cpp
    ../../apps/ecat-studio/services/EtherCATComplianceService.cpp
    ../../apps/ecat-studio/services/EtherCATComplianceService.h
)
target_include_directories(ethercat_compliance_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(ethercat_compliance_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_compliance_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_compliance_performance_test COMMAND ethercat_compliance_performance_test)

# ── EtherCATCertificationService Performance Test ───────────────────
add_executable(ethercat_certification_performance_test
    ethercat_certification_performance_test.cpp
    ../../apps/ecat-studio/services/EtherCATCertificationService.cpp
    ../../apps/ecat-studio/services/EtherCATCertificationService.h
)
target_include_directories(ethercat_certification_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(ethercat_certification_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_certification_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_certification_performance_test COMMAND ethercat_certification_performance_test)

# ── EtherCATDeploymentService Performance ────────────────────────────
add_executable(ethercat_deployment_performance_test
    ethercat_deployment_performance_test.cpp
    ../../apps/ecat-studio/services/EtherCATDeploymentService.cpp
    ../../apps/ecat-studio/services/EtherCATDeploymentService.h
)
target_include_directories(ethercat_deployment_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(ethercat_deployment_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_deployment_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_deployment_performance_test COMMAND ethercat_deployment_performance_test)

# ── EtherCATUpdateService Performance ────────────────────────────────
add_executable(ethercat_update_performance_test
    ethercat_update_performance_test.cpp
    ../../apps/ecat-studio/services/EtherCATUpdateService.cpp
    ../../apps/ecat-studio/services/EtherCATUpdateService.h
)
target_include_directories(ethercat_update_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(ethercat_update_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_update_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_update_performance_test COMMAND ethercat_update_performance_test)

# ── EtherCATMaintenanceService Performance ───────────────────────────
add_executable(ethercat_maintenance_performance_test
    ethercat_maintenance_performance_test.cpp
    ../../apps/ecat-studio/services/EtherCATMaintenanceService.cpp
    ../../apps/ecat-studio/services/EtherCATMaintenanceService.h
)
target_include_directories(ethercat_maintenance_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(ethercat_maintenance_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_maintenance_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_maintenance_performance_test COMMAND ethercat_maintenance_performance_test)

# ── EtherCATIntegrationService Performance ──────────────────────────
add_executable(ethercat_integration_performance_test
    ethercat_integration_performance_test.cpp
    ../../apps/ecat-studio/services/EtherCATIntegrationService.cpp
    ../../apps/ecat-studio/services/EtherCATIntegrationService.h
    ../../apps/ecat-studio/services/EventBus.cpp
    ../../apps/ecat-studio/services/EventBus.h
    ../../apps/ecat-studio/infra/EcatClient.cpp
    ../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(ethercat_integration_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(ethercat_integration_performance_test PRIVATE Qt6::Core Qt6::Network Qt6::Test ecat_core)
set_target_properties(ethercat_integration_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_integration_performance_test COMMAND ethercat_integration_performance_test)

# ── EtherCATVisualizationService Performance ────────────────────────
add_executable(ethercat_visualization_performance_test
    ethercat_visualization_performance_test.cpp
    ../../apps/ecat-studio/services/EtherCATVisualizationService.cpp
    ../../apps/ecat-studio/services/EtherCATVisualizationService.h
    ../../apps/ecat-studio/services/EtherCATAnalyticsService.cpp
    ../../apps/ecat-studio/services/EtherCATAnalyticsService.h
    ../../apps/ecat-studio/services/EtherCATMonitorService.cpp
    ../../apps/ecat-studio/services/EtherCATMonitorService.h
    ../../apps/ecat-studio/services/NetworkDiagnosticsService.cpp
    ../../apps/ecat-studio/services/NetworkDiagnosticsService.h
    ../../apps/ecat-studio/services/EventBus.cpp
    ../../apps/ecat-studio/services/EventBus.h
    ../../apps/ecat-studio/infra/EcatClient.cpp
    ../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(ethercat_visualization_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(ethercat_visualization_performance_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
set_target_properties(ethercat_visualization_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_visualization_performance_test COMMAND ethercat_visualization_performance_test)
set_tests_properties(ethercat_visualization_performance_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── EtherCATReportingService Performance ─────────────────────────────
add_executable(ethercat_reporting_performance_test
    ethercat_reporting_performance_test.cpp
    ../../apps/ecat-studio/services/EtherCATReportingService.cpp
    ../../apps/ecat-studio/services/EtherCATReportingService.h
    ../../apps/ecat-studio/services/EventBus.cpp
    ../../apps/ecat-studio/services/EventBus.h
    ../../apps/ecat-studio/infra/EcatClient.cpp
    ../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(ethercat_reporting_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(ethercat_reporting_performance_test PRIVATE Qt6::Core Qt6::Network Qt6::Test ecat_core)
set_target_properties(ethercat_reporting_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_reporting_performance_test COMMAND ethercat_reporting_performance_test)

# ── EtherCATDocumentationService Performance ─────────────────────────
add_executable(ethercat_documentation_performance_test
    ethercat_documentation_performance_test.cpp
    ../../apps/ecat-studio/services/EtherCATDocumentationService.cpp
    ../../apps/ecat-studio/services/EtherCATDocumentationService.h
    ../../apps/ecat-studio/services/EventBus.cpp
    ../../apps/ecat-studio/services/EventBus.h
    ../../apps/ecat-studio/infra/EcatClient.cpp
    ../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(ethercat_documentation_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(ethercat_documentation_performance_test PRIVATE Qt6::Core Qt6::Network Qt6::Test ecat_core)
set_target_properties(ethercat_documentation_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_documentation_performance_test COMMAND ethercat_documentation_performance_test)

# ── Experimental Cloud/AI/Digital-Twin Tests ─────────────────────
if(ECAT_EXPERIMENTAL_SERVICES)

# ── Workflow Automation Performance ──────────────────────────────────
add_executable(workflow_automation_performance_test
    workflow_automation_performance_test.cpp
    ../../apps/ecat-studio/services/WorkflowAutomationService.cpp
    ../../apps/ecat-studio/services/WorkflowAutomationService.h
)
target_include_directories(workflow_automation_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_automation_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_automation_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_automation_performance_test COMMAND workflow_automation_performance_test)
set_tests_properties(workflow_automation_performance_test PROPERTIES
    TIMEOUT 60
    LABELS "performance;workflow"
)

# ── Workflow Optimization Performance ────────────────────────────────
add_executable(workflow_optimization_performance_test
    workflow_optimization_performance_test.cpp
    ../../apps/ecat-studio/services/WorkflowOptimizationService.cpp
    ../../apps/ecat-studio/services/WorkflowOptimizationService.h
)
target_include_directories(workflow_optimization_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_optimization_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_optimization_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_optimization_performance_test COMMAND workflow_optimization_performance_test)
set_tests_properties(workflow_optimization_performance_test PROPERTIES
    TIMEOUT 60
    LABELS "performance;workflow"
)

# ── Workflow Scheduling Performance ──────────────────────────────────
add_executable(workflow_scheduling_performance_test
    workflow_scheduling_performance_test.cpp
    ../../apps/ecat-studio/services/WorkflowSchedulingService.cpp
    ../../apps/ecat-studio/services/WorkflowSchedulingService.h
)
target_include_directories(workflow_scheduling_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_scheduling_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_scheduling_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_scheduling_performance_test COMMAND workflow_scheduling_performance_test)
set_tests_properties(workflow_scheduling_performance_test PROPERTIES
    TIMEOUT 60
    LABELS "performance;workflow"
)

# ── WorkflowAnalyticsService Performance Test ────────────────────────
add_executable(workflow_analytics_performance_test
    workflow_analytics_performance_test.cpp
    ../../apps/ecat-studio/services/WorkflowAnalyticsService.cpp
    ../../apps/ecat-studio/services/WorkflowAnalyticsService.h
)
target_include_directories(workflow_analytics_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_analytics_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_analytics_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_analytics_performance_test COMMAND workflow_analytics_performance_test)
set_tests_properties(workflow_analytics_performance_test PROPERTIES
    TIMEOUT 60
    LABELS "performance;workflow"
)

# ── WorkflowMonitoringService Performance Test ───────────────────────
add_executable(workflow_monitoring_performance_test
    workflow_monitoring_performance_test.cpp
    ../../apps/ecat-studio/services/WorkflowMonitoringService.cpp
    ../../apps/ecat-studio/services/WorkflowMonitoringService.h
)
target_include_directories(workflow_monitoring_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_monitoring_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_monitoring_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_monitoring_performance_test COMMAND workflow_monitoring_performance_test)
set_tests_properties(workflow_monitoring_performance_test PROPERTIES
    TIMEOUT 60
    LABELS "performance;workflow"
)

# ── Workflow Deployment Performance ─────────────────────────────────
add_executable(workflow_deployment_performance_test
    workflow_deployment_performance_test.cpp
    ../../apps/ecat-studio/services/WorkflowDeploymentService.cpp
    ../../apps/ecat-studio/services/WorkflowDeploymentService.h
)
target_include_directories(workflow_deployment_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_deployment_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_deployment_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_deployment_performance_test COMMAND workflow_deployment_performance_test)
set_tests_properties(workflow_deployment_performance_test PROPERTIES
    TIMEOUT 60
    LABELS "performance;workflow"
)

# ── Workflow Update Performance ─────────────────────────────────────
add_executable(workflow_update_performance_test
    workflow_update_performance_test.cpp
    ../../apps/ecat-studio/services/WorkflowUpdateService.cpp
    ../../apps/ecat-studio/services/WorkflowUpdateService.h
)
target_include_directories(workflow_update_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_update_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_update_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_update_performance_test COMMAND workflow_update_performance_test)
set_tests_properties(workflow_update_performance_test PROPERTIES
    TIMEOUT 60
    LABELS "performance;workflow"
)

# ── Workflow Maintenance Performance ────────────────────────────────
add_executable(workflow_maintenance_performance_test
    workflow_maintenance_performance_test.cpp
    ../../apps/ecat-studio/services/WorkflowMaintenanceService.cpp
    ../../apps/ecat-studio/services/WorkflowMaintenanceService.h
)
target_include_directories(workflow_maintenance_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_maintenance_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_maintenance_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_maintenance_performance_test COMMAND workflow_maintenance_performance_test)
set_tests_properties(workflow_maintenance_performance_test PROPERTIES
    TIMEOUT 60
    LABELS "performance;workflow"
)

# ── WorkflowCloudService Performance Test ─────────────────────────
add_executable(workflow_cloud_service_performance_test
    workflow_cloud_service_performance_test.cpp
    ../../apps/ecat-studio/services/WorkflowCloudService.cpp
    ../../apps/ecat-studio/services/WorkflowCloudService.h
)
target_include_directories(workflow_cloud_service_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_cloud_service_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_cloud_service_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_cloud_service_performance_test COMMAND workflow_cloud_service_performance_test)

# ── WorkflowEdgeService Performance Test ─────────────────────────
add_executable(workflow_edge_service_performance_test
    workflow_edge_service_performance_test.cpp
    ../../apps/ecat-studio/services/WorkflowEdgeService.cpp
    ../../apps/ecat-studio/services/WorkflowEdgeService.h
)
target_include_directories(workflow_edge_service_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_edge_service_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_edge_service_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_edge_service_performance_test COMMAND workflow_edge_service_performance_test)

# ── WorkflowAIService Performance Test ─────────────────────────
add_executable(workflow_ai_service_performance_test
    workflow_ai_service_performance_test.cpp
    ../../apps/ecat-studio/services/WorkflowAIService.cpp
    ../../apps/ecat-studio/services/WorkflowAIService.h
)
target_include_directories(workflow_ai_service_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_ai_service_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_ai_service_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_ai_service_performance_test COMMAND workflow_ai_service_performance_test)

# ── Workflow Visualization Studio Performance Test ───────────────────
add_executable(workflow_visualization_studio_performance_test
    workflow_visualization_studio_performance_test.cpp
    ../../apps/ecat-studio/services/WorkflowVisualizationStudioService.cpp
    ../../apps/ecat-studio/services/WorkflowVisualizationStudioService.h
)
target_include_directories(workflow_visualization_studio_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_visualization_studio_performance_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
set_target_properties(workflow_visualization_studio_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_visualization_studio_performance_test COMMAND workflow_visualization_studio_performance_test)

# ── Workflow Report Designer Performance Test ───────────────────────
add_executable(workflow_report_designer_performance_test
    workflow_report_designer_performance_test.cpp
    ../../apps/ecat-studio/services/WorkflowReportDesignerService.cpp
    ../../apps/ecat-studio/services/WorkflowReportDesignerService.h
)
target_include_directories(workflow_report_designer_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_report_designer_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_report_designer_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_report_designer_performance_test COMMAND workflow_report_designer_performance_test)

# ── Workflow Documentation Browser Performance Test ─────────────────
add_executable(workflow_documentation_browser_performance_test
    workflow_documentation_browser_performance_test.cpp
    ../../apps/ecat-studio/services/WorkflowDocumentationBrowserService.cpp
    ../../apps/ecat-studio/services/WorkflowDocumentationBrowserService.h
    ../../apps/ecat-studio/services/WorkflowSecurityManagerService.cpp
    ../../apps/ecat-studio/services/WorkflowSecurityManagerService.h
    ../../apps/ecat-studio/services/WorkflowComplianceManagerService.cpp
    ../../apps/ecat-studio/services/WorkflowComplianceManagerService.h
    ../../apps/ecat-studio/services/WorkflowCertificationManagerService.cpp
    ../../apps/ecat-studio/services/WorkflowCertificationManagerService.h
)
target_include_directories(workflow_documentation_browser_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_documentation_browser_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_documentation_browser_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_documentation_browser_performance_test COMMAND workflow_documentation_browser_performance_test)

# ── Workflow Security Manager Performance Test ─────────────────────
add_executable(workflow_security_manager_performance_test
    workflow_security_manager_performance_test.cpp
    ../../apps/ecat-studio/services/WorkflowSecurityManagerService.cpp
    ../../apps/ecat-studio/services/WorkflowSecurityManagerService.h
)
target_include_directories(workflow_security_manager_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_security_manager_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_security_manager_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_security_manager_performance_test COMMAND workflow_security_manager_performance_test)

# ── Workflow Compliance Manager Performance Test ────────────────────
add_executable(workflow_compliance_manager_performance_test
    workflow_compliance_manager_performance_test.cpp
    ../../apps/ecat-studio/services/WorkflowComplianceManagerService.cpp
    ../../apps/ecat-studio/services/WorkflowComplianceManagerService.h
)
target_include_directories(workflow_compliance_manager_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_compliance_manager_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_compliance_manager_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_compliance_manager_performance_test COMMAND workflow_compliance_manager_performance_test)

# ── Workflow Certification Manager Performance Test ─────────────────
add_executable(workflow_certification_manager_performance_test
    workflow_certification_manager_performance_test.cpp
    ../../apps/ecat-studio/services/WorkflowCertificationManagerService.cpp
    ../../apps/ecat-studio/services/WorkflowCertificationManagerService.h
)
target_include_directories(workflow_certification_manager_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_certification_manager_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_certification_manager_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_certification_manager_performance_test COMMAND workflow_certification_manager_performance_test)

# ── Core Performance Benchmarks ──────────────────────────────────────
add_executable(core_performance_test
    core_performance_test.cpp
    ../../src/core/JsonProtocol.cpp
    ../../src/core/JsonProtocol.h
    ../../src/core/EthercatTypes.cpp
    ../../src/core/EthercatTypes.h
    ../../apps/ecat-studio/services/EventBus.cpp
    ../../apps/ecat-studio/services/EventBus.h
)
target_include_directories(core_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(core_performance_test PRIVATE Qt6::Core Qt6::Test ecat_core)
set_target_properties(core_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME core_performance_test COMMAND core_performance_test)
