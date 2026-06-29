# Unit Tests - services
# Auto-generated from monolithic tests/CMakeLists.txt

# ── DcSyncPlugin + DcSyncService (GUI) ──────────────────────────────
add_executable(dc_sync_service_test
    dc_sync_service_test.cpp
    ../../../apps/ecat-studio/plugins/dcsync/DcSyncPlugin.cpp
    ../../../apps/ecat-studio/plugins/dcsync/DcSyncPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../../apps/ecat-studio/services/DcSyncService.cpp
    ../../../apps/ecat-studio/services/DcSyncService.h
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)

target_include_directories(dc_sync_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/src/core
)

target_link_libraries(dc_sync_service_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::Test
    ecat_core
)

set_target_properties(dc_sync_service_test PROPERTIES AUTOMOC ON)

add_test(NAME dc_sync_service_test COMMAND dc_sync_service_test)
set_tests_properties(dc_sync_service_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── AlEventPlugin + AlEventService (GUI) ──────────────────────────────
add_executable(al_event_service_test
    al_event_service_test.cpp
    ../../../apps/ecat-studio/plugins/alevent/AlEventPlugin.cpp
    ../../../apps/ecat-studio/plugins/alevent/AlEventPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../../apps/ecat-studio/services/AlEventService.cpp
    ../../../apps/ecat-studio/services/AlEventService.h
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)

target_include_directories(al_event_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/src/core
)

target_link_libraries(al_event_service_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::Test
    ecat_core
)

set_target_properties(al_event_service_test PROPERTIES AUTOMOC ON)

add_test(NAME al_event_service_test COMMAND al_event_service_test)
set_tests_properties(al_event_service_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── SignalPlugin + SignalService (GUI) ────────────────────────────────
add_executable(signal_service_test
    signal_service_test.cpp
    ../../../apps/ecat-studio/plugins/signal/SignalPlugin.cpp
    ../../../apps/ecat-studio/plugins/signal/SignalPlugin.h
    ../../../apps/ecat-studio/plugins/signal/SignalChartWidget.cpp
    ../../../apps/ecat-studio/plugins/signal/SignalChartWidget.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../../apps/ecat-studio/services/SignalService.cpp
    ../../../apps/ecat-studio/services/SignalService.h
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)

target_include_directories(signal_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/src/core
)

target_link_libraries(signal_service_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::Test
    ecat_core
)

set_target_properties(signal_service_test PROPERTIES AUTOMOC ON)

add_test(NAME signal_service_test COMMAND signal_service_test)
set_tests_properties(signal_service_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── Service Cascade Integration Test ────────────────────────────────
add_executable(service_cascade_test
    service_cascade_test.cpp
    ../../../apps/ecat-studio/services/SdoService.cpp
    ../../../apps/ecat-studio/services/SdoService.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_link_libraries(service_cascade_test PRIVATE Qt6::Core Qt6::Network Qt6::Test ecat_core)
target_include_directories(service_cascade_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
set_target_properties(service_cascade_test PROPERTIES AUTOMOC ON)
add_test(NAME service_cascade_test COMMAND service_cascade_test)
set_tests_properties(service_cascade_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── WatchdogService ──────────────────────────────────────────────────
add_executable(watchdog_service_test
    watchdog_service_test.cpp
    ../../../apps/ecat-studio/services/WatchdogService.cpp
    ../../../apps/ecat-studio/services/WatchdogService.h
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)

target_include_directories(watchdog_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/src/core
)

target_link_libraries(watchdog_service_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::Test
    ecat_core
)

set_target_properties(watchdog_service_test PROPERTIES AUTOMOC ON)
add_test(NAME watchdog_service_test COMMAND watchdog_service_test)
set_tests_properties(watchdog_service_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── DiagnosticReportService ─────────────────────────────────────────
add_executable(diagnostic_report_service_test
    diagnostic_report_service_test.cpp
    ../../../apps/ecat-studio/services/DiagnosticReportService.cpp
    ../../../apps/ecat-studio/services/DiagnosticReportService.h
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
    ../../../apps/ecat-studio/services/TopologyService.cpp
    ../../../apps/ecat-studio/services/TopologyService.h
    ../../../apps/ecat-studio/services/DcSyncService.cpp
    ../../../apps/ecat-studio/services/DcSyncService.h
    ../../../apps/ecat-studio/services/PerformanceMonitorService.cpp
    ../../../apps/ecat-studio/services/PerformanceMonitorService.h
    ../../../apps/ecat-studio/services/WatchdogService.cpp
    ../../../apps/ecat-studio/services/WatchdogService.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)

target_include_directories(diagnostic_report_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/src/core
)

target_link_libraries(diagnostic_report_service_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::Test
    ecat_core
)

set_target_properties(diagnostic_report_service_test PROPERTIES AUTOMOC ON)
add_test(NAME diagnostic_report_service_test COMMAND diagnostic_report_service_test)
set_tests_properties(diagnostic_report_service_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── NetworkDiagnosticsService (unit) ────────────────────────────────
add_executable(network_diagnostics_service_test
    network_diagnostics_service_test.cpp
    ../../../apps/ecat-studio/services/NetworkDiagnosticsService.cpp
    ../../../apps/ecat-studio/services/NetworkDiagnosticsService.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(network_diagnostics_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(network_diagnostics_service_test PRIVATE
    Qt6::Core Qt6::Network Qt6::Test ecat_core
)
set_target_properties(network_diagnostics_service_test PROPERTIES AUTOMOC ON)
add_test(NAME network_diagnostics_service_test COMMAND network_diagnostics_service_test)

# ── EcatHealthService (unit) ────────────────────────────────────────
add_executable(ecat_health_service_test
    ecat_health_service_test.cpp
    ../../../apps/ecat-studio/services/EcatHealthService.cpp
    ../../../apps/ecat-studio/services/EcatHealthService.h
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
    ../../../apps/ecat-studio/services/TopologyService.cpp
    ../../../apps/ecat-studio/services/TopologyService.h
    ../../../apps/ecat-studio/services/DcSyncService.cpp
    ../../../apps/ecat-studio/services/DcSyncService.h
    ../../../apps/ecat-studio/services/AlEventService.cpp
    ../../../apps/ecat-studio/services/AlEventService.h
    ../../../apps/ecat-studio/services/WatchdogService.cpp
    ../../../apps/ecat-studio/services/WatchdogService.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(ecat_health_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(ecat_health_service_test PRIVATE
    Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core
)
set_target_properties(ecat_health_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ecat_health_service_test COMMAND ecat_health_service_test)
set_tests_properties(ecat_health_service_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── ExportService (unit) ────────────────────────────────────────────
add_executable(export_service_test
    export_service_test.cpp
    ../../../apps/ecat-studio/services/ExportService.cpp
    ../../../apps/ecat-studio/services/ExportService.h
)
target_include_directories(export_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(export_service_test PRIVATE
    Qt6::Core Qt6::Widgets Qt6::Test
)
set_target_properties(export_service_test PROPERTIES AUTOMOC ON)
add_test(NAME export_service_test COMMAND export_service_test)
set_tests_properties(export_service_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── DataPipelineService (unit) ──────────────────────────────────────
add_executable(data_pipeline_service_test
    data_pipeline_service_test.cpp
    ../../../apps/ecat-studio/services/DataPipelineService.cpp
    ../../../apps/ecat-studio/services/DataPipelineService.h
)
target_include_directories(data_pipeline_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(data_pipeline_service_test PRIVATE
    Qt6::Core Qt6::Test
)
set_target_properties(data_pipeline_service_test PROPERTIES AUTOMOC ON)
add_test(NAME data_pipeline_service_test COMMAND data_pipeline_service_test)

# ── DeviceManagerService (unit) ─────────────────────────────────────
add_executable(device_manager_service_test
    device_manager_service_test.cpp
    ../../../apps/ecat-studio/services/DeviceManagerService.cpp
    ../../../apps/ecat-studio/services/DeviceManagerService.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(device_manager_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(device_manager_service_test PRIVATE
    Qt6::Core Qt6::Network Qt6::Test ecat_core
)
set_target_properties(device_manager_service_test PROPERTIES AUTOMOC ON)
add_test(NAME device_manager_service_test COMMAND device_manager_service_test)

# ── FirmwareUpdateService (unit) ────────────────────────────────────
add_executable(firmware_update_service_test
    firmware_update_service_test.cpp
    ../../../apps/ecat-studio/services/FirmwareUpdateService.cpp
    ../../../apps/ecat-studio/services/FirmwareUpdateService.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(firmware_update_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(firmware_update_service_test PRIVATE
    Qt6::Core Qt6::Network Qt6::Test ecat_core
)
set_target_properties(firmware_update_service_test PROPERTIES AUTOMOC ON)
add_test(NAME firmware_update_service_test COMMAND firmware_update_service_test)

# ── ReportGeneratorService (unit) ───────────────────────────────────
add_executable(report_generator_service_test
    report_generator_service_test.cpp
    ../../../apps/ecat-studio/services/ReportGeneratorService.cpp
    ../../../apps/ecat-studio/services/ReportGeneratorService.h
)
target_include_directories(report_generator_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(report_generator_service_test PRIVATE
    Qt6::Core Qt6::Test
)
set_target_properties(report_generator_service_test PROPERTIES AUTOMOC ON)
add_test(NAME report_generator_service_test COMMAND report_generator_service_test)

# ── CoE/FoE/EoE Services (unit) ─────────────────────────────────────
add_executable(coe_foe_eoe_service_test
    coe_foe_eoe_service_test.cpp
    ../../../apps/ecat-studio/services/CoEService.cpp
    ../../../apps/ecat-studio/services/CoEService.h
    ../../../apps/ecat-studio/services/FoEService.cpp
    ../../../apps/ecat-studio/services/FoEService.h
    ../../../apps/ecat-studio/services/EoEService.cpp
    ../../../apps/ecat-studio/services/EoEService.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(coe_foe_eoe_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(coe_foe_eoe_service_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Test
    ecat_core
)
target_compile_definitions(coe_foe_eoe_service_test PRIVATE SOURCE_ROOT="${CMAKE_SOURCE_DIR}")
set_target_properties(coe_foe_eoe_service_test PROPERTIES AUTOMOC ON)
add_test(NAME coe_foe_eoe_service_test COMMAND coe_foe_eoe_service_test)

# ── MasterApiService (unit) ────────────────────────────────────────
add_executable(master_api_service_test
    master_api_service_test.cpp
    ../../../apps/ecat-studio/services/MasterApiService.cpp
    ../../../apps/ecat-studio/services/MasterApiService.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(master_api_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
target_compile_definitions(master_api_service_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)
target_link_libraries(master_api_service_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Test
    ecat_core
)
set_target_properties(master_api_service_test PROPERTIES AUTOMOC ON)
add_test(NAME master_api_service_test COMMAND master_api_service_test)

# ── DomainService (unit) ───────────────────────────────────────────
add_executable(domain_service_test
    domain_service_test.cpp
    ../../../apps/ecat-studio/services/DomainService.cpp
    ../../../apps/ecat-studio/services/DomainService.h
)
target_include_directories(domain_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(domain_service_test PRIVATE
    Qt6::Core
    Qt6::Test
)
set_target_properties(domain_service_test PROPERTIES AUTOMOC ON)
add_test(NAME domain_service_test COMMAND domain_service_test)

# ── SyncManagerService (unit) ──────────────────────────────────────
add_executable(sync_manager_service_test
    sync_manager_service_test.cpp
    ../../../apps/ecat-studio/services/SyncManagerService.cpp
    ../../../apps/ecat-studio/services/SyncManagerService.h
)
target_include_directories(sync_manager_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(sync_manager_service_test PRIVATE
    Qt6::Core
    Qt6::Test
)
set_target_properties(sync_manager_service_test PROPERTIES AUTOMOC ON)
add_test(NAME sync_manager_service_test COMMAND sync_manager_service_test)

# ── StateMachineService (unit) ──────────────────────────────────────
add_executable(state_machine_service_test
    state_machine_service_test.cpp
    ../../../apps/ecat-studio/services/StateMachineService.cpp
    ../../../apps/ecat-studio/services/StateMachineService.h
)
target_include_directories(state_machine_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(state_machine_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(state_machine_service_test PROPERTIES AUTOMOC ON)
add_test(NAME state_machine_service_test COMMAND state_machine_service_test)

# ── ErrorHandlingService (unit) ─────────────────────────────────────
add_executable(error_handling_service_test
    error_handling_service_test.cpp
    ../../../apps/ecat-studio/services/ErrorHandlingService.cpp
    ../../../apps/ecat-studio/services/ErrorHandlingService.h
)
target_include_directories(error_handling_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(error_handling_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(error_handling_service_test PROPERTIES AUTOMOC ON)
add_test(NAME error_handling_service_test COMMAND error_handling_service_test)

# ── DiagnosticService (unit) ────────────────────────────────────────
add_executable(diagnostic_service_test
    diagnostic_service_test.cpp
    ../../../apps/ecat-studio/services/DiagnosticService.cpp
    ../../../apps/ecat-studio/services/DiagnosticService.h
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(diagnostic_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(diagnostic_service_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Test
    ecat_core
)
set_target_properties(diagnostic_service_test PROPERTIES AUTOMOC ON)
add_test(NAME diagnostic_service_test COMMAND diagnostic_service_test)

# ── HotConnectService Test ──────────────────────────────────────────
add_executable(hot_connect_service_test
    hot_connect_service_test.cpp
    ../../../apps/ecat-studio/services/HotConnectService.cpp
    ../../../apps/ecat-studio/services/HotConnectService.h
)
target_include_directories(hot_connect_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(hot_connect_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(hot_connect_service_test PROPERTIES AUTOMOC ON)
add_test(NAME hot_connect_service_test COMMAND hot_connect_service_test)

# ── RedundancyService Test ──────────────────────────────────────────
add_executable(redundancy_service_test
    redundancy_service_test.cpp
    ../../../apps/ecat-studio/services/RedundancyService.cpp
    ../../../apps/ecat-studio/services/RedundancyService.h
)
target_include_directories(redundancy_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(redundancy_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(redundancy_service_test PROPERTIES AUTOMOC ON)
add_test(NAME redundancy_service_test COMMAND redundancy_service_test)

# ── CableDiagnosticsService Test ────────────────────────────────────
add_executable(cable_diagnostics_service_test
    cable_diagnostics_service_test.cpp
    ../../../apps/ecat-studio/services/CableDiagnosticsService.cpp
    ../../../apps/ecat-studio/services/CableDiagnosticsService.h
)
target_include_directories(cable_diagnostics_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(cable_diagnostics_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(cable_diagnostics_service_test PROPERTIES AUTOMOC ON)
add_test(NAME cable_diagnostics_service_test COMMAND cable_diagnostics_service_test)

# ── EtherCATMonitorService Test ─────────────────────────────────────
add_executable(ethercat_monitor_service_test
    ethercat_monitor_service_test.cpp
    ../../../apps/ecat-studio/services/EtherCATMonitorService.cpp
    ../../../apps/ecat-studio/services/EtherCATMonitorService.h
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(ethercat_monitor_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(ethercat_monitor_service_test PRIVATE
    Qt6::Core Qt6::Network Qt6::Test ecat_core
)
set_target_properties(ethercat_monitor_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_monitor_service_test COMMAND ethercat_monitor_service_test)

# ── EtherCATAnalyzerService Test ────────────────────────────────────
add_executable(ethercat_analyzer_service_test
    ethercat_analyzer_service_test.cpp
    ../../../apps/ecat-studio/services/EtherCATAnalyzerService.cpp
    ../../../apps/ecat-studio/services/EtherCATAnalyzerService.h
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(ethercat_analyzer_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(ethercat_analyzer_service_test PRIVATE
    Qt6::Core Qt6::Network Qt6::Test ecat_core
)
set_target_properties(ethercat_analyzer_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_analyzer_service_test COMMAND ethercat_analyzer_service_test)

# ── EtherCATOptimizerService Test ───────────────────────────────────
add_executable(ethercat_optimizer_service_test
    ethercat_optimizer_service_test.cpp
    ../../../apps/ecat-studio/services/EtherCATOptimizerService.cpp
    ../../../apps/ecat-studio/services/EtherCATOptimizerService.h
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(ethercat_optimizer_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
target_compile_definitions(ethercat_optimizer_service_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)
target_link_libraries(ethercat_optimizer_service_test PRIVATE
    Qt6::Core Qt6::Network Qt6::Test ecat_core
)
set_target_properties(ethercat_optimizer_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_optimizer_service_test COMMAND ethercat_optimizer_service_test)

# ── EtherCATConfigService (unit) ────────────────────────────────────
add_executable(ethercat_config_service_test
    ethercat_config_service_test.cpp
    ../../../apps/ecat-studio/services/EtherCATConfigService.cpp
    ../../../apps/ecat-studio/services/EtherCATConfigService.h
)
target_link_libraries(ethercat_config_service_test PRIVATE Qt6::Core Qt6::Test)
target_include_directories(ethercat_config_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
set_target_properties(ethercat_config_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_config_service_test COMMAND ethercat_config_service_test)

# ── EtherCATBackupService (unit) ────────────────────────────────────
add_executable(ethercat_backup_service_test
    ethercat_backup_service_test.cpp
    ../../../apps/ecat-studio/services/EtherCATBackupService.cpp
    ../../../apps/ecat-studio/services/EtherCATBackupService.h
)
target_link_libraries(ethercat_backup_service_test PRIVATE Qt6::Core Qt6::Test)
target_include_directories(ethercat_backup_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_compile_definitions(ethercat_backup_service_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)
set_target_properties(ethercat_backup_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_backup_service_test COMMAND ethercat_backup_service_test)

# ── EtherCATRecoveryService (unit) ──────────────────────────────────
add_executable(ethercat_recovery_service_test
    ethercat_recovery_service_test.cpp
    ../../../apps/ecat-studio/services/EtherCATRecoveryService.cpp
    ../../../apps/ecat-studio/services/EtherCATRecoveryService.h
)
target_link_libraries(ethercat_recovery_service_test PRIVATE Qt6::Core Qt6::Test)
target_include_directories(ethercat_recovery_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
set_target_properties(ethercat_recovery_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_recovery_service_test COMMAND ethercat_recovery_service_test)

# ── EtherCATSimulationService Test ─────────────────────────────────
add_executable(ethercat_simulation_service_test
    ethercat_simulation_service_test.cpp
    ../../../apps/ecat-studio/services/EtherCATSimulationService.cpp
    ../../../apps/ecat-studio/services/EtherCATSimulationService.h
)
target_link_libraries(ethercat_simulation_service_test PRIVATE Qt6::Core Qt6::Test)
target_include_directories(ethercat_simulation_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
set_target_properties(ethercat_simulation_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_simulation_service_test COMMAND ethercat_simulation_service_test)

# ── EtherCATTestingService Test ────────────────────────────────────
add_executable(ethercat_testing_service_test
    ethercat_testing_service_test.cpp
    ../../../apps/ecat-studio/services/EtherCATTestingService.cpp
    ../../../apps/ecat-studio/services/EtherCATTestingService.h
)
target_link_libraries(ethercat_testing_service_test PRIVATE Qt6::Core Qt6::Test)
target_include_directories(ethercat_testing_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
set_target_properties(ethercat_testing_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_testing_service_test COMMAND ethercat_testing_service_test)

# ── EtherCATValidationService Test ─────────────────────────────────
add_executable(ethercat_validation_service_test
    ethercat_validation_service_test.cpp
    ../../../apps/ecat-studio/services/EtherCATValidationService.cpp
    ../../../apps/ecat-studio/services/EtherCATValidationService.h
)
target_link_libraries(ethercat_validation_service_test PRIVATE Qt6::Core Qt6::Test)
target_include_directories(ethercat_validation_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_compile_definitions(ethercat_validation_service_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)
set_target_properties(ethercat_validation_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_validation_service_test COMMAND ethercat_validation_service_test)

# ── EtherCATSecurityService Test ────────────────────────────────────
add_executable(ethercat_security_service_test
    ethercat_security_service_test.cpp
    ../../../apps/ecat-studio/services/EtherCATSecurityService.cpp
    ../../../apps/ecat-studio/services/EtherCATSecurityService.h
)
target_link_libraries(ethercat_security_service_test PRIVATE Qt6::Core Qt6::Test)
target_include_directories(ethercat_security_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
set_target_properties(ethercat_security_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_security_service_test COMMAND ethercat_security_service_test)

# ── EtherCATComplianceService Test ──────────────────────────────────
add_executable(ethercat_compliance_service_test
    ethercat_compliance_service_test.cpp
    ../../../apps/ecat-studio/services/EtherCATComplianceService.cpp
    ../../../apps/ecat-studio/services/EtherCATComplianceService.h
)
target_link_libraries(ethercat_compliance_service_test PRIVATE Qt6::Core Qt6::Test)
target_include_directories(ethercat_compliance_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_compile_definitions(ethercat_compliance_service_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)
set_target_properties(ethercat_compliance_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_compliance_service_test COMMAND ethercat_compliance_service_test)

# ── EtherCATCertificationService Test ───────────────────────────────
add_executable(ethercat_certification_service_test
    ethercat_certification_service_test.cpp
    ../../../apps/ecat-studio/services/EtherCATCertificationService.cpp
    ../../../apps/ecat-studio/services/EtherCATCertificationService.h
)
target_link_libraries(ethercat_certification_service_test PRIVATE Qt6::Core Qt6::Test)
target_include_directories(ethercat_certification_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_compile_definitions(ethercat_certification_service_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)
set_target_properties(ethercat_certification_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_certification_service_test COMMAND ethercat_certification_service_test)

# ── EtherCATAnalyticsService Test ─────────────────────────────────
add_executable(ethercat_analytics_service_test
    ethercat_analytics_service_test.cpp
    ../../../apps/ecat-studio/services/EtherCATAnalyticsService.cpp
    ../../../apps/ecat-studio/services/EtherCATAnalyticsService.h
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(ethercat_analytics_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(ethercat_analytics_service_test PRIVATE
    Qt6::Core Qt6::Network Qt6::Test ecat_core
)
set_target_properties(ethercat_analytics_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_analytics_service_test COMMAND ethercat_analytics_service_test)

# ── EtherCATDeploymentService ─────────────────────────────────────────
add_executable(ethercat_deployment_service_test
    ethercat_deployment_service_test.cpp
    ../../../apps/ecat-studio/services/EtherCATDeploymentService.cpp
    ../../../apps/ecat-studio/services/EtherCATDeploymentService.h
)
target_include_directories(ethercat_deployment_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(ethercat_deployment_service_test PRIVATE Qt6::Core Qt6::Test)
target_compile_definitions(ethercat_deployment_service_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)
set_target_properties(ethercat_deployment_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_deployment_service_test COMMAND ethercat_deployment_service_test)

# ── EtherCATUpdateService ─────────────────────────────────────────────
add_executable(ethercat_update_service_test
    ethercat_update_service_test.cpp
    ../../../apps/ecat-studio/services/EtherCATUpdateService.cpp
    ../../../apps/ecat-studio/services/EtherCATUpdateService.h
)
target_include_directories(ethercat_update_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(ethercat_update_service_test PRIVATE Qt6::Core Qt6::Test)
target_compile_definitions(ethercat_update_service_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)
set_target_properties(ethercat_update_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_update_service_test COMMAND ethercat_update_service_test)

# ── EtherCATMaintenanceService ────────────────────────────────────────
add_executable(ethercat_maintenance_service_test
    ethercat_maintenance_service_test.cpp
    ../../../apps/ecat-studio/services/EtherCATMaintenanceService.cpp
    ../../../apps/ecat-studio/services/EtherCATMaintenanceService.h
)
target_include_directories(ethercat_maintenance_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(ethercat_maintenance_service_test PRIVATE Qt6::Core Qt6::Test)
target_compile_definitions(ethercat_maintenance_service_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)
set_target_properties(ethercat_maintenance_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_maintenance_service_test COMMAND ethercat_maintenance_service_test)

# ── EtherCAT Advanced Services Test ─────────────────────────────────
add_executable(test_advanced_services_test
    test_advanced_services_test.cpp
    ../../../apps/ecat-studio/services/EtherCATIntegrationService.cpp
    ../../../apps/ecat-studio/services/EtherCATIntegrationService.h
    ../../../apps/ecat-studio/services/EtherCATSyncService.cpp
    ../../../apps/ecat-studio/services/EtherCATSyncService.h
    ../../../apps/ecat-studio/services/EtherCATReplicationService.cpp
    ../../../apps/ecat-studio/services/EtherCATReplicationService.h
    ../../../apps/ecat-studio/services/EtherCATVisualizationService.cpp
    ../../../apps/ecat-studio/services/EtherCATVisualizationService.h
    ../../../apps/ecat-studio/services/EtherCATReportingService.cpp
    ../../../apps/ecat-studio/services/EtherCATReportingService.h
    ../../../apps/ecat-studio/services/EtherCATDocumentationService.cpp
    ../../../apps/ecat-studio/services/EtherCATDocumentationService.h
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(test_advanced_services_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(test_advanced_services_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
set_target_properties(test_advanced_services_test PROPERTIES AUTOMOC ON)
add_test(NAME test_advanced_services_test COMMAND test_advanced_services_test)
set_tests_properties(test_advanced_services_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── EtherCATIntegrationService ──────────────────────────────────────
add_executable(ethercat_integration_service_test
    ethercat_integration_service_test.cpp
    ../../../apps/ecat-studio/services/EtherCATIntegrationService.cpp
    ../../../apps/ecat-studio/services/EtherCATIntegrationService.h
)
target_include_directories(ethercat_integration_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(ethercat_integration_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_integration_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_integration_service_test COMMAND ethercat_integration_service_test)

# ── EtherCATVisualizationService ────────────────────────────────────
add_executable(ethercat_visualization_service_test
    ethercat_visualization_service_test.cpp
    ../../../apps/ecat-studio/services/EtherCATVisualizationService.cpp
    ../../../apps/ecat-studio/services/EtherCATVisualizationService.h
    ../../../apps/ecat-studio/services/EtherCATAnalyticsService.cpp
    ../../../apps/ecat-studio/services/EtherCATAnalyticsService.h
    ../../../apps/ecat-studio/services/EtherCATMonitorService.cpp
    ../../../apps/ecat-studio/services/EtherCATMonitorService.h
    ../../../apps/ecat-studio/services/NetworkDiagnosticsService.cpp
    ../../../apps/ecat-studio/services/NetworkDiagnosticsService.h
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(ethercat_visualization_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(ethercat_visualization_service_test PRIVATE
    Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core
)
set_target_properties(ethercat_visualization_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_visualization_service_test COMMAND ethercat_visualization_service_test)
set_tests_properties(ethercat_visualization_service_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── EtherCATReportingService ────────────────────────────────────────
add_executable(ethercat_reporting_service_test
    ethercat_reporting_service_test.cpp
    ../../../apps/ecat-studio/services/EtherCATReportingService.cpp
    ../../../apps/ecat-studio/services/EtherCATReportingService.h
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(ethercat_reporting_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(ethercat_reporting_service_test PRIVATE
    Qt6::Core Qt6::Network Qt6::Test ecat_core
)
set_target_properties(ethercat_reporting_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_reporting_service_test COMMAND ethercat_reporting_service_test)

# ── EtherCATDocumentationService ────────────────────────────────────
add_executable(ethercat_documentation_service_test
    ethercat_documentation_service_test.cpp
    ../../../apps/ecat-studio/services/EtherCATDocumentationService.cpp
    ../../../apps/ecat-studio/services/EtherCATDocumentationService.h
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(ethercat_documentation_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(ethercat_documentation_service_test PRIVATE
    Qt6::Core Qt6::Network Qt6::Test ecat_core
)
set_target_properties(ethercat_documentation_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_documentation_service_test COMMAND ethercat_documentation_service_test)

# ── WorkflowOptimizationService (unit) ──────────────────────────────
add_executable(workflow_optimization_service_test
    workflow_optimization_service_test.cpp
    ../../../apps/ecat-studio/services/WorkflowOptimizationService.cpp
    ../../../apps/ecat-studio/services/WorkflowOptimizationService.h
)
target_include_directories(workflow_optimization_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_optimization_service_test PRIVATE
    Qt6::Core
    Qt6::Test
)
set_target_properties(workflow_optimization_service_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_optimization_service_test COMMAND workflow_optimization_service_test)
set_tests_properties(workflow_optimization_service_test PROPERTIES
    TIMEOUT 30
    LABELS "unit;optimization"
)

add_executable(project_management_service_test
    project_management_service_test.cpp
    ../../../apps/ecat-studio/services/ProjectManagementService.cpp
    ../../../apps/ecat-studio/services/ProjectManagementService.h
)
target_include_directories(project_management_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(project_management_service_test PRIVATE
    Qt6::Core
    Qt6::Test
)
set_target_properties(project_management_service_test PROPERTIES AUTOMOC ON)
add_test(NAME project_management_service_test COMMAND project_management_service_test)
set_tests_properties(project_management_service_test PROPERTIES
    TIMEOUT 30
    LABELS "unit;project"
)

add_executable(task_management_service_test
    task_management_service_test.cpp
    ../../../apps/ecat-studio/services/TaskManagementService.cpp
    ../../../apps/ecat-studio/services/TaskManagementService.h
)
target_include_directories(task_management_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(task_management_service_test PRIVATE
    Qt6::Core
    Qt6::Test
)
set_target_properties(task_management_service_test PROPERTIES AUTOMOC ON)
add_test(NAME task_management_service_test COMMAND task_management_service_test)
set_tests_properties(task_management_service_test PROPERTIES
    TIMEOUT 30
    LABELS "unit;task"
)

add_executable(resource_management_service_test
    resource_management_service_test.cpp
    ../../../apps/ecat-studio/services/ResourceManagementService.cpp
    ../../../apps/ecat-studio/services/ResourceManagementService.h
)
target_include_directories(resource_management_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(resource_management_service_test PRIVATE
    Qt6::Core
    Qt6::Test
)
set_target_properties(resource_management_service_test PROPERTIES AUTOMOC ON)
add_test(NAME resource_management_service_test COMMAND resource_management_service_test)
set_tests_properties(resource_management_service_test PROPERTIES
    TIMEOUT 30
    LABELS "unit;resource"
)

add_executable(project_planning_service_test
    project_planning_service_test.cpp
    ../../../apps/ecat-studio/services/ProjectPlanningService.cpp
    ../../../apps/ecat-studio/services/ProjectPlanningService.h
)
target_include_directories(project_planning_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(project_planning_service_test PRIVATE
    Qt6::Core
    Qt6::Test
)
set_target_properties(project_planning_service_test PROPERTIES AUTOMOC ON)
add_test(NAME project_planning_service_test COMMAND project_planning_service_test)
set_tests_properties(project_planning_service_test PROPERTIES
    TIMEOUT 30
    LABELS "unit;project"
)

add_executable(project_tracking_service_test
    project_tracking_service_test.cpp
    ../../../apps/ecat-studio/services/ProjectTrackingService.cpp
    ../../../apps/ecat-studio/services/ProjectTrackingService.h
)
target_include_directories(project_tracking_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(project_tracking_service_test PRIVATE
    Qt6::Core
    Qt6::Test
)
set_target_properties(project_tracking_service_test PROPERTIES AUTOMOC ON)
add_test(NAME project_tracking_service_test COMMAND project_tracking_service_test)
set_tests_properties(project_tracking_service_test PROPERTIES
    TIMEOUT 30
    LABELS "unit;project"
)

add_executable(project_reporting_service_test
    project_reporting_service_test.cpp
    ../../../apps/ecat-studio/services/ProjectReportingService.cpp
    ../../../apps/ecat-studio/services/ProjectReportingService.h
)
target_include_directories(project_reporting_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(project_reporting_service_test PRIVATE
    Qt6::Core
    Qt6::Test
)
set_target_properties(project_reporting_service_test PROPERTIES AUTOMOC ON)
add_test(NAME project_reporting_service_test COMMAND project_reporting_service_test)
set_tests_properties(project_reporting_service_test PROPERTIES
    TIMEOUT 30
    LABELS "unit;project"
)

# ── Workflow Automation Service ─────────────────────────────────────
add_executable(workflow_automation_service_test
    workflow_automation_service_test.cpp
    ../../../apps/ecat-studio/services/WorkflowAutomationService.cpp
    ../../../apps/ecat-studio/services/WorkflowAutomationService.h
)
target_include_directories(workflow_automation_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_automation_service_test PRIVATE
    Qt6::Core
    Qt6::Test
)
set_target_properties(workflow_automation_service_test PROPERTIES AUTOMOC ON)
target_compile_definitions(workflow_automation_service_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)
add_test(NAME workflow_automation_service_test COMMAND workflow_automation_service_test)
set_tests_properties(workflow_automation_service_test PROPERTIES
    TIMEOUT 30
    LABELS "unit;automation"
)

# ── Workflow Scheduling Service ─────────────────────────────────────
add_executable(workflow_scheduling_service_test
    workflow_scheduling_service_test.cpp
    ../../../apps/ecat-studio/services/WorkflowSchedulingService.cpp
    ../../../apps/ecat-studio/services/WorkflowSchedulingService.h
)
target_include_directories(workflow_scheduling_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_scheduling_service_test PRIVATE
    Qt6::Core
    Qt6::Test
)
set_target_properties(workflow_scheduling_service_test PROPERTIES AUTOMOC ON)
target_compile_definitions(workflow_scheduling_service_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)
add_test(NAME workflow_scheduling_service_test COMMAND workflow_scheduling_service_test)
set_tests_properties(workflow_scheduling_service_test PROPERTIES
    TIMEOUT 30
    LABELS "unit;scheduling"
)

# ── Workflow Monitoring Service ─────────────────────────────────────
add_executable(workflow_monitoring_service_test
    workflow_monitoring_service_test.cpp
    ../../../apps/ecat-studio/services/WorkflowMonitoringService.cpp
    ../../../apps/ecat-studio/services/WorkflowMonitoringService.h
)
target_include_directories(workflow_monitoring_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_monitoring_service_test PRIVATE
    Qt6::Core
    Qt6::Test
)
set_target_properties(workflow_monitoring_service_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_monitoring_service_test COMMAND workflow_monitoring_service_test)
set_tests_properties(workflow_monitoring_service_test PROPERTIES
    TIMEOUT 30
    LABELS "unit;monitoring"
)

# ── WorkflowAnalyticsService (unit) ─────────────────────────────────
add_executable(workflow_analytics_service_test
    workflow_analytics_service_test.cpp
    ../../../apps/ecat-studio/services/WorkflowAnalyticsService.cpp
    ../../../apps/ecat-studio/services/WorkflowAnalyticsService.h
)
target_include_directories(workflow_analytics_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_analytics_service_test PRIVATE
    Qt6::Core Qt6::Test
)
set_target_properties(workflow_analytics_service_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_analytics_service_test COMMAND workflow_analytics_service_test)
set_tests_properties(workflow_analytics_service_test PROPERTIES
    TIMEOUT 30
    LABELS "unit;analytics"
)

# ── WorkflowVisualizationService Test ─────────────────────────────
add_executable(workflow_visualization_service_test
    workflow_visualization_service_test.cpp
    ../../../apps/ecat-studio/services/WorkflowVisualizationService.cpp
    ../../../apps/ecat-studio/services/WorkflowVisualizationService.h
)
target_include_directories(workflow_visualization_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_visualization_service_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
set_target_properties(workflow_visualization_service_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_visualization_service_test COMMAND workflow_visualization_service_test)

# ── WorkflowReportingService Test ─────────────────────────────────
add_executable(workflow_reporting_service_test
    workflow_reporting_service_test.cpp
    ../../../apps/ecat-studio/services/WorkflowReportingService.cpp
    ../../../apps/ecat-studio/services/WorkflowReportingService.h
)
target_include_directories(workflow_reporting_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_reporting_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_reporting_service_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_reporting_service_test COMMAND workflow_reporting_service_test)

# ── WorkflowIntegrationService Test ───────────────────────────────
add_executable(workflow_integration_service_test
    workflow_integration_service_test.cpp
    ../../../apps/ecat-studio/services/WorkflowIntegrationService.cpp
    ../../../apps/ecat-studio/services/WorkflowIntegrationService.h
)
target_include_directories(workflow_integration_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_integration_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_integration_service_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_integration_service_test COMMAND workflow_integration_service_test)

# ── WorkflowSecurityService ────────────────────────────────────────────
add_executable(workflow_security_service_test
    workflow_security_service_test.cpp
    ../../../apps/ecat-studio/services/WorkflowSecurityService.cpp
    ../../../apps/ecat-studio/services/WorkflowSecurityService.h
)
target_include_directories(workflow_security_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_security_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_security_service_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_security_service_test COMMAND workflow_security_service_test)
set_tests_properties(workflow_security_service_test PROPERTIES
    TIMEOUT 30
    LABELS "unit;security"
)

# ── Experimental Workflow Enterprise/Cloud/AI/Digital-Twin Tests ───────
if(ECAT_EXPERIMENTAL_SERVICES)

# ── WorkflowComplianceService ──────────────────────────────────────────
add_executable(workflow_compliance_service_test
    workflow_compliance_service_test.cpp
    ../../../apps/ecat-studio/services/WorkflowComplianceService.cpp
    ../../../apps/ecat-studio/services/WorkflowComplianceService.h
)
target_include_directories(workflow_compliance_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_compliance_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_compliance_service_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_compliance_service_test COMMAND workflow_compliance_service_test)
set_tests_properties(workflow_compliance_service_test PROPERTIES
    TIMEOUT 30
    LABELS "unit;compliance"
)

# ── WorkflowCertificationService ───────────────────────────────────────
add_executable(workflow_certification_service_test
    workflow_certification_service_test.cpp
    ../../../apps/ecat-studio/services/WorkflowCertificationService.cpp
    ../../../apps/ecat-studio/services/WorkflowCertificationService.h
)
target_include_directories(workflow_certification_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_certification_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_certification_service_test PROPERTIES AUTOMOC ON)
target_compile_definitions(workflow_certification_service_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)
add_test(NAME workflow_certification_service_test COMMAND workflow_certification_service_test)
set_tests_properties(workflow_certification_service_test PROPERTIES
    TIMEOUT 30
    LABELS "unit;certification"
)

# ── WorkflowSecurityManagerService ───────────────────────────────────
add_executable(workflow_security_manager_service_test
    workflow_security_manager_service_test.cpp
    ../../../apps/ecat-studio/services/WorkflowSecurityManagerService.cpp
    ../../../apps/ecat-studio/services/WorkflowSecurityManagerService.h
)
target_include_directories(workflow_security_manager_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_security_manager_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_security_manager_service_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_security_manager_service_test COMMAND workflow_security_manager_service_test)
set_tests_properties(workflow_security_manager_service_test PROPERTIES
    TIMEOUT 30
    LABELS "unit;security"
)

# ── WorkflowComplianceManagerService ─────────────────────────────────
add_executable(workflow_compliance_manager_service_test
    workflow_compliance_manager_service_test.cpp
    ../../../apps/ecat-studio/services/WorkflowComplianceManagerService.cpp
    ../../../apps/ecat-studio/services/WorkflowComplianceManagerService.h
)
target_include_directories(workflow_compliance_manager_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_compliance_manager_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_compliance_manager_service_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_compliance_manager_service_test COMMAND workflow_compliance_manager_service_test)
set_tests_properties(workflow_compliance_manager_service_test PROPERTIES
    TIMEOUT 30
    LABELS "unit;compliance"
)

# ── WorkflowCertificationManagerService ──────────────────────────────
add_executable(workflow_certification_manager_service_test
    workflow_certification_manager_service_test.cpp
    ../../../apps/ecat-studio/services/WorkflowCertificationManagerService.cpp
    ../../../apps/ecat-studio/services/WorkflowCertificationManagerService.h
)
target_include_directories(workflow_certification_manager_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_certification_manager_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_certification_manager_service_test PROPERTIES AUTOMOC ON)
target_compile_definitions(workflow_certification_manager_service_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)
add_test(NAME workflow_certification_manager_service_test COMMAND workflow_certification_manager_service_test)
set_tests_properties(workflow_certification_manager_service_test PROPERTIES
    TIMEOUT 30
    LABELS "unit;certification"
)

# ── Workflow Deployment Service ─────────────────────────────────────
add_executable(workflow_deployment_service_test
    workflow_deployment_service_test.cpp
    ../../../apps/ecat-studio/services/WorkflowDeploymentService.cpp
    ../../../apps/ecat-studio/services/WorkflowDeploymentService.h
)
target_include_directories(workflow_deployment_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_deployment_service_test PRIVATE
    Qt6::Core
    Qt6::Test
)
set_target_properties(workflow_deployment_service_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_deployment_service_test COMMAND workflow_deployment_service_test)
set_tests_properties(workflow_deployment_service_test PROPERTIES
    TIMEOUT 30
    LABELS "unit;deployment"
)

# ── Workflow Update Service ─────────────────────────────────────────
add_executable(workflow_update_service_test
    workflow_update_service_test.cpp
    ../../../apps/ecat-studio/services/WorkflowUpdateService.cpp
    ../../../apps/ecat-studio/services/WorkflowUpdateService.h
)
target_include_directories(workflow_update_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_update_service_test PRIVATE
    Qt6::Core
    Qt6::Test
)
set_target_properties(workflow_update_service_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_update_service_test COMMAND workflow_update_service_test)
set_tests_properties(workflow_update_service_test PROPERTIES
    TIMEOUT 30
    LABELS "unit;update"
)

# ── Workflow Maintenance Service ────────────────────────────────────
add_executable(workflow_maintenance_service_test
    workflow_maintenance_service_test.cpp
    ../../../apps/ecat-studio/services/WorkflowMaintenanceService.cpp
    ../../../apps/ecat-studio/services/WorkflowMaintenanceService.h
)
target_include_directories(workflow_maintenance_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_maintenance_service_test PRIVATE
    Qt6::Core
    Qt6::Test
)
set_target_properties(workflow_maintenance_service_test PROPERTIES AUTOMOC ON)
target_compile_definitions(workflow_maintenance_service_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)
add_test(NAME workflow_maintenance_service_test COMMAND workflow_maintenance_service_test)
set_tests_properties(workflow_maintenance_service_test PROPERTIES
    TIMEOUT 30
    LABELS "unit;maintenance"
)

# ── WorkflowIntegrationHubService Test ─────────────────────────────
add_executable(workflow_integration_hub_service_test
    workflow_integration_hub_service_test.cpp
    ../../../apps/ecat-studio/services/WorkflowIntegrationHubService.cpp
    ../../../apps/ecat-studio/services/WorkflowIntegrationHubService.h
)
target_include_directories(workflow_integration_hub_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_integration_hub_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_integration_hub_service_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_integration_hub_service_test COMMAND workflow_integration_hub_service_test)

# ── Workflow Optimization Services Tests ───────────────────────────────
add_executable(workflow_visualization_studio_service_test
    workflow_visualization_studio_service_test.cpp
    ../../../apps/ecat-studio/services/WorkflowVisualizationStudioService.cpp
    ../../../apps/ecat-studio/services/WorkflowVisualizationStudioService.h
)
target_include_directories(workflow_visualization_studio_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_visualization_studio_service_test PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)
set_target_properties(workflow_visualization_studio_service_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_visualization_studio_service_test COMMAND workflow_visualization_studio_service_test)
set_tests_properties(workflow_visualization_studio_service_test PROPERTIES
    TIMEOUT 30
    LABELS "unit;workflow"
)

add_executable(workflow_report_designer_service_test
    workflow_report_designer_service_test.cpp
    ../../../apps/ecat-studio/services/WorkflowReportDesignerService.cpp
    ../../../apps/ecat-studio/services/WorkflowReportDesignerService.h
)
target_include_directories(workflow_report_designer_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_report_designer_service_test PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)
target_compile_definitions(workflow_report_designer_service_test PRIVATE SOURCE_ROOT="${CMAKE_SOURCE_DIR}")
set_target_properties(workflow_report_designer_service_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_report_designer_service_test COMMAND workflow_report_designer_service_test)
set_tests_properties(workflow_report_designer_service_test PROPERTIES
    TIMEOUT 30
    LABELS "unit;workflow"
)

add_executable(workflow_documentation_browser_service_test
    workflow_documentation_browser_service_test.cpp
    ../../../apps/ecat-studio/services/WorkflowDocumentationBrowserService.cpp
    ../../../apps/ecat-studio/services/WorkflowDocumentationBrowserService.h
    ../../../apps/ecat-studio/services/WorkflowSecurityManagerService.cpp
    ../../../apps/ecat-studio/services/WorkflowSecurityManagerService.h
    ../../../apps/ecat-studio/services/WorkflowComplianceManagerService.cpp
    ../../../apps/ecat-studio/services/WorkflowComplianceManagerService.h
    ../../../apps/ecat-studio/services/WorkflowCertificationManagerService.cpp
    ../../../apps/ecat-studio/services/WorkflowCertificationManagerService.h
)
target_include_directories(workflow_documentation_browser_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_documentation_browser_service_test PRIVATE
    Qt6::Core
    Qt6::Test
)
set_target_properties(workflow_documentation_browser_service_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_documentation_browser_service_test COMMAND workflow_documentation_browser_service_test)
set_tests_properties(workflow_documentation_browser_service_test PROPERTIES
    TIMEOUT 30
    LABELS "unit;workflow"
)

# ── FreeRunConfigurationService Test ─────────────────────────────────
add_executable(free_run_configuration_service_test
    free_run_configuration_service_test.cpp
    ../../../apps/ecat-studio/services/FreeRunConfigurationService.cpp
    ../../../apps/ecat-studio/services/FreeRunConfigurationService.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(free_run_configuration_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(free_run_configuration_service_test PRIVATE
    Qt6::Core Qt6::Network Qt6::Test ecat_core
)
set_target_properties(free_run_configuration_service_test PROPERTIES AUTOMOC ON)
target_compile_definitions(free_run_configuration_service_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)
add_test(NAME free_run_configuration_service_test COMMAND free_run_configuration_service_test)

# ── FreeRunMonitoringService Test ─────────────────────────────────────
add_executable(free_run_monitoring_service_test
    free_run_monitoring_service_test.cpp
    ../../../apps/ecat-studio/services/FreeRunMonitoringService.cpp
    ../../../apps/ecat-studio/services/FreeRunMonitoringService.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(free_run_monitoring_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(free_run_monitoring_service_test PRIVATE
    Qt6::Core Qt6::Network Qt6::Test ecat_core
)
set_target_properties(free_run_monitoring_service_test PROPERTIES AUTOMOC ON)
add_test(NAME free_run_monitoring_service_test COMMAND free_run_monitoring_service_test)

# ── PdoConfigurationService Test ────────────────────────────────────
add_executable(pdo_configuration_service_test
    pdo_configuration_service_test.cpp
    ../../../apps/ecat-studio/services/PdoConfigurationService.cpp
    ../../../apps/ecat-studio/services/PdoConfigurationService.h
)
target_include_directories(pdo_configuration_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(pdo_configuration_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(pdo_configuration_service_test PROPERTIES AUTOMOC ON)
add_test(NAME pdo_configuration_service_test COMMAND pdo_configuration_service_test)

# ── PdoMappingService Test ──────────────────────────────────────────
add_executable(pdo_mapping_service_test
    pdo_mapping_service_test.cpp
    ../../../apps/ecat-studio/services/PdoMappingService.cpp
    ../../../apps/ecat-studio/services/PdoMappingService.h
)
target_include_directories(pdo_mapping_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(pdo_mapping_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(pdo_mapping_service_test PROPERTIES AUTOMOC ON)
add_test(NAME pdo_mapping_service_test COMMAND pdo_mapping_service_test)

# ── OpStateService Test ─────────────────────────────────────────────
add_executable(op_state_service_test
    op_state_service_test.cpp
    ../../../apps/ecat-studio/services/OpStateService.cpp
    ../../../apps/ecat-studio/services/OpStateService.h
)
target_include_directories(op_state_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(op_state_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(op_state_service_test PROPERTIES AUTOMOC ON)
add_test(NAME op_state_service_test COMMAND op_state_service_test)

# ── PerformanceMonitorService Test ──────────────────────────────────
add_executable(performance_monitor_service_test
    performance_monitor_service_test.cpp
    ../../../apps/ecat-studio/services/PerformanceMonitorService.cpp
    ../../../apps/ecat-studio/services/PerformanceMonitorService.h
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(performance_monitor_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
)
target_link_libraries(performance_monitor_service_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
set_target_properties(performance_monitor_service_test PROPERTIES AUTOMOC ON)
add_test(NAME performance_monitor_service_test COMMAND performance_monitor_service_test)
set_tests_properties(performance_monitor_service_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)
