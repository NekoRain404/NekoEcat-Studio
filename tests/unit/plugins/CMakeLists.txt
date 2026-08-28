# Unit Tests - plugins
# Auto-generated from monolithic tests/CMakeLists.txt

add_executable(workspace_widgets_test
    workspace_widgets_test.cpp
)

target_include_directories(workspace_widgets_test PRIVATE
    ../../../apps/ecat-studio
    ../../../apps/ecat-studio/workspaces
)
target_link_libraries(workspace_widgets_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)

set_target_properties(workspace_widgets_test PROPERTIES AUTOMOC ON)

add_test(NAME workspace_widgets_test COMMAND workspace_widgets_test)
set_tests_properties(workspace_widgets_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── Workspace plugin interface ──────────────────────────────────────────
add_executable(workspace_plugin_interface_test
    workspace_plugin_interface_test.cpp
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)

target_include_directories(workspace_plugin_interface_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(workspace_plugin_interface_test PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

set_target_properties(workspace_plugin_interface_test PROPERTIES AUTOMOC ON)

add_test(NAME workspace_plugin_interface_test
    COMMAND workspace_plugin_interface_test
)
set_tests_properties(workspace_plugin_interface_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── PluginRegistry ─────────────────────────────────────────────────────
add_executable(plugin_registry_test
    plugin_registry_test.cpp
    ../../../apps/ecat-studio/plugins/PluginRegistry.cpp
    ../../../apps/ecat-studio/plugins/PluginRegistry.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)

target_include_directories(plugin_registry_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(plugin_registry_test PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

set_target_properties(plugin_registry_test PROPERTIES AUTOMOC ON)

add_test(NAME plugin_registry_test
    COMMAND plugin_registry_test
)
set_tests_properties(plugin_registry_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── NotesPlugin ─────────────────────────────────────────────────────
add_executable(notes_plugin_test
    notes_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/notes/NotesPlugin.cpp
    ../../../apps/ecat-studio/plugins/notes/NotesPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(notes_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(notes_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(notes_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME notes_plugin_test COMMAND notes_plugin_test)
set_tests_properties(notes_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── ExportPlugin (unit) ───────────────────────────────────────────────
add_executable(export_plugin_test
    export_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/export/ExportPlugin.cpp
    ../../../apps/ecat-studio/plugins/export/ExportPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    TestServiceContainer.cpp
    ../../../apps/ecat-studio/services/ServiceContainer.h
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
    ../../../apps/ecat-studio/utils/TableHelpers.cpp
    ../../../apps/ecat-studio/utils/TableHelpers.h
    ../../../apps/ecat-studio/utils/TextHelpers.cpp
    ../../../apps/ecat-studio/utils/TextHelpers.h
)
target_include_directories(export_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(export_plugin_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::Test
    ecat_core
)
set_target_properties(export_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME export_plugin_test COMMAND export_plugin_test)
set_tests_properties(export_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── RtTestPlugin (unit) ───────────────────────────────────────────────
add_executable(rttest_plugin_test
    rttest_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/rttest/RtTestPlugin.cpp
    ../../../apps/ecat-studio/plugins/rttest/RtTestPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    TestServiceContainer.cpp
    ../../../apps/ecat-studio/services/ServiceContainer.h
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(rttest_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(rttest_plugin_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::Test
    ecat_core
)
set_target_properties(rttest_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME rttest_plugin_test COMMAND rttest_plugin_test)
set_tests_properties(rttest_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── DcSyncOptimizerPlugin + DcSyncOptimizerService ─────────────────────
# DcSyncOptimizer/PdoMappingOptimization/RealtimeOptimizer/ErrorAnalysis
# plugins are NOT compiled into the app build — gate their tests.
if(ECAT_EXPERIMENTAL_SERVICES)
add_executable(dcsyncoptimizer_plugin_test
    dcsyncoptimizer_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/dcsyncoptimizer/DcSyncOptimizerPlugin.cpp
    ../../../apps/ecat-studio/plugins/dcsyncoptimizer/DcSyncOptimizerPlugin.h
    ../../../apps/ecat-studio/plugins/dcsyncoptimizer/SyncOptimizerWidget.cpp
    ../../../apps/ecat-studio/plugins/dcsyncoptimizer/SyncOptimizerWidget.h
    ../../../apps/ecat-studio/plugins/dcsyncoptimizer/DriftOptimizerWidget.cpp
    ../../../apps/ecat-studio/plugins/dcsyncoptimizer/DriftOptimizerWidget.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../../apps/ecat-studio/services/DcSyncOptimizerService.cpp
    ../../../apps/ecat-studio/services/DcSyncOptimizerService.h
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)

target_include_directories(dcsyncoptimizer_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins/dcsyncoptimizer
    ${CMAKE_SOURCE_DIR}/src/core
)

target_link_libraries(dcsyncoptimizer_plugin_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::Test
    ecat_core
)

set_target_properties(dcsyncoptimizer_plugin_test PROPERTIES AUTOMOC ON)
target_compile_definitions(dcsyncoptimizer_plugin_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)

add_test(NAME dcsyncoptimizer_plugin_test COMMAND dcsyncoptimizer_plugin_test)
set_tests_properties(dcsyncoptimizer_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── PdoMappingOptimizationPlugin + PdoMappingOptimizationService ────────
add_executable(pdomappingoptimization_plugin_test
    pdomappingoptimization_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/pdomappingoptimization/PdoMappingOptimizationPlugin.cpp
    ../../../apps/ecat-studio/plugins/pdomappingoptimization/PdoMappingOptimizationPlugin.h
    ../../../apps/ecat-studio/plugins/pdomappingoptimization/MappingOptimizerWidget.cpp
    ../../../apps/ecat-studio/plugins/pdomappingoptimization/MappingOptimizerWidget.h
    ../../../apps/ecat-studio/plugins/pdomappingoptimization/SizeOptimizerWidget.cpp
    ../../../apps/ecat-studio/plugins/pdomappingoptimization/SizeOptimizerWidget.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../../apps/ecat-studio/services/PdoMappingOptimizationService.cpp
    ../../../apps/ecat-studio/services/PdoMappingOptimizationService.h
)

target_include_directories(pdomappingoptimization_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins/pdomappingoptimization
)
target_compile_definitions(pdomappingoptimization_plugin_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)

target_link_libraries(pdomappingoptimization_plugin_test PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

set_target_properties(pdomappingoptimization_plugin_test PROPERTIES AUTOMOC ON)

add_test(NAME pdomappingoptimization_plugin_test COMMAND pdomappingoptimization_plugin_test)
set_tests_properties(pdomappingoptimization_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── Plugin Widget Tests (Latency/Throughput Optimizer, ErrorTimeline) ─
add_executable(plugin_widgets_test
    plugin_widgets_test.cpp
    ../../../apps/ecat-studio/plugins/realtimeoptimizer/LatencyOptimizerWidget.cpp
    ../../../apps/ecat-studio/plugins/realtimeoptimizer/LatencyOptimizerWidget.h
    ../../../apps/ecat-studio/plugins/realtimeoptimizer/ThroughputOptimizerWidget.cpp
    ../../../apps/ecat-studio/plugins/realtimeoptimizer/ThroughputOptimizerWidget.h
    ../../../apps/ecat-studio/plugins/erroranalysis/ErrorTimelineWidget.cpp
    ../../../apps/ecat-studio/plugins/erroranalysis/ErrorTimelineWidget.h
    ../../../apps/ecat-studio/services/EtherCATOptimizerService.h
)

target_include_directories(plugin_widgets_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)

target_link_libraries(plugin_widgets_test PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

set_target_properties(plugin_widgets_test PROPERTIES AUTOMOC ON)

add_test(NAME plugin_widgets_test COMMAND plugin_widgets_test)
set_tests_properties(plugin_widgets_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)
endif() # ECAT_EXPERIMENTAL_SERVICES (dcsyncoptimizer/pdomappingoptimization/plugin_widgets)

# ── BusStatsPlugin + BusStatsService (GUI) ──────────────────────────
add_executable(busstats_plugin_test
    busstats_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/busstats/BusStatsPlugin.cpp
    ../../../apps/ecat-studio/plugins/busstats/BusStatsPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../../apps/ecat-studio/services/BusStatsService.cpp
    ../../../apps/ecat-studio/services/BusStatsService.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)

target_include_directories(busstats_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/src/core
)

target_link_libraries(busstats_plugin_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::Test
    ecat_core
)

set_target_properties(busstats_plugin_test PROPERTIES AUTOMOC ON)

add_test(NAME busstats_plugin_test COMMAND busstats_plugin_test)
set_tests_properties(busstats_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── OnlineDiagnosticsPlugin ─────────────────────────────────────────
add_executable(onlinediagnostics_plugin_test
    onlinediagnostics_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/onlinediagnostics/OnlineDiagnosticsPlugin.cpp
    ../../../apps/ecat-studio/plugins/onlinediagnostics/OnlineDiagnosticsPlugin.h
    ../../../apps/ecat-studio/plugins/onlinediagnostics/BusMonitorWidget.cpp
    ../../../apps/ecat-studio/plugins/onlinediagnostics/BusMonitorWidget.h
    ../../../apps/ecat-studio/plugins/onlinediagnostics/ErrorAnalyzerWidget.cpp
    ../../../apps/ecat-studio/plugins/onlinediagnostics/ErrorAnalyzerWidget.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../../apps/ecat-studio/services/OnlineDiagnosticsService.cpp
    ../../../apps/ecat-studio/services/OnlineDiagnosticsService.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)

target_include_directories(onlinediagnostics_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/src/core
)

target_link_libraries(onlinediagnostics_plugin_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::Test
    ecat_core
)

target_compile_definitions(onlinediagnostics_plugin_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)

set_target_properties(onlinediagnostics_plugin_test PROPERTIES AUTOMOC ON)

add_test(NAME onlinediagnostics_plugin_test COMMAND onlinediagnostics_plugin_test)
set_tests_properties(onlinediagnostics_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── AlarmPlugin + AlarmService + LoggingService ─────────────────────
if(ECAT_EXPERIMENTAL_SERVICES)
add_executable(alarm_plugin_test
    alarm_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/alarm/AlarmPlugin.cpp
    ../../../apps/ecat-studio/plugins/alarm/AlarmPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../../apps/ecat-studio/services/AlarmService.cpp
    ../../../apps/ecat-studio/services/AlarmService.h
    ../../../apps/ecat-studio/services/LoggingService.cpp
    ../../../apps/ecat-studio/services/LoggingService.h
)

target_include_directories(alarm_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
)

target_link_libraries(alarm_plugin_test PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

set_target_properties(alarm_plugin_test PROPERTIES AUTOMOC ON)

add_test(NAME alarm_plugin_test COMMAND alarm_plugin_test)
set_tests_properties(alarm_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)
endif()

# ── ProjectPlugin + ProjectManagerService + ConfigurationService ─────
add_executable(project_plugin_test
    project_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/project/ProjectPlugin.cpp
    ../../../apps/ecat-studio/plugins/project/ProjectPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../../apps/ecat-studio/services/ProjectManagerService.cpp
    ../../../apps/ecat-studio/services/ProjectManagerService.h
    ../../../apps/ecat-studio/services/ConfigurationService.cpp
    ../../../apps/ecat-studio/services/ConfigurationService.h
)

target_include_directories(project_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
)

target_link_libraries(project_plugin_test PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

set_target_properties(project_plugin_test PROPERTIES AUTOMOC ON)

add_test(NAME project_plugin_test COMMAND project_plugin_test)
set_tests_properties(project_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── OscilloscopePlugin + OscilloscopeService (GUI) ────────────────────
add_executable(oscilloscope_plugin_test
    oscilloscope_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/oscilloscope/OscilloscopePlugin.cpp
    ../../../apps/ecat-studio/plugins/oscilloscope/OscilloscopePlugin.h
    ../../../apps/ecat-studio/plugins/oscilloscope/OscilloscopeWidget.cpp
    ../../../apps/ecat-studio/plugins/oscilloscope/OscilloscopeWidget.h
    ../../../apps/ecat-studio/plugins/oscilloscope/OscilloscopeService.cpp
    ../../../apps/ecat-studio/plugins/oscilloscope/OscilloscopeService.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)

target_include_directories(oscilloscope_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
)

target_compile_definitions(oscilloscope_plugin_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)

target_link_libraries(oscilloscope_plugin_test PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

set_target_properties(oscilloscope_plugin_test PROPERTIES AUTOMOC ON)

add_test(NAME oscilloscope_plugin_test COMMAND oscilloscope_plugin_test)
set_tests_properties(oscilloscope_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── ProtocolAnalyzerPlugin + ProtocolAnalyzerService (GUI) ────────────
add_executable(protocol_analyzer_plugin_test
    protocol_analyzer_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/protocol/ProtocolAnalyzerPlugin.cpp
    ../../../apps/ecat-studio/plugins/protocol/ProtocolAnalyzerPlugin.h
    ../../../apps/ecat-studio/plugins/protocol/ProtocolAnalyzerService.cpp
    ../../../apps/ecat-studio/plugins/protocol/ProtocolAnalyzerService.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)

target_include_directories(protocol_analyzer_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
)

target_compile_definitions(protocol_analyzer_plugin_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)

target_link_libraries(protocol_analyzer_plugin_test PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

set_target_properties(protocol_analyzer_plugin_test PROPERTIES AUTOMOC ON)

add_test(NAME protocol_analyzer_plugin_test COMMAND protocol_analyzer_plugin_test)
set_tests_properties(protocol_analyzer_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── ChartPlugin + ChartService (GUI) ────────────────────────────────
add_executable(chart_plugin_test
    chart_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/chart/ChartPlugin.cpp
    ../../../apps/ecat-studio/plugins/chart/ChartPlugin.h
    ../../../apps/ecat-studio/plugins/chart/EcatChartWidget.cpp
    ../../../apps/ecat-studio/plugins/chart/EcatChartWidget.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../../apps/ecat-studio/services/ChartService.cpp
    ../../../apps/ecat-studio/services/ChartService.h
)

target_include_directories(chart_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins/chart
)

target_link_libraries(chart_plugin_test PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

set_target_properties(chart_plugin_test PROPERTIES AUTOMOC ON)

add_test(NAME chart_plugin_test COMMAND chart_plugin_test)
set_tests_properties(chart_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── DashboardPlugin + ChartService (GUI) ────────────────────────────
# DashboardPlugin is NOT in the app build — gated.
if(ECAT_EXPERIMENTAL_SERVICES)
add_executable(dashboard_plugin_test
    dashboard_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/dashboard/DashboardPlugin.cpp
    ../../../apps/ecat-studio/plugins/dashboard/DashboardPlugin.h
    ../../../apps/ecat-studio/plugins/chart/EcatChartWidget.cpp
    ../../../apps/ecat-studio/plugins/chart/EcatChartWidget.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../../apps/ecat-studio/services/ChartService.cpp
    ../../../apps/ecat-studio/services/ChartService.h
)

target_include_directories(dashboard_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins/chart
)

target_compile_definitions(dashboard_plugin_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)

target_link_libraries(dashboard_plugin_test PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

set_target_properties(dashboard_plugin_test PROPERTIES AUTOMOC ON)

add_test(NAME dashboard_plugin_test COMMAND dashboard_plugin_test)
set_tests_properties(dashboard_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)
endif() # ECAT_EXPERIMENTAL_SERVICES (dashboard)

if(Qt6Qml_FOUND)

    add_executable(automation_plugin_test
        automation_plugin_test.cpp
        ../../../apps/ecat-studio/plugins/automation/AutomationPlugin.cpp
        ../../../apps/ecat-studio/plugins/automation/AutomationPlugin.h
        ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
        ../../../apps/ecat-studio/services/ScriptingService.cpp
        ../../../apps/ecat-studio/services/ScriptingService.h
        ../../../apps/ecat-studio/services/BatchOperationService.cpp
        ../../../apps/ecat-studio/services/BatchOperationService.h
        ../../../apps/ecat-studio/services/SdoService.cpp
        ../../../apps/ecat-studio/services/SdoService.h
        ../../../apps/ecat-studio/services/TopologyService.cpp
        ../../../apps/ecat-studio/services/TopologyService.h
        ../../../apps/ecat-studio/infra/EcatClient.cpp
        ../../../apps/ecat-studio/infra/EcatClient.h
        ../../../apps/ecat-studio/infra/ProcessDataTypes.h
        ../../../src/core/EthercatTypes.cpp
        ../../../src/core/EthercatTypes.h
    )

    target_include_directories(automation_plugin_test PRIVATE
        ${CMAKE_SOURCE_DIR}/apps/ecat-studio
        ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
        ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
        ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
        ${CMAKE_SOURCE_DIR}/src/core
    )

    target_link_libraries(automation_plugin_test PRIVATE
        Qt6::Core
        Qt6::Network
        Qt6::Widgets
        Qt6::Qml
        Qt6::Test
        ecat_core
    )
    target_compile_definitions(automation_plugin_test PRIVATE
        ECAT_SCRIPTING_ENABLED
        SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
    )

    set_target_properties(automation_plugin_test PROPERTIES AUTOMOC ON)

    add_test(NAME automation_plugin_test COMMAND automation_plugin_test)
    set_tests_properties(automation_plugin_test PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
    )
endif()

# ── NetworkDiagnosticsPlugin + NetworkDiagnosticsService (GUI) ───────────
# NetworkDiagnostics/DataPipeline/DeviceManager/MasterManager/SdoCache
# plugins are NOT compiled into the app build — gate their tests.
if(ECAT_EXPERIMENTAL_SERVICES)
add_executable(network_diagnostics_plugin_test
    network_diagnostics_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/network/NetworkDiagnosticsPlugin.cpp
    ../../../apps/ecat-studio/plugins/network/NetworkDiagnosticsPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../../apps/ecat-studio/services/NetworkDiagnosticsService.cpp
    ../../../apps/ecat-studio/services/NetworkDiagnosticsService.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(network_diagnostics_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(network_diagnostics_plugin_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::Test
    ecat_core
)
set_target_properties(network_diagnostics_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME network_diagnostics_plugin_test COMMAND network_diagnostics_plugin_test)
set_tests_properties(network_diagnostics_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── DataPipelinePlugin (integration) ───────────────────────────────
add_executable(data_pipeline_plugin_test
    data_pipeline_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/datapipeline/DataPipelinePlugin.cpp
    ../../../apps/ecat-studio/plugins/datapipeline/DataPipelinePlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../../apps/ecat-studio/services/DataPipelineService.cpp
    ../../../apps/ecat-studio/services/DataPipelineService.h
)
target_include_directories(data_pipeline_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
)
target_link_libraries(data_pipeline_plugin_test PRIVATE
    Qt6::Core Qt6::Widgets Qt6::Test
)
set_target_properties(data_pipeline_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME data_pipeline_plugin_test COMMAND data_pipeline_plugin_test)
set_tests_properties(data_pipeline_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── DeviceManagerPlugin (integration) ───────────────────────────────
add_executable(device_manager_plugin_test
    device_manager_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/devicemanager/DeviceManagerPlugin.cpp
    ../../../apps/ecat-studio/plugins/devicemanager/DeviceManagerPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../../apps/ecat-studio/services/DeviceManagerService.cpp
    ../../../apps/ecat-studio/services/DeviceManagerService.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(device_manager_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(device_manager_plugin_test PRIVATE
    Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core
)
set_target_properties(device_manager_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME device_manager_plugin_test COMMAND device_manager_plugin_test)
set_tests_properties(device_manager_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── MasterManagerPlugin + MasterManagerService + DistributedClockService ──
add_executable(master_manager_plugin_test
    master_manager_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/master/MasterManagerPlugin.cpp
    ../../../apps/ecat-studio/plugins/master/MasterManagerPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../../apps/ecat-studio/services/MasterManagerService.cpp
    ../../../apps/ecat-studio/services/MasterManagerService.h
    ../../../apps/ecat-studio/services/DistributedClockService.cpp
    ../../../apps/ecat-studio/services/DistributedClockService.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(master_manager_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/src/core
)
target_compile_definitions(master_manager_plugin_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)
target_link_libraries(master_manager_plugin_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::Test
    ecat_core
)
set_target_properties(master_manager_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME master_manager_plugin_test COMMAND master_manager_plugin_test)
set_tests_properties(master_manager_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── SdoCachePlugin + SdoCacheService + PdoMappingService ──────────────
add_executable(sdo_cache_plugin_test
    sdo_cache_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/sdocache/SdoCachePlugin.cpp
    ../../../apps/ecat-studio/plugins/sdocache/SdoCachePlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../../apps/ecat-studio/services/SdoCacheService.cpp
    ../../../apps/ecat-studio/services/SdoCacheService.h
    ../../../apps/ecat-studio/services/PdoMappingService.cpp
    ../../../apps/ecat-studio/services/PdoMappingService.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(sdo_cache_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(sdo_cache_plugin_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::Test
    ecat_core
)
set_target_properties(sdo_cache_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME sdo_cache_plugin_test COMMAND sdo_cache_plugin_test)
set_tests_properties(sdo_cache_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)
endif() # ECAT_EXPERIMENTAL_SERVICES (network/data_pipeline/device_manager/master/sdo_cache plugins)

# ── ThemeCustomizerPlugin (GUI) ──────────────────────────────────────
add_executable(theme_customizer_plugin_test
    theme_customizer_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/themecustomizer/ThemeCustomizerPlugin.cpp
    ../../../apps/ecat-studio/plugins/themecustomizer/ThemeCustomizerPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(theme_customizer_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(theme_customizer_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(theme_customizer_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME theme_customizer_plugin_test COMMAND theme_customizer_plugin_test)
set_tests_properties(theme_customizer_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── KeyboardShortcutsPlugin (GUI) ────────────────────────────────────
add_executable(keyboard_shortcuts_plugin_test
    keyboard_shortcuts_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/shortcuts/KeyboardShortcutsPlugin.cpp
    ../../../apps/ecat-studio/plugins/shortcuts/KeyboardShortcutsPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(keyboard_shortcuts_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(keyboard_shortcuts_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(keyboard_shortcuts_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME keyboard_shortcuts_plugin_test COMMAND keyboard_shortcuts_plugin_test)
set_tests_properties(keyboard_shortcuts_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── UserPreferencesPlugin (GUI) ──────────────────────────────────────
add_executable(user_preferences_plugin_test
    user_preferences_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/preferences/UserPreferencesPlugin.cpp
    ../../../apps/ecat-studio/plugins/preferences/UserPreferencesPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(user_preferences_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(user_preferences_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(user_preferences_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME user_preferences_plugin_test COMMAND user_preferences_plugin_test)
set_tests_properties(user_preferences_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── TracePlugin + TraceService (GUI) ────────────────────────────────────
# Trace/LogicAnalyzer/Diagram/Formula/ScriptLibrary plugins are NOT in the
# app build — gated.
if(ECAT_EXPERIMENTAL_SERVICES)
add_executable(trace_plugin_test
    trace_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/trace/TracePlugin.cpp
    ../../../apps/ecat-studio/plugins/trace/TracePlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../../apps/ecat-studio/services/TraceService.cpp
    ../../../apps/ecat-studio/services/TraceService.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
    ../../mocks/MockEcatClient.cpp
    ../../mocks/MockEcatClient.h
)

target_include_directories(trace_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/tests/mocks
)

target_link_libraries(trace_plugin_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::Test
    ecat_core
)

set_target_properties(trace_plugin_test PROPERTIES AUTOMOC ON)

add_test(NAME trace_plugin_test COMMAND trace_plugin_test)
set_tests_properties(trace_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── LogicAnalyzerPlugin (GUI) ────────────────────────────────────────────
add_executable(logicanalyzer_plugin_test
    logicanalyzer_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/logicanalyzer/LogicAnalyzerPlugin.cpp
    ../../../apps/ecat-studio/plugins/logicanalyzer/LogicAnalyzerPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../../apps/ecat-studio/services/TraceService.cpp
    ../../../apps/ecat-studio/services/TraceService.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
    ../../mocks/MockEcatClient.cpp
    ../../mocks/MockEcatClient.h
)

target_include_directories(logicanalyzer_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/tests/mocks
)

target_compile_definitions(logicanalyzer_plugin_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)

target_link_libraries(logicanalyzer_plugin_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::Test
    ecat_core
)

set_target_properties(logicanalyzer_plugin_test PROPERTIES AUTOMOC ON)

add_test(NAME logicanalyzer_plugin_test COMMAND logicanalyzer_plugin_test)
set_tests_properties(logicanalyzer_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── DiagramPlugin (unit) ────────────────────────────────────────────
add_executable(diagram_plugin_test
    diagram_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/diagram/DiagramPlugin.cpp
    ../../../apps/ecat-studio/plugins/diagram/DiagramPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(diagram_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(diagram_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(diagram_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME diagram_plugin_test COMMAND diagram_plugin_test)
set_tests_properties(diagram_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── FormulaPlugin (unit) ────────────────────────────────────────────
add_executable(formula_plugin_test
    formula_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/formula/FormulaPlugin.cpp
    ../../../apps/ecat-studio/plugins/formula/FormulaPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(formula_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(formula_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(formula_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME formula_plugin_test COMMAND formula_plugin_test)
set_tests_properties(formula_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── ScriptLibraryPlugin (unit) ──────────────────────────────────────
add_executable(scriptlibrary_plugin_test
    scriptlibrary_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/scriptlibrary/ScriptLibraryPlugin.cpp
    ../../../apps/ecat-studio/plugins/scriptlibrary/ScriptLibraryPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(scriptlibrary_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(scriptlibrary_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(scriptlibrary_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME scriptlibrary_plugin_test COMMAND scriptlibrary_plugin_test)
set_tests_properties(scriptlibrary_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
endif() # ECAT_EXPERIMENTAL_SERVICES (trace/logicanalyzer/diagram/formula/scriptlibrary)

# ── SimulationPlugin (integration) ──────────────────────────────────
# Simulation/Calibration/Documentation/Wizard/Template plugins are NOT in
# the app build — gated.
if(ECAT_EXPERIMENTAL_SERVICES)
add_executable(simulation_plugin_test
    simulation_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/simulation/SimulationPlugin.cpp
    ../../../apps/ecat-studio/plugins/simulation/SimulationPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_include_directories(simulation_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(simulation_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
set_target_properties(simulation_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME simulation_plugin_test COMMAND simulation_plugin_test)
set_tests_properties(simulation_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── CalibrationPlugin (integration) ─────────────────────────────────
add_executable(calibration_plugin_test
    calibration_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/calibration/CalibrationPlugin.cpp
    ../../../apps/ecat-studio/plugins/calibration/CalibrationPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_include_directories(calibration_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_compile_definitions(calibration_plugin_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)
target_link_libraries(calibration_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
set_target_properties(calibration_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME calibration_plugin_test COMMAND calibration_plugin_test)
set_tests_properties(calibration_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── DocumentationPlugin (integration) ───────────────────────────────
add_executable(documentation_plugin_test
    documentation_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/documentation/DocumentationPlugin.cpp
    ../../../apps/ecat-studio/plugins/documentation/DocumentationPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_include_directories(documentation_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(documentation_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
set_target_properties(documentation_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME documentation_plugin_test COMMAND documentation_plugin_test)
set_tests_properties(documentation_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── WizardPlugin (integration) ──────────────────────────────────────
add_executable(wizard_plugin_test
    wizard_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/wizard/WizardPlugin.cpp
    ../../../apps/ecat-studio/plugins/wizard/WizardPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_include_directories(wizard_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(wizard_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
set_target_properties(wizard_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME wizard_plugin_test COMMAND wizard_plugin_test)
set_tests_properties(wizard_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── TemplatePlugin (integration) ────────────────────────────────────
add_executable(template_plugin_test
    template_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/template/TemplatePlugin.cpp
    ../../../apps/ecat-studio/plugins/template/TemplatePlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_include_directories(template_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(template_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
set_target_properties(template_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME template_plugin_test COMMAND template_plugin_test)
set_tests_properties(template_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)
endif() # ECAT_EXPERIMENTAL_SERVICES (simulation/calibration/documentation/wizard/template)

# ── SecurityManagerPlugin ──────────────────────────────────────────
# SecurityManager/ComplianceChecker/CertificationManager/Report/
# DashboardDesigner/AlarmManager/DataLogger plugins are NOT in the app
# build — gated.
if(ECAT_EXPERIMENTAL_SERVICES)
add_executable(securitymanager_plugin_test
    securitymanager_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/securitymanager/SecurityManagerPlugin.cpp
    ../../../apps/ecat-studio/plugins/securitymanager/SecurityManagerPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_include_directories(securitymanager_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(securitymanager_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
set_target_properties(securitymanager_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME securitymanager_plugin_test COMMAND securitymanager_plugin_test)
set_tests_properties(securitymanager_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── ComplianceCheckerPlugin ────────────────────────────────────────
add_executable(compliancechecker_plugin_test
    compliancechecker_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/compliancechecker/ComplianceCheckerPlugin.cpp
    ../../../apps/ecat-studio/plugins/compliancechecker/ComplianceCheckerPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_include_directories(compliancechecker_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(compliancechecker_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
set_target_properties(compliancechecker_plugin_test PROPERTIES AUTOMOC ON)
target_compile_definitions(compliancechecker_plugin_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)
add_test(NAME compliancechecker_plugin_test COMMAND compliancechecker_plugin_test)
set_tests_properties(compliancechecker_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── CertificationManagerPlugin ─────────────────────────────────────
add_executable(certificationmanager_plugin_test
    certificationmanager_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/certificationmanager/CertificationManagerPlugin.cpp
    ../../../apps/ecat-studio/plugins/certificationmanager/CertificationManagerPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_include_directories(certificationmanager_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(certificationmanager_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
set_target_properties(certificationmanager_plugin_test PROPERTIES AUTOMOC ON)
target_compile_definitions(certificationmanager_plugin_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)
add_test(NAME certificationmanager_plugin_test COMMAND certificationmanager_plugin_test)
set_tests_properties(certificationmanager_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── ReportPlugin (integration) ──────────────────────────────────────
add_executable(report_plugin_test
    report_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/report/ReportPlugin.cpp
    ../../../apps/ecat-studio/plugins/report/ReportPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_include_directories(report_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(report_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
set_target_properties(report_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME report_plugin_test COMMAND report_plugin_test)
set_tests_properties(report_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── DashboardDesignerPlugin (integration) ─────────────────────────
add_executable(dashboarddesigner_plugin_test
    dashboarddesigner_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/dashboarddesigner/DashboardDesignerPlugin.cpp
    ../../../apps/ecat-studio/plugins/dashboarddesigner/DashboardDesignerPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_include_directories(dashboarddesigner_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(dashboarddesigner_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
set_target_properties(dashboarddesigner_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME dashboarddesigner_plugin_test COMMAND dashboarddesigner_plugin_test)
set_tests_properties(dashboarddesigner_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── AlarmManagerPlugin (integration) ─────────────────────────────
add_executable(alarmmanager_plugin_test
    alarmmanager_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/alarmmanager/AlarmManagerPlugin.cpp
    ../../../apps/ecat-studio/plugins/alarmmanager/AlarmManagerPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_include_directories(alarmmanager_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(alarmmanager_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
set_target_properties(alarmmanager_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME alarmmanager_plugin_test COMMAND alarmmanager_plugin_test)
set_tests_properties(alarmmanager_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── DataLoggerPlugin (integration) ───────────────────────────────
add_executable(datalogger_plugin_test
    datalogger_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/datalogger/DataLoggerPlugin.cpp
    ../../../apps/ecat-studio/plugins/datalogger/DataLoggerPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_include_directories(datalogger_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(datalogger_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
set_target_properties(datalogger_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME datalogger_plugin_test COMMAND datalogger_plugin_test)
set_tests_properties(datalogger_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)
endif() # ECAT_EXPERIMENTAL_SERVICES (security/compliance/certification/report/dashboarddesigner/alarmmanager/datalogger)

# ── WorkflowDesignerPlugin (unit) ─────────────────────────────────
# WorkflowDesigner/TestSuite/Deployment/DeploymentManager/UpdateManager/
# MaintenanceScheduler/ConfigEditor/NetworkAnalyzer/SystemMonitor/
# OptimizationDashboard/MonitoringDashboard/AnalyticsDashboard/
# IntegrationHub/SyncManager/ReplicationManager/VisualizationStudio/
# ReportDesigner/DocumentationBrowser/WorkflowOptimizer/WorkflowDashboard
# plugins are NOT compiled into the app build — gated.
if(ECAT_EXPERIMENTAL_SERVICES)
add_executable(workflowdesigner_plugin_test
    workflowdesigner_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/workflowdesigner/WorkflowDesignerPlugin.cpp
    ../../../apps/ecat-studio/plugins/workflowdesigner/WorkflowDesignerPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(workflowdesigner_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(workflowdesigner_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(workflowdesigner_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME workflowdesigner_plugin_test COMMAND workflowdesigner_plugin_test)
set_tests_properties(workflowdesigner_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── TestSuitePlugin (unit) ────────────────────────────────────────
add_executable(testsuite_plugin_test
    testsuite_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/testsuite/TestSuitePlugin.cpp
    ../../../apps/ecat-studio/plugins/testsuite/TestSuitePlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(testsuite_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(testsuite_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(testsuite_plugin_test PROPERTIES AUTOMOC ON)
target_compile_definitions(testsuite_plugin_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)
add_test(NAME testsuite_plugin_test COMMAND testsuite_plugin_test)
set_tests_properties(testsuite_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── DeploymentPlugin (unit) ───────────────────────────────────────
add_executable(deployment_plugin_test
    deployment_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/deployment/DeploymentPlugin.cpp
    ../../../apps/ecat-studio/plugins/deployment/DeploymentPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(deployment_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(deployment_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(deployment_plugin_test PROPERTIES AUTOMOC ON)
target_compile_definitions(deployment_plugin_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)
add_test(NAME deployment_plugin_test COMMAND deployment_plugin_test)
set_tests_properties(deployment_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── DeploymentManagerPlugin (unit) ────────────────────────────────
add_executable(deploymentmanager_plugin_test
    deploymentmanager_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/deploymentmanager/DeploymentManagerPlugin.cpp
    ../../../apps/ecat-studio/plugins/deploymentmanager/DeploymentManagerPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(deploymentmanager_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(deploymentmanager_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(deploymentmanager_plugin_test PROPERTIES AUTOMOC ON)
target_compile_definitions(deploymentmanager_plugin_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)
add_test(NAME deploymentmanager_plugin_test COMMAND deploymentmanager_plugin_test)
set_tests_properties(deploymentmanager_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── UpdateManagerPlugin (unit) ────────────────────────────────────
add_executable(updatemanager_plugin_test
    updatemanager_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/updatemanager/UpdateManagerPlugin.cpp
    ../../../apps/ecat-studio/plugins/updatemanager/UpdateManagerPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(updatemanager_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(updatemanager_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(updatemanager_plugin_test PROPERTIES AUTOMOC ON)
target_compile_definitions(updatemanager_plugin_test PRIVATE SOURCE_ROOT="${CMAKE_SOURCE_DIR}")
add_test(NAME updatemanager_plugin_test COMMAND updatemanager_plugin_test)
set_tests_properties(updatemanager_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── MaintenanceSchedulerPlugin (unit) ─────────────────────────────
add_executable(maintenancescheduler_plugin_test
    maintenancescheduler_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/maintenancescheduler/MaintenanceSchedulerPlugin.cpp
    ../../../apps/ecat-studio/plugins/maintenancescheduler/MaintenanceSchedulerPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(maintenancescheduler_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(maintenancescheduler_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
target_compile_definitions(maintenancescheduler_plugin_test PRIVATE SOURCE_ROOT="${CMAKE_SOURCE_DIR}")
set_target_properties(maintenancescheduler_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME maintenancescheduler_plugin_test COMMAND maintenancescheduler_plugin_test)
set_tests_properties(maintenancescheduler_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── ConfigurationEditorPlugin (GUI) ────────────────────────────────────
add_executable(configeditor_plugin_test
    configeditor_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/configeditor/ConfigurationEditorPlugin.cpp
    ../../../apps/ecat-studio/plugins/configeditor/ConfigurationEditorPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(configeditor_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(configeditor_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(configeditor_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME configeditor_plugin_test COMMAND configeditor_plugin_test)
set_tests_properties(configeditor_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── NetworkAnalyzerPlugin (GUI) ────────────────────────────────────────
add_executable(networkanalyzer_plugin_test
    networkanalyzer_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/networkanalyzer/NetworkAnalyzerPlugin.cpp
    ../../../apps/ecat-studio/plugins/networkanalyzer/NetworkAnalyzerPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(networkanalyzer_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(networkanalyzer_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(networkanalyzer_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME networkanalyzer_plugin_test COMMAND networkanalyzer_plugin_test)
set_tests_properties(networkanalyzer_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── SystemMonitorPlugin (GUI) ──────────────────────────────────────────
add_executable(systemmonitor_plugin_test
    systemmonitor_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/systemmonitor/SystemMonitorPlugin.cpp
    ../../../apps/ecat-studio/plugins/systemmonitor/SystemMonitorPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(systemmonitor_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(systemmonitor_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(systemmonitor_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME systemmonitor_plugin_test COMMAND systemmonitor_plugin_test)
set_tests_properties(systemmonitor_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── OptimizationDashboardPlugin (GUI) ──────────────────────────────────
add_executable(optimizationdashboard_plugin_test
    optimizationdashboard_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/optimizationdashboard/OptimizationDashboardPlugin.cpp
    ../../../apps/ecat-studio/plugins/optimizationdashboard/OptimizationDashboardPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(optimizationdashboard_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(optimizationdashboard_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(optimizationdashboard_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME optimizationdashboard_plugin_test COMMAND optimizationdashboard_plugin_test)
set_tests_properties(optimizationdashboard_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── MonitoringDashboardPlugin (GUI) ────────────────────────────────────
add_executable(monitoringdashboard_plugin_test
    monitoringdashboard_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/monitoringdashboard/MonitoringDashboardPlugin.cpp
    ../../../apps/ecat-studio/plugins/monitoringdashboard/MonitoringDashboardPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(monitoringdashboard_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(monitoringdashboard_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(monitoringdashboard_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME monitoringdashboard_plugin_test COMMAND monitoringdashboard_plugin_test)
set_tests_properties(monitoringdashboard_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── AnalyticsDashboardPlugin (GUI) ─────────────────────────────────────
add_executable(analyticsdashboard_plugin_test
    analyticsdashboard_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/analyticsdashboard/AnalyticsDashboardPlugin.cpp
    ../../../apps/ecat-studio/plugins/analyticsdashboard/AnalyticsDashboardPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(analyticsdashboard_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(analyticsdashboard_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(analyticsdashboard_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME analyticsdashboard_plugin_test COMMAND analyticsdashboard_plugin_test)
set_tests_properties(analyticsdashboard_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── IntegrationHubPlugin ───────────────────────────────────────────
add_executable(integrationhub_plugin_test
    integrationhub_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/integrationhub/IntegrationHubPlugin.cpp
    ../../../apps/ecat-studio/plugins/integrationhub/IntegrationHubPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_include_directories(integrationhub_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(integrationhub_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
set_target_properties(integrationhub_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME integrationhub_plugin_test COMMAND integrationhub_plugin_test)
set_tests_properties(integrationhub_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── SyncManagerPlugin ─────────────────────────────────────────────
add_executable(syncmanager_plugin_test
    syncmanager_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/syncmanager/SyncManagerPlugin.cpp
    ../../../apps/ecat-studio/plugins/syncmanager/SyncManagerPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_include_directories(syncmanager_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(syncmanager_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
set_target_properties(syncmanager_plugin_test PROPERTIES AUTOMOC ON)
target_compile_definitions(syncmanager_plugin_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)
add_test(NAME syncmanager_plugin_test COMMAND syncmanager_plugin_test)
set_tests_properties(syncmanager_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── ReplicationManagerPlugin ───────────────────────────────────────
add_executable(replicationmanager_plugin_test
    replicationmanager_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/replicationmanager/ReplicationManagerPlugin.cpp
    ../../../apps/ecat-studio/plugins/replicationmanager/ReplicationManagerPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_include_directories(replicationmanager_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(replicationmanager_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
set_target_properties(replicationmanager_plugin_test PROPERTIES AUTOMOC ON)
target_compile_definitions(replicationmanager_plugin_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)
add_test(NAME replicationmanager_plugin_test COMMAND replicationmanager_plugin_test)
set_tests_properties(replicationmanager_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── VisualizationStudioPlugin ───────────────────────────────────────
add_executable(visualizationstudio_plugin_test
    visualizationstudio_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/visualizationstudio/VisualizationStudioPlugin.cpp
    ../../../apps/ecat-studio/plugins/visualizationstudio/VisualizationStudioPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_include_directories(visualizationstudio_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(visualizationstudio_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
set_target_properties(visualizationstudio_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME visualizationstudio_plugin_test COMMAND visualizationstudio_plugin_test)
set_tests_properties(visualizationstudio_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── ReportDesignerPlugin ────────────────────────────────────────────
add_executable(reportdesigner_plugin_test
    reportdesigner_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/reportdesigner/ReportDesignerPlugin.cpp
    ../../../apps/ecat-studio/plugins/reportdesigner/ReportDesignerPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_include_directories(reportdesigner_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(reportdesigner_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
set_target_properties(reportdesigner_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME reportdesigner_plugin_test COMMAND reportdesigner_plugin_test)
set_tests_properties(reportdesigner_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── DocumentationBrowserPlugin ──────────────────────────────────────
add_executable(documentationbrowser_plugin_test
    documentationbrowser_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/documentationbrowser/DocumentationBrowserPlugin.cpp
    ../../../apps/ecat-studio/plugins/documentationbrowser/DocumentationBrowserPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_include_directories(documentationbrowser_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(documentationbrowser_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
set_target_properties(documentationbrowser_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME documentationbrowser_plugin_test COMMAND documentationbrowser_plugin_test)
set_tests_properties(documentationbrowser_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── WorkflowOptimizerPlugin (GUI) ──────────────────────────────────
add_executable(workflowoptimizer_plugin_test
    workflowoptimizer_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/workflowoptimizer/WorkflowOptimizerPlugin.cpp
    ../../../apps/ecat-studio/plugins/workflowoptimizer/WorkflowOptimizerPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../../apps/ecat-studio/services/WorkflowAnalyticsService.cpp
    ../../../apps/ecat-studio/services/WorkflowAnalyticsService.h
)
target_include_directories(workflowoptimizer_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
)
target_link_libraries(workflowoptimizer_plugin_test PRIVATE
    Qt6::Core Qt6::Widgets Qt6::Test
)
set_target_properties(workflowoptimizer_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME workflowoptimizer_plugin_test COMMAND workflowoptimizer_plugin_test)
set_tests_properties(workflowoptimizer_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
    TIMEOUT 30
    LABELS "unit;plugin"
)

# ── WorkflowDashboardPlugin (GUI) ──────────────────────────────────
add_executable(workflowdashboard_plugin_test
    workflowdashboard_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/workflowdashboard/WorkflowDashboardPlugin.cpp
    ../../../apps/ecat-studio/plugins/workflowdashboard/WorkflowDashboardPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../../apps/ecat-studio/services/WorkflowMonitoringService.cpp
    ../../../apps/ecat-studio/services/WorkflowMonitoringService.h
)
target_include_directories(workflowdashboard_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
)
target_link_libraries(workflowdashboard_plugin_test PRIVATE
    Qt6::Core Qt6::Widgets Qt6::Test
)
set_target_properties(workflowdashboard_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME workflowdashboard_plugin_test COMMAND workflowdashboard_plugin_test)
set_tests_properties(workflowdashboard_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
    TIMEOUT 30
    LABELS "unit;plugin"
)
endif() # ECAT_EXPERIMENTAL_SERVICES (workflow/analytics/dashboard/editor plugins)

# ── ESI Browser Plugin Test ──────────────────────────────────────────
add_executable(esi_browser_plugin_test
    esi_browser_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/esi/EsiBrowserPlugin.cpp
    ../../../apps/ecat-studio/plugins/esi/EsiBrowserPlugin.h
    ../../../apps/ecat-studio/plugins/esi/EsiParser.cpp
    ../../../apps/ecat-studio/plugins/esi/EsiParser.h
    ../../../apps/ecat-studio/plugins/esi/EsiDeviceMatcher.cpp
    ../../../apps/ecat-studio/plugins/esi/EsiDeviceMatcher.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../../apps/ecat-studio/services/EsiService.cpp
    ../../../apps/ecat-studio/services/EsiService.h
)

target_include_directories(esi_browser_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
)

target_link_libraries(esi_browser_plugin_test PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

set_target_properties(esi_browser_plugin_test PROPERTIES AUTOMOC ON)

add_test(NAME esi_browser_plugin_test COMMAND esi_browser_plugin_test)
set_tests_properties(esi_browser_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── DcSyncPrecisionPlugin + DriftMonitorWidget + JitterAnalysisWidget ──
add_executable(dcsyncprecision_plugin_test
    dcsyncprecision_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/dcsyncprecision/DcSyncPrecisionPlugin.cpp
    ../../../apps/ecat-studio/plugins/dcsyncprecision/DcSyncPrecisionPlugin.h
    ../../../apps/ecat-studio/plugins/dcsyncprecision/DriftMonitorWidget.cpp
    ../../../apps/ecat-studio/plugins/dcsyncprecision/DriftMonitorWidget.h
    ../../../apps/ecat-studio/plugins/dcsyncprecision/JitterAnalysisWidget.cpp
    ../../../apps/ecat-studio/plugins/dcsyncprecision/JitterAnalysisWidget.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../../apps/ecat-studio/services/DcSyncPrecisionService.cpp
    ../../../apps/ecat-studio/services/DcSyncPrecisionService.h
    ../../../apps/ecat-studio/services/DcSyncOptimizerService.cpp
    ../../../apps/ecat-studio/services/DcSyncOptimizerService.h
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(dcsyncprecision_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(dcsyncprecision_plugin_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::Test
    ecat_core
)
set_target_properties(dcsyncprecision_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME dcsyncprecision_plugin_test COMMAND dcsyncprecision_plugin_test)
set_tests_properties(dcsyncprecision_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── FreeRunOptimizationPlugin + FreeRunOptimizationService ───────────
# FreeRunOptimization/SdoOptimization plugins are NOT in the app build —
# gated.
if(ECAT_EXPERIMENTAL_SERVICES)
add_executable(freerunoptimization_plugin_test
    freerunoptimization_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/freerunoptimization/FreeRunOptimizationPlugin.cpp
    ../../../apps/ecat-studio/plugins/freerunoptimization/FreeRunOptimizationPlugin.h
    ../../../apps/ecat-studio/plugins/freerunoptimization/CycleTimeOptimizerWidget.cpp
    ../../../apps/ecat-studio/plugins/freerunoptimization/CycleTimeOptimizerWidget.h
    ../../../apps/ecat-studio/plugins/freerunoptimization/DataMappingOptimizerWidget.cpp
    ../../../apps/ecat-studio/plugins/freerunoptimization/DataMappingOptimizerWidget.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../../apps/ecat-studio/services/FreeRunOptimizationService.cpp
    ../../../apps/ecat-studio/services/FreeRunOptimizationService.h
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)

target_include_directories(freerunoptimization_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins/freerunoptimization
    ${CMAKE_SOURCE_DIR}/src/core
)

target_compile_definitions(freerunoptimization_plugin_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)

target_link_libraries(freerunoptimization_plugin_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::Test
    ecat_core
)

set_target_properties(freerunoptimization_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME freerunoptimization_plugin_test COMMAND freerunoptimization_plugin_test)
set_tests_properties(freerunoptimization_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

add_executable(sdooptimization_plugin_test
    sdooptimization_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/sdooptimization/SdoOptimizationPlugin.cpp
    ../../../apps/ecat-studio/plugins/sdooptimization/SdoOptimizationPlugin.h
    ../../../apps/ecat-studio/plugins/sdooptimization/CacheOptimizerWidget.cpp
    ../../../apps/ecat-studio/plugins/sdooptimization/CacheOptimizerWidget.h
    ../../../apps/ecat-studio/plugins/sdooptimization/BatchOptimizerWidget.cpp
    ../../../apps/ecat-studio/plugins/sdooptimization/BatchOptimizerWidget.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../../apps/ecat-studio/services/SdoOptimizationService.cpp
    ../../../apps/ecat-studio/services/SdoOptimizationService.h
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)

target_include_directories(sdooptimization_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins/sdooptimization
    ${CMAKE_SOURCE_DIR}/src/core
)
target_compile_definitions(sdooptimization_plugin_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)

target_link_libraries(sdooptimization_plugin_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::Test
    ecat_core
)

set_target_properties(sdooptimization_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME sdooptimization_plugin_test COMMAND sdooptimization_plugin_test)
set_tests_properties(sdooptimization_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)
endif() # ECAT_EXPERIMENTAL_SERVICES (freerunoptimization/sdooptimization)

# ── OverviewPlugin (unit) ────────────────────────────────────────────
add_executable(overview_plugin_test
    overview_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/overview/OverviewPlugin.cpp
    ../../../apps/ecat-studio/plugins/overview/OverviewPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    TestServiceContainer.cpp
    ../../../apps/ecat-studio/services/ServiceContainer.h
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(overview_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(overview_plugin_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::Test
    ecat_core
)
set_target_properties(overview_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME overview_plugin_test COMMAND overview_plugin_test)
set_tests_properties(overview_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── WatchPlugin (unit) ───────────────────────────────────────────────
add_executable(watch_plugin_test
    watch_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/watch/WatchPlugin.cpp
    ../../../apps/ecat-studio/plugins/watch/WatchPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    TestServiceContainer.cpp
    ../../../apps/ecat-studio/services/ServiceContainer.h
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
    ../../../apps/ecat-studio/services/WatchService.cpp
    ../../../apps/ecat-studio/services/WatchService.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(watch_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(watch_plugin_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::Test
    ecat_core
)
set_target_properties(watch_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME watch_plugin_test COMMAND watch_plugin_test)
set_tests_properties(watch_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── StateMachinePlugin (unit) ───────────────────────────────────────
add_executable(statemachine_plugin_test
    statemachine_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/statemachine/StateMachinePlugin.cpp
    ../../../apps/ecat-studio/plugins/statemachine/StateMachinePlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    TestServiceContainer.cpp
    ../../../apps/ecat-studio/services/ServiceContainer.h
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
    ../../../apps/ecat-studio/services/StateMachineService.cpp
    ../../../apps/ecat-studio/services/StateMachineService.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(statemachine_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(statemachine_plugin_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::Test
    ecat_core
)
set_target_properties(statemachine_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME statemachine_plugin_test COMMAND statemachine_plugin_test)
set_tests_properties(statemachine_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── SessionPlugin (unit) ────────────────────────────────────────────
add_executable(session_plugin_test
    session_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/session/SessionPlugin.cpp
    ../../../apps/ecat-studio/plugins/session/SessionPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    TestServiceContainer.cpp
    ../../../apps/ecat-studio/services/ServiceContainer.h
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(session_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(session_plugin_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::Test
    ecat_core
)
set_target_properties(session_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME session_plugin_test COMMAND session_plugin_test)
set_tests_properties(session_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── IoVariablePlugin (unit) ─────────────────────────────────────────
add_executable(iovariable_plugin_test
    iovariable_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/iovariable/IoVariablePlugin.cpp
    ../../../apps/ecat-studio/plugins/iovariable/IoVariablePlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    TestServiceContainer.cpp
    ../../../apps/ecat-studio/services/ServiceContainer.h
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(iovariable_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(iovariable_plugin_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::Test
    ecat_core
)
set_target_properties(iovariable_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME iovariable_plugin_test COMMAND iovariable_plugin_test)
set_tests_properties(iovariable_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── FreeRunPlugin (unit) ────────────────────────────────────────────
add_executable(freerun_plugin_test
    freerun_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/freerun/FreeRunPlugin.cpp
    ../../../apps/ecat-studio/plugins/freerun/FreeRunPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    TestServiceContainer.cpp
    ../../../apps/ecat-studio/services/ServiceContainer.h
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
    ../../../apps/ecat-studio/services/FreeRunMonitoringService.cpp
    ../../../apps/ecat-studio/services/FreeRunMonitoringService.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(freerun_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(freerun_plugin_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::Test
    ecat_core
)
set_target_properties(freerun_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME freerun_plugin_test COMMAND freerun_plugin_test)
set_tests_properties(freerun_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── OdPlugin (unit) ─────────────────────────────────────────────────
add_executable(od_plugin_test
    od_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/od/OdPlugin.cpp
    ../../../apps/ecat-studio/plugins/od/OdPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    TestServiceContainer.cpp
    ../../../apps/ecat-studio/services/ServiceContainer.h
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
    ../../../apps/ecat-studio/services/SdoCacheService.cpp
    ../../../apps/ecat-studio/services/SdoCacheService.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(od_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(od_plugin_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::Test
    ecat_core
)
set_target_properties(od_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME od_plugin_test COMMAND od_plugin_test)
set_tests_properties(od_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── ConsistencyPlugin + ConsistencyTableAdapter (unit) ──────────────
add_executable(consistency_plugin_test
    consistency_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/consistency/ConsistencyPlugin.cpp
    ../../../apps/ecat-studio/plugins/consistency/ConsistencyPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../../apps/ecat-studio/adapters/ConsistencyTableAdapter.cpp
    ../../../apps/ecat-studio/adapters/ConsistencyTableAdapter.h
    TestServiceContainer.cpp
    ../../../apps/ecat-studio/services/ServiceContainer.h
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
    ../../../apps/ecat-studio/services/SdoCacheService.cpp
    ../../../apps/ecat-studio/services/SdoCacheService.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
    ../../../apps/ecat-studio/utils/TableHelpers.cpp
    ../../../apps/ecat-studio/utils/TableHelpers.h
    ../../../apps/ecat-studio/utils/TextHelpers.cpp
    ../../../apps/ecat-studio/models/ConsistencyModel.cpp
    ../../../apps/ecat-studio/models/ConsistencyModel.h
)
target_include_directories(consistency_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/adapters
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(consistency_plugin_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::Test
    ecat_core
)
set_target_properties(consistency_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME consistency_plugin_test COMMAND consistency_plugin_test)
set_tests_properties(consistency_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── TopologyPlugin + TopologyGraphWidget + SlaveNodeItem (unit) ─────
add_executable(topology_plugin_test
    topology_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/topology/TopologyPlugin.cpp
    ../../../apps/ecat-studio/plugins/topology/TopologyPlugin.h
    ../../../apps/ecat-studio/plugins/topology/TopologyGraphWidget.cpp
    ../../../apps/ecat-studio/plugins/topology/TopologyGraphWidget.h
    ../../../apps/ecat-studio/plugins/topology/SlaveNodeItem.cpp
    ../../../apps/ecat-studio/plugins/topology/SlaveNodeItem.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
)
target_include_directories(topology_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins/topology
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(topology_plugin_test PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
    ecat_core
)
set_target_properties(topology_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME topology_plugin_test COMMAND topology_plugin_test)
set_tests_properties(topology_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

	# ── Test Optimization Settings ──────────────────────────────────────
# Parallel test execution (CTest -jN)
set(CTEST_PARALLEL_LEVEL "4" CACHE STRING "Number of parallel tests")

# Test timeouts (default 120s for all tests, specific overrides above)
set(CTEST_TEST_TIMEOUT 120 CACHE STRING "Default test timeout")

# Test reporting
set(CTEST_CUSTOM_MAXIMUM_PASSED_TEST_OUTPUT_SIZE 1024)
set(CTEST_CUSTOM_MAXIMUM_FAILED_TEST_OUTPUT_SIZE 4096)

# Test labels for categorization
set_tests_properties(${RELEASE_SMOKE_TESTS} PROPERTIES LABELS "unit;smoke")

# ── PdoMappingEditorPlugin + PdoMappingCanvas + PdoMappingValidator ──
add_executable(pdomapping_plugin_test
    pdomapping_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/pdomapping/PdoMappingEditorPlugin.cpp
    ../../../apps/ecat-studio/plugins/pdomapping/PdoMappingEditorPlugin.h
    ../../../apps/ecat-studio/plugins/pdomapping/PdoMappingCanvas.cpp
    ../../../apps/ecat-studio/plugins/pdomapping/PdoMappingCanvas.h
    ../../../apps/ecat-studio/plugins/pdomapping/PdoMappingValidator.cpp
    ../../../apps/ecat-studio/plugins/pdomapping/PdoMappingValidator.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../../apps/ecat-studio/services/PdoMappingService.cpp
    ../../../apps/ecat-studio/services/PdoMappingService.h
)
target_include_directories(pdomapping_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
)
target_link_libraries(pdomapping_plugin_test PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)
set_target_properties(pdomapping_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME pdomapping_plugin_test COMMAND pdomapping_plugin_test)
set_tests_properties(pdomapping_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

add_executable(realtimeperf_plugin_test
    realtimeperf_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/realtimeperf/RealtimePerformancePlugin.cpp
    ../../../apps/ecat-studio/plugins/realtimeperf/RealtimePerformancePlugin.h
    ../../../apps/ecat-studio/plugins/realtimeperf/LatencyMonitorWidget.cpp
    ../../../apps/ecat-studio/plugins/realtimeperf/LatencyMonitorWidget.h
    ../../../apps/ecat-studio/plugins/realtimeperf/ThroughputMonitorWidget.cpp
    ../../../apps/ecat-studio/plugins/realtimeperf/ThroughputMonitorWidget.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../../apps/ecat-studio/services/RealtimePerformanceService.cpp
    ../../../apps/ecat-studio/services/RealtimePerformanceService.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(realtimeperf_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(realtimeperf_plugin_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::Test
    ecat_core
)
set_target_properties(realtimeperf_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME realtimeperf_plugin_test COMMAND realtimeperf_plugin_test)
set_tests_properties(realtimeperf_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── RealtimeOptimizerPlugin + RealtimeOptimizerService (GUI) ──────────
# RealtimeOptimizer/MultiMaster/ErrorAnalysis/HardwareVerification/
# PerformanceDashboard plugins are NOT compiled into the app build — gated.
if(ECAT_EXPERIMENTAL_SERVICES)
add_executable(realtimeoptimizer_plugin_test
    realtimeoptimizer_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/realtimeoptimizer/RealtimeOptimizerPlugin.cpp
    ../../../apps/ecat-studio/plugins/realtimeoptimizer/RealtimeOptimizerPlugin.h
    ../../../apps/ecat-studio/plugins/realtimeoptimizer/LatencyOptimizerWidget.cpp
    ../../../apps/ecat-studio/plugins/realtimeoptimizer/LatencyOptimizerWidget.h
    ../../../apps/ecat-studio/plugins/realtimeoptimizer/ThroughputOptimizerWidget.cpp
    ../../../apps/ecat-studio/plugins/realtimeoptimizer/ThroughputOptimizerWidget.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../../apps/ecat-studio/services/RealtimeOptimizerService.cpp
    ../../../apps/ecat-studio/services/RealtimeOptimizerService.h
)

target_include_directories(realtimeoptimizer_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/src/core
)

target_compile_definitions(realtimeoptimizer_plugin_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)

target_link_libraries(realtimeoptimizer_plugin_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::Test
    ecat_core
)

set_target_properties(realtimeoptimizer_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME realtimeoptimizer_plugin_test COMMAND realtimeoptimizer_plugin_test)
set_tests_properties(realtimeoptimizer_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── MultiMasterPlugin + MultiMasterService (GUI) ────────────────────
add_executable(multimaster_plugin_test
    multimaster_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/multimaster/MultiMasterPlugin.cpp
    ../../../apps/ecat-studio/plugins/multimaster/MultiMasterPlugin.h
    ../../../apps/ecat-studio/plugins/multimaster/MasterComparisonWidget.cpp
    ../../../apps/ecat-studio/plugins/multimaster/MasterComparisonWidget.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../../apps/ecat-studio/services/MultiMasterService.cpp
    ../../../apps/ecat-studio/services/MultiMasterService.h
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)

target_include_directories(multimaster_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/src/core
)

target_link_libraries(multimaster_plugin_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::Test
    ecat_core
)

set_target_properties(multimaster_plugin_test PROPERTIES AUTOMOC ON)

add_test(NAME multimaster_plugin_test COMMAND multimaster_plugin_test)
set_tests_properties(multimaster_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── AdvancedErrorAnalysisPlugin + AdvancedErrorAnalysisService (GUI) ──
add_executable(erroranalysis_plugin_test
    erroranalysis_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/erroranalysis/AdvancedErrorAnalysisPlugin.cpp
    ../../../apps/ecat-studio/plugins/erroranalysis/AdvancedErrorAnalysisPlugin.h
    ../../../apps/ecat-studio/plugins/erroranalysis/ErrorTimelineWidget.cpp
    ../../../apps/ecat-studio/plugins/erroranalysis/ErrorTimelineWidget.h
    ../../../apps/ecat-studio/plugins/erroranalysis/ErrorCorrelationWidget.cpp
    ../../../apps/ecat-studio/plugins/erroranalysis/ErrorCorrelationWidget.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../../apps/ecat-studio/services/AdvancedErrorAnalysisService.cpp
    ../../../apps/ecat-studio/services/AdvancedErrorAnalysisService.h
)

target_include_directories(erroranalysis_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
)

target_link_libraries(erroranalysis_plugin_test PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

set_target_properties(erroranalysis_plugin_test PROPERTIES AUTOMOC ON)

add_test(NAME erroranalysis_plugin_test COMMAND erroranalysis_plugin_test)
set_tests_properties(erroranalysis_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── HardwareVerificationPlugin + HardwareVerificationService (GUI) ──
add_executable(hardwareverification_plugin_test
    hardwareverification_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/hardwareverification/HardwareVerificationPlugin.cpp
    ../../../apps/ecat-studio/plugins/hardwareverification/HardwareVerificationPlugin.h
    ../../../apps/ecat-studio/plugins/hardwareverification/DeviceVerificationWidget.cpp
    ../../../apps/ecat-studio/plugins/hardwareverification/DeviceVerificationWidget.h
    ../../../apps/ecat-studio/plugins/hardwareverification/NetworkVerificationWidget.cpp
    ../../../apps/ecat-studio/plugins/hardwareverification/NetworkVerificationWidget.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../../apps/ecat-studio/services/HardwareVerificationService.cpp
    ../../../apps/ecat-studio/services/HardwareVerificationService.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)

target_include_directories(hardwareverification_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/src/core
)

target_compile_definitions(hardwareverification_plugin_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)

target_link_libraries(hardwareverification_plugin_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::Test
    ecat_core
)

set_target_properties(hardwareverification_plugin_test PROPERTIES AUTOMOC ON)

add_test(NAME hardwareverification_plugin_test COMMAND hardwareverification_plugin_test)
set_tests_properties(hardwareverification_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── PerformanceDashboardPlugin Test ─────────────────────────────────
add_executable(performance_dashboard_plugin_test
    performance_dashboard_plugin_test.cpp
    ../../../apps/ecat-studio/plugins/performancedashboard/PerformanceDashboardPlugin.cpp
    ../../../apps/ecat-studio/plugins/performancedashboard/PerformanceDashboardPlugin.h
    ../../../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../../../apps/ecat-studio/services/PerformanceMonitorService.cpp
    ../../../apps/ecat-studio/services/PerformanceMonitorService.h
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
    ../../../apps/ecat-studio/infra/EcatClient.cpp
    ../../../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(performance_dashboard_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
)
target_link_libraries(performance_dashboard_plugin_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
set_target_properties(performance_dashboard_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME performance_dashboard_plugin_test COMMAND performance_dashboard_plugin_test)
set_tests_properties(performance_dashboard_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

endif() # ECAT_EXPERIMENTAL_SERVICES (realtimeoptimizer/multimaster/erroranalysis/hardwareverification/performance_dashboard)

# Label every test in this directory as "unit" so `ctest -L unit` is meaningful
# (existing sub-labels are preserved via APPEND).
get_property(_unit_tests DIRECTORY PROPERTY TESTS)
set_property(TEST ${_unit_tests} APPEND PROPERTY LABELS "unit")
