# Unit Tests - core
# Auto-generated from monolithic tests/CMakeLists.txt

add_executable(ethercat_types_test
    ethercat_types_test.cpp
    ../../../src/core/EthercatTypes.cpp
    ../../../src/core/EthercatTypes.h
)
target_include_directories(ethercat_types_test PRIVATE ../../../src/core)
target_link_libraries(ethercat_types_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_types_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_types_test COMMAND ethercat_types_test)

add_executable(product_boundary_test
    product_boundary_test.cpp
)
target_compile_definitions(product_boundary_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)
target_link_libraries(product_boundary_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(product_boundary_test PROPERTIES AUTOMOC ON)
add_test(NAME product_boundary_test COMMAND product_boundary_test)

add_executable(io_variable_filter_test
    io_variable_filter_test.cpp
    ../../../apps/ecat-studio/models/IoVariableModel.cpp
    ../../../apps/ecat-studio/models/IoVariableModel.h
    ../../../apps/ecat-studio/models/ProcessDataRowModel.cpp
    ../../../apps/ecat-studio/models/ProcessDataRowModel.h
    ../../../apps/ecat-studio/infra/ProcessDataTypes.h
    ../../../apps/ecat-studio/models/EvidenceModel.cpp
    ../../../apps/ecat-studio/models/EvidenceModel.h
    ../../../apps/ecat-studio/utils/TextHelpers.cpp
    ../../../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(io_variable_filter_test PRIVATE
    ../../../apps/ecat-studio
)
target_link_libraries(io_variable_filter_test PRIVATE Qt6::Core)

add_test(NAME io_variable_filter_test
    COMMAND io_variable_filter_test
)

add_executable(io_variable_handoff_test
    ../../../apps/ecat-studio/models/IoVariableModel.cpp
    ../../../apps/ecat-studio/models/IoVariableModel.h
    io_variable_handoff_test.cpp
    ../../../apps/ecat-studio/models/ProcessDataRowModel.cpp
    ../../../apps/ecat-studio/models/ProcessDataRowModel.h
    ../../../apps/ecat-studio/infra/ProcessDataTypes.h
    ../../../apps/ecat-studio/models/EvidenceModel.cpp
    ../../../apps/ecat-studio/models/EvidenceModel.h
    ../../../apps/ecat-studio/utils/TextHelpers.cpp
    ../../../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(io_variable_handoff_test PRIVATE
    ../../../apps/ecat-studio
)
target_link_libraries(io_variable_handoff_test PRIVATE Qt6::Core)

add_test(NAME io_variable_handoff_test
    COMMAND io_variable_handoff_test
)

add_executable(io_variable_naming_test
    ../../../apps/ecat-studio/models/IoVariableModel.cpp
    ../../../apps/ecat-studio/models/IoVariableModel.h
    io_variable_naming_test.cpp
    ../../../apps/ecat-studio/models/ProcessDataRowModel.cpp
    ../../../apps/ecat-studio/models/ProcessDataRowModel.h
    ../../../apps/ecat-studio/infra/ProcessDataTypes.h
    ../../../apps/ecat-studio/models/EvidenceModel.cpp
    ../../../apps/ecat-studio/models/EvidenceModel.h
    ../../../apps/ecat-studio/utils/TextHelpers.cpp
    ../../../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(io_variable_naming_test PRIVATE
    ../../../apps/ecat-studio
)
target_link_libraries(io_variable_naming_test PRIVATE Qt6::Core)

add_test(NAME io_variable_naming_test
    COMMAND io_variable_naming_test
)

add_executable(topology_baseline_test
    topology_baseline_test.cpp
    ../../../apps/ecat-studio/models/TopologyModel.cpp
    ../../../apps/ecat-studio/models/TopologyModel.h
)

target_include_directories(topology_baseline_test PRIVATE
    ../../../apps/ecat-studio
    ../../../src/core
)
target_link_libraries(topology_baseline_test PRIVATE ecat_core Qt6::Core)

add_test(NAME topology_baseline_test COMMAND topology_baseline_test)

add_executable(topology_change_test
    topology_change_test.cpp
    ../../../apps/ecat-studio/models/TopologyModel.cpp
    ../../../apps/ecat-studio/models/TopologyModel.h
)

target_include_directories(topology_change_test PRIVATE
    ../../../apps/ecat-studio
    ../../../src/core
)
target_link_libraries(topology_change_test PRIVATE ecat_core Qt6::Core)

add_test(NAME topology_change_test COMMAND topology_change_test)

add_executable(state_recommendation_test
    state_recommendation_test.cpp
    ../../../apps/ecat-studio/models/EvidenceModel.cpp
    ../../../apps/ecat-studio/models/EvidenceModel.h
)

target_include_directories(state_recommendation_test PRIVATE
    ../../../apps/ecat-studio
)
target_link_libraries(state_recommendation_test PRIVATE Qt6::Core)

add_test(NAME state_recommendation_test COMMAND state_recommendation_test)

add_executable(evidence_status_test
    evidence_status_test.cpp
    ../../../apps/ecat-studio/models/EvidenceModel.cpp
    ../../../apps/ecat-studio/models/EvidenceModel.h
)

target_include_directories(evidence_status_test PRIVATE
    ../../../apps/ecat-studio
    ../../../src/core
)
target_link_libraries(evidence_status_test PRIVATE Qt6::Core)

add_test(NAME evidence_status_test COMMAND evidence_status_test)

add_executable(json_protocol_test
    json_protocol_test.cpp
    ../../../src/core/JsonProtocol.cpp
    ../../../src/core/JsonProtocol.h
)
target_include_directories(json_protocol_test PRIVATE ../../../src/core)
target_link_libraries(json_protocol_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(json_protocol_test PROPERTIES AUTOMOC ON)
add_test(NAME json_protocol_test COMMAND json_protocol_test)

add_executable(command_dispatcher_test
    command_dispatcher_test.cpp
    ../../../apps/ecatd/CommandDispatcher.cpp
    ../../../apps/ecatd/CommandDispatcher.h
    ../../../src/core/JsonProtocol.cpp
    ../../../src/core/JsonProtocol.h
)

target_include_directories(command_dispatcher_test PRIVATE
    ../../../apps/ecatd
    ../../../src/core
)
target_link_libraries(command_dispatcher_test PRIVATE Qt6::Core Qt6::Network)

add_test(NAME command_dispatcher_test COMMAND command_dispatcher_test)

add_executable(workspace_boundary_types_test
    workspace_boundary_types_test.cpp
    ../../../apps/ecat-studio/detail/WorkspaceBoundaryDetail.cpp
    ../../../apps/ecat-studio/detail/WorkspaceBoundaryDetail.h
)

target_include_directories(workspace_boundary_types_test PRIVATE
    ../../../apps/ecat-studio
)
target_link_libraries(workspace_boundary_types_test PRIVATE Qt6::Core Qt6::Test)

set_target_properties(workspace_boundary_types_test PROPERTIES AUTOMOC ON)

add_test(NAME workspace_boundary_types_test COMMAND workspace_boundary_types_test)

add_executable(event_bus_test
    event_bus_test.cpp
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
)
target_link_libraries(event_bus_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(event_bus_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
add_test(NAME event_bus_test COMMAND event_bus_test)
set_tests_properties(event_bus_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── Edge Cases / Boundary Values Test ──────────────────────────────────
add_executable(edge_cases_test
    edge_cases_test.cpp
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
    ../../../src/core/EthercatTypes.cpp
    ../../../src/core/EthercatTypes.h
)
target_link_libraries(edge_cases_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(edge_cases_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/src/core
)
set_target_properties(edge_cases_test PROPERTIES AUTOMOC ON)
add_test(NAME edge_cases_test COMMAND edge_cases_test)
set_tests_properties(edge_cases_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── AlEventHandler (daemon) ─────────────────────────────────────────
add_executable(al_event_handler_test
    al_event_handler_test.cpp
    ../../../apps/ecatd/handlers/AlEventHandler.cpp
    ../../../apps/ecatd/handlers/AlEventHandler.h
)
target_include_directories(al_event_handler_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecatd
)
target_link_libraries(al_event_handler_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(al_event_handler_test PROPERTIES AUTOMOC ON)
add_test(NAME al_event_handler_test COMMAND al_event_handler_test)

# ── AdapterHandler (daemon) ───────────────────────────────────────────
add_executable(adapter_handler_test
    adapter_handler_test.cpp
    ../../../apps/ecatd/handlers/AdapterHandler.cpp
    ../../../apps/ecatd/handlers/AdapterHandler.h
)
target_include_directories(adapter_handler_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecatd
)
target_link_libraries(adapter_handler_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(adapter_handler_test PROPERTIES AUTOMOC ON)
add_test(NAME adapter_handler_test COMMAND adapter_handler_test)

# ── DcSyncHandler (daemon) ─────────────────────────────────────────
add_executable(dc_sync_handler_test
    dc_sync_handler_test.cpp
    ../../../apps/ecatd/handlers/DcSyncHandler.cpp
    ../../../apps/ecatd/handlers/DcSyncHandler.h
    ../../../apps/ecatd/CommandDispatcher.cpp
    ../../../apps/ecatd/CommandDispatcher.h
)
target_include_directories(dc_sync_handler_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecatd
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(dc_sync_handler_test PRIVATE Qt6::Core Qt6::Test ecat_igh ethercat)
set_target_properties(dc_sync_handler_test PROPERTIES AUTOMOC ON)
add_test(NAME dc_sync_handler_test COMMAND dc_sync_handler_test)

# ── FoEHandler (daemon) ─────────────────────────────────────────────
add_executable(foe_handler_test
    foe_handler_test.cpp
    ../../../apps/ecatd/handlers/FoEHandler.cpp
    ../../../apps/ecatd/handlers/FoEHandler.h
    ../../../apps/ecatd/CommandDispatcher.cpp
    ../../../apps/ecatd/CommandDispatcher.h
    ../../../src/core/JsonProtocol.cpp
    ../../../src/core/JsonProtocol.h
)
target_include_directories(foe_handler_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecatd
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(foe_handler_test PRIVATE Qt6::Core Qt6::Network Qt6::Test ecat_igh)
set_target_properties(foe_handler_test PROPERTIES AUTOMOC ON)
add_test(NAME foe_handler_test COMMAND foe_handler_test)

# ── SignalHandler (daemon) ─────────────────────────────────────────
add_executable(signal_handler_test
    signal_handler_test.cpp
    ../../../apps/ecatd/handlers/SignalHandler.cpp
    ../../../apps/ecatd/handlers/SignalHandler.h
    ../../../apps/ecatd/CommandDispatcher.cpp
    ../../../apps/ecatd/CommandDispatcher.h
)
target_include_directories(signal_handler_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecatd
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(signal_handler_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(signal_handler_test PROPERTIES AUTOMOC ON)
add_test(NAME signal_handler_test COMMAND signal_handler_test)

# ── Empty Data Boundary Test ─────────────────────────────────────────
add_executable(empty_data_boundary_test
    empty_data_boundary_test.cpp
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
)
target_link_libraries(empty_data_boundary_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(empty_data_boundary_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/src/core
)
set_target_properties(empty_data_boundary_test PROPERTIES AUTOMOC ON)
add_test(NAME empty_data_boundary_test COMMAND empty_data_boundary_test)
set_tests_properties(empty_data_boundary_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── Large Data Boundary Test ─────────────────────────────────────────
add_executable(large_data_boundary_test
    large_data_boundary_test.cpp
    ../../../apps/ecat-studio/services/EventBus.cpp
    ../../../apps/ecat-studio/services/EventBus.h
)
target_link_libraries(large_data_boundary_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(large_data_boundary_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/src/core
)
set_target_properties(large_data_boundary_test PROPERTIES AUTOMOC ON)
add_test(NAME large_data_boundary_test COMMAND large_data_boundary_test)
set_tests_properties(large_data_boundary_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── SafetyController ─────────────────────────────────────────────────
add_executable(safety_controller_test
    safety_controller_test.cpp
    ../../../apps/ecat-studio/services/SafetyController.cpp
    ../../../apps/ecat-studio/services/SafetyController.h
)

target_include_directories(safety_controller_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)

target_link_libraries(safety_controller_test PRIVATE Qt6::Core Qt6::Test)

set_target_properties(safety_controller_test PROPERTIES AUTOMOC ON)
add_test(NAME safety_controller_test COMMAND safety_controller_test)
set_tests_properties(safety_controller_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── MemoryPool ──────────────────────────────────────────────────────
add_executable(memory_pool_test
    memory_pool_test.cpp
    ../../../apps/ecat-studio/utils/MemoryPool.h
)

target_include_directories(memory_pool_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)

target_link_libraries(memory_pool_test PRIVATE Qt6::Core Qt6::Test)

set_target_properties(memory_pool_test PROPERTIES AUTOMOC ON)
add_test(NAME memory_pool_test COMMAND memory_pool_test)

# ── Daemon Handler Test ──────────────────────────────────────────────
add_executable(daemon_handler_test
    daemon_handler_test.cpp
    ../../../apps/ecatd/EcatDaemon.cpp
    ../../../apps/ecatd/EcatDaemon.h
    ../../../apps/ecatd/CommandDispatcher.cpp
    ../../../apps/ecatd/CommandDispatcher.h
    ../../../apps/ecatd/FreeRunController.cpp
    ../../../apps/ecatd/FreeRunController.h
    ../../../apps/ecatd/RtTestController.cpp
    ../../../apps/ecatd/RtTestController.h
    ../../../apps/ecatd/handlers/AlEventHandler.cpp
    ../../../apps/ecatd/handlers/AlEventHandler.h
    ../../../apps/ecatd/handlers/DcSyncHandler.cpp
    ../../../apps/ecatd/handlers/DcSyncHandler.h
    ../../../apps/ecatd/handlers/AdapterHandler.cpp
    ../../../apps/ecatd/handlers/AdapterHandler.h
    ../../../apps/ecatd/handlers/SignalHandler.cpp
    ../../../apps/ecatd/handlers/SignalHandler.h
    ../../../apps/ecatd/handlers/FoEHandler.cpp
    ../../../apps/ecatd/handlers/FoEHandler.h
    ../../../src/core/JsonProtocol.cpp
    ../../../src/core/JsonProtocol.h
    ../../../src/core/EthercatTypes.cpp
    ../../../src/core/EthercatTypes.h
)
target_include_directories(daemon_handler_test PRIVATE
    ../../../apps/ecatd
    ../../../src/core
)
target_link_libraries(daemon_handler_test PRIVATE Qt6::Core Qt6::Network Qt6::Test ecat_igh ethercat)
set_target_properties(daemon_handler_test PROPERTIES AUTOMOC ON)
add_test(NAME daemon_handler_test COMMAND daemon_handler_test)
