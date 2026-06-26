add_executable(ethercat_types_test
    ethercat_types_test.cpp
    ../src/core/EthercatTypes.cpp
    ../src/core/EthercatTypes.h
)
target_include_directories(ethercat_types_test PRIVATE ../src/core)
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

add_executable(warnings_log_test
    warnings_log_test.cpp
)
target_link_libraries(warnings_log_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(warnings_log_test PROPERTIES AUTOMOC ON)
add_test(NAME warnings_log_test COMMAND warnings_log_test)

add_executable(sdo_evidence_model_test
    sdo_evidence_model_test.cpp
    ../apps/ecat-studio/models/SdoEvidenceModel.cpp
    ../apps/ecat-studio/models/SdoEvidenceModel.h
    ../apps/ecat-studio/utils/TextHelpers.cpp
    ../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(sdo_evidence_model_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(sdo_evidence_model_test PRIVATE Qt6::Core)

add_test(NAME sdo_evidence_model_test COMMAND sdo_evidence_model_test)

add_executable(process_data_row_model_test
    process_data_row_model_test.cpp
    ../apps/ecat-studio/models/ProcessDataRowModel.cpp
    ../apps/ecat-studio/models/ProcessDataRowModel.h
    ../apps/ecat-studio/infra/ProcessDataTypes.h
    ../apps/ecat-studio/models/EvidenceModel.cpp
    ../apps/ecat-studio/models/EvidenceModel.h
    ../apps/ecat-studio/utils/TextHelpers.cpp
    ../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(process_data_row_model_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(process_data_row_model_test PRIVATE Qt6::Core)

add_test(NAME process_data_row_model_test
    COMMAND process_data_row_model_test
)

add_executable(io_variable_detail_test
    io_variable_detail_test.cpp
    ../apps/ecat-studio/detail/IoVariableDetail.cpp
    ../apps/ecat-studio/detail/IoVariableDetail.h
    ../apps/ecat-studio/models/ProcessDataRowModel.cpp
    ../apps/ecat-studio/models/ProcessDataRowModel.h
    ../apps/ecat-studio/infra/ProcessDataTypes.h
    ../apps/ecat-studio/models/EvidenceModel.cpp
    ../apps/ecat-studio/models/EvidenceModel.h
    ../apps/ecat-studio/utils/TextHelpers.cpp
    ../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(io_variable_detail_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(io_variable_detail_test PRIVATE Qt6::Core)

add_test(NAME io_variable_detail_test
    COMMAND io_variable_detail_test
)

add_executable(free_run_entry_detail_test
    free_run_entry_detail_test.cpp
    ../apps/ecat-studio/detail/FreeRunEntryDetail.cpp
    ../apps/ecat-studio/detail/FreeRunEntryDetail.h
    ../apps/ecat-studio/infra/ProcessDataTypes.h
)

target_include_directories(free_run_entry_detail_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(free_run_entry_detail_test PRIVATE Qt6::Core)

add_test(NAME free_run_entry_detail_test
    COMMAND free_run_entry_detail_test
)

add_executable(pdo_map_detail_test
    pdo_map_detail_test.cpp
    ../apps/ecat-studio/detail/PdoMapDetail.cpp
    ../apps/ecat-studio/detail/PdoMapDetail.h
    ../apps/ecat-studio/models/ProcessDataRowModel.cpp
    ../apps/ecat-studio/models/ProcessDataRowModel.h
    ../apps/ecat-studio/infra/ProcessDataTypes.h
    ../apps/ecat-studio/models/EvidenceModel.cpp
    ../apps/ecat-studio/models/EvidenceModel.h
    ../apps/ecat-studio/utils/TextHelpers.cpp
    ../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(pdo_map_detail_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(pdo_map_detail_test PRIVATE Qt6::Core)

add_test(NAME pdo_map_detail_test
    COMMAND pdo_map_detail_test
)

add_executable(io_variable_filter_test
    io_variable_filter_test.cpp
    ../apps/ecat-studio/models/IoVariableModel.cpp
    ../apps/ecat-studio/models/IoVariableModel.h
    ../apps/ecat-studio/models/ProcessDataRowModel.cpp
    ../apps/ecat-studio/models/ProcessDataRowModel.h
    ../apps/ecat-studio/infra/ProcessDataTypes.h
    ../apps/ecat-studio/models/EvidenceModel.cpp
    ../apps/ecat-studio/models/EvidenceModel.h
    ../apps/ecat-studio/utils/TextHelpers.cpp
    ../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(io_variable_filter_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(io_variable_filter_test PRIVATE Qt6::Core)

add_test(NAME io_variable_filter_test
    COMMAND io_variable_filter_test
)

add_executable(process_data_table_adapter_test
    process_data_table_adapter_test.cpp
    ../apps/ecat-studio/adapters/ProcessDataTableAdapter.cpp
    ../apps/ecat-studio/adapters/ProcessDataTableAdapter.h
    ../apps/ecat-studio/models/ProcessDataRowModel.cpp
    ../apps/ecat-studio/models/ProcessDataRowModel.h
    ../apps/ecat-studio/infra/ProcessDataTypes.h
    ../apps/ecat-studio/models/EvidenceModel.cpp
    ../apps/ecat-studio/models/EvidenceModel.h
    ../apps/ecat-studio/utils/TableHelpers.cpp
    ../apps/ecat-studio/utils/TableHelpers.h
    ../apps/ecat-studio/utils/TextHelpers.cpp
    ../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(process_data_table_adapter_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(process_data_table_adapter_test PRIVATE
    Qt6::Core
    Qt6::Widgets
)

add_test(NAME process_data_table_adapter_test
    COMMAND process_data_table_adapter_test
)
set_tests_properties(process_data_table_adapter_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

add_executable(io_variable_handoff_test
    ../apps/ecat-studio/models/IoVariableModel.cpp
    ../apps/ecat-studio/models/IoVariableModel.h
    io_variable_handoff_test.cpp
    ../apps/ecat-studio/models/ProcessDataRowModel.cpp
    ../apps/ecat-studio/models/ProcessDataRowModel.h
    ../apps/ecat-studio/infra/ProcessDataTypes.h
    ../apps/ecat-studio/models/EvidenceModel.cpp
    ../apps/ecat-studio/models/EvidenceModel.h
    ../apps/ecat-studio/utils/TextHelpers.cpp
    ../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(io_variable_handoff_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(io_variable_handoff_test PRIVATE Qt6::Core)

add_test(NAME io_variable_handoff_test
    COMMAND io_variable_handoff_test
)

add_executable(io_variable_naming_test
    ../apps/ecat-studio/models/IoVariableModel.cpp
    ../apps/ecat-studio/models/IoVariableModel.h
    io_variable_naming_test.cpp
    ../apps/ecat-studio/models/ProcessDataRowModel.cpp
    ../apps/ecat-studio/models/ProcessDataRowModel.h
    ../apps/ecat-studio/infra/ProcessDataTypes.h
    ../apps/ecat-studio/models/EvidenceModel.cpp
    ../apps/ecat-studio/models/EvidenceModel.h
    ../apps/ecat-studio/utils/TextHelpers.cpp
    ../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(io_variable_naming_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(io_variable_naming_test PRIVATE Qt6::Core)

add_test(NAME io_variable_naming_test
    COMMAND io_variable_naming_test
)

add_executable(sdo_dictionary_table_adapter_test
    sdo_dictionary_table_adapter_test.cpp
    ../apps/ecat-studio/adapters/SdoDictionaryTableAdapter.cpp
    ../apps/ecat-studio/adapters/SdoDictionaryTableAdapter.h
    ../apps/ecat-studio/utils/TableHelpers.cpp
    ../apps/ecat-studio/utils/TableHelpers.h
    ../apps/ecat-studio/utils/TextHelpers.cpp
    ../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(sdo_dictionary_table_adapter_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(sdo_dictionary_table_adapter_test PRIVATE
    Qt6::Core
    Qt6::Widgets
)

add_test(NAME sdo_dictionary_table_adapter_test
    COMMAND sdo_dictionary_table_adapter_test
)
set_tests_properties(sdo_dictionary_table_adapter_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

add_executable(sdo_evidence_table_adapter_test
    sdo_evidence_table_adapter_test.cpp
    ../apps/ecat-studio/models/SdoEvidenceModel.cpp
    ../apps/ecat-studio/models/SdoEvidenceModel.h
    ../apps/ecat-studio/adapters/SdoEvidenceTableAdapter.cpp
    ../apps/ecat-studio/adapters/SdoEvidenceTableAdapter.h
    ../apps/ecat-studio/utils/TableHelpers.cpp
    ../apps/ecat-studio/utils/TableHelpers.h
    ../apps/ecat-studio/utils/TextHelpers.cpp
    ../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(sdo_evidence_table_adapter_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(sdo_evidence_table_adapter_test PRIVATE
    Qt6::Core
    Qt6::Widgets
)

add_test(NAME sdo_evidence_table_adapter_test
    COMMAND sdo_evidence_table_adapter_test
)
set_tests_properties(sdo_evidence_table_adapter_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

add_executable(sdo_history_row_detail_test
    sdo_history_row_detail_test.cpp
    ../apps/ecat-studio/detail/SdoHistoryRowDetail.cpp
    ../apps/ecat-studio/detail/SdoHistoryRowDetail.h
    ../apps/ecat-studio/adapters/SdoEvidenceTableAdapter.cpp
    ../apps/ecat-studio/adapters/SdoEvidenceTableAdapter.h
    ../apps/ecat-studio/models/SdoEvidenceModel.cpp
    ../apps/ecat-studio/models/SdoEvidenceModel.h
    ../apps/ecat-studio/utils/TableHelpers.cpp
    ../apps/ecat-studio/utils/TableHelpers.h
    ../apps/ecat-studio/utils/TextHelpers.cpp
    ../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(sdo_history_row_detail_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(sdo_history_row_detail_test PRIVATE
    Qt6::Core
    Qt6::Widgets
)

add_test(NAME sdo_history_row_detail_test
    COMMAND sdo_history_row_detail_test
)

add_executable(object_bookmark_detail_test
    object_bookmark_detail_test.cpp
    ../apps/ecat-studio/detail/ObjectBookmarkDetail.cpp
    ../apps/ecat-studio/detail/ObjectBookmarkDetail.h
    ../apps/ecat-studio/adapters/SdoEvidenceTableAdapter.cpp
    ../apps/ecat-studio/adapters/SdoEvidenceTableAdapter.h
    ../apps/ecat-studio/models/SdoEvidenceModel.cpp
    ../apps/ecat-studio/models/SdoEvidenceModel.h
    ../apps/ecat-studio/utils/TableHelpers.cpp
    ../apps/ecat-studio/utils/TableHelpers.h
    ../apps/ecat-studio/utils/TextHelpers.cpp
    ../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(object_bookmark_detail_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(object_bookmark_detail_test PRIVATE
    Qt6::Core
    Qt6::Widgets
)

add_test(NAME object_bookmark_detail_test
    COMMAND object_bookmark_detail_test
)

add_executable(sdo_target_trail_detail_test
    sdo_target_trail_detail_test.cpp
    ../apps/ecat-studio/detail/SdoTargetTrailDetail.cpp
    ../apps/ecat-studio/detail/SdoTargetTrailDetail.h
    ../apps/ecat-studio/adapters/SdoEvidenceTableAdapter.cpp
    ../apps/ecat-studio/adapters/SdoEvidenceTableAdapter.h
    ../apps/ecat-studio/models/SdoEvidenceModel.cpp
    ../apps/ecat-studio/models/SdoEvidenceModel.h
    ../apps/ecat-studio/utils/TableHelpers.cpp
    ../apps/ecat-studio/utils/TableHelpers.h
    ../apps/ecat-studio/utils/TextHelpers.cpp
    ../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(sdo_target_trail_detail_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(sdo_target_trail_detail_test PRIVATE
    Qt6::Core
    Qt6::Widgets
)

add_test(NAME sdo_target_trail_detail_test
    COMMAND sdo_target_trail_detail_test
)

add_executable(sdo_target_panel_route_test
    ../apps/ecat-studio/models/SdoEvidenceModel.cpp
    ../apps/ecat-studio/models/SdoEvidenceModel.h
    ../apps/ecat-studio/utils/TextHelpers.cpp
    ../apps/ecat-studio/utils/TextHelpers.h
    sdo_target_panel_route_test.cpp
)

target_include_directories(sdo_target_panel_route_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(sdo_target_panel_route_test PRIVATE Qt6::Core)

add_test(NAME sdo_target_panel_route_test
    COMMAND sdo_target_panel_route_test
)

add_executable(watch_startup_model_test
    watch_startup_model_test.cpp
    ../apps/ecat-studio/models/WatchStartupModel.cpp
    ../apps/ecat-studio/models/WatchStartupModel.h
    ../apps/ecat-studio/models/SdoEvidenceModel.cpp
    ../apps/ecat-studio/models/SdoEvidenceModel.h
    ../apps/ecat-studio/utils/TextHelpers.cpp
    ../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(watch_startup_model_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(watch_startup_model_test PRIVATE Qt6::Core)

add_test(NAME watch_startup_model_test COMMAND watch_startup_model_test)

add_executable(watch_row_detail_test
    watch_row_detail_test.cpp
    ../apps/ecat-studio/detail/WatchRowDetail.cpp
    ../apps/ecat-studio/detail/WatchRowDetail.h
    ../apps/ecat-studio/models/WatchStartupModel.h
    ../apps/ecat-studio/utils/TextHelpers.cpp
    ../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(watch_row_detail_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(watch_row_detail_test PRIVATE Qt6::Core)

add_test(NAME watch_row_detail_test
    COMMAND watch_row_detail_test
)

add_executable(startup_sdo_row_detail_test
    startup_sdo_row_detail_test.cpp
    ../apps/ecat-studio/detail/StartupSdoRowDetail.cpp
    ../apps/ecat-studio/detail/StartupSdoRowDetail.h
    ../apps/ecat-studio/models/WatchStartupModel.h
)

target_include_directories(startup_sdo_row_detail_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(startup_sdo_row_detail_test PRIVATE Qt6::Core)

add_test(NAME startup_sdo_row_detail_test
    COMMAND startup_sdo_row_detail_test
)

add_executable(topology_baseline_test
    topology_baseline_test.cpp
    ../apps/ecat-studio/models/TopologyModel.cpp
    ../apps/ecat-studio/models/TopologyModel.h
)

target_include_directories(topology_baseline_test PRIVATE
    ../apps/ecat-studio
    ../src/core
)
target_link_libraries(topology_baseline_test PRIVATE ecat_core Qt6::Core)

add_test(NAME topology_baseline_test COMMAND topology_baseline_test)

add_executable(topology_change_test
    topology_change_test.cpp
    ../apps/ecat-studio/models/TopologyModel.cpp
    ../apps/ecat-studio/models/TopologyModel.h
)

target_include_directories(topology_change_test PRIVATE
    ../apps/ecat-studio
    ../src/core
)
target_link_libraries(topology_change_test PRIVATE ecat_core Qt6::Core)

add_test(NAME topology_change_test COMMAND topology_change_test)

add_executable(state_recommendation_test
    state_recommendation_test.cpp
    ../apps/ecat-studio/models/EvidenceModel.cpp
    ../apps/ecat-studio/models/EvidenceModel.h
)

target_include_directories(state_recommendation_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(state_recommendation_test PRIVATE Qt6::Core)

add_test(NAME state_recommendation_test COMMAND state_recommendation_test)

add_executable(state_machine_row_detail_test
    state_machine_row_detail_test.cpp
    ../apps/ecat-studio/detail/StateMachineRowDetail.cpp
    ../apps/ecat-studio/detail/StateMachineRowDetail.h
    ../apps/ecat-studio/adapters/StateMachineTableAdapter.h
)

target_include_directories(state_machine_row_detail_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(state_machine_row_detail_test PRIVATE Qt6::Core)

add_test(NAME state_machine_row_detail_test
    COMMAND state_machine_row_detail_test
)

add_executable(state_machine_table_adapter_test
    state_machine_table_adapter_test.cpp
    ../apps/ecat-studio/adapters/StateMachineTableAdapter.cpp
    ../apps/ecat-studio/adapters/StateMachineTableAdapter.h
    ../apps/ecat-studio/utils/TableHelpers.cpp
    ../apps/ecat-studio/utils/TableHelpers.h
    ../apps/ecat-studio/utils/TextHelpers.cpp
    ../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(state_machine_table_adapter_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(state_machine_table_adapter_test PRIVATE
    Qt6::Core
    Qt6::Widgets
)

add_test(NAME state_machine_table_adapter_test
    COMMAND state_machine_table_adapter_test
)
set_tests_properties(state_machine_table_adapter_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

add_executable(consistency_detail_test
    consistency_detail_test.cpp
    ../apps/ecat-studio/detail/ConsistencyDetail.cpp
    ../apps/ecat-studio/detail/ConsistencyDetail.h
)

target_include_directories(consistency_detail_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(consistency_detail_test PRIVATE Qt6::Core)

add_test(NAME consistency_detail_test
    COMMAND consistency_detail_test
)

add_executable(consistency_route_model_test
    consistency_route_model_test.cpp
    ../apps/ecat-studio/detail/ConsistencyDetail.h
    ../apps/ecat-studio/models/ConsistencyModel.cpp
    ../apps/ecat-studio/models/ConsistencyModel.h
    ../apps/ecat-studio/utils/TextHelpers.cpp
    ../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(consistency_route_model_test PRIVATE
    ../apps/ecat-studio
    ../src/core
)
target_link_libraries(consistency_route_model_test PRIVATE Qt6::Core)

add_test(NAME consistency_route_model_test
    COMMAND consistency_route_model_test
)

add_executable(consistency_gate_test
    consistency_gate_test.cpp
    ../apps/ecat-studio/models/ConsistencyModel.cpp
    ../apps/ecat-studio/models/ConsistencyModel.h
    ../apps/ecat-studio/utils/TextHelpers.cpp
    ../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(consistency_gate_test PRIVATE
    ../apps/ecat-studio
    ../src/core
)
target_link_libraries(consistency_gate_test PRIVATE Qt6::Core)

add_test(NAME consistency_gate_test COMMAND consistency_gate_test)

add_executable(consistency_table_adapter_test
    consistency_table_adapter_test.cpp
    ../apps/ecat-studio/models/ConsistencyModel.cpp
    ../apps/ecat-studio/models/ConsistencyModel.h
    ../apps/ecat-studio/adapters/ConsistencyTableAdapter.cpp
    ../apps/ecat-studio/adapters/ConsistencyTableAdapter.h
    ../apps/ecat-studio/utils/TableHelpers.cpp
    ../apps/ecat-studio/utils/TableHelpers.h
    ../apps/ecat-studio/utils/TextHelpers.cpp
    ../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(consistency_table_adapter_test PRIVATE
    ../apps/ecat-studio
    ../src/core
)
target_link_libraries(consistency_table_adapter_test PRIVATE
    Qt6::Core
    Qt6::Widgets
)

add_test(NAME consistency_table_adapter_test
    COMMAND consistency_table_adapter_test
)
set_tests_properties(consistency_table_adapter_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

add_executable(evidence_status_test
    evidence_status_test.cpp
    ../apps/ecat-studio/models/EvidenceModel.cpp
    ../apps/ecat-studio/models/EvidenceModel.h
)

target_include_directories(evidence_status_test PRIVATE
    ../apps/ecat-studio
    ../src/core
)
target_link_libraries(evidence_status_test PRIVATE Qt6::Core)

add_test(NAME evidence_status_test COMMAND evidence_status_test)

add_executable(cia402_drive_model_test
    cia402_drive_model_test.cpp
    ../apps/ecat-studio/models/Cia402DriveModel.cpp
    ../apps/ecat-studio/models/Cia402DriveModel.h
)

target_include_directories(cia402_drive_model_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(cia402_drive_model_test PRIVATE Qt6::Core)

add_test(NAME cia402_drive_model_test COMMAND cia402_drive_model_test)

add_executable(table_helpers_test
    table_helpers_test.cpp
    ../apps/ecat-studio/utils/TableHelpers.cpp
    ../apps/ecat-studio/utils/TableHelpers.h
    ../apps/ecat-studio/utils/TextHelpers.cpp
    ../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(table_helpers_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(table_helpers_test PRIVATE
    Qt6::Core
    Qt6::Widgets
)

add_test(NAME table_helpers_test COMMAND table_helpers_test)
set_tests_properties(table_helpers_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

add_executable(commissioning_workflow_model_test
    commissioning_workflow_model_test.cpp
    ../apps/ecat-studio/models/CommissioningWorkflowModel.cpp
    ../apps/ecat-studio/models/CommissioningWorkflowModel.h
)

target_include_directories(commissioning_workflow_model_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(commissioning_workflow_model_test PRIVATE Qt6::Core)

add_test(NAME commissioning_workflow_model_test
    COMMAND commissioning_workflow_model_test
)

add_executable(commissioning_workflow_detail_test
    commissioning_workflow_detail_test.cpp
    ../apps/ecat-studio/models/CommissioningWorkflowModel.cpp
    ../apps/ecat-studio/models/CommissioningWorkflowModel.h
    ../apps/ecat-studio/detail/CommissioningWorkflowDetail.cpp
    ../apps/ecat-studio/detail/CommissioningWorkflowDetail.h
)

target_include_directories(commissioning_workflow_detail_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(commissioning_workflow_detail_test PRIVATE Qt6::Core)

add_test(NAME commissioning_workflow_detail_test
    COMMAND commissioning_workflow_detail_test
)

add_executable(workflow_step_detail_test
    workflow_step_detail_test.cpp
    ../apps/ecat-studio/models/CommissioningWorkflowModel.cpp
    ../apps/ecat-studio/models/CommissioningWorkflowModel.h
    ../apps/ecat-studio/detail/WorkflowStepDetail.cpp
    ../apps/ecat-studio/detail/WorkflowStepDetail.h
    ../apps/ecat-studio/adapters/WorkflowTableAdapter.h
    ../apps/ecat-studio/detail/CommissioningWorkflowDetail.cpp
    ../apps/ecat-studio/detail/CommissioningWorkflowDetail.h
)

target_include_directories(workflow_step_detail_test
    PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(workflow_step_detail_test
    PRIVATE
    Qt6::Core
)

add_test(NAME workflow_step_detail_test
    COMMAND workflow_step_detail_test
)

add_executable(next_best_action_model_test
    next_best_action_model_test.cpp
    ../apps/ecat-studio/models/CommissioningWorkflowModel.h
    ../apps/ecat-studio/models/NextBestActionModel.cpp
    ../apps/ecat-studio/models/NextBestActionModel.h
)

target_include_directories(next_best_action_model_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(next_best_action_model_test PRIVATE Qt6::Core)

add_test(NAME next_best_action_model_test
    COMMAND next_best_action_model_test
)

add_executable(next_best_action_detail_test
    next_best_action_detail_test.cpp
    ../apps/ecat-studio/models/CommissioningWorkflowModel.h
    ../apps/ecat-studio/models/NextBestActionModel.cpp
    ../apps/ecat-studio/models/NextBestActionModel.h
    ../apps/ecat-studio/detail/NextBestActionDetail.cpp
    ../apps/ecat-studio/detail/NextBestActionDetail.h
)

target_include_directories(next_best_action_detail_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(next_best_action_detail_test PRIVATE Qt6::Core)

add_test(NAME next_best_action_detail_test
    COMMAND next_best_action_detail_test
)

add_executable(host_health_detail_test
    host_health_detail_test.cpp
    ../apps/ecat-studio/detail/HostHealthDetail.cpp
    ../apps/ecat-studio/detail/HostHealthDetail.h
)

target_include_directories(host_health_detail_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(host_health_detail_test PRIVATE Qt6::Core)

add_test(NAME host_health_detail_test
    COMMAND host_health_detail_test
)

add_executable(diagnostics_event_detail_test
    diagnostics_event_detail_test.cpp
    ../apps/ecat-studio/detail/DiagnosticsEventDetail.cpp
    ../apps/ecat-studio/detail/DiagnosticsEventDetail.h
)

target_include_directories(diagnostics_event_detail_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(diagnostics_event_detail_test PRIVATE Qt6::Core)

add_test(NAME diagnostics_event_detail_test
    COMMAND diagnostics_event_detail_test
)

add_executable(workspace_tab_badge_detail_test
    workspace_tab_badge_detail_test.cpp
    ../apps/ecat-studio/detail/WorkspaceTabBadgeDetail.cpp
    ../apps/ecat-studio/detail/WorkspaceTabBadgeDetail.h
)

target_include_directories(workspace_tab_badge_detail_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(workspace_tab_badge_detail_test PRIVATE Qt6::Core)

add_test(NAME workspace_tab_badge_detail_test
    COMMAND workspace_tab_badge_detail_test
)

add_executable(workspace_tab_badge_table_adapter_test
    workspace_tab_badge_table_adapter_test.cpp
    ../apps/ecat-studio/models/ConsistencyModel.cpp
    ../apps/ecat-studio/models/ConsistencyModel.h
    ../apps/ecat-studio/adapters/ConsistencyTableAdapter.cpp
    ../apps/ecat-studio/adapters/ConsistencyTableAdapter.h
    ../apps/ecat-studio/detail/DiagnosticsEventDetail.cpp
    ../apps/ecat-studio/detail/DiagnosticsEventDetail.h
    ../apps/ecat-studio/models/EvidenceModel.cpp
    ../apps/ecat-studio/models/EvidenceModel.h
    ../apps/ecat-studio/models/ProcessDataRowModel.cpp
    ../apps/ecat-studio/models/ProcessDataRowModel.h
    ../apps/ecat-studio/adapters/ProcessDataTableAdapter.cpp
    ../apps/ecat-studio/adapters/ProcessDataTableAdapter.h
    ../apps/ecat-studio/infra/ProcessDataTypes.h
    ../apps/ecat-studio/models/SdoEvidenceModel.cpp
    ../apps/ecat-studio/models/SdoEvidenceModel.h
    ../apps/ecat-studio/adapters/SlaveEvidenceTableAdapter.cpp
    ../apps/ecat-studio/adapters/SlaveEvidenceTableAdapter.h
    ../apps/ecat-studio/models/SlaveEvidenceModel.cpp
    ../apps/ecat-studio/models/SlaveEvidenceModel.h
    ../apps/ecat-studio/utils/TableHelpers.cpp
    ../apps/ecat-studio/utils/TableHelpers.h
    ../apps/ecat-studio/utils/TextHelpers.cpp
    ../apps/ecat-studio/utils/TextHelpers.h
    ../apps/ecat-studio/models/WatchStartupModel.cpp
    ../apps/ecat-studio/models/WatchStartupModel.h
    ../apps/ecat-studio/adapters/WatchStartupTableAdapter.cpp
    ../apps/ecat-studio/adapters/WatchStartupTableAdapter.h
    ../apps/ecat-studio/adapters/WorkspaceTabBadgeTableAdapter.cpp
    ../apps/ecat-studio/adapters/WorkspaceTabBadgeTableAdapter.h
    ../apps/ecat-studio/detail/WorkspaceTabBadgeDetail.h
)

target_include_directories(workspace_tab_badge_table_adapter_test PRIVATE
    ../apps/ecat-studio
    ../src/core
)
target_link_libraries(workspace_tab_badge_table_adapter_test PRIVATE
    Qt6::Core
    Qt6::Widgets
)

add_test(NAME workspace_tab_badge_table_adapter_test
    COMMAND workspace_tab_badge_table_adapter_test
)
set_tests_properties(workspace_tab_badge_table_adapter_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

add_executable(workspace_boundary_detail_test
    workspace_boundary_detail_test.cpp
    ../apps/ecat-studio/detail/WorkspaceBoundaryDetail.cpp
    ../apps/ecat-studio/detail/WorkspaceBoundaryDetail.h
)

target_include_directories(workspace_boundary_detail_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(workspace_boundary_detail_test PRIVATE Qt6::Core)

add_test(NAME workspace_boundary_detail_test
    COMMAND workspace_boundary_detail_test
)

add_executable(workflow_table_adapter_test
    workflow_table_adapter_test.cpp
    ../apps/ecat-studio/adapters/WorkflowTableAdapter.cpp
    ../apps/ecat-studio/adapters/WorkflowTableAdapter.h
    ../apps/ecat-studio/utils/TableHelpers.cpp
    ../apps/ecat-studio/utils/TableHelpers.h
    ../apps/ecat-studio/utils/TextHelpers.cpp
    ../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(workflow_table_adapter_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(workflow_table_adapter_test PRIVATE
    Qt6::Core
    Qt6::Widgets
)

add_test(NAME workflow_table_adapter_test
    COMMAND workflow_table_adapter_test
)
set_tests_properties(workflow_table_adapter_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

add_executable(session_brief_model_test
    session_brief_model_test.cpp
    ../apps/ecat-studio/models/SessionBriefModel.cpp
    ../apps/ecat-studio/models/SessionBriefModel.h
)

target_include_directories(session_brief_model_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(session_brief_model_test PRIVATE Qt6::Core)

add_test(NAME session_brief_model_test COMMAND session_brief_model_test)

add_executable(session_brief_table_adapter_test
    session_brief_table_adapter_test.cpp
    ../apps/ecat-studio/adapters/SessionBriefTableAdapter.cpp
    ../apps/ecat-studio/adapters/SessionBriefTableAdapter.h
    ../apps/ecat-studio/utils/TableHelpers.cpp
    ../apps/ecat-studio/utils/TableHelpers.h
    ../apps/ecat-studio/utils/TextHelpers.cpp
    ../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(session_brief_table_adapter_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(session_brief_table_adapter_test PRIVATE
    Qt6::Core
    Qt6::Widgets
)

add_test(NAME session_brief_table_adapter_test
    COMMAND session_brief_table_adapter_test
)
set_tests_properties(session_brief_table_adapter_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

add_executable(session_brief_detail_test
    session_brief_detail_test.cpp
    ../apps/ecat-studio/detail/SessionBriefDetail.cpp
    ../apps/ecat-studio/detail/SessionBriefDetail.h
    ../apps/ecat-studio/models/SessionBriefModel.h
)

target_include_directories(session_brief_detail_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(session_brief_detail_test PRIVATE Qt6::Core)

add_test(NAME session_brief_detail_test COMMAND session_brief_detail_test)

add_executable(slave_evidence_model_test
    slave_evidence_model_test.cpp
    ../apps/ecat-studio/models/SlaveEvidenceModel.cpp
    ../apps/ecat-studio/models/SlaveEvidenceModel.h
)

target_include_directories(slave_evidence_model_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(slave_evidence_model_test PRIVATE Qt6::Core)

add_test(NAME slave_evidence_model_test COMMAND slave_evidence_model_test)

add_executable(selected_drive_summary_detail_test
    selected_drive_summary_detail_test.cpp
    ../apps/ecat-studio/models/Cia402DriveModel.cpp
    ../apps/ecat-studio/models/Cia402DriveModel.h
    ../apps/ecat-studio/models/EvidenceModel.cpp
    ../apps/ecat-studio/models/EvidenceModel.h
    ../apps/ecat-studio/detail/SelectedDriveSummaryDetail.cpp
    ../apps/ecat-studio/detail/SelectedDriveSummaryDetail.h
    ../apps/ecat-studio/models/WatchStartupModel.h
)

target_include_directories(selected_drive_summary_detail_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(selected_drive_summary_detail_test PRIVATE Qt6::Core)

add_test(NAME selected_drive_summary_detail_test
    COMMAND selected_drive_summary_detail_test
)

add_executable(slave_evidence_summary_detail_test
    slave_evidence_summary_detail_test.cpp
    ../apps/ecat-studio/detail/SlaveEvidenceSummaryDetail.cpp
    ../apps/ecat-studio/detail/SlaveEvidenceSummaryDetail.h
    ../apps/ecat-studio/models/SlaveEvidenceModel.h
)

target_include_directories(slave_evidence_summary_detail_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(slave_evidence_summary_detail_test PRIVATE
    Qt6::Core
)

add_test(NAME slave_evidence_summary_detail_test
    COMMAND slave_evidence_summary_detail_test
)

add_executable(slave_evidence_detail_test
    slave_evidence_detail_test.cpp
    ../apps/ecat-studio/detail/SlaveEvidenceDetail.cpp
    ../apps/ecat-studio/detail/SlaveEvidenceDetail.h
    ../apps/ecat-studio/models/SlaveEvidenceModel.cpp
    ../apps/ecat-studio/models/SlaveEvidenceModel.h
)

target_include_directories(slave_evidence_detail_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(slave_evidence_detail_test PRIVATE Qt6::Core)

add_test(NAME slave_evidence_detail_test COMMAND slave_evidence_detail_test)

add_executable(slave_evidence_table_adapter_test
    slave_evidence_table_adapter_test.cpp
    ../apps/ecat-studio/adapters/SlaveEvidenceTableAdapter.cpp
    ../apps/ecat-studio/adapters/SlaveEvidenceTableAdapter.h
    ../apps/ecat-studio/models/SlaveEvidenceModel.cpp
    ../apps/ecat-studio/models/SlaveEvidenceModel.h
    ../apps/ecat-studio/models/EvidenceModel.cpp
    ../apps/ecat-studio/models/EvidenceModel.h
    ../apps/ecat-studio/utils/TableHelpers.cpp
    ../apps/ecat-studio/utils/TableHelpers.h
    ../apps/ecat-studio/utils/TextHelpers.cpp
    ../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(slave_evidence_table_adapter_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(slave_evidence_table_adapter_test PRIVATE
    Qt6::Core
    Qt6::Widgets
)

add_test(NAME slave_evidence_table_adapter_test
    COMMAND slave_evidence_table_adapter_test
)
set_tests_properties(slave_evidence_table_adapter_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

add_executable(json_protocol_test
    json_protocol_test.cpp
    ../src/core/JsonProtocol.cpp
    ../src/core/JsonProtocol.h
)
target_include_directories(json_protocol_test PRIVATE ../src/core)
target_link_libraries(json_protocol_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(json_protocol_test PROPERTIES AUTOMOC ON)
add_test(NAME json_protocol_test COMMAND json_protocol_test)

set(RELEASE_SMOKE_TESTS
    sdo_evidence_model_test
    process_data_row_model_test
    evidence_status_test
    cia402_drive_model_test
    watch_startup_model_test
    selected_drive_summary_detail_test
    slave_evidence_summary_detail_test
    workspace_tab_badge_detail_test
    workspace_boundary_detail_test
    session_brief_model_test
    slave_evidence_model_test
    consistency_gate_test
    state_recommendation_test
    topology_baseline_test
    topology_change_test
)
set_tests_properties(${RELEASE_SMOKE_TESTS} PROPERTIES LABELS release_smoke)

add_custom_target(release-smoke
    COMMAND ${CMAKE_CTEST_COMMAND}
            --test-dir ${CMAKE_BINARY_DIR}
            --output-on-failure
            -L release_smoke
    DEPENDS
        sdo_evidence_model_test
        process_data_row_model_test
        evidence_status_test
        cia402_drive_model_test
        watch_startup_model_test
        selected_drive_summary_detail_test
        slave_evidence_summary_detail_test
        workspace_tab_badge_detail_test
        workspace_boundary_detail_test
        session_brief_model_test
        slave_evidence_model_test
        consistency_gate_test
        state_recommendation_test
        topology_baseline_test
        topology_change_test
    USES_TERMINAL
)

add_executable(native_backend_test
    native_backend_test.cpp
)

target_include_directories(native_backend_test PRIVATE
    ../src/igh
    ../src/core
)

target_link_libraries(native_backend_test PRIVATE
    Qt6::Core
    Qt6::Test
    ecat_igh
    ethercat
)

set_target_properties(native_backend_test PROPERTIES AUTOMOC ON)
add_test(NAME native_backend_test COMMAND native_backend_test)

add_executable(native_backend_integration_test
    native_backend_integration_test.cpp
)

target_include_directories(native_backend_integration_test PRIVATE
    ../src/igh
    ../src/core
)

target_link_libraries(native_backend_integration_test PRIVATE
    Qt6::Core
    Qt6::Test
    ecat_igh
    ethercat
)

set_target_properties(native_backend_integration_test PROPERTIES AUTOMOC ON)
add_test(NAME native_backend_integration_test COMMAND native_backend_integration_test)

add_executable(command_dispatcher_test
    command_dispatcher_test.cpp
    ../apps/ecatd/CommandDispatcher.cpp
    ../apps/ecatd/CommandDispatcher.h
    ../src/core/JsonProtocol.cpp
    ../src/core/JsonProtocol.h
)

target_include_directories(command_dispatcher_test PRIVATE
    ../apps/ecatd
    ../src/core
)
target_link_libraries(command_dispatcher_test PRIVATE Qt6::Core Qt6::Network)

add_test(NAME command_dispatcher_test COMMAND command_dispatcher_test)

add_executable(protocol_integration_test
    protocol_integration_test.cpp
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
    ../apps/ecatd/CommandDispatcher.cpp
    ../apps/ecatd/CommandDispatcher.h
    ../src/core/JsonProtocol.cpp
    ../src/core/JsonProtocol.h
    ../src/core/EthercatTypes.cpp
    ../src/core/EthercatTypes.h
)

target_include_directories(protocol_integration_test PRIVATE
    ../apps/ecatd
    ../apps/ecat-studio/infra
    ../src/core
)
target_link_libraries(protocol_integration_test PRIVATE Qt6::Core Qt6::Network)

set_target_properties(protocol_integration_test PROPERTIES AUTOMOC ON)

add_test(NAME protocol_integration_test COMMAND protocol_integration_test)

add_executable(connection_lifecycle_test
    connection_lifecycle_test.cpp
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
    ../apps/ecatd/CommandDispatcher.cpp
    ../apps/ecatd/CommandDispatcher.h
    ../src/core/JsonProtocol.cpp
    ../src/core/JsonProtocol.h
    ../src/core/EthercatTypes.cpp
    ../src/core/EthercatTypes.h
)

target_include_directories(connection_lifecycle_test PRIVATE
    ../apps/ecatd
    ../apps/ecat-studio/infra
    ../src/core
)
target_link_libraries(connection_lifecycle_test PRIVATE Qt6::Core Qt6::Network)

set_target_properties(connection_lifecycle_test PROPERTIES AUTOMOC ON)

add_test(NAME connection_lifecycle_test COMMAND connection_lifecycle_test)

add_executable(ecat_client_methods_test
    ecat_client_methods_test.cpp
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
    ../src/core/JsonProtocol.cpp
    ../src/core/JsonProtocol.h
    ../src/core/EthercatTypes.cpp
    ../src/core/EthercatTypes.h
)
target_include_directories(ecat_client_methods_test PRIVATE
    ../apps/ecat-studio/infra
    ../src/core
)
target_link_libraries(ecat_client_methods_test PRIVATE Qt6::Core Qt6::Network Qt6::Test)
set_target_properties(ecat_client_methods_test PROPERTIES AUTOMOC ON)
add_test(NAME ecat_client_methods_test COMMAND ecat_client_methods_test)

add_executable(language_manager_test
    language_manager_test.cpp
    ../apps/ecat-studio/infra/LanguageManager.cpp
    ../apps/ecat-studio/infra/LanguageManager.h
)

target_include_directories(language_manager_test PRIVATE
    ../apps/ecat-studio/infra
)
target_link_libraries(language_manager_test PRIVATE Qt6::Core Qt6::Test)

set_target_properties(language_manager_test PROPERTIES AUTOMOC ON)

add_test(NAME language_manager_test COMMAND language_manager_test)

add_executable(workspace_widgets_test
    workspace_widgets_test.cpp
)

target_include_directories(workspace_widgets_test PRIVATE
    ../apps/ecat-studio
    ../apps/ecat-studio/workspaces
)
target_link_libraries(workspace_widgets_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)

set_target_properties(workspace_widgets_test PROPERTIES AUTOMOC ON)

add_test(NAME workspace_widgets_test COMMAND workspace_widgets_test)
set_tests_properties(workspace_widgets_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

add_executable(workspace_boundary_types_test
    workspace_boundary_types_test.cpp
    ../apps/ecat-studio/detail/WorkspaceBoundaryDetail.cpp
    ../apps/ecat-studio/detail/WorkspaceBoundaryDetail.h
)

target_include_directories(workspace_boundary_types_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(workspace_boundary_types_test PRIVATE Qt6::Core Qt6::Test)

set_target_properties(workspace_boundary_types_test PROPERTIES AUTOMOC ON)

add_test(NAME workspace_boundary_types_test COMMAND workspace_boundary_types_test)

add_executable(settings_persistence_test
    settings_persistence_test.cpp
    ../apps/ecat-studio/infra/SettingsDialog.cpp
    ../apps/ecat-studio/infra/LanguageManager.cpp
    ../apps/ecat-studio/infra/LanguageManager.h
    ../apps/ecat-studio/infra/SettingsDialog.h
)

target_include_directories(settings_persistence_test PRIVATE
    ../apps/ecat-studio/infra
)
target_link_libraries(settings_persistence_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)

set_target_properties(settings_persistence_test PROPERTIES AUTOMOC ON)

add_test(NAME settings_persistence_test COMMAND settings_persistence_test)
set_tests_properties(settings_persistence_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── Workspace plugin interface ──────────────────────────────────────────
add_executable(workspace_plugin_interface_test
    workspace_plugin_interface_test.cpp
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
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

add_executable(event_bus_test
    event_bus_test.cpp
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
)
target_link_libraries(event_bus_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(event_bus_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
add_test(NAME event_bus_test COMMAND event_bus_test)
set_tests_properties(event_bus_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── Edge Cases / Boundary Values Test ──────────────────────────────────
add_executable(edge_cases_test
    edge_cases_test.cpp
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../src/core/EthercatTypes.cpp
    ../src/core/EthercatTypes.h
)
target_link_libraries(edge_cases_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(edge_cases_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/src/core
)
set_target_properties(edge_cases_test PROPERTIES AUTOMOC ON)
add_test(NAME edge_cases_test COMMAND edge_cases_test)
set_tests_properties(edge_cases_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── EventBus Integration Test ─────────────────────────────────────────
add_executable(event_bus_integration_test
    event_bus_integration_test.cpp
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
)
target_link_libraries(event_bus_integration_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(event_bus_integration_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/src/core
)
set_target_properties(event_bus_integration_test PROPERTIES AUTOMOC ON)
add_test(NAME event_bus_integration_test COMMAND event_bus_integration_test)
set_tests_properties(event_bus_integration_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── PluginRegistry ─────────────────────────────────────────────────────
add_executable(plugin_registry_test
    plugin_registry_test.cpp
    ../apps/ecat-studio/plugins/PluginRegistry.cpp
    ../apps/ecat-studio/plugins/PluginRegistry.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
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
    ../apps/ecat-studio/plugins/notes/NotesPlugin.cpp
    ../apps/ecat-studio/plugins/notes/NotesPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(notes_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(notes_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(notes_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME notes_plugin_test COMMAND notes_plugin_test)
set_tests_properties(notes_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── AlEventHandler (daemon) ─────────────────────────────────────────
add_executable(al_event_handler_test
    al_event_handler_test.cpp
    ../apps/ecatd/handlers/AlEventHandler.cpp
    ../apps/ecatd/handlers/AlEventHandler.h
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
    ../apps/ecatd/handlers/AdapterHandler.cpp
    ../apps/ecatd/handlers/AdapterHandler.h
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
    ../apps/ecatd/handlers/DcSyncHandler.cpp
    ../apps/ecatd/handlers/DcSyncHandler.h
    ../apps/ecatd/CommandDispatcher.cpp
    ../apps/ecatd/CommandDispatcher.h
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
    ../apps/ecatd/handlers/FoEHandler.cpp
    ../apps/ecatd/handlers/FoEHandler.h
    ../apps/ecatd/CommandDispatcher.cpp
    ../apps/ecatd/CommandDispatcher.h
    ../src/core/JsonProtocol.cpp
    ../src/core/JsonProtocol.h
)
target_include_directories(foe_handler_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecatd
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(foe_handler_test PRIVATE Qt6::Core Qt6::Network Qt6::Test ecat_igh)
set_target_properties(foe_handler_test PROPERTIES AUTOMOC ON)
add_test(NAME foe_handler_test COMMAND foe_handler_test)

# ── DcSyncPlugin + DcSyncService (GUI) ──────────────────────────────
add_executable(dc_sync_service_test
    dc_sync_service_test.cpp
    ../apps/ecat-studio/plugins/dcsync/DcSyncPlugin.cpp
    ../apps/ecat-studio/plugins/dcsync/DcSyncPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/DcSyncService.cpp
    ../apps/ecat-studio/services/DcSyncService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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

# ── DcSyncOptimizerPlugin + DcSyncOptimizerService ─────────────────────
add_executable(dcsyncoptimizer_plugin_test
    dcsyncoptimizer_plugin_test.cpp
    ../apps/ecat-studio/plugins/dcsyncoptimizer/DcSyncOptimizerPlugin.cpp
    ../apps/ecat-studio/plugins/dcsyncoptimizer/DcSyncOptimizerPlugin.h
    ../apps/ecat-studio/plugins/dcsyncoptimizer/SyncOptimizerWidget.cpp
    ../apps/ecat-studio/plugins/dcsyncoptimizer/SyncOptimizerWidget.h
    ../apps/ecat-studio/plugins/dcsyncoptimizer/DriftOptimizerWidget.cpp
    ../apps/ecat-studio/plugins/dcsyncoptimizer/DriftOptimizerWidget.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/DcSyncOptimizerService.cpp
    ../apps/ecat-studio/services/DcSyncOptimizerService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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

add_test(NAME dcsyncoptimizer_plugin_test COMMAND dcsyncoptimizer_plugin_test)
set_tests_properties(dcsyncoptimizer_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── PdoMappingOptimizationPlugin + PdoMappingOptimizationService ────────
add_executable(pdomappingoptimization_plugin_test
    pdomappingoptimization_plugin_test.cpp
    ../apps/ecat-studio/plugins/pdomappingoptimization/PdoMappingOptimizationPlugin.cpp
    ../apps/ecat-studio/plugins/pdomappingoptimization/PdoMappingOptimizationPlugin.h
    ../apps/ecat-studio/plugins/pdomappingoptimization/MappingOptimizerWidget.cpp
    ../apps/ecat-studio/plugins/pdomappingoptimization/MappingOptimizerWidget.h
    ../apps/ecat-studio/plugins/pdomappingoptimization/SizeOptimizerWidget.cpp
    ../apps/ecat-studio/plugins/pdomappingoptimization/SizeOptimizerWidget.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/PdoMappingOptimizationService.cpp
    ../apps/ecat-studio/services/PdoMappingOptimizationService.h
)

target_include_directories(pdomappingoptimization_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins/pdomappingoptimization
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
    ../apps/ecat-studio/plugins/realtimeoptimizer/LatencyOptimizerWidget.cpp
    ../apps/ecat-studio/plugins/realtimeoptimizer/LatencyOptimizerWidget.h
    ../apps/ecat-studio/plugins/realtimeoptimizer/ThroughputOptimizerWidget.cpp
    ../apps/ecat-studio/plugins/realtimeoptimizer/ThroughputOptimizerWidget.h
    ../apps/ecat-studio/plugins/erroranalysis/ErrorTimelineWidget.cpp
    ../apps/ecat-studio/plugins/erroranalysis/ErrorTimelineWidget.h
    ../apps/ecat-studio/services/EtherCATOptimizerService.h
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

# ── AlEventPlugin + AlEventService (GUI) ──────────────────────────────
add_executable(al_event_service_test
    al_event_service_test.cpp
    ../apps/ecat-studio/plugins/alevent/AlEventPlugin.cpp
    ../apps/ecat-studio/plugins/alevent/AlEventPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/AlEventService.cpp
    ../apps/ecat-studio/services/AlEventService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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

# ── SignalHandler (daemon) ─────────────────────────────────────────
add_executable(signal_handler_test
    signal_handler_test.cpp
    ../apps/ecatd/handlers/SignalHandler.cpp
    ../apps/ecatd/handlers/SignalHandler.h
    ../apps/ecatd/CommandDispatcher.cpp
    ../apps/ecatd/CommandDispatcher.h
)
target_include_directories(signal_handler_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecatd
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(signal_handler_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(signal_handler_test PROPERTIES AUTOMOC ON)
add_test(NAME signal_handler_test COMMAND signal_handler_test)

# ── SignalPlugin + SignalService (GUI) ────────────────────────────────
add_executable(signal_service_test
    signal_service_test.cpp
    ../apps/ecat-studio/plugins/signal/SignalPlugin.cpp
    ../apps/ecat-studio/plugins/signal/SignalPlugin.h
    ../apps/ecat-studio/plugins/signal/SignalChartWidget.cpp
    ../apps/ecat-studio/plugins/signal/SignalChartWidget.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/SignalService.cpp
    ../apps/ecat-studio/services/SignalService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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

# ── Plugin Integration Test ─────────────────────────────────────────
add_executable(plugin_integration_test
    plugin_integration_test.cpp
    ../apps/ecat-studio/plugins/PluginRegistry.cpp
    ../apps/ecat-studio/plugins/PluginRegistry.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/plugins/notes/NotesPlugin.cpp
    ../apps/ecat-studio/plugins/notes/NotesPlugin.h
    ../apps/ecat-studio/plugins/statemachine/StateMachinePlugin.cpp
    ../apps/ecat-studio/plugins/statemachine/StateMachinePlugin.h
    ../apps/ecat-studio/services/ServiceContainer.cpp
    ../apps/ecat-studio/services/SdoOptimizationService.cpp
    ../apps/ecat-studio/services/SdoOptimizationService.h
    ../apps/ecat-studio/services/PdoMappingOptimizationService.cpp
    ../apps/ecat-studio/services/PdoMappingOptimizationService.h
    ../apps/ecat-studio/services/HardwareVerificationService.cpp
    ../apps/ecat-studio/services/HardwareVerificationService.h
    ../apps/ecat-studio/services/AdvancedErrorAnalysisService.cpp
    ../apps/ecat-studio/services/AdvancedErrorAnalysisService.h
    ../apps/ecat-studio/services/FreeRunConfigurationService.cpp
    ../apps/ecat-studio/services/FreeRunConfigurationService.h
    ../apps/ecat-studio/services/FreeRunMonitoringService.cpp
    ../apps/ecat-studio/services/FreeRunMonitoringService.h
    ../apps/ecat-studio/services/PdoConfigurationService.cpp
    ../apps/ecat-studio/services/PdoConfigurationService.h
    ../apps/ecat-studio/services/OpStateService.cpp
    ../apps/ecat-studio/services/OpStateService.h
    ../apps/ecat-studio/plugins/oscilloscope/OscilloscopeService.cpp
    ../apps/ecat-studio/plugins/oscilloscope/OscilloscopeService.h
    ../apps/ecat-studio/plugins/protocol/ProtocolAnalyzerService.cpp
    ../apps/ecat-studio/plugins/protocol/ProtocolAnalyzerService.h
    ../apps/ecat-studio/services/WorkflowAnalyticsService.cpp
    ../apps/ecat-studio/services/WorkflowAnalyticsService.h
    ../apps/ecat-studio/services/FreeRunOptimizationService.cpp
    ../apps/ecat-studio/services/FreeRunOptimizationService.h
    ../apps/ecat-studio/services/RealtimePerformanceService.cpp
    ../apps/ecat-studio/services/RealtimePerformanceService.h
    ../apps/ecat-studio/services/RealtimeOptimizerService.cpp
    ../apps/ecat-studio/services/RealtimeOptimizerService.h
    ../apps/ecat-studio/services/ScriptingService.cpp
    ../apps/ecat-studio/services/ScriptingService.h
    ../apps/ecat-studio/services/ProjectManagementService.cpp
    ../apps/ecat-studio/services/ProjectManagementService.h
    ../apps/ecat-studio/services/TaskManagementService.cpp
    ../apps/ecat-studio/services/TaskManagementService.h
    ../apps/ecat-studio/services/ResourceManagementService.cpp
    ../apps/ecat-studio/services/ResourceManagementService.h
    ../apps/ecat-studio/services/ProjectPlanningService.cpp
    ../apps/ecat-studio/services/ProjectPlanningService.h
    ../apps/ecat-studio/services/ProjectTrackingService.cpp
    ../apps/ecat-studio/services/ProjectTrackingService.h
    ../apps/ecat-studio/services/ProjectReportingService.cpp
    ../apps/ecat-studio/services/ProjectReportingService.h
    ../apps/ecat-studio/services/WorkflowAutomationService.cpp
    ../apps/ecat-studio/services/WorkflowAutomationService.h
    ../apps/ecat-studio/services/WorkflowSchedulingService.cpp
    ../apps/ecat-studio/services/WorkflowSchedulingService.h
    ../apps/ecat-studio/services/WorkflowMonitoringService.cpp
    ../apps/ecat-studio/services/WorkflowMonitoringService.h
    ../apps/ecat-studio/services/WorkflowVisualizationService.cpp
    ../apps/ecat-studio/services/WorkflowVisualizationService.h
    ../apps/ecat-studio/services/WorkflowReportingService.cpp
    ../apps/ecat-studio/services/WorkflowReportingService.h
    ../apps/ecat-studio/services/WorkflowIntegrationService.cpp
    ../apps/ecat-studio/services/WorkflowIntegrationService.h
    ../apps/ecat-studio/services/WorkflowSecurityService.cpp
    ../apps/ecat-studio/services/WorkflowSecurityService.h
    ../apps/ecat-studio/services/WorkflowUpdateService.cpp
    ../apps/ecat-studio/services/WorkflowUpdateService.h
    ../apps/ecat-studio/services/WorkflowMaintenanceService.cpp
    ../apps/ecat-studio/services/WorkflowMaintenanceService.h
    ../apps/ecat-studio/services/WorkflowIntegrationHubService.cpp
    ../apps/ecat-studio/services/WorkflowIntegrationHubService.h
    ../apps/ecat-studio/services/WorkflowSyncService.cpp
    ../apps/ecat-studio/services/WorkflowSyncService.h
    ../apps/ecat-studio/services/WorkflowVisualizationStudioService.cpp
    ../apps/ecat-studio/services/WorkflowVisualizationStudioService.h
    ../apps/ecat-studio/services/WorkflowReportDesignerService.cpp
    ../apps/ecat-studio/services/WorkflowReportDesignerService.h
    ../apps/ecat-studio/services/WorkflowDocumentationBrowserService.cpp
    ../apps/ecat-studio/services/WorkflowDocumentationBrowserService.h
    ../apps/ecat-studio/services/WorkflowSecurityManagerService.cpp
    ../apps/ecat-studio/services/WorkflowSecurityManagerService.h
    ../apps/ecat-studio/services/WorkflowComplianceManagerService.cpp
    ../apps/ecat-studio/services/WorkflowComplianceManagerService.h
    ../apps/ecat-studio/services/WorkflowCertificationManagerService.cpp
    ../apps/ecat-studio/services/WorkflowCertificationManagerService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/services/SdoService.cpp
    ../apps/ecat-studio/services/SdoService.h
    ../apps/ecat-studio/services/WatchService.cpp
    ../apps/ecat-studio/services/WatchService.h
    ../apps/ecat-studio/services/TopologyService.cpp
    ../apps/ecat-studio/services/TopologyService.h
    ../apps/ecat-studio/services/DcSyncService.cpp
    ../apps/ecat-studio/services/DcSyncService.h
    ../apps/ecat-studio/services/AlEventService.cpp
    ../apps/ecat-studio/services/AlEventService.h
    ../apps/ecat-studio/services/SignalService.cpp
    ../apps/ecat-studio/services/SignalService.h
    ../apps/ecat-studio/services/PerformanceMonitorService.cpp
    ../apps/ecat-studio/services/PerformanceMonitorService.h
    ../apps/ecat-studio/services/EsiService.cpp
    ../apps/ecat-studio/services/EsiService.h
    ../apps/ecat-studio/services/BusStatsService.cpp
    ../apps/ecat-studio/services/BusStatsService.h
    ../apps/ecat-studio/services/WatchdogService.cpp
    ../apps/ecat-studio/services/WatchdogService.h
    ../apps/ecat-studio/services/SafetyController.cpp
    ../apps/ecat-studio/services/SafetyController.h
    ../apps/ecat-studio/services/DiagnosticReportService.cpp
    ../apps/ecat-studio/services/DiagnosticReportService.h
    ../apps/ecat-studio/services/ProjectManagerService.cpp
    ../apps/ecat-studio/services/ProjectManagerService.h
    ../apps/ecat-studio/services/ConfigurationService.cpp
    ../apps/ecat-studio/services/ConfigurationService.h
    ../apps/ecat-studio/services/AlarmService.cpp
    ../apps/ecat-studio/services/AlarmService.h
    ../apps/ecat-studio/services/LoggingService.cpp
    ../apps/ecat-studio/services/LoggingService.h
    ../apps/ecat-studio/services/ChartService.cpp
    ../apps/ecat-studio/services/ChartService.h
    ../apps/ecat-studio/services/BatchOperationService.cpp
    ../apps/ecat-studio/services/BatchOperationService.h
    ../apps/ecat-studio/services/NetworkDiagnosticsService.cpp
    ../apps/ecat-studio/services/NetworkDiagnosticsService.h
    ../apps/ecat-studio/services/EcatHealthService.cpp
    ../apps/ecat-studio/services/EcatHealthService.h
    ../apps/ecat-studio/services/ExportService.cpp
    ../apps/ecat-studio/services/ExportService.h
    ../apps/ecat-studio/services/DataPipelineService.cpp
    ../apps/ecat-studio/services/DataPipelineService.h
    ../apps/ecat-studio/services/DeviceManagerService.cpp
    ../apps/ecat-studio/services/DeviceManagerService.h
    ../apps/ecat-studio/services/FirmwareUpdateService.cpp
    ../apps/ecat-studio/services/FirmwareUpdateService.h
    ../apps/ecat-studio/services/ReportGeneratorService.cpp
    ../apps/ecat-studio/services/ReportGeneratorService.h
    ../apps/ecat-studio/services/MasterManagerService.cpp
    ../apps/ecat-studio/services/MasterManagerService.h
    ../apps/ecat-studio/services/DistributedClockService.cpp
    ../apps/ecat-studio/services/DistributedClockService.h
    ../apps/ecat-studio/services/DcSyncPrecisionService.cpp
    ../apps/ecat-studio/services/DcSyncPrecisionService.h
    ../apps/ecat-studio/services/DcSyncOptimizerService.cpp
    ../apps/ecat-studio/services/DcSyncOptimizerService.h
    ../apps/ecat-studio/services/OnlineDiagnosticsService.cpp
    ../apps/ecat-studio/services/OnlineDiagnosticsService.h
    ../apps/ecat-studio/services/MultiMasterService.cpp
    ../apps/ecat-studio/services/MultiMasterService.h
    ../apps/ecat-studio/services/SdoCacheService.cpp
    ../apps/ecat-studio/services/SdoCacheService.h
    ../apps/ecat-studio/services/PdoMappingService.cpp
    ../apps/ecat-studio/services/PdoMappingService.h
    ../apps/ecat-studio/services/CoEService.cpp
    ../apps/ecat-studio/services/CoEService.h
    ../apps/ecat-studio/services/FoEService.cpp
    ../apps/ecat-studio/services/FoEService.h
    ../apps/ecat-studio/services/EoEService.cpp
    ../apps/ecat-studio/services/EoEService.h
    ../apps/ecat-studio/services/TraceService.cpp
    ../apps/ecat-studio/services/TraceService.h
    ../apps/ecat-studio/services/MasterApiService.cpp
    ../apps/ecat-studio/services/MasterApiService.h
    ../apps/ecat-studio/services/DomainService.cpp
    ../apps/ecat-studio/services/DomainService.h
    ../apps/ecat-studio/services/SyncManagerService.cpp
    ../apps/ecat-studio/services/SyncManagerService.h
    ../apps/ecat-studio/services/StateMachineService.cpp
    ../apps/ecat-studio/services/StateMachineService.h
    ../apps/ecat-studio/services/ErrorHandlingService.cpp
    ../apps/ecat-studio/services/ErrorHandlingService.h
    ../apps/ecat-studio/services/DiagnosticService.cpp
    ../apps/ecat-studio/services/DiagnosticService.h
    ../apps/ecat-studio/services/HotConnectService.cpp
    ../apps/ecat-studio/services/HotConnectService.h
    ../apps/ecat-studio/services/RedundancyService.cpp
    ../apps/ecat-studio/services/RedundancyService.h
    ../apps/ecat-studio/services/CableDiagnosticsService.cpp
    ../apps/ecat-studio/services/CableDiagnosticsService.h
    ../apps/ecat-studio/services/EtherCATMonitorService.cpp
    ../apps/ecat-studio/services/EtherCATMonitorService.h
    ../apps/ecat-studio/services/EtherCATAnalyzerService.cpp
    ../apps/ecat-studio/services/EtherCATAnalyzerService.h
    ../apps/ecat-studio/services/EtherCATOptimizerService.cpp
    ../apps/ecat-studio/services/EtherCATOptimizerService.h
    ../apps/ecat-studio/services/EtherCATConfigService.cpp
    ../apps/ecat-studio/services/EtherCATConfigService.h
    ../apps/ecat-studio/services/EtherCATBackupService.cpp
    ../apps/ecat-studio/services/EtherCATBackupService.h
    ../apps/ecat-studio/services/EtherCATRecoveryService.cpp
    ../apps/ecat-studio/services/EtherCATRecoveryService.h
    ../apps/ecat-studio/services/EtherCATSimulationService.cpp
    ../apps/ecat-studio/services/EtherCATSimulationService.h
    ../apps/ecat-studio/services/EtherCATTestingService.cpp
    ../apps/ecat-studio/services/EtherCATTestingService.h
    ../apps/ecat-studio/services/EtherCATValidationService.cpp
    ../apps/ecat-studio/services/EtherCATValidationService.h
    ../apps/ecat-studio/services/EtherCATSecurityService.cpp
    ../apps/ecat-studio/services/EtherCATSecurityService.h
    ../apps/ecat-studio/services/EtherCATComplianceService.cpp
    ../apps/ecat-studio/services/EtherCATComplianceService.h
    ../apps/ecat-studio/services/EtherCATCertificationService.cpp
    ../apps/ecat-studio/services/EtherCATCertificationService.h
    ../apps/ecat-studio/services/EtherCATSecurityService.cpp
    ../apps/ecat-studio/services/EtherCATSecurityService.h
    ../apps/ecat-studio/services/EtherCATComplianceService.cpp
    ../apps/ecat-studio/services/EtherCATComplianceService.h
    ../apps/ecat-studio/services/EtherCATCertificationService.cpp
    ../apps/ecat-studio/services/EtherCATCertificationService.h
    ../apps/ecat-studio/services/EtherCATAnalyticsService.cpp
    ../apps/ecat-studio/services/EtherCATAnalyticsService.h
    ../apps/ecat-studio/services/EtherCATDeploymentService.cpp
    ../apps/ecat-studio/services/EtherCATDeploymentService.h
    ../apps/ecat-studio/services/EtherCATUpdateService.cpp
    ../apps/ecat-studio/services/EtherCATUpdateService.h
    ../apps/ecat-studio/services/EtherCATMaintenanceService.cpp
    ../apps/ecat-studio/services/EtherCATMaintenanceService.h
    ../apps/ecat-studio/services/EtherCATIntegrationService.cpp
    ../apps/ecat-studio/services/EtherCATIntegrationService.h
    ../apps/ecat-studio/services/EtherCATSyncService.cpp
    ../apps/ecat-studio/services/EtherCATSyncService.h
    ../apps/ecat-studio/services/EtherCATReplicationService.cpp
    ../apps/ecat-studio/services/EtherCATReplicationService.h
    ../apps/ecat-studio/services/EtherCATVisualizationService.cpp
    ../apps/ecat-studio/services/EtherCATVisualizationService.h
    ../apps/ecat-studio/services/EtherCATReportingService.cpp
    ../apps/ecat-studio/services/EtherCATReportingService.h
    ../apps/ecat-studio/services/EtherCATDocumentationService.cpp
    ../apps/ecat-studio/services/EtherCATDocumentationService.h
    ../apps/ecat-studio/services/ProjectManagementService.cpp
    ../apps/ecat-studio/services/ProjectManagementService.h
    ../apps/ecat-studio/services/TaskManagementService.cpp
    ../apps/ecat-studio/services/TaskManagementService.h
    ../apps/ecat-studio/services/ResourceManagementService.cpp
    ../apps/ecat-studio/services/ResourceManagementService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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
    ../apps/ecat-studio/services/ServiceContainer.cpp
    ../apps/ecat-studio/services/SdoOptimizationService.cpp
    ../apps/ecat-studio/services/SdoOptimizationService.h
    ../apps/ecat-studio/services/PdoMappingOptimizationService.cpp
    ../apps/ecat-studio/services/PdoMappingOptimizationService.h
    ../apps/ecat-studio/services/HardwareVerificationService.cpp
    ../apps/ecat-studio/services/HardwareVerificationService.h
    ../apps/ecat-studio/services/AdvancedErrorAnalysisService.cpp
    ../apps/ecat-studio/services/AdvancedErrorAnalysisService.h
    ../apps/ecat-studio/services/FreeRunConfigurationService.cpp
    ../apps/ecat-studio/services/FreeRunConfigurationService.h
    ../apps/ecat-studio/services/FreeRunMonitoringService.cpp
    ../apps/ecat-studio/services/FreeRunMonitoringService.h
    ../apps/ecat-studio/services/PdoConfigurationService.cpp
    ../apps/ecat-studio/services/PdoConfigurationService.h
    ../apps/ecat-studio/services/OpStateService.cpp
    ../apps/ecat-studio/services/OpStateService.h
    ../apps/ecat-studio/plugins/oscilloscope/OscilloscopeService.cpp
    ../apps/ecat-studio/plugins/oscilloscope/OscilloscopeService.h
    ../apps/ecat-studio/plugins/protocol/ProtocolAnalyzerService.cpp
    ../apps/ecat-studio/plugins/protocol/ProtocolAnalyzerService.h
    ../apps/ecat-studio/services/WorkflowAnalyticsService.cpp
    ../apps/ecat-studio/services/WorkflowAnalyticsService.h
    ../apps/ecat-studio/services/FreeRunOptimizationService.cpp
    ../apps/ecat-studio/services/FreeRunOptimizationService.h
    ../apps/ecat-studio/services/RealtimePerformanceService.cpp
    ../apps/ecat-studio/services/RealtimePerformanceService.h
    ../apps/ecat-studio/services/RealtimeOptimizerService.cpp
    ../apps/ecat-studio/services/RealtimeOptimizerService.h
    ../apps/ecat-studio/services/ScriptingService.cpp
    ../apps/ecat-studio/services/ScriptingService.h
    ../apps/ecat-studio/services/ProjectManagementService.cpp
    ../apps/ecat-studio/services/ProjectManagementService.h
    ../apps/ecat-studio/services/TaskManagementService.cpp
    ../apps/ecat-studio/services/TaskManagementService.h
    ../apps/ecat-studio/services/ResourceManagementService.cpp
    ../apps/ecat-studio/services/ResourceManagementService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/services/SdoService.cpp
    ../apps/ecat-studio/services/SdoService.h
    ../apps/ecat-studio/services/WatchService.cpp
    ../apps/ecat-studio/services/WatchService.h
    ../apps/ecat-studio/services/TopologyService.cpp
    ../apps/ecat-studio/services/TopologyService.h
    ../apps/ecat-studio/services/DcSyncService.cpp
    ../apps/ecat-studio/services/DcSyncService.h
    ../apps/ecat-studio/services/AlEventService.cpp
    ../apps/ecat-studio/services/AlEventService.h
    ../apps/ecat-studio/services/SignalService.cpp
    ../apps/ecat-studio/services/SignalService.h
    ../apps/ecat-studio/services/PerformanceMonitorService.cpp
    ../apps/ecat-studio/services/PerformanceMonitorService.h
    ../apps/ecat-studio/services/EsiService.cpp
    ../apps/ecat-studio/services/EsiService.h
    ../apps/ecat-studio/services/BusStatsService.cpp
    ../apps/ecat-studio/services/BusStatsService.h
    ../apps/ecat-studio/services/WatchdogService.cpp
    ../apps/ecat-studio/services/WatchdogService.h
    ../apps/ecat-studio/services/SafetyController.cpp
    ../apps/ecat-studio/services/SafetyController.h
    ../apps/ecat-studio/services/DiagnosticReportService.cpp
    ../apps/ecat-studio/services/DiagnosticReportService.h
    ../apps/ecat-studio/services/ProjectManagerService.cpp
    ../apps/ecat-studio/services/ProjectManagerService.h
    ../apps/ecat-studio/services/ConfigurationService.cpp
    ../apps/ecat-studio/services/ConfigurationService.h
    ../apps/ecat-studio/services/AlarmService.cpp
    ../apps/ecat-studio/services/AlarmService.h
    ../apps/ecat-studio/services/LoggingService.cpp
    ../apps/ecat-studio/services/LoggingService.h
    ../apps/ecat-studio/services/ChartService.cpp
    ../apps/ecat-studio/services/ChartService.h
    ../apps/ecat-studio/services/BatchOperationService.cpp
    ../apps/ecat-studio/services/BatchOperationService.h
    ../apps/ecat-studio/services/NetworkDiagnosticsService.cpp
    ../apps/ecat-studio/services/NetworkDiagnosticsService.h
    ../apps/ecat-studio/services/EcatHealthService.cpp
    ../apps/ecat-studio/services/EcatHealthService.h
    ../apps/ecat-studio/services/ExportService.cpp
    ../apps/ecat-studio/services/ExportService.h
    ../apps/ecat-studio/services/DataPipelineService.cpp
    ../apps/ecat-studio/services/DataPipelineService.h
    ../apps/ecat-studio/services/DeviceManagerService.cpp
    ../apps/ecat-studio/services/DeviceManagerService.h
    ../apps/ecat-studio/services/FirmwareUpdateService.cpp
    ../apps/ecat-studio/services/FirmwareUpdateService.h
    ../apps/ecat-studio/services/ReportGeneratorService.cpp
    ../apps/ecat-studio/services/ReportGeneratorService.h
    ../apps/ecat-studio/services/MasterManagerService.cpp
    ../apps/ecat-studio/services/MasterManagerService.h
    ../apps/ecat-studio/services/DistributedClockService.cpp
    ../apps/ecat-studio/services/DistributedClockService.h
    ../apps/ecat-studio/services/DcSyncPrecisionService.cpp
    ../apps/ecat-studio/services/DcSyncPrecisionService.h
    ../apps/ecat-studio/services/DcSyncOptimizerService.cpp
    ../apps/ecat-studio/services/DcSyncOptimizerService.h
    ../apps/ecat-studio/services/OnlineDiagnosticsService.cpp
    ../apps/ecat-studio/services/OnlineDiagnosticsService.h
    ../apps/ecat-studio/services/MultiMasterService.cpp
    ../apps/ecat-studio/services/MultiMasterService.h
    ../apps/ecat-studio/services/SdoCacheService.cpp
    ../apps/ecat-studio/services/SdoCacheService.h
    ../apps/ecat-studio/services/PdoMappingService.cpp
    ../apps/ecat-studio/services/PdoMappingService.h
    ../apps/ecat-studio/services/CoEService.cpp
    ../apps/ecat-studio/services/CoEService.h
    ../apps/ecat-studio/services/FoEService.cpp
    ../apps/ecat-studio/services/FoEService.h
    ../apps/ecat-studio/services/EoEService.cpp
    ../apps/ecat-studio/services/EoEService.h
    ../apps/ecat-studio/services/TraceService.cpp
    ../apps/ecat-studio/services/TraceService.h
    ../apps/ecat-studio/services/MasterApiService.cpp
    ../apps/ecat-studio/services/MasterApiService.h
    ../apps/ecat-studio/services/DomainService.cpp
    ../apps/ecat-studio/services/DomainService.h
    ../apps/ecat-studio/services/SyncManagerService.cpp
    ../apps/ecat-studio/services/SyncManagerService.h
    ../apps/ecat-studio/services/StateMachineService.cpp
    ../apps/ecat-studio/services/StateMachineService.h
    ../apps/ecat-studio/services/ErrorHandlingService.cpp
    ../apps/ecat-studio/services/ErrorHandlingService.h
    ../apps/ecat-studio/services/DiagnosticService.cpp
    ../apps/ecat-studio/services/DiagnosticService.h
    ../apps/ecat-studio/services/HotConnectService.cpp
    ../apps/ecat-studio/services/HotConnectService.h
    ../apps/ecat-studio/services/RedundancyService.cpp
    ../apps/ecat-studio/services/RedundancyService.h
    ../apps/ecat-studio/services/CableDiagnosticsService.cpp
    ../apps/ecat-studio/services/CableDiagnosticsService.h
    ../apps/ecat-studio/services/EtherCATMonitorService.cpp
    ../apps/ecat-studio/services/EtherCATMonitorService.h
    ../apps/ecat-studio/services/EtherCATAnalyzerService.cpp
    ../apps/ecat-studio/services/EtherCATAnalyzerService.h
    ../apps/ecat-studio/services/EtherCATOptimizerService.cpp
    ../apps/ecat-studio/services/EtherCATOptimizerService.h
    ../apps/ecat-studio/services/EtherCATConfigService.cpp
    ../apps/ecat-studio/services/EtherCATConfigService.h
    ../apps/ecat-studio/services/EtherCATBackupService.cpp
    ../apps/ecat-studio/services/EtherCATBackupService.h
    ../apps/ecat-studio/services/EtherCATRecoveryService.cpp
    ../apps/ecat-studio/services/EtherCATRecoveryService.h
    ../apps/ecat-studio/services/EtherCATSimulationService.cpp
    ../apps/ecat-studio/services/EtherCATSimulationService.h
    ../apps/ecat-studio/services/EtherCATTestingService.cpp
    ../apps/ecat-studio/services/EtherCATTestingService.h
    ../apps/ecat-studio/services/EtherCATValidationService.cpp
    ../apps/ecat-studio/services/EtherCATValidationService.h
    ../apps/ecat-studio/services/EtherCATSecurityService.cpp
    ../apps/ecat-studio/services/EtherCATSecurityService.h
    ../apps/ecat-studio/services/EtherCATComplianceService.cpp
    ../apps/ecat-studio/services/EtherCATComplianceService.h
    ../apps/ecat-studio/services/EtherCATCertificationService.cpp
    ../apps/ecat-studio/services/EtherCATCertificationService.h
    ../apps/ecat-studio/services/EtherCATSecurityService.cpp
    ../apps/ecat-studio/services/EtherCATSecurityService.h
    ../apps/ecat-studio/services/EtherCATComplianceService.cpp
    ../apps/ecat-studio/services/EtherCATComplianceService.h
    ../apps/ecat-studio/services/EtherCATCertificationService.cpp
    ../apps/ecat-studio/services/EtherCATCertificationService.h
    ../apps/ecat-studio/services/EtherCATAnalyticsService.cpp
    ../apps/ecat-studio/services/EtherCATAnalyticsService.h
    ../apps/ecat-studio/services/EtherCATDeploymentService.cpp
    ../apps/ecat-studio/services/EtherCATDeploymentService.h
    ../apps/ecat-studio/services/EtherCATUpdateService.cpp
    ../apps/ecat-studio/services/EtherCATUpdateService.h
    ../apps/ecat-studio/services/EtherCATMaintenanceService.cpp
    ../apps/ecat-studio/services/EtherCATMaintenanceService.h
    ../apps/ecat-studio/services/EtherCATIntegrationService.cpp
    ../apps/ecat-studio/services/EtherCATIntegrationService.h
    ../apps/ecat-studio/services/EtherCATSyncService.cpp
    ../apps/ecat-studio/services/EtherCATSyncService.h
    ../apps/ecat-studio/services/EtherCATReplicationService.cpp
    ../apps/ecat-studio/services/EtherCATReplicationService.h
    ../apps/ecat-studio/services/EtherCATVisualizationService.cpp
    ../apps/ecat-studio/services/EtherCATVisualizationService.h
    ../apps/ecat-studio/services/EtherCATReportingService.cpp
    ../apps/ecat-studio/services/EtherCATReportingService.h
    ../apps/ecat-studio/services/EtherCATDocumentationService.cpp
    ../apps/ecat-studio/services/EtherCATDocumentationService.h
    ../apps/ecat-studio/services/ProjectManagementService.cpp
    ../apps/ecat-studio/services/ProjectManagementService.h
    ../apps/ecat-studio/services/TaskManagementService.cpp
    ../apps/ecat-studio/services/TaskManagementService.h
    ../apps/ecat-studio/services/ResourceManagementService.cpp
    ../apps/ecat-studio/services/ResourceManagementService.h
    ../apps/ecat-studio/services/ProjectPlanningService.cpp
    ../apps/ecat-studio/services/ProjectPlanningService.h
    ../apps/ecat-studio/services/ProjectTrackingService.cpp
    ../apps/ecat-studio/services/ProjectTrackingService.h
    ../apps/ecat-studio/services/ProjectReportingService.cpp
    ../apps/ecat-studio/services/ProjectReportingService.h
    ../apps/ecat-studio/services/WorkflowAutomationService.cpp
    ../apps/ecat-studio/services/WorkflowAutomationService.h
    ../apps/ecat-studio/services/WorkflowSchedulingService.cpp
    ../apps/ecat-studio/services/WorkflowSchedulingService.h
    ../apps/ecat-studio/services/WorkflowMonitoringService.cpp
    ../apps/ecat-studio/services/WorkflowMonitoringService.h
    ../apps/ecat-studio/services/WorkflowVisualizationService.cpp
    ../apps/ecat-studio/services/WorkflowVisualizationService.h
    ../apps/ecat-studio/services/WorkflowReportingService.cpp
    ../apps/ecat-studio/services/WorkflowReportingService.h
    ../apps/ecat-studio/services/WorkflowIntegrationService.cpp
    ../apps/ecat-studio/services/WorkflowIntegrationService.h
    ../apps/ecat-studio/services/WorkflowSecurityService.cpp
    ../apps/ecat-studio/services/WorkflowSecurityService.h
    ../apps/ecat-studio/services/WorkflowUpdateService.cpp
    ../apps/ecat-studio/services/WorkflowUpdateService.h
    ../apps/ecat-studio/services/WorkflowMaintenanceService.cpp
    ../apps/ecat-studio/services/WorkflowMaintenanceService.h
    ../apps/ecat-studio/services/WorkflowIntegrationHubService.cpp
    ../apps/ecat-studio/services/WorkflowIntegrationHubService.h
    ../apps/ecat-studio/services/WorkflowSyncService.cpp
    ../apps/ecat-studio/services/WorkflowSyncService.h
    ../apps/ecat-studio/services/WorkflowVisualizationStudioService.cpp
    ../apps/ecat-studio/services/WorkflowVisualizationStudioService.h
    ../apps/ecat-studio/services/WorkflowReportDesignerService.cpp
    ../apps/ecat-studio/services/WorkflowReportDesignerService.h
    ../apps/ecat-studio/services/WorkflowDocumentationBrowserService.cpp
    ../apps/ecat-studio/services/WorkflowDocumentationBrowserService.h
    ../apps/ecat-studio/services/WorkflowSecurityManagerService.cpp
    ../apps/ecat-studio/services/WorkflowSecurityManagerService.h
    ../apps/ecat-studio/services/WorkflowComplianceManagerService.cpp
    ../apps/ecat-studio/services/WorkflowComplianceManagerService.h
    ../apps/ecat-studio/services/WorkflowCertificationManagerService.cpp
    ../apps/ecat-studio/services/WorkflowCertificationManagerService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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

# ── Service Cascade Integration Test ────────────────────────────────
add_executable(service_cascade_test
    service_cascade_test.cpp
    ../apps/ecat-studio/services/SdoService.cpp
    ../apps/ecat-studio/services/SdoService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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

# ── Empty Data Boundary Test ─────────────────────────────────────────
add_executable(empty_data_boundary_test
    empty_data_boundary_test.cpp
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
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
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
)
target_link_libraries(large_data_boundary_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(large_data_boundary_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/src/core
)
set_target_properties(large_data_boundary_test PROPERTIES AUTOMOC ON)
add_test(NAME large_data_boundary_test COMMAND large_data_boundary_test)
set_tests_properties(large_data_boundary_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── Concurrent Access Test ─────────────────────────────────────────
add_executable(concurrent_access_test
    concurrent_access_test.cpp
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
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
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
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
    ../apps/ecat-studio/plugins/notes/NotesPlugin.cpp
    ../apps/ecat-studio/plugins/notes/NotesPlugin.h
    ../apps/ecat-studio/plugins/statemachine/StateMachinePlugin.cpp
    ../apps/ecat-studio/plugins/statemachine/StateMachinePlugin.h
    ../apps/ecat-studio/plugins/session/SessionPlugin.cpp
    ../apps/ecat-studio/plugins/session/SessionPlugin.h
    ../apps/ecat-studio/plugins/overview/OverviewPlugin.cpp
    ../apps/ecat-studio/plugins/overview/OverviewPlugin.h
    ../apps/ecat-studio/plugins/iovariable/IoVariablePlugin.cpp
    ../apps/ecat-studio/plugins/iovariable/IoVariablePlugin.h
    ../apps/ecat-studio/plugins/rttest/RtTestPlugin.cpp
    ../apps/ecat-studio/plugins/rttest/RtTestPlugin.h
    ../apps/ecat-studio/plugins/watch/WatchPlugin.cpp
    ../apps/ecat-studio/plugins/watch/WatchPlugin.h
    ../apps/ecat-studio/plugins/export/ExportPlugin.cpp
    ../apps/ecat-studio/plugins/export/ExportPlugin.h
    ../apps/ecat-studio/plugins/od/OdPlugin.cpp
    ../apps/ecat-studio/plugins/od/OdPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/utils/TableHelpers.cpp
    ../apps/ecat-studio/utils/TableHelpers.h
    ../apps/ecat-studio/utils/TextHelpers.cpp
    ../apps/ecat-studio/utils/TextHelpers.h
    ../apps/ecat-studio/services/ServiceContainer.cpp
    ../apps/ecat-studio/services/SdoOptimizationService.cpp
    ../apps/ecat-studio/services/SdoOptimizationService.h
    ../apps/ecat-studio/services/PdoMappingOptimizationService.cpp
    ../apps/ecat-studio/services/PdoMappingOptimizationService.h
    ../apps/ecat-studio/services/HardwareVerificationService.cpp
    ../apps/ecat-studio/services/HardwareVerificationService.h
    ../apps/ecat-studio/services/AdvancedErrorAnalysisService.cpp
    ../apps/ecat-studio/services/AdvancedErrorAnalysisService.h
    ../apps/ecat-studio/services/FreeRunConfigurationService.cpp
    ../apps/ecat-studio/services/FreeRunConfigurationService.h
    ../apps/ecat-studio/services/FreeRunMonitoringService.cpp
    ../apps/ecat-studio/services/FreeRunMonitoringService.h
    ../apps/ecat-studio/services/PdoConfigurationService.cpp
    ../apps/ecat-studio/services/PdoConfigurationService.h
    ../apps/ecat-studio/services/OpStateService.cpp
    ../apps/ecat-studio/services/OpStateService.h
    ../apps/ecat-studio/plugins/oscilloscope/OscilloscopeService.cpp
    ../apps/ecat-studio/plugins/oscilloscope/OscilloscopeService.h
    ../apps/ecat-studio/plugins/protocol/ProtocolAnalyzerService.cpp
    ../apps/ecat-studio/plugins/protocol/ProtocolAnalyzerService.h
    ../apps/ecat-studio/services/WorkflowAnalyticsService.cpp
    ../apps/ecat-studio/services/WorkflowAnalyticsService.h
    ../apps/ecat-studio/services/FreeRunOptimizationService.cpp
    ../apps/ecat-studio/services/FreeRunOptimizationService.h
    ../apps/ecat-studio/services/RealtimePerformanceService.cpp
    ../apps/ecat-studio/services/RealtimePerformanceService.h
    ../apps/ecat-studio/services/RealtimeOptimizerService.cpp
    ../apps/ecat-studio/services/RealtimeOptimizerService.h
    ../apps/ecat-studio/services/ScriptingService.cpp
    ../apps/ecat-studio/services/ScriptingService.h
    ../apps/ecat-studio/services/ProjectManagementService.cpp
    ../apps/ecat-studio/services/ProjectManagementService.h
    ../apps/ecat-studio/services/TaskManagementService.cpp
    ../apps/ecat-studio/services/TaskManagementService.h
    ../apps/ecat-studio/services/ResourceManagementService.cpp
    ../apps/ecat-studio/services/ResourceManagementService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/services/SdoService.cpp
    ../apps/ecat-studio/services/SdoService.h
    ../apps/ecat-studio/services/WatchService.cpp
    ../apps/ecat-studio/services/WatchService.h
    ../apps/ecat-studio/services/TopologyService.cpp
    ../apps/ecat-studio/services/TopologyService.h
    ../apps/ecat-studio/services/DcSyncService.cpp
    ../apps/ecat-studio/services/DcSyncService.h
    ../apps/ecat-studio/services/AlEventService.cpp
    ../apps/ecat-studio/services/AlEventService.h
    ../apps/ecat-studio/services/SignalService.cpp
    ../apps/ecat-studio/services/SignalService.h
    ../apps/ecat-studio/services/PerformanceMonitorService.cpp
    ../apps/ecat-studio/services/PerformanceMonitorService.h
    ../apps/ecat-studio/services/EsiService.cpp
    ../apps/ecat-studio/services/EsiService.h
    ../apps/ecat-studio/services/BusStatsService.cpp
    ../apps/ecat-studio/services/BusStatsService.h
    ../apps/ecat-studio/services/WatchdogService.cpp
    ../apps/ecat-studio/services/WatchdogService.h
    ../apps/ecat-studio/services/SafetyController.cpp
    ../apps/ecat-studio/services/SafetyController.h
    ../apps/ecat-studio/services/DiagnosticReportService.cpp
    ../apps/ecat-studio/services/DiagnosticReportService.h
    ../apps/ecat-studio/services/ProjectManagerService.cpp
    ../apps/ecat-studio/services/ProjectManagerService.h
    ../apps/ecat-studio/services/ConfigurationService.cpp
    ../apps/ecat-studio/services/ConfigurationService.h
    ../apps/ecat-studio/services/AlarmService.cpp
    ../apps/ecat-studio/services/AlarmService.h
    ../apps/ecat-studio/services/LoggingService.cpp
    ../apps/ecat-studio/services/LoggingService.h
    ../apps/ecat-studio/services/ChartService.cpp
    ../apps/ecat-studio/services/ChartService.h
    ../apps/ecat-studio/services/BatchOperationService.cpp
    ../apps/ecat-studio/services/BatchOperationService.h
    ../apps/ecat-studio/services/NetworkDiagnosticsService.cpp
    ../apps/ecat-studio/services/NetworkDiagnosticsService.h
    ../apps/ecat-studio/services/EcatHealthService.cpp
    ../apps/ecat-studio/services/EcatHealthService.h
    ../apps/ecat-studio/services/ExportService.cpp
    ../apps/ecat-studio/services/ExportService.h
    ../apps/ecat-studio/services/DataPipelineService.cpp
    ../apps/ecat-studio/services/DataPipelineService.h
    ../apps/ecat-studio/services/DeviceManagerService.cpp
    ../apps/ecat-studio/services/DeviceManagerService.h
    ../apps/ecat-studio/services/FirmwareUpdateService.cpp
    ../apps/ecat-studio/services/FirmwareUpdateService.h
    ../apps/ecat-studio/services/ReportGeneratorService.cpp
    ../apps/ecat-studio/services/ReportGeneratorService.h
    ../apps/ecat-studio/services/MasterManagerService.cpp
    ../apps/ecat-studio/services/MasterManagerService.h
    ../apps/ecat-studio/services/DistributedClockService.cpp
    ../apps/ecat-studio/services/DistributedClockService.h
    ../apps/ecat-studio/services/DcSyncPrecisionService.cpp
    ../apps/ecat-studio/services/DcSyncPrecisionService.h
    ../apps/ecat-studio/services/DcSyncOptimizerService.cpp
    ../apps/ecat-studio/services/DcSyncOptimizerService.h
    ../apps/ecat-studio/services/OnlineDiagnosticsService.cpp
    ../apps/ecat-studio/services/OnlineDiagnosticsService.h
    ../apps/ecat-studio/services/MultiMasterService.cpp
    ../apps/ecat-studio/services/MultiMasterService.h
    ../apps/ecat-studio/services/SdoCacheService.cpp
    ../apps/ecat-studio/services/SdoCacheService.h
    ../apps/ecat-studio/services/PdoMappingService.cpp
    ../apps/ecat-studio/services/PdoMappingService.h
    ../apps/ecat-studio/services/CoEService.cpp
    ../apps/ecat-studio/services/CoEService.h
    ../apps/ecat-studio/services/FoEService.cpp
    ../apps/ecat-studio/services/FoEService.h
    ../apps/ecat-studio/services/EoEService.cpp
    ../apps/ecat-studio/services/EoEService.h
    ../apps/ecat-studio/services/TraceService.cpp
    ../apps/ecat-studio/services/TraceService.h
    ../apps/ecat-studio/services/MasterApiService.cpp
    ../apps/ecat-studio/services/MasterApiService.h
    ../apps/ecat-studio/services/DomainService.cpp
    ../apps/ecat-studio/services/DomainService.h
    ../apps/ecat-studio/services/SyncManagerService.cpp
    ../apps/ecat-studio/services/SyncManagerService.h
    ../apps/ecat-studio/services/StateMachineService.cpp
    ../apps/ecat-studio/services/StateMachineService.h
    ../apps/ecat-studio/services/ErrorHandlingService.cpp
    ../apps/ecat-studio/services/ErrorHandlingService.h
    ../apps/ecat-studio/services/DiagnosticService.cpp
    ../apps/ecat-studio/services/DiagnosticService.h
    ../apps/ecat-studio/services/HotConnectService.cpp
    ../apps/ecat-studio/services/HotConnectService.h
    ../apps/ecat-studio/services/RedundancyService.cpp
    ../apps/ecat-studio/services/RedundancyService.h
    ../apps/ecat-studio/services/CableDiagnosticsService.cpp
    ../apps/ecat-studio/services/CableDiagnosticsService.h
    ../apps/ecat-studio/services/EtherCATMonitorService.cpp
    ../apps/ecat-studio/services/EtherCATMonitorService.h
    ../apps/ecat-studio/services/EtherCATAnalyzerService.cpp
    ../apps/ecat-studio/services/EtherCATAnalyzerService.h
    ../apps/ecat-studio/services/EtherCATOptimizerService.cpp
    ../apps/ecat-studio/services/EtherCATOptimizerService.h
    ../apps/ecat-studio/services/EtherCATConfigService.cpp
    ../apps/ecat-studio/services/EtherCATConfigService.h
    ../apps/ecat-studio/services/EtherCATBackupService.cpp
    ../apps/ecat-studio/services/EtherCATBackupService.h
    ../apps/ecat-studio/services/EtherCATRecoveryService.cpp
    ../apps/ecat-studio/services/EtherCATRecoveryService.h
    ../apps/ecat-studio/services/EtherCATSimulationService.cpp
    ../apps/ecat-studio/services/EtherCATSimulationService.h
    ../apps/ecat-studio/services/EtherCATTestingService.cpp
    ../apps/ecat-studio/services/EtherCATTestingService.h
    ../apps/ecat-studio/services/EtherCATValidationService.cpp
    ../apps/ecat-studio/services/EtherCATValidationService.h
    ../apps/ecat-studio/services/EtherCATSecurityService.cpp
    ../apps/ecat-studio/services/EtherCATSecurityService.h
    ../apps/ecat-studio/services/EtherCATComplianceService.cpp
    ../apps/ecat-studio/services/EtherCATComplianceService.h
    ../apps/ecat-studio/services/EtherCATCertificationService.cpp
    ../apps/ecat-studio/services/EtherCATCertificationService.h
    ../apps/ecat-studio/services/EtherCATSecurityService.cpp
    ../apps/ecat-studio/services/EtherCATSecurityService.h
    ../apps/ecat-studio/services/EtherCATComplianceService.cpp
    ../apps/ecat-studio/services/EtherCATComplianceService.h
    ../apps/ecat-studio/services/EtherCATCertificationService.cpp
    ../apps/ecat-studio/services/EtherCATCertificationService.h
    ../apps/ecat-studio/services/EtherCATAnalyticsService.cpp
    ../apps/ecat-studio/services/EtherCATAnalyticsService.h
    ../apps/ecat-studio/services/EtherCATDeploymentService.cpp
    ../apps/ecat-studio/services/EtherCATDeploymentService.h
    ../apps/ecat-studio/services/EtherCATUpdateService.cpp
    ../apps/ecat-studio/services/EtherCATUpdateService.h
    ../apps/ecat-studio/services/EtherCATMaintenanceService.cpp
    ../apps/ecat-studio/services/EtherCATMaintenanceService.h
    ../apps/ecat-studio/services/EtherCATIntegrationService.cpp
    ../apps/ecat-studio/services/EtherCATIntegrationService.h
    ../apps/ecat-studio/services/EtherCATSyncService.cpp
    ../apps/ecat-studio/services/EtherCATSyncService.h
    ../apps/ecat-studio/services/EtherCATReplicationService.cpp
    ../apps/ecat-studio/services/EtherCATReplicationService.h
    ../apps/ecat-studio/services/EtherCATVisualizationService.cpp
    ../apps/ecat-studio/services/EtherCATVisualizationService.h
    ../apps/ecat-studio/services/EtherCATReportingService.cpp
    ../apps/ecat-studio/services/EtherCATReportingService.h
    ../apps/ecat-studio/services/EtherCATDocumentationService.cpp
    ../apps/ecat-studio/services/EtherCATDocumentationService.h
    ../apps/ecat-studio/services/ProjectManagementService.cpp
    ../apps/ecat-studio/services/ProjectManagementService.h
    ../apps/ecat-studio/services/TaskManagementService.cpp
    ../apps/ecat-studio/services/TaskManagementService.h
    ../apps/ecat-studio/services/ResourceManagementService.cpp
    ../apps/ecat-studio/services/ResourceManagementService.h
    ../apps/ecat-studio/services/ProjectPlanningService.cpp
    ../apps/ecat-studio/services/ProjectPlanningService.h
    ../apps/ecat-studio/services/ProjectTrackingService.cpp
    ../apps/ecat-studio/services/ProjectTrackingService.h
    ../apps/ecat-studio/services/ProjectReportingService.cpp
    ../apps/ecat-studio/services/ProjectReportingService.h
    ../apps/ecat-studio/services/WorkflowAutomationService.cpp
    ../apps/ecat-studio/services/WorkflowAutomationService.h
    ../apps/ecat-studio/services/WorkflowSchedulingService.cpp
    ../apps/ecat-studio/services/WorkflowSchedulingService.h
    ../apps/ecat-studio/services/WorkflowMonitoringService.cpp
    ../apps/ecat-studio/services/WorkflowMonitoringService.h
    ../apps/ecat-studio/services/WorkflowVisualizationService.cpp
    ../apps/ecat-studio/services/WorkflowVisualizationService.h
    ../apps/ecat-studio/services/WorkflowReportingService.cpp
    ../apps/ecat-studio/services/WorkflowReportingService.h
    ../apps/ecat-studio/services/WorkflowIntegrationService.cpp
    ../apps/ecat-studio/services/WorkflowIntegrationService.h
    ../apps/ecat-studio/services/WorkflowSecurityService.cpp
    ../apps/ecat-studio/services/WorkflowSecurityService.h
    ../apps/ecat-studio/services/WorkflowUpdateService.cpp
    ../apps/ecat-studio/services/WorkflowUpdateService.h
    ../apps/ecat-studio/services/WorkflowMaintenanceService.cpp
    ../apps/ecat-studio/services/WorkflowMaintenanceService.h
    ../apps/ecat-studio/services/WorkflowIntegrationHubService.cpp
    ../apps/ecat-studio/services/WorkflowIntegrationHubService.h
    ../apps/ecat-studio/services/WorkflowSyncService.cpp
    ../apps/ecat-studio/services/WorkflowSyncService.h
    ../apps/ecat-studio/services/WorkflowVisualizationStudioService.cpp
    ../apps/ecat-studio/services/WorkflowVisualizationStudioService.h
    ../apps/ecat-studio/services/WorkflowReportDesignerService.cpp
    ../apps/ecat-studio/services/WorkflowReportDesignerService.h
    ../apps/ecat-studio/services/WorkflowDocumentationBrowserService.cpp
    ../apps/ecat-studio/services/WorkflowDocumentationBrowserService.h
    ../apps/ecat-studio/services/WorkflowSecurityManagerService.cpp
    ../apps/ecat-studio/services/WorkflowSecurityManagerService.h
    ../apps/ecat-studio/services/WorkflowComplianceManagerService.cpp
    ../apps/ecat-studio/services/WorkflowComplianceManagerService.h
    ../apps/ecat-studio/services/WorkflowCertificationManagerService.cpp
    ../apps/ecat-studio/services/WorkflowCertificationManagerService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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

# ── BusStatsPlugin + BusStatsService (GUI) ──────────────────────────
add_executable(busstats_plugin_test
    busstats_plugin_test.cpp
    ../apps/ecat-studio/plugins/busstats/BusStatsPlugin.cpp
    ../apps/ecat-studio/plugins/busstats/BusStatsPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/BusStatsService.cpp
    ../apps/ecat-studio/services/BusStatsService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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
    ../apps/ecat-studio/plugins/onlinediagnostics/OnlineDiagnosticsPlugin.cpp
    ../apps/ecat-studio/plugins/onlinediagnostics/OnlineDiagnosticsPlugin.h
    ../apps/ecat-studio/plugins/onlinediagnostics/BusMonitorWidget.cpp
    ../apps/ecat-studio/plugins/onlinediagnostics/BusMonitorWidget.h
    ../apps/ecat-studio/plugins/onlinediagnostics/ErrorAnalyzerWidget.cpp
    ../apps/ecat-studio/plugins/onlinediagnostics/ErrorAnalyzerWidget.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/OnlineDiagnosticsService.cpp
    ../apps/ecat-studio/services/OnlineDiagnosticsService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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

set_target_properties(onlinediagnostics_plugin_test PROPERTIES AUTOMOC ON)

add_test(NAME onlinediagnostics_plugin_test COMMAND onlinediagnostics_plugin_test)
set_tests_properties(onlinediagnostics_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── WatchdogService ──────────────────────────────────────────────────
add_executable(watchdog_service_test
    watchdog_service_test.cpp
    ../apps/ecat-studio/services/WatchdogService.cpp
    ../apps/ecat-studio/services/WatchdogService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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

# ── SafetyController ─────────────────────────────────────────────────
add_executable(safety_controller_test
    safety_controller_test.cpp
    ../apps/ecat-studio/services/SafetyController.cpp
    ../apps/ecat-studio/services/SafetyController.h
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

# ── DiagnosticReportService ─────────────────────────────────────────
add_executable(diagnostic_report_service_test
    diagnostic_report_service_test.cpp
    ../apps/ecat-studio/services/DiagnosticReportService.cpp
    ../apps/ecat-studio/services/DiagnosticReportService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/services/TopologyService.cpp
    ../apps/ecat-studio/services/TopologyService.h
    ../apps/ecat-studio/services/DcSyncService.cpp
    ../apps/ecat-studio/services/DcSyncService.h
    ../apps/ecat-studio/services/PerformanceMonitorService.cpp
    ../apps/ecat-studio/services/PerformanceMonitorService.h
    ../apps/ecat-studio/services/WatchdogService.cpp
    ../apps/ecat-studio/services/WatchdogService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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

# ── CacheService ────────────────────────────────────────────────────
add_executable(cache_service_test
    cache_service_test.cpp
    ../apps/ecat-studio/services/CacheService.cpp
    ../apps/ecat-studio/services/CacheService.h
)

target_include_directories(cache_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)

target_link_libraries(cache_service_test PRIVATE Qt6::Core Qt6::Test)

set_target_properties(cache_service_test PROPERTIES AUTOMOC ON)
add_test(NAME cache_service_test COMMAND cache_service_test)

# ── AsyncOperationManager ──────────────────────────────────────────
add_executable(async_operation_manager_test
    async_operation_manager_test.cpp
    ../apps/ecat-studio/services/AsyncOperationManager.cpp
    ../apps/ecat-studio/services/AsyncOperationManager.h
)

target_include_directories(async_operation_manager_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)

target_link_libraries(async_operation_manager_test PRIVATE Qt6::Core Qt6::Test)

set_target_properties(async_operation_manager_test PROPERTIES AUTOMOC ON)
add_test(NAME async_operation_manager_test COMMAND async_operation_manager_test)

# ── MemoryPool ──────────────────────────────────────────────────────
add_executable(memory_pool_test
    memory_pool_test.cpp
    ../apps/ecat-studio/utils/MemoryPool.h
)

target_include_directories(memory_pool_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)

target_link_libraries(memory_pool_test PRIVATE Qt6::Core Qt6::Test)

set_target_properties(memory_pool_test PROPERTIES AUTOMOC ON)
add_test(NAME memory_pool_test COMMAND memory_pool_test)

# ── AlarmPlugin + AlarmService + LoggingService ─────────────────────
add_executable(alarm_plugin_test
    alarm_plugin_test.cpp
    ../apps/ecat-studio/plugins/alarm/AlarmPlugin.cpp
    ../apps/ecat-studio/plugins/alarm/AlarmPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/AlarmService.cpp
    ../apps/ecat-studio/services/AlarmService.h
    ../apps/ecat-studio/services/LoggingService.cpp
    ../apps/ecat-studio/services/LoggingService.h
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

# ── ProjectPlugin + ProjectManagerService + ConfigurationService ─────
add_executable(project_plugin_test
    project_plugin_test.cpp
    ../apps/ecat-studio/plugins/project/ProjectPlugin.cpp
    ../apps/ecat-studio/plugins/project/ProjectPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/ProjectManagerService.cpp
    ../apps/ecat-studio/services/ProjectManagerService.h
    ../apps/ecat-studio/services/ConfigurationService.cpp
    ../apps/ecat-studio/services/ConfigurationService.h
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
    ../apps/ecat-studio/plugins/oscilloscope/OscilloscopePlugin.cpp
    ../apps/ecat-studio/plugins/oscilloscope/OscilloscopePlugin.h
    ../apps/ecat-studio/plugins/oscilloscope/OscilloscopeWidget.cpp
    ../apps/ecat-studio/plugins/oscilloscope/OscilloscopeWidget.h
    ../apps/ecat-studio/plugins/oscilloscope/OscilloscopeService.cpp
    ../apps/ecat-studio/plugins/oscilloscope/OscilloscopeService.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
)

target_include_directories(oscilloscope_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
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
    ../apps/ecat-studio/plugins/protocol/ProtocolAnalyzerPlugin.cpp
    ../apps/ecat-studio/plugins/protocol/ProtocolAnalyzerPlugin.h
    ../apps/ecat-studio/plugins/protocol/ProtocolAnalyzerService.cpp
    ../apps/ecat-studio/plugins/protocol/ProtocolAnalyzerService.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
)

target_include_directories(protocol_analyzer_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
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
    ../apps/ecat-studio/plugins/chart/ChartPlugin.cpp
    ../apps/ecat-studio/plugins/chart/ChartPlugin.h
    ../apps/ecat-studio/plugins/chart/EcatChartWidget.cpp
    ../apps/ecat-studio/plugins/chart/EcatChartWidget.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/ChartService.cpp
    ../apps/ecat-studio/services/ChartService.h
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
add_executable(dashboard_plugin_test
    dashboard_plugin_test.cpp
    ../apps/ecat-studio/plugins/dashboard/DashboardPlugin.cpp
    ../apps/ecat-studio/plugins/dashboard/DashboardPlugin.h
    ../apps/ecat-studio/plugins/chart/EcatChartWidget.cpp
    ../apps/ecat-studio/plugins/chart/EcatChartWidget.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/ChartService.cpp
    ../apps/ecat-studio/services/ChartService.h
)

target_include_directories(dashboard_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins/chart
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

if(Qt6Qml_FOUND)
    add_executable(automation_plugin_test
        automation_plugin_test.cpp
        ../apps/ecat-studio/plugins/automation/AutomationPlugin.cpp
        ../apps/ecat-studio/plugins/automation/AutomationPlugin.h
        ../apps/ecat-studio/plugins/WorkspacePlugin.h
        ../apps/ecat-studio/services/ScriptingService.cpp
        ../apps/ecat-studio/services/ScriptingService.h
        ../apps/ecat-studio/services/BatchOperationService.cpp
        ../apps/ecat-studio/services/BatchOperationService.h
        ../apps/ecat-studio/services/SdoService.cpp
        ../apps/ecat-studio/services/SdoService.h
        ../apps/ecat-studio/services/TopologyService.cpp
        ../apps/ecat-studio/services/TopologyService.h
        ../apps/ecat-studio/infra/EcatClient.cpp
        ../apps/ecat-studio/infra/EcatClient.h
        ../apps/ecat-studio/infra/ProcessDataTypes.h
        ../src/core/EthercatTypes.cpp
        ../src/core/EthercatTypes.h
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
    target_compile_definitions(automation_plugin_test PRIVATE ECAT_SCRIPTING_ENABLED)

    set_target_properties(automation_plugin_test PROPERTIES AUTOMOC ON)

    add_test(NAME automation_plugin_test COMMAND automation_plugin_test)
    set_tests_properties(automation_plugin_test PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
    )
endif()

# ── Daemon Lifecycle Test (integration) ──────────────────────────────
add_executable(daemon_lifecycle_test
    integration/daemon_lifecycle_test.cpp
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../src/core/JsonProtocol.cpp
    ../src/core/JsonProtocol.h
    ../src/core/EthercatTypes.cpp
    ../src/core/EthercatTypes.h
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
    integration/plugin_lifecycle_test.cpp
    ../apps/ecat-studio/plugins/PluginRegistry.cpp
    ../apps/ecat-studio/plugins/PluginRegistry.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/plugins/notes/NotesPlugin.cpp
    ../apps/ecat-studio/plugins/notes/NotesPlugin.h
    ../apps/ecat-studio/plugins/statemachine/StateMachinePlugin.cpp
    ../apps/ecat-studio/plugins/statemachine/StateMachinePlugin.h
    ../apps/ecat-studio/services/ServiceContainer.cpp
    ../apps/ecat-studio/services/SdoOptimizationService.cpp
    ../apps/ecat-studio/services/SdoOptimizationService.h
    ../apps/ecat-studio/services/PdoMappingOptimizationService.cpp
    ../apps/ecat-studio/services/PdoMappingOptimizationService.h
    ../apps/ecat-studio/services/HardwareVerificationService.cpp
    ../apps/ecat-studio/services/HardwareVerificationService.h
    ../apps/ecat-studio/services/AdvancedErrorAnalysisService.cpp
    ../apps/ecat-studio/services/AdvancedErrorAnalysisService.h
    ../apps/ecat-studio/services/FreeRunConfigurationService.cpp
    ../apps/ecat-studio/services/FreeRunConfigurationService.h
    ../apps/ecat-studio/services/FreeRunMonitoringService.cpp
    ../apps/ecat-studio/services/FreeRunMonitoringService.h
    ../apps/ecat-studio/services/PdoConfigurationService.cpp
    ../apps/ecat-studio/services/PdoConfigurationService.h
    ../apps/ecat-studio/services/OpStateService.cpp
    ../apps/ecat-studio/services/OpStateService.h
    ../apps/ecat-studio/plugins/oscilloscope/OscilloscopeService.cpp
    ../apps/ecat-studio/plugins/oscilloscope/OscilloscopeService.h
    ../apps/ecat-studio/plugins/protocol/ProtocolAnalyzerService.cpp
    ../apps/ecat-studio/plugins/protocol/ProtocolAnalyzerService.h
    ../apps/ecat-studio/services/WorkflowAnalyticsService.cpp
    ../apps/ecat-studio/services/WorkflowAnalyticsService.h
    ../apps/ecat-studio/services/FreeRunOptimizationService.cpp
    ../apps/ecat-studio/services/FreeRunOptimizationService.h
    ../apps/ecat-studio/services/RealtimePerformanceService.cpp
    ../apps/ecat-studio/services/RealtimePerformanceService.h
    ../apps/ecat-studio/services/RealtimeOptimizerService.cpp
    ../apps/ecat-studio/services/RealtimeOptimizerService.h
    ../apps/ecat-studio/services/ProjectManagementService.cpp
    ../apps/ecat-studio/services/ProjectManagementService.h
    ../apps/ecat-studio/services/TaskManagementService.cpp
    ../apps/ecat-studio/services/TaskManagementService.h
    ../apps/ecat-studio/services/ResourceManagementService.cpp
    ../apps/ecat-studio/services/ResourceManagementService.h
    ../apps/ecat-studio/services/ProjectPlanningService.cpp
    ../apps/ecat-studio/services/ProjectPlanningService.h
    ../apps/ecat-studio/services/ProjectTrackingService.cpp
    ../apps/ecat-studio/services/ProjectTrackingService.h
    ../apps/ecat-studio/services/ProjectReportingService.cpp
    ../apps/ecat-studio/services/ProjectReportingService.h
    ../apps/ecat-studio/services/WorkflowAutomationService.cpp
    ../apps/ecat-studio/services/WorkflowAutomationService.h
    ../apps/ecat-studio/services/WorkflowSchedulingService.cpp
    ../apps/ecat-studio/services/WorkflowSchedulingService.h
    ../apps/ecat-studio/services/WorkflowMonitoringService.cpp
    ../apps/ecat-studio/services/WorkflowMonitoringService.h
    ../apps/ecat-studio/services/WorkflowVisualizationService.cpp
    ../apps/ecat-studio/services/WorkflowVisualizationService.h
    ../apps/ecat-studio/services/WorkflowReportingService.cpp
    ../apps/ecat-studio/services/WorkflowReportingService.h
    ../apps/ecat-studio/services/WorkflowIntegrationService.cpp
    ../apps/ecat-studio/services/WorkflowIntegrationService.h
    ../apps/ecat-studio/services/WorkflowSecurityService.cpp
    ../apps/ecat-studio/services/WorkflowSecurityService.h
    ../apps/ecat-studio/services/WorkflowUpdateService.cpp
    ../apps/ecat-studio/services/WorkflowUpdateService.h
    ../apps/ecat-studio/services/WorkflowMaintenanceService.cpp
    ../apps/ecat-studio/services/WorkflowMaintenanceService.h
    ../apps/ecat-studio/services/WorkflowIntegrationHubService.cpp
    ../apps/ecat-studio/services/WorkflowIntegrationHubService.h
    ../apps/ecat-studio/services/WorkflowSyncService.cpp
    ../apps/ecat-studio/services/WorkflowSyncService.h
    ../apps/ecat-studio/services/WorkflowVisualizationStudioService.cpp
    ../apps/ecat-studio/services/WorkflowVisualizationStudioService.h
    ../apps/ecat-studio/services/WorkflowReportDesignerService.cpp
    ../apps/ecat-studio/services/WorkflowReportDesignerService.h
    ../apps/ecat-studio/services/WorkflowDocumentationBrowserService.cpp
    ../apps/ecat-studio/services/WorkflowDocumentationBrowserService.h
    ../apps/ecat-studio/services/WorkflowSecurityManagerService.cpp
    ../apps/ecat-studio/services/WorkflowSecurityManagerService.h
    ../apps/ecat-studio/services/WorkflowComplianceManagerService.cpp
    ../apps/ecat-studio/services/WorkflowComplianceManagerService.h
    ../apps/ecat-studio/services/WorkflowCertificationManagerService.cpp
    ../apps/ecat-studio/services/WorkflowCertificationManagerService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/services/SdoService.cpp
    ../apps/ecat-studio/services/SdoService.h
    ../apps/ecat-studio/services/WatchService.cpp
    ../apps/ecat-studio/services/WatchService.h
    ../apps/ecat-studio/services/TopologyService.cpp
    ../apps/ecat-studio/services/TopologyService.h
    ../apps/ecat-studio/services/DcSyncService.cpp
    ../apps/ecat-studio/services/DcSyncService.h
    ../apps/ecat-studio/services/AlEventService.cpp
    ../apps/ecat-studio/services/AlEventService.h
    ../apps/ecat-studio/services/SignalService.cpp
    ../apps/ecat-studio/services/SignalService.h
    ../apps/ecat-studio/services/PerformanceMonitorService.cpp
    ../apps/ecat-studio/services/PerformanceMonitorService.h
    ../apps/ecat-studio/services/EsiService.cpp
    ../apps/ecat-studio/services/EsiService.h
    ../apps/ecat-studio/services/BusStatsService.cpp
    ../apps/ecat-studio/services/BusStatsService.h
    ../apps/ecat-studio/services/WatchdogService.cpp
    ../apps/ecat-studio/services/WatchdogService.h
    ../apps/ecat-studio/services/SafetyController.cpp
    ../apps/ecat-studio/services/SafetyController.h
    ../apps/ecat-studio/services/DiagnosticReportService.cpp
    ../apps/ecat-studio/services/DiagnosticReportService.h
    ../apps/ecat-studio/services/ProjectManagerService.cpp
    ../apps/ecat-studio/services/ProjectManagerService.h
    ../apps/ecat-studio/services/ConfigurationService.cpp
    ../apps/ecat-studio/services/ConfigurationService.h
    ../apps/ecat-studio/services/AlarmService.cpp
    ../apps/ecat-studio/services/AlarmService.h
    ../apps/ecat-studio/services/LoggingService.cpp
    ../apps/ecat-studio/services/LoggingService.h
    ../apps/ecat-studio/services/ChartService.cpp
    ../apps/ecat-studio/services/ChartService.h
    ../apps/ecat-studio/services/ScriptingService.cpp
    ../apps/ecat-studio/services/ScriptingService.h
    ../apps/ecat-studio/services/BatchOperationService.cpp
    ../apps/ecat-studio/services/BatchOperationService.h
    ../apps/ecat-studio/services/NetworkDiagnosticsService.cpp
    ../apps/ecat-studio/services/NetworkDiagnosticsService.h
    ../apps/ecat-studio/services/EcatHealthService.cpp
    ../apps/ecat-studio/services/EcatHealthService.h
    ../apps/ecat-studio/services/ExportService.cpp
    ../apps/ecat-studio/services/ExportService.h
    ../apps/ecat-studio/services/DataPipelineService.cpp
    ../apps/ecat-studio/services/DataPipelineService.h
    ../apps/ecat-studio/services/DeviceManagerService.cpp
    ../apps/ecat-studio/services/DeviceManagerService.h
    ../apps/ecat-studio/services/FirmwareUpdateService.cpp
    ../apps/ecat-studio/services/FirmwareUpdateService.h
    ../apps/ecat-studio/services/ReportGeneratorService.cpp
    ../apps/ecat-studio/services/ReportGeneratorService.h
    ../apps/ecat-studio/services/MasterManagerService.cpp
    ../apps/ecat-studio/services/MasterManagerService.h
    ../apps/ecat-studio/services/DistributedClockService.cpp
    ../apps/ecat-studio/services/DistributedClockService.h
    ../apps/ecat-studio/services/DcSyncPrecisionService.cpp
    ../apps/ecat-studio/services/DcSyncPrecisionService.h
    ../apps/ecat-studio/services/DcSyncOptimizerService.cpp
    ../apps/ecat-studio/services/DcSyncOptimizerService.h
    ../apps/ecat-studio/services/OnlineDiagnosticsService.cpp
    ../apps/ecat-studio/services/OnlineDiagnosticsService.h
    ../apps/ecat-studio/services/MultiMasterService.cpp
    ../apps/ecat-studio/services/MultiMasterService.h
    ../apps/ecat-studio/services/SdoCacheService.cpp
    ../apps/ecat-studio/services/SdoCacheService.h
    ../apps/ecat-studio/services/PdoMappingService.cpp
    ../apps/ecat-studio/services/PdoMappingService.h
    ../apps/ecat-studio/services/CoEService.cpp
    ../apps/ecat-studio/services/CoEService.h
    ../apps/ecat-studio/services/FoEService.cpp
    ../apps/ecat-studio/services/FoEService.h
    ../apps/ecat-studio/services/EoEService.cpp
    ../apps/ecat-studio/services/EoEService.h
    ../apps/ecat-studio/services/TraceService.cpp
    ../apps/ecat-studio/services/TraceService.h
    ../apps/ecat-studio/services/MasterApiService.cpp
    ../apps/ecat-studio/services/MasterApiService.h
    ../apps/ecat-studio/services/DomainService.cpp
    ../apps/ecat-studio/services/DomainService.h
    ../apps/ecat-studio/services/SyncManagerService.cpp
    ../apps/ecat-studio/services/SyncManagerService.h
    ../apps/ecat-studio/services/StateMachineService.cpp
    ../apps/ecat-studio/services/StateMachineService.h
    ../apps/ecat-studio/services/ErrorHandlingService.cpp
    ../apps/ecat-studio/services/ErrorHandlingService.h
    ../apps/ecat-studio/services/DiagnosticService.cpp
    ../apps/ecat-studio/services/DiagnosticService.h
    ../apps/ecat-studio/services/HotConnectService.cpp
    ../apps/ecat-studio/services/HotConnectService.h
    ../apps/ecat-studio/services/RedundancyService.cpp
    ../apps/ecat-studio/services/RedundancyService.h
    ../apps/ecat-studio/services/CableDiagnosticsService.cpp
    ../apps/ecat-studio/services/CableDiagnosticsService.h
    ../apps/ecat-studio/services/EtherCATMonitorService.cpp
    ../apps/ecat-studio/services/EtherCATMonitorService.h
    ../apps/ecat-studio/services/EtherCATAnalyzerService.cpp
    ../apps/ecat-studio/services/EtherCATAnalyzerService.h
    ../apps/ecat-studio/services/EtherCATOptimizerService.cpp
    ../apps/ecat-studio/services/EtherCATOptimizerService.h
    ../apps/ecat-studio/services/EtherCATConfigService.cpp
    ../apps/ecat-studio/services/EtherCATConfigService.h
    ../apps/ecat-studio/services/EtherCATBackupService.cpp
    ../apps/ecat-studio/services/EtherCATBackupService.h
    ../apps/ecat-studio/services/EtherCATRecoveryService.cpp
    ../apps/ecat-studio/services/EtherCATRecoveryService.h
    ../apps/ecat-studio/services/EtherCATSimulationService.cpp
    ../apps/ecat-studio/services/EtherCATSimulationService.h
    ../apps/ecat-studio/services/EtherCATTestingService.cpp
    ../apps/ecat-studio/services/EtherCATTestingService.h
    ../apps/ecat-studio/services/EtherCATValidationService.cpp
    ../apps/ecat-studio/services/EtherCATValidationService.h
    ../apps/ecat-studio/services/EtherCATSecurityService.cpp
    ../apps/ecat-studio/services/EtherCATSecurityService.h
    ../apps/ecat-studio/services/EtherCATComplianceService.cpp
    ../apps/ecat-studio/services/EtherCATComplianceService.h
    ../apps/ecat-studio/services/EtherCATCertificationService.cpp
    ../apps/ecat-studio/services/EtherCATCertificationService.h
    ../apps/ecat-studio/services/EtherCATSecurityService.cpp
    ../apps/ecat-studio/services/EtherCATSecurityService.h
    ../apps/ecat-studio/services/EtherCATComplianceService.cpp
    ../apps/ecat-studio/services/EtherCATComplianceService.h
    ../apps/ecat-studio/services/EtherCATCertificationService.cpp
    ../apps/ecat-studio/services/EtherCATCertificationService.h
    ../apps/ecat-studio/services/EtherCATAnalyticsService.cpp
    ../apps/ecat-studio/services/EtherCATAnalyticsService.h
    ../apps/ecat-studio/services/EtherCATDeploymentService.cpp
    ../apps/ecat-studio/services/EtherCATDeploymentService.h
    ../apps/ecat-studio/services/EtherCATUpdateService.cpp
    ../apps/ecat-studio/services/EtherCATUpdateService.h
    ../apps/ecat-studio/services/EtherCATMaintenanceService.cpp
    ../apps/ecat-studio/services/EtherCATMaintenanceService.h
    ../apps/ecat-studio/services/EtherCATIntegrationService.cpp
    ../apps/ecat-studio/services/EtherCATIntegrationService.h
    ../apps/ecat-studio/services/EtherCATSyncService.cpp
    ../apps/ecat-studio/services/EtherCATSyncService.h
    ../apps/ecat-studio/services/EtherCATReplicationService.cpp
    ../apps/ecat-studio/services/EtherCATReplicationService.h
    ../apps/ecat-studio/services/EtherCATVisualizationService.cpp
    ../apps/ecat-studio/services/EtherCATVisualizationService.h
    ../apps/ecat-studio/services/EtherCATReportingService.cpp
    ../apps/ecat-studio/services/EtherCATReportingService.h
    ../apps/ecat-studio/services/EtherCATDocumentationService.cpp
    ../apps/ecat-studio/services/EtherCATDocumentationService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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

# ── EventBus Performance Test ────────────────────────────────────────
add_executable(eventbus_performance_test
    performance/eventbus_performance_test.cpp
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
)
target_include_directories(eventbus_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(eventbus_performance_test PRIVATE Qt6::Core Qt6::Test ecat_core)
set_target_properties(eventbus_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME eventbus_performance_test COMMAND eventbus_performance_test)

# ── NetworkDiagnosticsPlugin + NetworkDiagnosticsService (GUI) ───────────
add_executable(network_diagnostics_plugin_test
    network_diagnostics_plugin_test.cpp
    ../apps/ecat-studio/plugins/network/NetworkDiagnosticsPlugin.cpp
    ../apps/ecat-studio/plugins/network/NetworkDiagnosticsPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/NetworkDiagnosticsService.cpp
    ../apps/ecat-studio/services/NetworkDiagnosticsService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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

# ── NetworkDiagnosticsService (unit) ────────────────────────────────
add_executable(network_diagnostics_service_test
    network_diagnostics_service_test.cpp
    ../apps/ecat-studio/services/NetworkDiagnosticsService.cpp
    ../apps/ecat-studio/services/NetworkDiagnosticsService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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
    ../apps/ecat-studio/services/EcatHealthService.cpp
    ../apps/ecat-studio/services/EcatHealthService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/services/TopologyService.cpp
    ../apps/ecat-studio/services/TopologyService.h
    ../apps/ecat-studio/services/DcSyncService.cpp
    ../apps/ecat-studio/services/DcSyncService.h
    ../apps/ecat-studio/services/AlEventService.cpp
    ../apps/ecat-studio/services/AlEventService.h
    ../apps/ecat-studio/services/WatchdogService.cpp
    ../apps/ecat-studio/services/WatchdogService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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
    ../apps/ecat-studio/services/ExportService.cpp
    ../apps/ecat-studio/services/ExportService.h
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
    ../apps/ecat-studio/services/DataPipelineService.cpp
    ../apps/ecat-studio/services/DataPipelineService.h
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
    ../apps/ecat-studio/services/DeviceManagerService.cpp
    ../apps/ecat-studio/services/DeviceManagerService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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
    ../apps/ecat-studio/services/FirmwareUpdateService.cpp
    ../apps/ecat-studio/services/FirmwareUpdateService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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
    ../apps/ecat-studio/services/ReportGeneratorService.cpp
    ../apps/ecat-studio/services/ReportGeneratorService.h
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

# ── DataPipelinePlugin (integration) ───────────────────────────────
add_executable(data_pipeline_plugin_test
    data_pipeline_plugin_test.cpp
    ../apps/ecat-studio/plugins/datapipeline/DataPipelinePlugin.cpp
    ../apps/ecat-studio/plugins/datapipeline/DataPipelinePlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/DataPipelineService.cpp
    ../apps/ecat-studio/services/DataPipelineService.h
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
    ../apps/ecat-studio/plugins/devicemanager/DeviceManagerPlugin.cpp
    ../apps/ecat-studio/plugins/devicemanager/DeviceManagerPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/DeviceManagerService.cpp
    ../apps/ecat-studio/services/DeviceManagerService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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
    ../apps/ecat-studio/plugins/master/MasterManagerPlugin.cpp
    ../apps/ecat-studio/plugins/master/MasterManagerPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/MasterManagerService.cpp
    ../apps/ecat-studio/services/MasterManagerService.h
    ../apps/ecat-studio/services/DistributedClockService.cpp
    ../apps/ecat-studio/services/DistributedClockService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(master_manager_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/src/core
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

# ── ExportService Performance Test ──────────────────────────────────
add_executable(export_service_performance_test
    performance/export_service_performance_test.cpp
    ../apps/ecat-studio/services/ExportService.cpp
    ../apps/ecat-studio/services/ExportService.h
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
    performance/data_pipeline_performance_test.cpp
    ../apps/ecat-studio/services/DataPipelineService.cpp
    ../apps/ecat-studio/services/DataPipelineService.h
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
    performance/device_manager_performance_test.cpp
    ../apps/ecat-studio/services/DeviceManagerService.cpp
    ../apps/ecat-studio/services/DeviceManagerService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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

# ── Advanced Test Utilities ───────────────────────────────────────────

# TestAutomation
add_executable(test_automation_test
    test_automation_test.cpp
    utils/TestAutomation.cpp
    utils/TestAutomation.h
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
    utils/TestDataGenerator.cpp
    utils/TestDataGenerator.h
    ../src/core/EthercatTypes.cpp
    ../src/core/EthercatTypes.h
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
    fixtures/PluginTestFixture.cpp
    fixtures/PluginTestFixture.h
    ../apps/ecat-studio/plugins/PluginRegistry.cpp
    ../apps/ecat-studio/plugins/PluginRegistry.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/ServiceContainer.cpp
    ../apps/ecat-studio/services/SdoOptimizationService.cpp
    ../apps/ecat-studio/services/SdoOptimizationService.h
    ../apps/ecat-studio/services/PdoMappingOptimizationService.cpp
    ../apps/ecat-studio/services/PdoMappingOptimizationService.h
    ../apps/ecat-studio/services/HardwareVerificationService.cpp
    ../apps/ecat-studio/services/HardwareVerificationService.h
    ../apps/ecat-studio/services/AdvancedErrorAnalysisService.cpp
    ../apps/ecat-studio/services/AdvancedErrorAnalysisService.h
    ../apps/ecat-studio/services/FreeRunConfigurationService.cpp
    ../apps/ecat-studio/services/FreeRunConfigurationService.h
    ../apps/ecat-studio/services/FreeRunMonitoringService.cpp
    ../apps/ecat-studio/services/FreeRunMonitoringService.h
    ../apps/ecat-studio/services/PdoConfigurationService.cpp
    ../apps/ecat-studio/services/PdoConfigurationService.h
    ../apps/ecat-studio/services/OpStateService.cpp
    ../apps/ecat-studio/services/OpStateService.h
    ../apps/ecat-studio/plugins/oscilloscope/OscilloscopeService.cpp
    ../apps/ecat-studio/plugins/oscilloscope/OscilloscopeService.h
    ../apps/ecat-studio/plugins/protocol/ProtocolAnalyzerService.cpp
    ../apps/ecat-studio/plugins/protocol/ProtocolAnalyzerService.h
    ../apps/ecat-studio/services/WorkflowAnalyticsService.cpp
    ../apps/ecat-studio/services/WorkflowAnalyticsService.h
    ../apps/ecat-studio/services/FreeRunOptimizationService.cpp
    ../apps/ecat-studio/services/FreeRunOptimizationService.h
    ../apps/ecat-studio/services/RealtimePerformanceService.cpp
    ../apps/ecat-studio/services/RealtimePerformanceService.h
    ../apps/ecat-studio/services/RealtimeOptimizerService.cpp
    ../apps/ecat-studio/services/RealtimeOptimizerService.h
    ../apps/ecat-studio/services/ScriptingService.cpp
    ../apps/ecat-studio/services/ScriptingService.h
    ../apps/ecat-studio/services/ProjectManagementService.cpp
    ../apps/ecat-studio/services/ProjectManagementService.h
    ../apps/ecat-studio/services/TaskManagementService.cpp
    ../apps/ecat-studio/services/TaskManagementService.h
    ../apps/ecat-studio/services/ResourceManagementService.cpp
    ../apps/ecat-studio/services/ResourceManagementService.h
    ../apps/ecat-studio/services/ProjectPlanningService.cpp
    ../apps/ecat-studio/services/ProjectPlanningService.h
    ../apps/ecat-studio/services/ProjectTrackingService.cpp
    ../apps/ecat-studio/services/ProjectTrackingService.h
    ../apps/ecat-studio/services/ProjectReportingService.cpp
    ../apps/ecat-studio/services/ProjectReportingService.h
    ../apps/ecat-studio/services/WorkflowAutomationService.cpp
    ../apps/ecat-studio/services/WorkflowAutomationService.h
    ../apps/ecat-studio/services/WorkflowSchedulingService.cpp
    ../apps/ecat-studio/services/WorkflowSchedulingService.h
    ../apps/ecat-studio/services/WorkflowMonitoringService.cpp
    ../apps/ecat-studio/services/WorkflowMonitoringService.h
    ../apps/ecat-studio/services/WorkflowVisualizationService.cpp
    ../apps/ecat-studio/services/WorkflowVisualizationService.h
    ../apps/ecat-studio/services/WorkflowReportingService.cpp
    ../apps/ecat-studio/services/WorkflowReportingService.h
    ../apps/ecat-studio/services/WorkflowIntegrationService.cpp
    ../apps/ecat-studio/services/WorkflowIntegrationService.h
    ../apps/ecat-studio/services/WorkflowSecurityService.cpp
    ../apps/ecat-studio/services/WorkflowSecurityService.h
    ../apps/ecat-studio/services/WorkflowUpdateService.cpp
    ../apps/ecat-studio/services/WorkflowUpdateService.h
    ../apps/ecat-studio/services/WorkflowMaintenanceService.cpp
    ../apps/ecat-studio/services/WorkflowMaintenanceService.h
    ../apps/ecat-studio/services/WorkflowIntegrationHubService.cpp
    ../apps/ecat-studio/services/WorkflowIntegrationHubService.h
    ../apps/ecat-studio/services/WorkflowSyncService.cpp
    ../apps/ecat-studio/services/WorkflowSyncService.h
    ../apps/ecat-studio/services/WorkflowVisualizationStudioService.cpp
    ../apps/ecat-studio/services/WorkflowVisualizationStudioService.h
    ../apps/ecat-studio/services/WorkflowReportDesignerService.cpp
    ../apps/ecat-studio/services/WorkflowReportDesignerService.h
    ../apps/ecat-studio/services/WorkflowDocumentationBrowserService.cpp
    ../apps/ecat-studio/services/WorkflowDocumentationBrowserService.h
    ../apps/ecat-studio/services/WorkflowSecurityManagerService.cpp
    ../apps/ecat-studio/services/WorkflowSecurityManagerService.h
    ../apps/ecat-studio/services/WorkflowComplianceManagerService.cpp
    ../apps/ecat-studio/services/WorkflowComplianceManagerService.h
    ../apps/ecat-studio/services/WorkflowCertificationManagerService.cpp
    ../apps/ecat-studio/services/WorkflowCertificationManagerService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/services/SdoService.cpp
    ../apps/ecat-studio/services/SdoService.h
    ../apps/ecat-studio/services/WatchService.cpp
    ../apps/ecat-studio/services/WatchService.h
    ../apps/ecat-studio/services/TopologyService.cpp
    ../apps/ecat-studio/services/TopologyService.h
    ../apps/ecat-studio/services/DcSyncService.cpp
    ../apps/ecat-studio/services/DcSyncService.h
    ../apps/ecat-studio/services/AlEventService.cpp
    ../apps/ecat-studio/services/AlEventService.h
    ../apps/ecat-studio/services/SignalService.cpp
    ../apps/ecat-studio/services/SignalService.h
    ../apps/ecat-studio/services/PerformanceMonitorService.cpp
    ../apps/ecat-studio/services/PerformanceMonitorService.h
    ../apps/ecat-studio/services/EsiService.cpp
    ../apps/ecat-studio/services/EsiService.h
    ../apps/ecat-studio/services/BusStatsService.cpp
    ../apps/ecat-studio/services/BusStatsService.h
    ../apps/ecat-studio/services/WatchdogService.cpp
    ../apps/ecat-studio/services/WatchdogService.h
    ../apps/ecat-studio/services/SafetyController.cpp
    ../apps/ecat-studio/services/SafetyController.h
    ../apps/ecat-studio/services/DiagnosticReportService.cpp
    ../apps/ecat-studio/services/DiagnosticReportService.h
    ../apps/ecat-studio/services/ProjectManagerService.cpp
    ../apps/ecat-studio/services/ProjectManagerService.h
    ../apps/ecat-studio/services/ConfigurationService.cpp
    ../apps/ecat-studio/services/ConfigurationService.h
    ../apps/ecat-studio/services/AlarmService.cpp
    ../apps/ecat-studio/services/AlarmService.h
    ../apps/ecat-studio/services/LoggingService.cpp
    ../apps/ecat-studio/services/LoggingService.h
    ../apps/ecat-studio/services/ChartService.cpp
    ../apps/ecat-studio/services/ChartService.h
    ../apps/ecat-studio/services/BatchOperationService.cpp
    ../apps/ecat-studio/services/BatchOperationService.h
    ../apps/ecat-studio/services/NetworkDiagnosticsService.cpp
    ../apps/ecat-studio/services/NetworkDiagnosticsService.h
    ../apps/ecat-studio/services/EcatHealthService.cpp
    ../apps/ecat-studio/services/EcatHealthService.h
    ../apps/ecat-studio/services/ExportService.cpp
    ../apps/ecat-studio/services/ExportService.h
    ../apps/ecat-studio/services/DataPipelineService.cpp
    ../apps/ecat-studio/services/DataPipelineService.h
    ../apps/ecat-studio/services/DeviceManagerService.cpp
    ../apps/ecat-studio/services/DeviceManagerService.h
    ../apps/ecat-studio/services/FirmwareUpdateService.cpp
    ../apps/ecat-studio/services/FirmwareUpdateService.h
    ../apps/ecat-studio/services/ReportGeneratorService.cpp
    ../apps/ecat-studio/services/ReportGeneratorService.h
    ../apps/ecat-studio/services/MasterManagerService.cpp
    ../apps/ecat-studio/services/MasterManagerService.h
    ../apps/ecat-studio/services/DistributedClockService.cpp
    ../apps/ecat-studio/services/DistributedClockService.h
    ../apps/ecat-studio/services/DcSyncPrecisionService.cpp
    ../apps/ecat-studio/services/DcSyncPrecisionService.h
    ../apps/ecat-studio/services/DcSyncOptimizerService.cpp
    ../apps/ecat-studio/services/DcSyncOptimizerService.h
    ../apps/ecat-studio/services/OnlineDiagnosticsService.cpp
    ../apps/ecat-studio/services/OnlineDiagnosticsService.h
    ../apps/ecat-studio/services/MultiMasterService.cpp
    ../apps/ecat-studio/services/MultiMasterService.h
    ../apps/ecat-studio/services/SdoCacheService.cpp
    ../apps/ecat-studio/services/SdoCacheService.h
    ../apps/ecat-studio/services/PdoMappingService.cpp
    ../apps/ecat-studio/services/PdoMappingService.h
    ../apps/ecat-studio/services/CoEService.cpp
    ../apps/ecat-studio/services/CoEService.h
    ../apps/ecat-studio/services/FoEService.cpp
    ../apps/ecat-studio/services/FoEService.h
    ../apps/ecat-studio/services/EoEService.cpp
    ../apps/ecat-studio/services/EoEService.h
    ../apps/ecat-studio/services/TraceService.cpp
    ../apps/ecat-studio/services/TraceService.h
    ../apps/ecat-studio/services/MasterApiService.cpp
    ../apps/ecat-studio/services/MasterApiService.h
    ../apps/ecat-studio/services/DomainService.cpp
    ../apps/ecat-studio/services/DomainService.h
    ../apps/ecat-studio/services/SyncManagerService.cpp
    ../apps/ecat-studio/services/SyncManagerService.h
    ../apps/ecat-studio/services/StateMachineService.cpp
    ../apps/ecat-studio/services/StateMachineService.h
    ../apps/ecat-studio/services/ErrorHandlingService.cpp
    ../apps/ecat-studio/services/ErrorHandlingService.h
    ../apps/ecat-studio/services/DiagnosticService.cpp
    ../apps/ecat-studio/services/DiagnosticService.h
    ../apps/ecat-studio/services/HotConnectService.cpp
    ../apps/ecat-studio/services/HotConnectService.h
    ../apps/ecat-studio/services/RedundancyService.cpp
    ../apps/ecat-studio/services/RedundancyService.h
    ../apps/ecat-studio/services/CableDiagnosticsService.cpp
    ../apps/ecat-studio/services/CableDiagnosticsService.h
    ../apps/ecat-studio/services/EtherCATMonitorService.cpp
    ../apps/ecat-studio/services/EtherCATMonitorService.h
    ../apps/ecat-studio/services/EtherCATAnalyzerService.cpp
    ../apps/ecat-studio/services/EtherCATAnalyzerService.h
    ../apps/ecat-studio/services/EtherCATOptimizerService.cpp
    ../apps/ecat-studio/services/EtherCATOptimizerService.h
    ../apps/ecat-studio/services/EtherCATConfigService.cpp
    ../apps/ecat-studio/services/EtherCATConfigService.h
    ../apps/ecat-studio/services/EtherCATBackupService.cpp
    ../apps/ecat-studio/services/EtherCATBackupService.h
    ../apps/ecat-studio/services/EtherCATRecoveryService.cpp
    ../apps/ecat-studio/services/EtherCATRecoveryService.h
    ../apps/ecat-studio/services/EtherCATSimulationService.cpp
    ../apps/ecat-studio/services/EtherCATSimulationService.h
    ../apps/ecat-studio/services/EtherCATTestingService.cpp
    ../apps/ecat-studio/services/EtherCATTestingService.h
    ../apps/ecat-studio/services/EtherCATValidationService.cpp
    ../apps/ecat-studio/services/EtherCATValidationService.h
    ../apps/ecat-studio/services/EtherCATSecurityService.cpp
    ../apps/ecat-studio/services/EtherCATSecurityService.h
    ../apps/ecat-studio/services/EtherCATComplianceService.cpp
    ../apps/ecat-studio/services/EtherCATComplianceService.h
    ../apps/ecat-studio/services/EtherCATCertificationService.cpp
    ../apps/ecat-studio/services/EtherCATCertificationService.h
    ../apps/ecat-studio/services/EtherCATSecurityService.cpp
    ../apps/ecat-studio/services/EtherCATSecurityService.h
    ../apps/ecat-studio/services/EtherCATComplianceService.cpp
    ../apps/ecat-studio/services/EtherCATComplianceService.h
    ../apps/ecat-studio/services/EtherCATCertificationService.cpp
    ../apps/ecat-studio/services/EtherCATCertificationService.h
    ../apps/ecat-studio/services/EtherCATAnalyticsService.cpp
    ../apps/ecat-studio/services/EtherCATAnalyticsService.h
    ../apps/ecat-studio/services/EtherCATDeploymentService.cpp
    ../apps/ecat-studio/services/EtherCATDeploymentService.h
    ../apps/ecat-studio/services/EtherCATUpdateService.cpp
    ../apps/ecat-studio/services/EtherCATUpdateService.h
    ../apps/ecat-studio/services/EtherCATMaintenanceService.cpp
    ../apps/ecat-studio/services/EtherCATMaintenanceService.h
    ../apps/ecat-studio/services/EtherCATIntegrationService.cpp
    ../apps/ecat-studio/services/EtherCATIntegrationService.h
    ../apps/ecat-studio/services/EtherCATSyncService.cpp
    ../apps/ecat-studio/services/EtherCATSyncService.h
    ../apps/ecat-studio/services/EtherCATReplicationService.cpp
    ../apps/ecat-studio/services/EtherCATReplicationService.h
    ../apps/ecat-studio/services/EtherCATVisualizationService.cpp
    ../apps/ecat-studio/services/EtherCATVisualizationService.h
    ../apps/ecat-studio/services/EtherCATReportingService.cpp
    ../apps/ecat-studio/services/EtherCATReportingService.h
    ../apps/ecat-studio/services/EtherCATDocumentationService.cpp
    ../apps/ecat-studio/services/EtherCATDocumentationService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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
    fixtures/ServiceTestFixture.cpp
    fixtures/ServiceTestFixture.h
    ../apps/ecat-studio/services/ServiceContainer.cpp
    ../apps/ecat-studio/services/SdoOptimizationService.cpp
    ../apps/ecat-studio/services/SdoOptimizationService.h
    ../apps/ecat-studio/services/PdoMappingOptimizationService.cpp
    ../apps/ecat-studio/services/PdoMappingOptimizationService.h
    ../apps/ecat-studio/services/HardwareVerificationService.cpp
    ../apps/ecat-studio/services/HardwareVerificationService.h
    ../apps/ecat-studio/services/AdvancedErrorAnalysisService.cpp
    ../apps/ecat-studio/services/AdvancedErrorAnalysisService.h
    ../apps/ecat-studio/services/FreeRunConfigurationService.cpp
    ../apps/ecat-studio/services/FreeRunConfigurationService.h
    ../apps/ecat-studio/services/FreeRunMonitoringService.cpp
    ../apps/ecat-studio/services/FreeRunMonitoringService.h
    ../apps/ecat-studio/services/PdoConfigurationService.cpp
    ../apps/ecat-studio/services/PdoConfigurationService.h
    ../apps/ecat-studio/services/OpStateService.cpp
    ../apps/ecat-studio/services/OpStateService.h
    ../apps/ecat-studio/plugins/oscilloscope/OscilloscopeService.cpp
    ../apps/ecat-studio/plugins/oscilloscope/OscilloscopeService.h
    ../apps/ecat-studio/plugins/protocol/ProtocolAnalyzerService.cpp
    ../apps/ecat-studio/plugins/protocol/ProtocolAnalyzerService.h
    ../apps/ecat-studio/services/WorkflowAnalyticsService.cpp
    ../apps/ecat-studio/services/WorkflowAnalyticsService.h
    ../apps/ecat-studio/services/FreeRunOptimizationService.cpp
    ../apps/ecat-studio/services/FreeRunOptimizationService.h
    ../apps/ecat-studio/services/RealtimePerformanceService.cpp
    ../apps/ecat-studio/services/RealtimePerformanceService.h
    ../apps/ecat-studio/services/RealtimeOptimizerService.cpp
    ../apps/ecat-studio/services/RealtimeOptimizerService.h
    ../apps/ecat-studio/services/ScriptingService.cpp
    ../apps/ecat-studio/services/ScriptingService.h
    ../apps/ecat-studio/services/ProjectManagementService.cpp
    ../apps/ecat-studio/services/ProjectManagementService.h
    ../apps/ecat-studio/services/TaskManagementService.cpp
    ../apps/ecat-studio/services/TaskManagementService.h
    ../apps/ecat-studio/services/ResourceManagementService.cpp
    ../apps/ecat-studio/services/ResourceManagementService.h
    ../apps/ecat-studio/services/ProjectPlanningService.cpp
    ../apps/ecat-studio/services/ProjectPlanningService.h
    ../apps/ecat-studio/services/ProjectTrackingService.cpp
    ../apps/ecat-studio/services/ProjectTrackingService.h
    ../apps/ecat-studio/services/ProjectReportingService.cpp
    ../apps/ecat-studio/services/ProjectReportingService.h
    ../apps/ecat-studio/services/WorkflowAutomationService.cpp
    ../apps/ecat-studio/services/WorkflowAutomationService.h
    ../apps/ecat-studio/services/WorkflowSchedulingService.cpp
    ../apps/ecat-studio/services/WorkflowSchedulingService.h
    ../apps/ecat-studio/services/WorkflowMonitoringService.cpp
    ../apps/ecat-studio/services/WorkflowMonitoringService.h
    ../apps/ecat-studio/services/WorkflowVisualizationService.cpp
    ../apps/ecat-studio/services/WorkflowVisualizationService.h
    ../apps/ecat-studio/services/WorkflowReportingService.cpp
    ../apps/ecat-studio/services/WorkflowReportingService.h
    ../apps/ecat-studio/services/WorkflowIntegrationService.cpp
    ../apps/ecat-studio/services/WorkflowIntegrationService.h
    ../apps/ecat-studio/services/WorkflowSecurityService.cpp
    ../apps/ecat-studio/services/WorkflowSecurityService.h
    ../apps/ecat-studio/services/WorkflowUpdateService.cpp
    ../apps/ecat-studio/services/WorkflowUpdateService.h
    ../apps/ecat-studio/services/WorkflowMaintenanceService.cpp
    ../apps/ecat-studio/services/WorkflowMaintenanceService.h
    ../apps/ecat-studio/services/WorkflowIntegrationHubService.cpp
    ../apps/ecat-studio/services/WorkflowIntegrationHubService.h
    ../apps/ecat-studio/services/WorkflowSyncService.cpp
    ../apps/ecat-studio/services/WorkflowSyncService.h
    ../apps/ecat-studio/services/WorkflowVisualizationStudioService.cpp
    ../apps/ecat-studio/services/WorkflowVisualizationStudioService.h
    ../apps/ecat-studio/services/WorkflowReportDesignerService.cpp
    ../apps/ecat-studio/services/WorkflowReportDesignerService.h
    ../apps/ecat-studio/services/WorkflowDocumentationBrowserService.cpp
    ../apps/ecat-studio/services/WorkflowDocumentationBrowserService.h
    ../apps/ecat-studio/services/WorkflowSecurityManagerService.cpp
    ../apps/ecat-studio/services/WorkflowSecurityManagerService.h
    ../apps/ecat-studio/services/WorkflowComplianceManagerService.cpp
    ../apps/ecat-studio/services/WorkflowComplianceManagerService.h
    ../apps/ecat-studio/services/WorkflowCertificationManagerService.cpp
    ../apps/ecat-studio/services/WorkflowCertificationManagerService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/services/SdoService.cpp
    ../apps/ecat-studio/services/SdoService.h
    ../apps/ecat-studio/services/WatchService.cpp
    ../apps/ecat-studio/services/WatchService.h
    ../apps/ecat-studio/services/TopologyService.cpp
    ../apps/ecat-studio/services/TopologyService.h
    ../apps/ecat-studio/services/DcSyncService.cpp
    ../apps/ecat-studio/services/DcSyncService.h
    ../apps/ecat-studio/services/AlEventService.cpp
    ../apps/ecat-studio/services/AlEventService.h
    ../apps/ecat-studio/services/SignalService.cpp
    ../apps/ecat-studio/services/SignalService.h
    ../apps/ecat-studio/services/PerformanceMonitorService.cpp
    ../apps/ecat-studio/services/PerformanceMonitorService.h
    ../apps/ecat-studio/services/EsiService.cpp
    ../apps/ecat-studio/services/EsiService.h
    ../apps/ecat-studio/services/BusStatsService.cpp
    ../apps/ecat-studio/services/BusStatsService.h
    ../apps/ecat-studio/services/WatchdogService.cpp
    ../apps/ecat-studio/services/WatchdogService.h
    ../apps/ecat-studio/services/SafetyController.cpp
    ../apps/ecat-studio/services/SafetyController.h
    ../apps/ecat-studio/services/DiagnosticReportService.cpp
    ../apps/ecat-studio/services/DiagnosticReportService.h
    ../apps/ecat-studio/services/ProjectManagerService.cpp
    ../apps/ecat-studio/services/ProjectManagerService.h
    ../apps/ecat-studio/services/ConfigurationService.cpp
    ../apps/ecat-studio/services/ConfigurationService.h
    ../apps/ecat-studio/services/AlarmService.cpp
    ../apps/ecat-studio/services/AlarmService.h
    ../apps/ecat-studio/services/LoggingService.cpp
    ../apps/ecat-studio/services/LoggingService.h
    ../apps/ecat-studio/services/ChartService.cpp
    ../apps/ecat-studio/services/ChartService.h
    ../apps/ecat-studio/services/BatchOperationService.cpp
    ../apps/ecat-studio/services/BatchOperationService.h
    ../apps/ecat-studio/services/NetworkDiagnosticsService.cpp
    ../apps/ecat-studio/services/NetworkDiagnosticsService.h
    ../apps/ecat-studio/services/EcatHealthService.cpp
    ../apps/ecat-studio/services/EcatHealthService.h
    ../apps/ecat-studio/services/ExportService.cpp
    ../apps/ecat-studio/services/ExportService.h
    ../apps/ecat-studio/services/DataPipelineService.cpp
    ../apps/ecat-studio/services/DataPipelineService.h
    ../apps/ecat-studio/services/DeviceManagerService.cpp
    ../apps/ecat-studio/services/DeviceManagerService.h
    ../apps/ecat-studio/services/FirmwareUpdateService.cpp
    ../apps/ecat-studio/services/FirmwareUpdateService.h
    ../apps/ecat-studio/services/ReportGeneratorService.cpp
    ../apps/ecat-studio/services/ReportGeneratorService.h
    ../apps/ecat-studio/services/MasterManagerService.cpp
    ../apps/ecat-studio/services/MasterManagerService.h
    ../apps/ecat-studio/services/DistributedClockService.cpp
    ../apps/ecat-studio/services/DistributedClockService.h
    ../apps/ecat-studio/services/DcSyncPrecisionService.cpp
    ../apps/ecat-studio/services/DcSyncPrecisionService.h
    ../apps/ecat-studio/services/DcSyncOptimizerService.cpp
    ../apps/ecat-studio/services/DcSyncOptimizerService.h
    ../apps/ecat-studio/services/OnlineDiagnosticsService.cpp
    ../apps/ecat-studio/services/OnlineDiagnosticsService.h
    ../apps/ecat-studio/services/MultiMasterService.cpp
    ../apps/ecat-studio/services/MultiMasterService.h
    ../apps/ecat-studio/services/SdoCacheService.cpp
    ../apps/ecat-studio/services/SdoCacheService.h
    ../apps/ecat-studio/services/PdoMappingService.cpp
    ../apps/ecat-studio/services/PdoMappingService.h
    ../apps/ecat-studio/services/CoEService.cpp
    ../apps/ecat-studio/services/CoEService.h
    ../apps/ecat-studio/services/FoEService.cpp
    ../apps/ecat-studio/services/FoEService.h
    ../apps/ecat-studio/services/EoEService.cpp
    ../apps/ecat-studio/services/EoEService.h
    ../apps/ecat-studio/services/TraceService.cpp
    ../apps/ecat-studio/services/TraceService.h
    ../apps/ecat-studio/services/MasterApiService.cpp
    ../apps/ecat-studio/services/MasterApiService.h
    ../apps/ecat-studio/services/DomainService.cpp
    ../apps/ecat-studio/services/DomainService.h
    ../apps/ecat-studio/services/SyncManagerService.cpp
    ../apps/ecat-studio/services/SyncManagerService.h
    ../apps/ecat-studio/services/StateMachineService.cpp
    ../apps/ecat-studio/services/StateMachineService.h
    ../apps/ecat-studio/services/ErrorHandlingService.cpp
    ../apps/ecat-studio/services/ErrorHandlingService.h
    ../apps/ecat-studio/services/DiagnosticService.cpp
    ../apps/ecat-studio/services/DiagnosticService.h
    ../apps/ecat-studio/services/HotConnectService.cpp
    ../apps/ecat-studio/services/HotConnectService.h
    ../apps/ecat-studio/services/RedundancyService.cpp
    ../apps/ecat-studio/services/RedundancyService.h
    ../apps/ecat-studio/services/CableDiagnosticsService.cpp
    ../apps/ecat-studio/services/CableDiagnosticsService.h
    ../apps/ecat-studio/services/EtherCATMonitorService.cpp
    ../apps/ecat-studio/services/EtherCATMonitorService.h
    ../apps/ecat-studio/services/EtherCATAnalyzerService.cpp
    ../apps/ecat-studio/services/EtherCATAnalyzerService.h
    ../apps/ecat-studio/services/EtherCATOptimizerService.cpp
    ../apps/ecat-studio/services/EtherCATOptimizerService.h
    ../apps/ecat-studio/services/EtherCATConfigService.cpp
    ../apps/ecat-studio/services/EtherCATConfigService.h
    ../apps/ecat-studio/services/EtherCATBackupService.cpp
    ../apps/ecat-studio/services/EtherCATBackupService.h
    ../apps/ecat-studio/services/EtherCATRecoveryService.cpp
    ../apps/ecat-studio/services/EtherCATRecoveryService.h
    ../apps/ecat-studio/services/EtherCATSimulationService.cpp
    ../apps/ecat-studio/services/EtherCATSimulationService.h
    ../apps/ecat-studio/services/EtherCATTestingService.cpp
    ../apps/ecat-studio/services/EtherCATTestingService.h
    ../apps/ecat-studio/services/EtherCATValidationService.cpp
    ../apps/ecat-studio/services/EtherCATValidationService.h
    ../apps/ecat-studio/services/EtherCATSecurityService.cpp
    ../apps/ecat-studio/services/EtherCATSecurityService.h
    ../apps/ecat-studio/services/EtherCATComplianceService.cpp
    ../apps/ecat-studio/services/EtherCATComplianceService.h
    ../apps/ecat-studio/services/EtherCATCertificationService.cpp
    ../apps/ecat-studio/services/EtherCATCertificationService.h
    ../apps/ecat-studio/services/EtherCATSecurityService.cpp
    ../apps/ecat-studio/services/EtherCATSecurityService.h
    ../apps/ecat-studio/services/EtherCATComplianceService.cpp
    ../apps/ecat-studio/services/EtherCATComplianceService.h
    ../apps/ecat-studio/services/EtherCATCertificationService.cpp
    ../apps/ecat-studio/services/EtherCATCertificationService.h
    ../apps/ecat-studio/services/EtherCATAnalyticsService.cpp
    ../apps/ecat-studio/services/EtherCATAnalyticsService.h
    ../apps/ecat-studio/services/EtherCATDeploymentService.cpp
    ../apps/ecat-studio/services/EtherCATDeploymentService.h
    ../apps/ecat-studio/services/EtherCATUpdateService.cpp
    ../apps/ecat-studio/services/EtherCATUpdateService.h
    ../apps/ecat-studio/services/EtherCATMaintenanceService.cpp
    ../apps/ecat-studio/services/EtherCATMaintenanceService.h
    ../apps/ecat-studio/services/EtherCATIntegrationService.cpp
    ../apps/ecat-studio/services/EtherCATIntegrationService.h
    ../apps/ecat-studio/services/EtherCATSyncService.cpp
    ../apps/ecat-studio/services/EtherCATSyncService.h
    ../apps/ecat-studio/services/EtherCATReplicationService.cpp
    ../apps/ecat-studio/services/EtherCATReplicationService.h
    ../apps/ecat-studio/services/EtherCATVisualizationService.cpp
    ../apps/ecat-studio/services/EtherCATVisualizationService.h
    ../apps/ecat-studio/services/EtherCATReportingService.cpp
    ../apps/ecat-studio/services/EtherCATReportingService.h
    ../apps/ecat-studio/services/EtherCATDocumentationService.cpp
    ../apps/ecat-studio/services/EtherCATDocumentationService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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
    fixtures/UITestFixture.cpp
    fixtures/UITestFixture.h
)
target_include_directories(ui_fixture_test PRIVATE
    ${CMAKE_SOURCE_DIR}/tests
)
target_link_libraries(ui_fixture_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
set_target_properties(ui_fixture_test PROPERTIES AUTOMOC ON)
add_test(NAME ui_fixture_test COMMAND ui_fixture_test)
set_tests_properties(ui_fixture_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── SdoCachePlugin + SdoCacheService + PdoMappingService ──────────────
add_executable(sdo_cache_plugin_test
    sdo_cache_plugin_test.cpp
    ../apps/ecat-studio/plugins/sdocache/SdoCachePlugin.cpp
    ../apps/ecat-studio/plugins/sdocache/SdoCachePlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/SdoCacheService.cpp
    ../apps/ecat-studio/services/SdoCacheService.h
    ../apps/ecat-studio/services/PdoMappingService.cpp
    ../apps/ecat-studio/services/PdoMappingService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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

# ── ThemeCustomizerPlugin (GUI) ──────────────────────────────────────
add_executable(theme_customizer_plugin_test
    theme_customizer_plugin_test.cpp
    ../apps/ecat-studio/plugins/themecustomizer/ThemeCustomizerPlugin.cpp
    ../apps/ecat-studio/plugins/themecustomizer/ThemeCustomizerPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(theme_customizer_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(theme_customizer_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(theme_customizer_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME theme_customizer_plugin_test COMMAND theme_customizer_plugin_test)
set_tests_properties(theme_customizer_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── KeyboardShortcutsPlugin (GUI) ────────────────────────────────────
add_executable(keyboard_shortcuts_plugin_test
    keyboard_shortcuts_plugin_test.cpp
    ../apps/ecat-studio/plugins/shortcuts/KeyboardShortcutsPlugin.cpp
    ../apps/ecat-studio/plugins/shortcuts/KeyboardShortcutsPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(keyboard_shortcuts_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(keyboard_shortcuts_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(keyboard_shortcuts_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME keyboard_shortcuts_plugin_test COMMAND keyboard_shortcuts_plugin_test)
set_tests_properties(keyboard_shortcuts_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── UserPreferencesPlugin (GUI) ──────────────────────────────────────
add_executable(user_preferences_plugin_test
    user_preferences_plugin_test.cpp
    ../apps/ecat-studio/plugins/preferences/UserPreferencesPlugin.cpp
    ../apps/ecat-studio/plugins/preferences/UserPreferencesPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(user_preferences_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(user_preferences_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(user_preferences_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME user_preferences_plugin_test COMMAND user_preferences_plugin_test)
set_tests_properties(user_preferences_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── CoE/FoE/EoE Services (unit) ─────────────────────────────────────
add_executable(coe_foe_eoe_service_test
    coe_foe_eoe_service_test.cpp
    ../apps/ecat-studio/services/CoEService.cpp
    ../apps/ecat-studio/services/CoEService.h
    ../apps/ecat-studio/services/FoEService.cpp
    ../apps/ecat-studio/services/FoEService.h
    ../apps/ecat-studio/services/EoEService.cpp
    ../apps/ecat-studio/services/EoEService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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
set_target_properties(coe_foe_eoe_service_test PROPERTIES AUTOMOC ON)
add_test(NAME coe_foe_eoe_service_test COMMAND coe_foe_eoe_service_test)

# ── TracePlugin + TraceService (GUI) ────────────────────────────────────
add_executable(trace_plugin_test
    trace_plugin_test.cpp
    ../apps/ecat-studio/plugins/trace/TracePlugin.cpp
    ../apps/ecat-studio/plugins/trace/TracePlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/TraceService.cpp
    ../apps/ecat-studio/services/TraceService.h
)

target_include_directories(trace_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
)

target_link_libraries(trace_plugin_test PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

set_target_properties(trace_plugin_test PROPERTIES AUTOMOC ON)

add_test(NAME trace_plugin_test COMMAND trace_plugin_test)
set_tests_properties(trace_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── LogicAnalyzerPlugin (GUI) ────────────────────────────────────────────
add_executable(logicanalyzer_plugin_test
    logicanalyzer_plugin_test.cpp
    ../apps/ecat-studio/plugins/logicanalyzer/LogicAnalyzerPlugin.cpp
    ../apps/ecat-studio/plugins/logicanalyzer/LogicAnalyzerPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/TraceService.cpp
    ../apps/ecat-studio/services/TraceService.h
)

target_include_directories(logicanalyzer_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
)

target_link_libraries(logicanalyzer_plugin_test PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

set_target_properties(logicanalyzer_plugin_test PROPERTIES AUTOMOC ON)

add_test(NAME logicanalyzer_plugin_test COMMAND logicanalyzer_plugin_test)
set_tests_properties(logicanalyzer_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── MasterApiService (unit) ────────────────────────────────────────
add_executable(master_api_service_test
    master_api_service_test.cpp
    ../apps/ecat-studio/services/MasterApiService.cpp
    ../apps/ecat-studio/services/MasterApiService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(master_api_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
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
    ../apps/ecat-studio/services/DomainService.cpp
    ../apps/ecat-studio/services/DomainService.h
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
    ../apps/ecat-studio/services/SyncManagerService.cpp
    ../apps/ecat-studio/services/SyncManagerService.h
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

# ── ConnectionPool (unit) ──────────────────────────────────────────
add_executable(connection_pool_test
    connection_pool_test.cpp
    ../apps/ecat-studio/services/ConnectionPool.cpp
    ../apps/ecat-studio/services/ConnectionPool.h
)
target_include_directories(connection_pool_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(connection_pool_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Test
)
set_target_properties(connection_pool_test PROPERTIES AUTOMOC ON)
add_test(NAME connection_pool_test COMMAND connection_pool_test)

# ── StartupOptimizer (unit) ────────────────────────────────────────
add_executable(startup_optimizer_test
    startup_optimizer_test.cpp
    ../apps/ecat-studio/services/StartupOptimizer.cpp
    ../apps/ecat-studio/services/StartupOptimizer.h
    ../apps/ecat-studio/services/ServiceContainer.cpp
    ../apps/ecat-studio/services/SdoOptimizationService.cpp
    ../apps/ecat-studio/services/SdoOptimizationService.h
    ../apps/ecat-studio/services/PdoMappingOptimizationService.cpp
    ../apps/ecat-studio/services/PdoMappingOptimizationService.h
    ../apps/ecat-studio/services/HardwareVerificationService.cpp
    ../apps/ecat-studio/services/HardwareVerificationService.h
    ../apps/ecat-studio/services/AdvancedErrorAnalysisService.cpp
    ../apps/ecat-studio/services/AdvancedErrorAnalysisService.h
    ../apps/ecat-studio/services/FreeRunConfigurationService.cpp
    ../apps/ecat-studio/services/FreeRunConfigurationService.h
    ../apps/ecat-studio/services/FreeRunMonitoringService.cpp
    ../apps/ecat-studio/services/FreeRunMonitoringService.h
    ../apps/ecat-studio/services/PdoConfigurationService.cpp
    ../apps/ecat-studio/services/PdoConfigurationService.h
    ../apps/ecat-studio/services/OpStateService.cpp
    ../apps/ecat-studio/services/OpStateService.h
    ../apps/ecat-studio/plugins/oscilloscope/OscilloscopeService.cpp
    ../apps/ecat-studio/plugins/oscilloscope/OscilloscopeService.h
    ../apps/ecat-studio/plugins/protocol/ProtocolAnalyzerService.cpp
    ../apps/ecat-studio/plugins/protocol/ProtocolAnalyzerService.h
    ../apps/ecat-studio/services/WorkflowAnalyticsService.cpp
    ../apps/ecat-studio/services/WorkflowAnalyticsService.h
    ../apps/ecat-studio/services/FreeRunOptimizationService.cpp
    ../apps/ecat-studio/services/FreeRunOptimizationService.h
    ../apps/ecat-studio/services/RealtimePerformanceService.cpp
    ../apps/ecat-studio/services/RealtimePerformanceService.h
    ../apps/ecat-studio/services/RealtimeOptimizerService.cpp
    ../apps/ecat-studio/services/RealtimeOptimizerService.h
    ../apps/ecat-studio/services/ScriptingService.cpp
    ../apps/ecat-studio/services/ScriptingService.h
    ../apps/ecat-studio/services/ProjectManagementService.cpp
    ../apps/ecat-studio/services/ProjectManagementService.h
    ../apps/ecat-studio/services/TaskManagementService.cpp
    ../apps/ecat-studio/services/TaskManagementService.h
    ../apps/ecat-studio/services/ResourceManagementService.cpp
    ../apps/ecat-studio/services/ResourceManagementService.h
    ../apps/ecat-studio/services/ProjectPlanningService.cpp
    ../apps/ecat-studio/services/ProjectPlanningService.h
    ../apps/ecat-studio/services/ProjectTrackingService.cpp
    ../apps/ecat-studio/services/ProjectTrackingService.h
    ../apps/ecat-studio/services/ProjectReportingService.cpp
    ../apps/ecat-studio/services/ProjectReportingService.h
    ../apps/ecat-studio/services/WorkflowAutomationService.cpp
    ../apps/ecat-studio/services/WorkflowAutomationService.h
    ../apps/ecat-studio/services/WorkflowSchedulingService.cpp
    ../apps/ecat-studio/services/WorkflowSchedulingService.h
    ../apps/ecat-studio/services/WorkflowMonitoringService.cpp
    ../apps/ecat-studio/services/WorkflowMonitoringService.h
    ../apps/ecat-studio/services/WorkflowVisualizationService.cpp
    ../apps/ecat-studio/services/WorkflowVisualizationService.h
    ../apps/ecat-studio/services/WorkflowReportingService.cpp
    ../apps/ecat-studio/services/WorkflowReportingService.h
    ../apps/ecat-studio/services/WorkflowIntegrationService.cpp
    ../apps/ecat-studio/services/WorkflowIntegrationService.h
    ../apps/ecat-studio/services/WorkflowSecurityService.cpp
    ../apps/ecat-studio/services/WorkflowSecurityService.h
    ../apps/ecat-studio/services/WorkflowUpdateService.cpp
    ../apps/ecat-studio/services/WorkflowUpdateService.h
    ../apps/ecat-studio/services/WorkflowMaintenanceService.cpp
    ../apps/ecat-studio/services/WorkflowMaintenanceService.h
    ../apps/ecat-studio/services/WorkflowIntegrationHubService.cpp
    ../apps/ecat-studio/services/WorkflowIntegrationHubService.h
    ../apps/ecat-studio/services/WorkflowSyncService.cpp
    ../apps/ecat-studio/services/WorkflowSyncService.h
    ../apps/ecat-studio/services/WorkflowVisualizationStudioService.cpp
    ../apps/ecat-studio/services/WorkflowVisualizationStudioService.h
    ../apps/ecat-studio/services/WorkflowReportDesignerService.cpp
    ../apps/ecat-studio/services/WorkflowReportDesignerService.h
    ../apps/ecat-studio/services/WorkflowDocumentationBrowserService.cpp
    ../apps/ecat-studio/services/WorkflowDocumentationBrowserService.h
    ../apps/ecat-studio/services/WorkflowSecurityManagerService.cpp
    ../apps/ecat-studio/services/WorkflowSecurityManagerService.h
    ../apps/ecat-studio/services/WorkflowComplianceManagerService.cpp
    ../apps/ecat-studio/services/WorkflowComplianceManagerService.h
    ../apps/ecat-studio/services/WorkflowCertificationManagerService.cpp
    ../apps/ecat-studio/services/WorkflowCertificationManagerService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/services/SdoService.cpp
    ../apps/ecat-studio/services/SdoService.h
    ../apps/ecat-studio/services/WatchService.cpp
    ../apps/ecat-studio/services/WatchService.h
    ../apps/ecat-studio/services/TopologyService.cpp
    ../apps/ecat-studio/services/TopologyService.h
    ../apps/ecat-studio/services/DcSyncService.cpp
    ../apps/ecat-studio/services/DcSyncService.h
    ../apps/ecat-studio/services/AlEventService.cpp
    ../apps/ecat-studio/services/AlEventService.h
    ../apps/ecat-studio/services/SignalService.cpp
    ../apps/ecat-studio/services/SignalService.h
    ../apps/ecat-studio/services/PerformanceMonitorService.cpp
    ../apps/ecat-studio/services/PerformanceMonitorService.h
    ../apps/ecat-studio/services/EsiService.cpp
    ../apps/ecat-studio/services/EsiService.h
    ../apps/ecat-studio/services/BusStatsService.cpp
    ../apps/ecat-studio/services/BusStatsService.h
    ../apps/ecat-studio/services/WatchdogService.cpp
    ../apps/ecat-studio/services/WatchdogService.h
    ../apps/ecat-studio/services/SafetyController.cpp
    ../apps/ecat-studio/services/SafetyController.h
    ../apps/ecat-studio/services/DiagnosticReportService.cpp
    ../apps/ecat-studio/services/DiagnosticReportService.h
    ../apps/ecat-studio/services/ProjectManagerService.cpp
    ../apps/ecat-studio/services/ProjectManagerService.h
    ../apps/ecat-studio/services/ConfigurationService.cpp
    ../apps/ecat-studio/services/ConfigurationService.h
    ../apps/ecat-studio/services/AlarmService.cpp
    ../apps/ecat-studio/services/AlarmService.h
    ../apps/ecat-studio/services/LoggingService.cpp
    ../apps/ecat-studio/services/LoggingService.h
    ../apps/ecat-studio/services/ChartService.cpp
    ../apps/ecat-studio/services/ChartService.h
    ../apps/ecat-studio/services/BatchOperationService.cpp
    ../apps/ecat-studio/services/BatchOperationService.h
    ../apps/ecat-studio/services/NetworkDiagnosticsService.cpp
    ../apps/ecat-studio/services/NetworkDiagnosticsService.h
    ../apps/ecat-studio/services/EcatHealthService.cpp
    ../apps/ecat-studio/services/EcatHealthService.h
    ../apps/ecat-studio/services/ExportService.cpp
    ../apps/ecat-studio/services/ExportService.h
    ../apps/ecat-studio/services/DataPipelineService.cpp
    ../apps/ecat-studio/services/DataPipelineService.h
    ../apps/ecat-studio/services/DeviceManagerService.cpp
    ../apps/ecat-studio/services/DeviceManagerService.h
    ../apps/ecat-studio/services/FirmwareUpdateService.cpp
    ../apps/ecat-studio/services/FirmwareUpdateService.h
    ../apps/ecat-studio/services/ReportGeneratorService.cpp
    ../apps/ecat-studio/services/ReportGeneratorService.h
    ../apps/ecat-studio/services/MasterManagerService.cpp
    ../apps/ecat-studio/services/MasterManagerService.h
    ../apps/ecat-studio/services/DistributedClockService.cpp
    ../apps/ecat-studio/services/DistributedClockService.h
    ../apps/ecat-studio/services/DcSyncPrecisionService.cpp
    ../apps/ecat-studio/services/DcSyncPrecisionService.h
    ../apps/ecat-studio/services/DcSyncOptimizerService.cpp
    ../apps/ecat-studio/services/DcSyncOptimizerService.h
    ../apps/ecat-studio/services/OnlineDiagnosticsService.cpp
    ../apps/ecat-studio/services/OnlineDiagnosticsService.h
    ../apps/ecat-studio/services/MultiMasterService.cpp
    ../apps/ecat-studio/services/MultiMasterService.h
    ../apps/ecat-studio/services/SdoCacheService.cpp
    ../apps/ecat-studio/services/SdoCacheService.h
    ../apps/ecat-studio/services/PdoMappingService.cpp
    ../apps/ecat-studio/services/PdoMappingService.h
    ../apps/ecat-studio/services/CoEService.cpp
    ../apps/ecat-studio/services/CoEService.h
    ../apps/ecat-studio/services/FoEService.cpp
    ../apps/ecat-studio/services/FoEService.h
    ../apps/ecat-studio/services/EoEService.cpp
    ../apps/ecat-studio/services/EoEService.h
    ../apps/ecat-studio/services/TraceService.cpp
    ../apps/ecat-studio/services/TraceService.h
    ../apps/ecat-studio/services/MasterApiService.cpp
    ../apps/ecat-studio/services/MasterApiService.h
    ../apps/ecat-studio/services/DomainService.cpp
    ../apps/ecat-studio/services/DomainService.h
    ../apps/ecat-studio/services/SyncManagerService.cpp
    ../apps/ecat-studio/services/SyncManagerService.h
    ../apps/ecat-studio/services/StateMachineService.cpp
    ../apps/ecat-studio/services/StateMachineService.h
    ../apps/ecat-studio/services/ErrorHandlingService.cpp
    ../apps/ecat-studio/services/ErrorHandlingService.h
    ../apps/ecat-studio/services/DiagnosticService.cpp
    ../apps/ecat-studio/services/DiagnosticService.h
    ../apps/ecat-studio/services/HotConnectService.cpp
    ../apps/ecat-studio/services/HotConnectService.h
    ../apps/ecat-studio/services/RedundancyService.cpp
    ../apps/ecat-studio/services/RedundancyService.h
    ../apps/ecat-studio/services/CableDiagnosticsService.cpp
    ../apps/ecat-studio/services/CableDiagnosticsService.h
    ../apps/ecat-studio/services/EtherCATMonitorService.cpp
    ../apps/ecat-studio/services/EtherCATMonitorService.h
    ../apps/ecat-studio/services/EtherCATAnalyzerService.cpp
    ../apps/ecat-studio/services/EtherCATAnalyzerService.h
    ../apps/ecat-studio/services/EtherCATOptimizerService.cpp
    ../apps/ecat-studio/services/EtherCATOptimizerService.h
    ../apps/ecat-studio/services/EtherCATConfigService.cpp
    ../apps/ecat-studio/services/EtherCATConfigService.h
    ../apps/ecat-studio/services/EtherCATBackupService.cpp
    ../apps/ecat-studio/services/EtherCATBackupService.h
    ../apps/ecat-studio/services/EtherCATRecoveryService.cpp
    ../apps/ecat-studio/services/EtherCATRecoveryService.h
    ../apps/ecat-studio/services/EtherCATSimulationService.cpp
    ../apps/ecat-studio/services/EtherCATSimulationService.h
    ../apps/ecat-studio/services/EtherCATTestingService.cpp
    ../apps/ecat-studio/services/EtherCATTestingService.h
    ../apps/ecat-studio/services/EtherCATValidationService.cpp
    ../apps/ecat-studio/services/EtherCATValidationService.h
    ../apps/ecat-studio/services/EtherCATSecurityService.cpp
    ../apps/ecat-studio/services/EtherCATSecurityService.h
    ../apps/ecat-studio/services/EtherCATComplianceService.cpp
    ../apps/ecat-studio/services/EtherCATComplianceService.h
    ../apps/ecat-studio/services/EtherCATCertificationService.cpp
    ../apps/ecat-studio/services/EtherCATCertificationService.h
    ../apps/ecat-studio/services/EtherCATSecurityService.cpp
    ../apps/ecat-studio/services/EtherCATSecurityService.h
    ../apps/ecat-studio/services/EtherCATComplianceService.cpp
    ../apps/ecat-studio/services/EtherCATComplianceService.h
    ../apps/ecat-studio/services/EtherCATCertificationService.cpp
    ../apps/ecat-studio/services/EtherCATCertificationService.h
    ../apps/ecat-studio/services/EtherCATAnalyticsService.cpp
    ../apps/ecat-studio/services/EtherCATAnalyticsService.h
    ../apps/ecat-studio/services/EtherCATDeploymentService.cpp
    ../apps/ecat-studio/services/EtherCATDeploymentService.h
    ../apps/ecat-studio/services/EtherCATUpdateService.cpp
    ../apps/ecat-studio/services/EtherCATUpdateService.h
    ../apps/ecat-studio/services/EtherCATMaintenanceService.cpp
    ../apps/ecat-studio/services/EtherCATMaintenanceService.h
    ../apps/ecat-studio/services/EtherCATIntegrationService.cpp
    ../apps/ecat-studio/services/EtherCATIntegrationService.h
    ../apps/ecat-studio/services/EtherCATSyncService.cpp
    ../apps/ecat-studio/services/EtherCATSyncService.h
    ../apps/ecat-studio/services/EtherCATReplicationService.cpp
    ../apps/ecat-studio/services/EtherCATReplicationService.h
    ../apps/ecat-studio/services/EtherCATVisualizationService.cpp
    ../apps/ecat-studio/services/EtherCATVisualizationService.h
    ../apps/ecat-studio/services/EtherCATReportingService.cpp
    ../apps/ecat-studio/services/EtherCATReportingService.h
    ../apps/ecat-studio/services/EtherCATDocumentationService.cpp
    ../apps/ecat-studio/services/EtherCATDocumentationService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(startup_optimizer_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(startup_optimizer_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::Test
    ecat_core
)
set_target_properties(startup_optimizer_test PROPERTIES AUTOMOC ON)
add_test(NAME startup_optimizer_test COMMAND startup_optimizer_test)

# ── DataCache (unit) ──────────────────────────────────────────────
add_executable(data_cache_test
    data_cache_test.cpp
    ../apps/ecat-studio/services/DataCache.cpp
    ../apps/ecat-studio/services/DataCache.h
)
target_include_directories(data_cache_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(data_cache_test PRIVATE
    Qt6::Core
    Qt6::Test
)
set_target_properties(data_cache_test PROPERTIES AUTOMOC ON)
add_test(NAME data_cache_test COMMAND data_cache_test)

# ── BatchProcessor (unit) ─────────────────────────────────────────
add_executable(batch_processor_test
    batch_processor_test.cpp
    ../apps/ecat-studio/services/BatchProcessor.cpp
    ../apps/ecat-studio/services/BatchProcessor.h
)
target_include_directories(batch_processor_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(batch_processor_test PRIVATE
    Qt6::Core
    Qt6::Test
)
set_target_properties(batch_processor_test PROPERTIES AUTOMOC ON)
add_test(NAME batch_processor_test COMMAND batch_processor_test)

# ── MasterApiService Performance Test ──────────────────────────────
add_executable(master_api_performance_test
    performance/master_api_performance_test.cpp
    ../apps/ecat-studio/services/MasterApiService.cpp
    ../apps/ecat-studio/services/MasterApiService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
    ../src/core/EthercatTypes.cpp
    ../src/core/EthercatTypes.h
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
    performance/domain_service_performance_test.cpp
    ../apps/ecat-studio/services/DomainService.cpp
    ../apps/ecat-studio/services/DomainService.h
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
    performance/sync_manager_performance_test.cpp
    ../apps/ecat-studio/services/SyncManagerService.cpp
    ../apps/ecat-studio/services/SyncManagerService.h
)
target_include_directories(sync_manager_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(sync_manager_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(sync_manager_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME sync_manager_performance_test COMMAND sync_manager_performance_test)

# ── DiagramPlugin (unit) ────────────────────────────────────────────
add_executable(diagram_plugin_test
    diagram_plugin_test.cpp
    ../apps/ecat-studio/plugins/diagram/DiagramPlugin.cpp
    ../apps/ecat-studio/plugins/diagram/DiagramPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(diagram_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(diagram_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(diagram_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME diagram_plugin_test COMMAND diagram_plugin_test)
set_tests_properties(diagram_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── FormulaPlugin (unit) ────────────────────────────────────────────
add_executable(formula_plugin_test
    formula_plugin_test.cpp
    ../apps/ecat-studio/plugins/formula/FormulaPlugin.cpp
    ../apps/ecat-studio/plugins/formula/FormulaPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(formula_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(formula_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(formula_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME formula_plugin_test COMMAND formula_plugin_test)
set_tests_properties(formula_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── ScriptLibraryPlugin (unit) ──────────────────────────────────────
add_executable(scriptlibrary_plugin_test
    scriptlibrary_plugin_test.cpp
    ../apps/ecat-studio/plugins/scriptlibrary/ScriptLibraryPlugin.cpp
    ../apps/ecat-studio/plugins/scriptlibrary/ScriptLibraryPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(scriptlibrary_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(scriptlibrary_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(scriptlibrary_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME scriptlibrary_plugin_test COMMAND scriptlibrary_plugin_test)
set_tests_properties(scriptlibrary_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── New Plugin Integration Test ─────────────────────────────────────
add_executable(plugin_new_integration_test
    plugin_new_integration_test.cpp
    ../apps/ecat-studio/plugins/diagram/DiagramPlugin.cpp
    ../apps/ecat-studio/plugins/diagram/DiagramPlugin.h
    ../apps/ecat-studio/plugins/diagram/DiagramWidget.cpp
    ../apps/ecat-studio/plugins/diagram/DiagramWidget.h
    ../apps/ecat-studio/plugins/formula/FormulaPlugin.cpp
    ../apps/ecat-studio/plugins/formula/FormulaPlugin.h
    ../apps/ecat-studio/plugins/scriptlibrary/ScriptLibraryPlugin.cpp
    ../apps/ecat-studio/plugins/scriptlibrary/ScriptLibraryPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
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

# ── StateMachineService (unit) ──────────────────────────────────────
add_executable(state_machine_service_test
    state_machine_service_test.cpp
    ../apps/ecat-studio/services/StateMachineService.cpp
    ../apps/ecat-studio/services/StateMachineService.h
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
    ../apps/ecat-studio/services/ErrorHandlingService.cpp
    ../apps/ecat-studio/services/ErrorHandlingService.h
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
    ../apps/ecat-studio/services/DiagnosticService.cpp
    ../apps/ecat-studio/services/DiagnosticService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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

# ── SimulationPlugin (integration) ──────────────────────────────────
add_executable(simulation_plugin_test
    simulation_plugin_test.cpp
    ../apps/ecat-studio/plugins/simulation/SimulationPlugin.cpp
    ../apps/ecat-studio/plugins/simulation/SimulationPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
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
    ../apps/ecat-studio/plugins/calibration/CalibrationPlugin.cpp
    ../apps/ecat-studio/plugins/calibration/CalibrationPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_include_directories(calibration_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
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
    ../apps/ecat-studio/plugins/documentation/DocumentationPlugin.cpp
    ../apps/ecat-studio/plugins/documentation/DocumentationPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
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
    ../apps/ecat-studio/plugins/wizard/WizardPlugin.cpp
    ../apps/ecat-studio/plugins/wizard/WizardPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
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
    ../apps/ecat-studio/plugins/template/TemplatePlugin.cpp
    ../apps/ecat-studio/plugins/template/TemplatePlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
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

# ── SecurityManagerPlugin ──────────────────────────────────────────
add_executable(securitymanager_plugin_test
    securitymanager_plugin_test.cpp
    ../apps/ecat-studio/plugins/securitymanager/SecurityManagerPlugin.cpp
    ../apps/ecat-studio/plugins/securitymanager/SecurityManagerPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
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
    ../apps/ecat-studio/plugins/compliancechecker/ComplianceCheckerPlugin.cpp
    ../apps/ecat-studio/plugins/compliancechecker/ComplianceCheckerPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_include_directories(compliancechecker_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(compliancechecker_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
set_target_properties(compliancechecker_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME compliancechecker_plugin_test COMMAND compliancechecker_plugin_test)
set_tests_properties(compliancechecker_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── CertificationManagerPlugin ─────────────────────────────────────
add_executable(certificationmanager_plugin_test
    certificationmanager_plugin_test.cpp
    ../apps/ecat-studio/plugins/certificationmanager/CertificationManagerPlugin.cpp
    ../apps/ecat-studio/plugins/certificationmanager/CertificationManagerPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_include_directories(certificationmanager_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(certificationmanager_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
set_target_properties(certificationmanager_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME certificationmanager_plugin_test COMMAND certificationmanager_plugin_test)
set_tests_properties(certificationmanager_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── ReportPlugin (integration) ──────────────────────────────────────
add_executable(report_plugin_test
    report_plugin_test.cpp
    ../apps/ecat-studio/plugins/report/ReportPlugin.cpp
    ../apps/ecat-studio/plugins/report/ReportPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
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

# ── StateMachineService Performance Test ────────────────────────────
add_executable(state_machine_performance_test
    performance/state_machine_performance_test.cpp
    ../apps/ecat-studio/services/StateMachineService.cpp
    ../apps/ecat-studio/services/StateMachineService.h
)
target_include_directories(state_machine_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(state_machine_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(state_machine_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME state_machine_performance_test COMMAND state_machine_performance_test)

# ── ErrorHandlingService Performance Test ───────────────────────────
add_executable(error_handling_performance_test
    performance/error_handling_performance_test.cpp
    ../apps/ecat-studio/services/ErrorHandlingService.cpp
    ../apps/ecat-studio/services/ErrorHandlingService.h
)
target_include_directories(error_handling_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(error_handling_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(error_handling_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME error_handling_performance_test COMMAND error_handling_performance_test)

# ── DiagnosticReportService Performance Test ────────────────────────
add_executable(diagnostic_report_performance_test
    performance/diagnostic_report_performance_test.cpp
    ../apps/ecat-studio/services/DiagnosticReportService.cpp
    ../apps/ecat-studio/services/DiagnosticReportService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/services/TopologyService.cpp
    ../apps/ecat-studio/services/TopologyService.h
    ../apps/ecat-studio/services/DcSyncService.cpp
    ../apps/ecat-studio/services/DcSyncService.h
    ../apps/ecat-studio/services/PerformanceMonitorService.cpp
    ../apps/ecat-studio/services/PerformanceMonitorService.h
    ../apps/ecat-studio/services/WatchdogService.cpp
    ../apps/ecat-studio/services/WatchdogService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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

# ── HotConnectService Test ──────────────────────────────────────────
add_executable(hot_connect_service_test
    hot_connect_service_test.cpp
    ../apps/ecat-studio/services/HotConnectService.cpp
    ../apps/ecat-studio/services/HotConnectService.h
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
    ../apps/ecat-studio/services/RedundancyService.cpp
    ../apps/ecat-studio/services/RedundancyService.h
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
    ../apps/ecat-studio/services/CableDiagnosticsService.cpp
    ../apps/ecat-studio/services/CableDiagnosticsService.h
)
target_include_directories(cable_diagnostics_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(cable_diagnostics_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(cable_diagnostics_service_test PROPERTIES AUTOMOC ON)
add_test(NAME cable_diagnostics_service_test COMMAND cable_diagnostics_service_test)

# ── HotConnectService Performance Test ──────────────────────────────
add_executable(hot_connect_performance_test
    performance/hot_connect_performance_test.cpp
    ../apps/ecat-studio/services/HotConnectService.cpp
    ../apps/ecat-studio/services/HotConnectService.h
)
target_include_directories(hot_connect_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(hot_connect_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(hot_connect_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME hot_connect_performance_test COMMAND hot_connect_performance_test)

# ── RedundancyService Performance Test ──────────────────────────────
add_executable(redundancy_performance_test
    performance/redundancy_performance_test.cpp
    ../apps/ecat-studio/services/RedundancyService.cpp
    ../apps/ecat-studio/services/RedundancyService.h
)
target_include_directories(redundancy_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(redundancy_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(redundancy_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME redundancy_performance_test COMMAND redundancy_performance_test)

# ── CableDiagnosticsService Performance Test ────────────────────────
add_executable(cable_diagnostics_performance_test
    performance/cable_diagnostics_performance_test.cpp
    ../apps/ecat-studio/services/CableDiagnosticsService.cpp
    ../apps/ecat-studio/services/CableDiagnosticsService.h
)
target_include_directories(cable_diagnostics_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(cable_diagnostics_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(cable_diagnostics_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME cable_diagnostics_performance_test COMMAND cable_diagnostics_performance_test)

# ── EtherCATMonitorService Test ─────────────────────────────────────
add_executable(ethercat_monitor_service_test
    ethercat_monitor_service_test.cpp
    ../apps/ecat-studio/services/EtherCATMonitorService.cpp
    ../apps/ecat-studio/services/EtherCATMonitorService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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
    ../apps/ecat-studio/services/EtherCATAnalyzerService.cpp
    ../apps/ecat-studio/services/EtherCATAnalyzerService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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
    ../apps/ecat-studio/services/EtherCATOptimizerService.cpp
    ../apps/ecat-studio/services/EtherCATOptimizerService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(ethercat_optimizer_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(ethercat_optimizer_service_test PRIVATE
    Qt6::Core Qt6::Network Qt6::Test ecat_core
)
set_target_properties(ethercat_optimizer_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_optimizer_service_test COMMAND ethercat_optimizer_service_test)

# ── EtherCATMonitorService Performance Test ────────────────────────
add_executable(ethercat_monitor_performance_test
    performance/ethercat_monitor_performance_test.cpp
    ../apps/ecat-studio/services/EtherCATMonitorService.cpp
    ../apps/ecat-studio/services/EtherCATMonitorService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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
    performance/ethercat_analyzer_performance_test.cpp
    ../apps/ecat-studio/services/EtherCATAnalyzerService.cpp
    ../apps/ecat-studio/services/EtherCATAnalyzerService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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
    performance/ethercat_optimizer_performance_test.cpp
    ../apps/ecat-studio/services/EtherCATOptimizerService.cpp
    ../apps/ecat-studio/services/EtherCATOptimizerService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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

# ── DashboardDesignerPlugin (integration) ─────────────────────────
add_executable(dashboarddesigner_plugin_test
    dashboarddesigner_plugin_test.cpp
    ../apps/ecat-studio/plugins/dashboarddesigner/DashboardDesignerPlugin.cpp
    ../apps/ecat-studio/plugins/dashboarddesigner/DashboardDesignerPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
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
    ../apps/ecat-studio/plugins/alarmmanager/AlarmManagerPlugin.cpp
    ../apps/ecat-studio/plugins/alarmmanager/AlarmManagerPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
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
    ../apps/ecat-studio/plugins/datalogger/DataLoggerPlugin.cpp
    ../apps/ecat-studio/plugins/datalogger/DataLoggerPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
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

# ── EtherCATConfigService (unit) ────────────────────────────────────
add_executable(ethercat_config_service_test
    ethercat_config_service_test.cpp
    ../apps/ecat-studio/services/EtherCATConfigService.cpp
    ../apps/ecat-studio/services/EtherCATConfigService.h
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
    ../apps/ecat-studio/services/EtherCATBackupService.cpp
    ../apps/ecat-studio/services/EtherCATBackupService.h
)
target_link_libraries(ethercat_backup_service_test PRIVATE Qt6::Core Qt6::Test)
target_include_directories(ethercat_backup_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
set_target_properties(ethercat_backup_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_backup_service_test COMMAND ethercat_backup_service_test)

# ── EtherCATRecoveryService (unit) ──────────────────────────────────
add_executable(ethercat_recovery_service_test
    ethercat_recovery_service_test.cpp
    ../apps/ecat-studio/services/EtherCATRecoveryService.cpp
    ../apps/ecat-studio/services/EtherCATRecoveryService.h
)
target_link_libraries(ethercat_recovery_service_test PRIVATE Qt6::Core Qt6::Test)
target_include_directories(ethercat_recovery_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
set_target_properties(ethercat_recovery_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_recovery_service_test COMMAND ethercat_recovery_service_test)

# ── WorkflowDesignerPlugin (unit) ─────────────────────────────────
add_executable(workflowdesigner_plugin_test
    workflowdesigner_plugin_test.cpp
    ../apps/ecat-studio/plugins/workflowdesigner/WorkflowDesignerPlugin.cpp
    ../apps/ecat-studio/plugins/workflowdesigner/WorkflowDesignerPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(workflowdesigner_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(workflowdesigner_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(workflowdesigner_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME workflowdesigner_plugin_test COMMAND workflowdesigner_plugin_test)
set_tests_properties(workflowdesigner_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── TestSuitePlugin (unit) ────────────────────────────────────────
add_executable(testsuite_plugin_test
    testsuite_plugin_test.cpp
    ../apps/ecat-studio/plugins/testsuite/TestSuitePlugin.cpp
    ../apps/ecat-studio/plugins/testsuite/TestSuitePlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(testsuite_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(testsuite_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(testsuite_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME testsuite_plugin_test COMMAND testsuite_plugin_test)
set_tests_properties(testsuite_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── DeploymentPlugin (unit) ───────────────────────────────────────
add_executable(deployment_plugin_test
    deployment_plugin_test.cpp
    ../apps/ecat-studio/plugins/deployment/DeploymentPlugin.cpp
    ../apps/ecat-studio/plugins/deployment/DeploymentPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(deployment_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(deployment_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(deployment_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME deployment_plugin_test COMMAND deployment_plugin_test)
set_tests_properties(deployment_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── DeploymentManagerPlugin (unit) ────────────────────────────────
add_executable(deploymentmanager_plugin_test
    deploymentmanager_plugin_test.cpp
    ../apps/ecat-studio/plugins/deploymentmanager/DeploymentManagerPlugin.cpp
    ../apps/ecat-studio/plugins/deploymentmanager/DeploymentManagerPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(deploymentmanager_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(deploymentmanager_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(deploymentmanager_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME deploymentmanager_plugin_test COMMAND deploymentmanager_plugin_test)
set_tests_properties(deploymentmanager_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── UpdateManagerPlugin (unit) ────────────────────────────────────
add_executable(updatemanager_plugin_test
    updatemanager_plugin_test.cpp
    ../apps/ecat-studio/plugins/updatemanager/UpdateManagerPlugin.cpp
    ../apps/ecat-studio/plugins/updatemanager/UpdateManagerPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(updatemanager_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(updatemanager_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(updatemanager_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME updatemanager_plugin_test COMMAND updatemanager_plugin_test)
set_tests_properties(updatemanager_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── MaintenanceSchedulerPlugin (unit) ─────────────────────────────
add_executable(maintenancescheduler_plugin_test
    maintenancescheduler_plugin_test.cpp
    ../apps/ecat-studio/plugins/maintenancescheduler/MaintenanceSchedulerPlugin.cpp
    ../apps/ecat-studio/plugins/maintenancescheduler/MaintenanceSchedulerPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(maintenancescheduler_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(maintenancescheduler_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(maintenancescheduler_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME maintenancescheduler_plugin_test COMMAND maintenancescheduler_plugin_test)
set_tests_properties(maintenancescheduler_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── EtherCATConfigService Performance Test ─────────────────────────
add_executable(ethercat_config_performance_test
    performance/ethercat_config_performance_test.cpp
    ../apps/ecat-studio/services/EtherCATConfigService.cpp
    ../apps/ecat-studio/services/EtherCATConfigService.h
)
target_include_directories(ethercat_config_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(ethercat_config_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_config_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_config_performance_test COMMAND ethercat_config_performance_test)

# ── EtherCATBackupService Performance Test ─────────────────────────
add_executable(ethercat_backup_performance_test
    performance/ethercat_backup_performance_test.cpp
    ../apps/ecat-studio/services/EtherCATBackupService.cpp
    ../apps/ecat-studio/services/EtherCATBackupService.h
)
target_include_directories(ethercat_backup_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(ethercat_backup_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_backup_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_backup_performance_test COMMAND ethercat_backup_performance_test)

# ── EtherCATRecoveryService Performance Test ───────────────────────
add_executable(ethercat_recovery_performance_test
    performance/ethercat_recovery_performance_test.cpp
    ../apps/ecat-studio/services/EtherCATRecoveryService.cpp
    ../apps/ecat-studio/services/EtherCATRecoveryService.h
)
target_include_directories(ethercat_recovery_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(ethercat_recovery_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_recovery_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_recovery_performance_test COMMAND ethercat_recovery_performance_test)

# ── EtherCATSimulationService Test ─────────────────────────────────
add_executable(ethercat_simulation_service_test
    ethercat_simulation_service_test.cpp
    ../apps/ecat-studio/services/EtherCATSimulationService.cpp
    ../apps/ecat-studio/services/EtherCATSimulationService.h
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
    ../apps/ecat-studio/services/EtherCATTestingService.cpp
    ../apps/ecat-studio/services/EtherCATTestingService.h
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
    ../apps/ecat-studio/services/EtherCATValidationService.cpp
    ../apps/ecat-studio/services/EtherCATValidationService.h
)
target_link_libraries(ethercat_validation_service_test PRIVATE Qt6::Core Qt6::Test)
target_include_directories(ethercat_validation_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
set_target_properties(ethercat_validation_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_validation_service_test COMMAND ethercat_validation_service_test)

# ── EtherCATSecurityService Test ────────────────────────────────────
add_executable(ethercat_security_service_test
    ethercat_security_service_test.cpp
    ../apps/ecat-studio/services/EtherCATSecurityService.cpp
    ../apps/ecat-studio/services/EtherCATSecurityService.h
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
    ../apps/ecat-studio/services/EtherCATComplianceService.cpp
    ../apps/ecat-studio/services/EtherCATComplianceService.h
)
target_link_libraries(ethercat_compliance_service_test PRIVATE Qt6::Core Qt6::Test)
target_include_directories(ethercat_compliance_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
set_target_properties(ethercat_compliance_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_compliance_service_test COMMAND ethercat_compliance_service_test)

# ── EtherCATCertificationService Test ───────────────────────────────
add_executable(ethercat_certification_service_test
    ethercat_certification_service_test.cpp
    ../apps/ecat-studio/services/EtherCATCertificationService.cpp
    ../apps/ecat-studio/services/EtherCATCertificationService.h
)
target_link_libraries(ethercat_certification_service_test PRIVATE Qt6::Core Qt6::Test)
target_include_directories(ethercat_certification_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
set_target_properties(ethercat_certification_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_certification_service_test COMMAND ethercat_certification_service_test)

# ── EtherCATSimulationService Performance Test ─────────────────────
add_executable(ethercat_simulation_performance_test
    performance/ethercat_simulation_performance_test.cpp
    ../apps/ecat-studio/services/EtherCATSimulationService.cpp
    ../apps/ecat-studio/services/EtherCATSimulationService.h
)
target_include_directories(ethercat_simulation_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(ethercat_simulation_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_simulation_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_simulation_performance_test COMMAND ethercat_simulation_performance_test)

# ── EtherCATTestingService Performance Test ────────────────────────
add_executable(ethercat_testing_performance_test
    performance/ethercat_testing_performance_test.cpp
    ../apps/ecat-studio/services/EtherCATTestingService.cpp
    ../apps/ecat-studio/services/EtherCATTestingService.h
)
target_include_directories(ethercat_testing_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(ethercat_testing_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_testing_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_testing_performance_test COMMAND ethercat_testing_performance_test)

# ── EtherCATValidationService Performance Test ─────────────────────
add_executable(ethercat_validation_performance_test
    performance/ethercat_validation_performance_test.cpp
    ../apps/ecat-studio/services/EtherCATValidationService.cpp
    ../apps/ecat-studio/services/EtherCATValidationService.h
)
target_include_directories(ethercat_validation_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(ethercat_validation_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_validation_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_validation_performance_test COMMAND ethercat_validation_performance_test)

# ── EtherCATSecurityService Performance Test ────────────────────────
add_executable(ethercat_security_performance_test
    performance/ethercat_security_performance_test.cpp
    ../apps/ecat-studio/services/EtherCATSecurityService.cpp
    ../apps/ecat-studio/services/EtherCATSecurityService.h
)
target_include_directories(ethercat_security_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(ethercat_security_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_security_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_security_performance_test COMMAND ethercat_security_performance_test)

# ── EtherCATComplianceService Performance Test ──────────────────────
add_executable(ethercat_compliance_performance_test
    performance/ethercat_compliance_performance_test.cpp
    ../apps/ecat-studio/services/EtherCATComplianceService.cpp
    ../apps/ecat-studio/services/EtherCATComplianceService.h
)
target_include_directories(ethercat_compliance_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(ethercat_compliance_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_compliance_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_compliance_performance_test COMMAND ethercat_compliance_performance_test)

# ── EtherCATCertificationService Performance Test ───────────────────
add_executable(ethercat_certification_performance_test
    performance/ethercat_certification_performance_test.cpp
    ../apps/ecat-studio/services/EtherCATCertificationService.cpp
    ../apps/ecat-studio/services/EtherCATCertificationService.h
)
target_include_directories(ethercat_certification_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(ethercat_certification_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_certification_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_certification_performance_test COMMAND ethercat_certification_performance_test)

# ── ConfigurationEditorPlugin (GUI) ────────────────────────────────────
add_executable(configeditor_plugin_test
    configeditor_plugin_test.cpp
    ../apps/ecat-studio/plugins/configeditor/ConfigurationEditorPlugin.cpp
    ../apps/ecat-studio/plugins/configeditor/ConfigurationEditorPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(configeditor_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(configeditor_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(configeditor_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME configeditor_plugin_test COMMAND configeditor_plugin_test)
set_tests_properties(configeditor_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── NetworkAnalyzerPlugin (GUI) ────────────────────────────────────────
add_executable(networkanalyzer_plugin_test
    networkanalyzer_plugin_test.cpp
    ../apps/ecat-studio/plugins/networkanalyzer/NetworkAnalyzerPlugin.cpp
    ../apps/ecat-studio/plugins/networkanalyzer/NetworkAnalyzerPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(networkanalyzer_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(networkanalyzer_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(networkanalyzer_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME networkanalyzer_plugin_test COMMAND networkanalyzer_plugin_test)
set_tests_properties(networkanalyzer_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── SystemMonitorPlugin (GUI) ──────────────────────────────────────────
add_executable(systemmonitor_plugin_test
    systemmonitor_plugin_test.cpp
    ../apps/ecat-studio/plugins/systemmonitor/SystemMonitorPlugin.cpp
    ../apps/ecat-studio/plugins/systemmonitor/SystemMonitorPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(systemmonitor_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(systemmonitor_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(systemmonitor_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME systemmonitor_plugin_test COMMAND systemmonitor_plugin_test)
set_tests_properties(systemmonitor_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")


# ── EtherCATAnalyticsService Test ─────────────────────────────────
add_executable(ethercat_analytics_service_test
    ethercat_analytics_service_test.cpp
    ../apps/ecat-studio/services/EtherCATAnalyticsService.cpp
    ../apps/ecat-studio/services/EtherCATAnalyticsService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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

# ── OptimizationDashboardPlugin (GUI) ──────────────────────────────────
add_executable(optimizationdashboard_plugin_test
    optimizationdashboard_plugin_test.cpp
    ../apps/ecat-studio/plugins/optimizationdashboard/OptimizationDashboardPlugin.cpp
    ../apps/ecat-studio/plugins/optimizationdashboard/OptimizationDashboardPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(optimizationdashboard_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(optimizationdashboard_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(optimizationdashboard_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME optimizationdashboard_plugin_test COMMAND optimizationdashboard_plugin_test)
set_tests_properties(optimizationdashboard_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── MonitoringDashboardPlugin (GUI) ────────────────────────────────────
add_executable(monitoringdashboard_plugin_test
    monitoringdashboard_plugin_test.cpp
    ../apps/ecat-studio/plugins/monitoringdashboard/MonitoringDashboardPlugin.cpp
    ../apps/ecat-studio/plugins/monitoringdashboard/MonitoringDashboardPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(monitoringdashboard_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(monitoringdashboard_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(monitoringdashboard_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME monitoringdashboard_plugin_test COMMAND monitoringdashboard_plugin_test)
set_tests_properties(monitoringdashboard_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── AnalyticsDashboardPlugin (GUI) ─────────────────────────────────────
add_executable(analyticsdashboard_plugin_test
    analyticsdashboard_plugin_test.cpp
    ../apps/ecat-studio/plugins/analyticsdashboard/AnalyticsDashboardPlugin.cpp
    ../apps/ecat-studio/plugins/analyticsdashboard/AnalyticsDashboardPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(analyticsdashboard_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(analyticsdashboard_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(analyticsdashboard_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME analyticsdashboard_plugin_test COMMAND analyticsdashboard_plugin_test)
set_tests_properties(analyticsdashboard_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── EtherCATDeploymentService ─────────────────────────────────────────
add_executable(ethercat_deployment_service_test
    ethercat_deployment_service_test.cpp
    ../apps/ecat-studio/services/EtherCATDeploymentService.cpp
    ../apps/ecat-studio/services/EtherCATDeploymentService.h
)
target_include_directories(ethercat_deployment_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(ethercat_deployment_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_deployment_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_deployment_service_test COMMAND ethercat_deployment_service_test)

# ── EtherCATUpdateService ─────────────────────────────────────────────
add_executable(ethercat_update_service_test
    ethercat_update_service_test.cpp
    ../apps/ecat-studio/services/EtherCATUpdateService.cpp
    ../apps/ecat-studio/services/EtherCATUpdateService.h
)
target_include_directories(ethercat_update_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(ethercat_update_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_update_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_update_service_test COMMAND ethercat_update_service_test)

# ── EtherCATMaintenanceService ────────────────────────────────────────
add_executable(ethercat_maintenance_service_test
    ethercat_maintenance_service_test.cpp
    ../apps/ecat-studio/services/EtherCATMaintenanceService.cpp
    ../apps/ecat-studio/services/EtherCATMaintenanceService.h
)
target_include_directories(ethercat_maintenance_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(ethercat_maintenance_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_maintenance_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_maintenance_service_test COMMAND ethercat_maintenance_service_test)

# ── EtherCATDeploymentService Performance ────────────────────────────
add_executable(ethercat_deployment_performance_test
    performance/ethercat_deployment_performance_test.cpp
    ../apps/ecat-studio/services/EtherCATDeploymentService.cpp
    ../apps/ecat-studio/services/EtherCATDeploymentService.h
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
    performance/ethercat_update_performance_test.cpp
    ../apps/ecat-studio/services/EtherCATUpdateService.cpp
    ../apps/ecat-studio/services/EtherCATUpdateService.h
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
    performance/ethercat_maintenance_performance_test.cpp
    ../apps/ecat-studio/services/EtherCATMaintenanceService.cpp
    ../apps/ecat-studio/services/EtherCATMaintenanceService.h
)
target_include_directories(ethercat_maintenance_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(ethercat_maintenance_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_maintenance_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_maintenance_performance_test COMMAND ethercat_maintenance_performance_test)

# ── EtherCAT Advanced Services Test ─────────────────────────────────
add_executable(test_advanced_services_test
    test_advanced_services_test.cpp
    ../apps/ecat-studio/services/EtherCATIntegrationService.cpp
    ../apps/ecat-studio/services/EtherCATIntegrationService.h
    ../apps/ecat-studio/services/EtherCATSyncService.cpp
    ../apps/ecat-studio/services/EtherCATSyncService.h
    ../apps/ecat-studio/services/EtherCATReplicationService.cpp
    ../apps/ecat-studio/services/EtherCATReplicationService.h
    ../apps/ecat-studio/services/EtherCATVisualizationService.cpp
    ../apps/ecat-studio/services/EtherCATVisualizationService.h
    ../apps/ecat-studio/services/EtherCATReportingService.cpp
    ../apps/ecat-studio/services/EtherCATReportingService.h
    ../apps/ecat-studio/services/EtherCATDocumentationService.cpp
    ../apps/ecat-studio/services/EtherCATDocumentationService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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

# ── IntegrationHubPlugin ───────────────────────────────────────────
add_executable(integrationhub_plugin_test
    integrationhub_plugin_test.cpp
    ../apps/ecat-studio/plugins/integrationhub/IntegrationHubPlugin.cpp
    ../apps/ecat-studio/plugins/integrationhub/IntegrationHubPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
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
    ../apps/ecat-studio/plugins/syncmanager/SyncManagerPlugin.cpp
    ../apps/ecat-studio/plugins/syncmanager/SyncManagerPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_include_directories(syncmanager_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(syncmanager_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
set_target_properties(syncmanager_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME syncmanager_plugin_test COMMAND syncmanager_plugin_test)
set_tests_properties(syncmanager_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── ReplicationManagerPlugin ───────────────────────────────────────
add_executable(replicationmanager_plugin_test
    replicationmanager_plugin_test.cpp
    ../apps/ecat-studio/plugins/replicationmanager/ReplicationManagerPlugin.cpp
    ../apps/ecat-studio/plugins/replicationmanager/ReplicationManagerPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_include_directories(replicationmanager_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(replicationmanager_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
set_target_properties(replicationmanager_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME replicationmanager_plugin_test COMMAND replicationmanager_plugin_test)
set_tests_properties(replicationmanager_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── VisualizationStudioPlugin ───────────────────────────────────────
add_executable(visualizationstudio_plugin_test
    visualizationstudio_plugin_test.cpp
    ../apps/ecat-studio/plugins/visualizationstudio/VisualizationStudioPlugin.cpp
    ../apps/ecat-studio/plugins/visualizationstudio/VisualizationStudioPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
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
    ../apps/ecat-studio/plugins/reportdesigner/ReportDesignerPlugin.cpp
    ../apps/ecat-studio/plugins/reportdesigner/ReportDesignerPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
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
    ../apps/ecat-studio/plugins/documentationbrowser/DocumentationBrowserPlugin.cpp
    ../apps/ecat-studio/plugins/documentationbrowser/DocumentationBrowserPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
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

# ── EtherCATIntegrationService ──────────────────────────────────────
add_executable(ethercat_integration_service_test
    ethercat_integration_service_test.cpp
    ../apps/ecat-studio/services/EtherCATIntegrationService.cpp
    ../apps/ecat-studio/services/EtherCATIntegrationService.h
)
target_include_directories(ethercat_integration_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(ethercat_integration_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_integration_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_integration_service_test COMMAND ethercat_integration_service_test)

# ── EtherCATSyncService ─────────────────────────────────────────────
add_executable(ethercat_sync_service_test
    ethercat_sync_service_test.cpp
    ../apps/ecat-studio/services/EtherCATSyncService.cpp
    ../apps/ecat-studio/services/EtherCATSyncService.h
)
target_include_directories(ethercat_sync_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_compile_definitions(ethercat_sync_service_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)
target_link_libraries(ethercat_sync_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_sync_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_sync_service_test COMMAND ethercat_sync_service_test)

# ── EtherCATReplicationService ──────────────────────────────────────
add_executable(ethercat_replication_service_test
    ethercat_replication_service_test.cpp
    ../apps/ecat-studio/services/EtherCATReplicationService.cpp
    ../apps/ecat-studio/services/EtherCATReplicationService.h
    ../apps/ecat-studio/services/EtherCATVisualizationService.cpp
    ../apps/ecat-studio/services/EtherCATVisualizationService.h
    ../apps/ecat-studio/services/EtherCATReportingService.cpp
    ../apps/ecat-studio/services/EtherCATReportingService.h
    ../apps/ecat-studio/services/EtherCATDocumentationService.cpp
    ../apps/ecat-studio/services/EtherCATDocumentationService.h
)
target_include_directories(ethercat_replication_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
target_compile_definitions(ethercat_replication_service_test PRIVATE
    SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
)
target_link_libraries(ethercat_replication_service_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
set_target_properties(ethercat_replication_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_replication_service_test COMMAND ethercat_replication_service_test)

# ── EtherCATIntegrationService Performance ──────────────────────────
add_executable(ethercat_integration_performance_test
    performance/ethercat_integration_performance_test.cpp
    ../apps/ecat-studio/services/EtherCATIntegrationService.cpp
    ../apps/ecat-studio/services/EtherCATIntegrationService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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

# ── EtherCATSyncService Performance ─────────────────────────────────
add_executable(ethercat_sync_performance_test
    performance/ethercat_sync_performance_test.cpp
    ../apps/ecat-studio/services/EtherCATSyncService.cpp
    ../apps/ecat-studio/services/EtherCATSyncService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(ethercat_sync_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(ethercat_sync_performance_test PRIVATE Qt6::Core Qt6::Network Qt6::Test ecat_core)
set_target_properties(ethercat_sync_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_sync_performance_test COMMAND ethercat_sync_performance_test)

# ── EtherCATReplicationService Performance ──────────────────────────
add_executable(ethercat_replication_performance_test
    performance/ethercat_replication_performance_test.cpp
    ../apps/ecat-studio/services/EtherCATReplicationService.cpp
    ../apps/ecat-studio/services/EtherCATReplicationService.h
    ../apps/ecat-studio/services/EtherCATVisualizationService.cpp
    ../apps/ecat-studio/services/EtherCATVisualizationService.h
    ../apps/ecat-studio/services/EtherCATReportingService.cpp
    ../apps/ecat-studio/services/EtherCATReportingService.h
    ../apps/ecat-studio/services/EtherCATDocumentationService.cpp
    ../apps/ecat-studio/services/EtherCATDocumentationService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
)
target_include_directories(ethercat_replication_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
target_link_libraries(ethercat_replication_performance_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
set_target_properties(ethercat_replication_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_replication_performance_test COMMAND ethercat_replication_performance_test)

# ── EtherCATVisualizationService ────────────────────────────────────
add_executable(ethercat_visualization_service_test
    ethercat_visualization_service_test.cpp
    ../apps/ecat-studio/services/EtherCATVisualizationService.cpp
    ../apps/ecat-studio/services/EtherCATVisualizationService.h
    ../apps/ecat-studio/services/EtherCATAnalyticsService.cpp
    ../apps/ecat-studio/services/EtherCATAnalyticsService.h
    ../apps/ecat-studio/services/EtherCATMonitorService.cpp
    ../apps/ecat-studio/services/EtherCATMonitorService.h
    ../apps/ecat-studio/services/NetworkDiagnosticsService.cpp
    ../apps/ecat-studio/services/NetworkDiagnosticsService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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
    ../apps/ecat-studio/services/EtherCATReportingService.cpp
    ../apps/ecat-studio/services/EtherCATReportingService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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
    ../apps/ecat-studio/services/EtherCATDocumentationService.cpp
    ../apps/ecat-studio/services/EtherCATDocumentationService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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

# ── EtherCATVisualizationService Performance ────────────────────────
add_executable(ethercat_visualization_performance_test
    performance/ethercat_visualization_performance_test.cpp
    ../apps/ecat-studio/services/EtherCATVisualizationService.cpp
    ../apps/ecat-studio/services/EtherCATVisualizationService.h
    ../apps/ecat-studio/services/EtherCATAnalyticsService.cpp
    ../apps/ecat-studio/services/EtherCATAnalyticsService.h
    ../apps/ecat-studio/services/EtherCATMonitorService.cpp
    ../apps/ecat-studio/services/EtherCATMonitorService.h
    ../apps/ecat-studio/services/NetworkDiagnosticsService.cpp
    ../apps/ecat-studio/services/NetworkDiagnosticsService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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
    performance/ethercat_reporting_performance_test.cpp
    ../apps/ecat-studio/services/EtherCATReportingService.cpp
    ../apps/ecat-studio/services/EtherCATReportingService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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
    performance/ethercat_documentation_performance_test.cpp
    ../apps/ecat-studio/services/EtherCATDocumentationService.cpp
    ../apps/ecat-studio/services/EtherCATDocumentationService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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
# ── CloudManagerPlugin ───────────────────────────────────────────
add_executable(cloudmanager_plugin_test
    cloudmanager_plugin_test.cpp
    ../apps/ecat-studio/plugins/cloudmanager/CloudManagerPlugin.cpp
    ../apps/ecat-studio/plugins/cloudmanager/CloudManagerPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_include_directories(cloudmanager_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(cloudmanager_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
set_target_properties(cloudmanager_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME cloudmanager_plugin_test COMMAND cloudmanager_plugin_test)
set_tests_properties(cloudmanager_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── EdgeComputingPlugin ──────────────────────────────────────────
add_executable(edgecomputing_plugin_test
    edgecomputing_plugin_test.cpp
    ../apps/ecat-studio/plugins/edgecomputing/EdgeComputingPlugin.cpp
    ../apps/ecat-studio/plugins/edgecomputing/EdgeComputingPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_include_directories(edgecomputing_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(edgecomputing_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
set_target_properties(edgecomputing_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME edgecomputing_plugin_test COMMAND edgecomputing_plugin_test)
set_tests_properties(edgecomputing_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── AIAssistantPlugin ────────────────────────────────────────────
add_executable(aiassistant_plugin_test
    aiassistant_plugin_test.cpp
    ../apps/ecat-studio/plugins/aiassistant/AIAssistantPlugin.cpp
    ../apps/ecat-studio/plugins/aiassistant/AIAssistantPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_include_directories(aiassistant_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(aiassistant_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
set_target_properties(aiassistant_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME aiassistant_plugin_test COMMAND aiassistant_plugin_test)
set_tests_properties(aiassistant_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── EtherCATCloudService Test ─────────────────────────────────
add_executable(ethercat_cloud_service_test
    ethercat_cloud_service_test.cpp
    ../apps/ecat-studio/services/EtherCATCloudService.cpp
    ../apps/ecat-studio/services/EtherCATCloudService.h
)
target_include_directories(ethercat_cloud_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(ethercat_cloud_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_cloud_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_cloud_service_test COMMAND ethercat_cloud_service_test)

# ── EtherCATEdgeService Test ─────────────────────────────────
add_executable(ethercat_edge_service_test
    ethercat_edge_service_test.cpp
    ../apps/ecat-studio/services/EtherCATEdgeService.cpp
    ../apps/ecat-studio/services/EtherCATEdgeService.h
)
target_include_directories(ethercat_edge_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(ethercat_edge_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_edge_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_edge_service_test COMMAND ethercat_edge_service_test)

# ── EtherCATAIService Test ─────────────────────────────────
add_executable(ethercat_ai_service_test
    ethercat_ai_service_test.cpp
    ../apps/ecat-studio/services/EtherCATAIService.cpp
    ../apps/ecat-studio/services/EtherCATAIService.h
)
target_include_directories(ethercat_ai_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(ethercat_ai_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_ai_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_ai_service_test COMMAND ethercat_ai_service_test)

# ── EtherCATCloudService Performance ─────────────────────────────
add_executable(ethercat_cloud_performance_test
    performance/ethercat_cloud_performance_test.cpp
    ../apps/ecat-studio/services/EtherCATCloudService.cpp
    ../apps/ecat-studio/services/EtherCATCloudService.h
)
target_include_directories(ethercat_cloud_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(ethercat_cloud_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_cloud_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_cloud_performance_test COMMAND ethercat_cloud_performance_test)

# ── EtherCATEdgeService Performance ──────────────────────────────
add_executable(ethercat_edge_performance_test
    performance/ethercat_edge_performance_test.cpp
    ../apps/ecat-studio/services/EtherCATEdgeService.cpp
    ../apps/ecat-studio/services/EtherCATEdgeService.h
)
target_include_directories(ethercat_edge_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(ethercat_edge_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_edge_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_edge_performance_test COMMAND ethercat_edge_performance_test)

# ── EtherCATAIService Performance ────────────────────────────────
add_executable(ethercat_ai_performance_test
    performance/ethercat_ai_performance_test.cpp
    ../apps/ecat-studio/services/EtherCATAIService.cpp
    ../apps/ecat-studio/services/EtherCATAIService.h
)
target_include_directories(ethercat_ai_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(ethercat_ai_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_ai_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_ai_performance_test COMMAND ethercat_ai_performance_test)

# ── EtherCATDigitalTwinService Performance ──────────────────────────
add_executable(ethercat_digital_twin_performance_test
    performance/ethercat_digital_twin_performance_test.cpp
    ../apps/ecat-studio/services/EtherCATDigitalTwinService.cpp
    ../apps/ecat-studio/services/EtherCATDigitalTwinService.h
)
target_include_directories(ethercat_digital_twin_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(ethercat_digital_twin_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_digital_twin_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_digital_twin_performance_test COMMAND ethercat_digital_twin_performance_test)

# ── EtherCATBlockchainService Performance ───────────────────────────
add_executable(ethercat_blockchain_performance_test
    performance/ethercat_blockchain_performance_test.cpp
    ../apps/ecat-studio/services/EtherCATBlockchainService.cpp
    ../apps/ecat-studio/services/EtherCATBlockchainService.h
)
target_include_directories(ethercat_blockchain_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(ethercat_blockchain_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_blockchain_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_blockchain_performance_test COMMAND ethercat_blockchain_performance_test)

# ── EtherCATQuantumService Performance ──────────────────────────────
add_executable(ethercat_quantum_performance_test
    performance/ethercat_quantum_performance_test.cpp
    ../apps/ecat-studio/services/EtherCATQuantumService.cpp
    ../apps/ecat-studio/services/EtherCATQuantumService.h
)
target_include_directories(ethercat_quantum_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(ethercat_quantum_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_quantum_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_quantum_performance_test COMMAND ethercat_quantum_performance_test)

# ── EtherCATDigitalTwinService Test ──────────────────────────────────
add_executable(ethercat_digital_twin_service_test
    ethercat_digital_twin_service_test.cpp
    ../apps/ecat-studio/services/EtherCATDigitalTwinService.cpp
    ../apps/ecat-studio/services/EtherCATDigitalTwinService.h
)
target_include_directories(ethercat_digital_twin_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(ethercat_digital_twin_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_digital_twin_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_digital_twin_service_test COMMAND ethercat_digital_twin_service_test)

# ── EtherCATBlockchainService Test ───────────────────────────────────
add_executable(ethercat_blockchain_service_test
    ethercat_blockchain_service_test.cpp
    ../apps/ecat-studio/services/EtherCATBlockchainService.cpp
    ../apps/ecat-studio/services/EtherCATBlockchainService.h
)
target_include_directories(ethercat_blockchain_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(ethercat_blockchain_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_blockchain_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_blockchain_service_test COMMAND ethercat_blockchain_service_test)

# ── EtherCATQuantumService Test ──────────────────────────────────────
add_executable(ethercat_quantum_service_test
    ethercat_quantum_service_test.cpp
    ../apps/ecat-studio/services/EtherCATQuantumService.cpp
    ../apps/ecat-studio/services/EtherCATQuantumService.h
)
target_include_directories(ethercat_quantum_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(ethercat_quantum_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(ethercat_quantum_service_test PROPERTIES AUTOMOC ON)
add_test(NAME ethercat_quantum_service_test COMMAND ethercat_quantum_service_test)

# ── DigitalTwinStudioPlugin (GUI) ────────────────────────────────────
add_executable(digitaltwinstudio_plugin_test
    digitaltwinstudio_plugin_test.cpp
    ../apps/ecat-studio/plugins/digitaltwinstudio/DigitalTwinStudioPlugin.cpp
    ../apps/ecat-studio/plugins/digitaltwinstudio/DigitalTwinStudioPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(digitaltwinstudio_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(digitaltwinstudio_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(digitaltwinstudio_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME digitaltwinstudio_plugin_test COMMAND digitaltwinstudio_plugin_test)
set_tests_properties(digitaltwinstudio_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── BlockchainExplorerPlugin (GUI) ───────────────────────────────────
add_executable(blockchainexplorer_plugin_test
    blockchainexplorer_plugin_test.cpp
    ../apps/ecat-studio/plugins/blockchainexplorer/BlockchainExplorerPlugin.cpp
    ../apps/ecat-studio/plugins/blockchainexplorer/BlockchainExplorerPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(blockchainexplorer_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(blockchainexplorer_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(blockchainexplorer_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME blockchainexplorer_plugin_test COMMAND blockchainexplorer_plugin_test)
set_tests_properties(blockchainexplorer_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── QuantumSecurityPlugin (GUI) ──────────────────────────────────────
add_executable(quantumsecurity_plugin_test
    quantumsecurity_plugin_test.cpp
    ../apps/ecat-studio/plugins/quantumsecurity/QuantumSecurityPlugin.cpp
    ../apps/ecat-studio/plugins/quantumsecurity/QuantumSecurityPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
)
target_link_libraries(quantumsecurity_plugin_test PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
target_include_directories(quantumsecurity_plugin_test PRIVATE ${CMAKE_SOURCE_DIR}/apps/ecat-studio)
set_target_properties(quantumsecurity_plugin_test PROPERTIES AUTOMOC ON)
add_test(NAME quantumsecurity_plugin_test COMMAND quantumsecurity_plugin_test)
set_tests_properties(quantumsecurity_plugin_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
endif()

# ── WorkflowOptimizationService (unit) ──────────────────────────────
add_executable(workflow_optimization_service_test
    workflow_optimization_service_test.cpp
    ../apps/ecat-studio/services/WorkflowOptimizationService.cpp
    ../apps/ecat-studio/services/WorkflowOptimizationService.h
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
    ../apps/ecat-studio/services/ProjectManagementService.cpp
    ../apps/ecat-studio/services/ProjectManagementService.h
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
    ../apps/ecat-studio/services/TaskManagementService.cpp
    ../apps/ecat-studio/services/TaskManagementService.h
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
    ../apps/ecat-studio/services/ResourceManagementService.cpp
    ../apps/ecat-studio/services/ResourceManagementService.h
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
    ../apps/ecat-studio/services/ProjectPlanningService.cpp
    ../apps/ecat-studio/services/ProjectPlanningService.h
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
    ../apps/ecat-studio/services/ProjectTrackingService.cpp
    ../apps/ecat-studio/services/ProjectTrackingService.h
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
    ../apps/ecat-studio/services/ProjectReportingService.cpp
    ../apps/ecat-studio/services/ProjectReportingService.h
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
    ../apps/ecat-studio/services/WorkflowAutomationService.cpp
    ../apps/ecat-studio/services/WorkflowAutomationService.h
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
add_test(NAME workflow_automation_service_test COMMAND workflow_automation_service_test)
set_tests_properties(workflow_automation_service_test PROPERTIES
    TIMEOUT 30
    LABELS "unit;automation"
)

# ── Workflow Scheduling Service ─────────────────────────────────────
add_executable(workflow_scheduling_service_test
    workflow_scheduling_service_test.cpp
    ../apps/ecat-studio/services/WorkflowSchedulingService.cpp
    ../apps/ecat-studio/services/WorkflowSchedulingService.h
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
add_test(NAME workflow_scheduling_service_test COMMAND workflow_scheduling_service_test)
set_tests_properties(workflow_scheduling_service_test PROPERTIES
    TIMEOUT 30
    LABELS "unit;scheduling"
)

# ── Workflow Monitoring Service ─────────────────────────────────────
add_executable(workflow_monitoring_service_test
    workflow_monitoring_service_test.cpp
    ../apps/ecat-studio/services/WorkflowMonitoringService.cpp
    ../apps/ecat-studio/services/WorkflowMonitoringService.h
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
    ../apps/ecat-studio/services/WorkflowAnalyticsService.cpp
    ../apps/ecat-studio/services/WorkflowAnalyticsService.h
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

# ── WorkflowOptimizerPlugin (GUI) ──────────────────────────────────
add_executable(workflowoptimizer_plugin_test
    workflowoptimizer_plugin_test.cpp
    ../apps/ecat-studio/plugins/workflowoptimizer/WorkflowOptimizerPlugin.cpp
    ../apps/ecat-studio/plugins/workflowoptimizer/WorkflowOptimizerPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/WorkflowAnalyticsService.cpp
    ../apps/ecat-studio/services/WorkflowAnalyticsService.h
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
    ../apps/ecat-studio/plugins/workflowdashboard/WorkflowDashboardPlugin.cpp
    ../apps/ecat-studio/plugins/workflowdashboard/WorkflowDashboardPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/WorkflowMonitoringService.cpp
    ../apps/ecat-studio/services/WorkflowMonitoringService.h
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

# ── Workflow Automation Performance ──────────────────────────────────
add_executable(workflow_automation_performance_test
    performance/workflow_automation_performance_test.cpp
    ../apps/ecat-studio/services/WorkflowAutomationService.cpp
    ../apps/ecat-studio/services/WorkflowAutomationService.h
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
    performance/workflow_optimization_performance_test.cpp
    ../apps/ecat-studio/services/WorkflowOptimizationService.cpp
    ../apps/ecat-studio/services/WorkflowOptimizationService.h
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
    performance/workflow_scheduling_performance_test.cpp
    ../apps/ecat-studio/services/WorkflowSchedulingService.cpp
    ../apps/ecat-studio/services/WorkflowSchedulingService.h
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
    performance/workflow_analytics_performance_test.cpp
    ../apps/ecat-studio/services/WorkflowAnalyticsService.cpp
    ../apps/ecat-studio/services/WorkflowAnalyticsService.h
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
    performance/workflow_monitoring_performance_test.cpp
    ../apps/ecat-studio/services/WorkflowMonitoringService.cpp
    ../apps/ecat-studio/services/WorkflowMonitoringService.h
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

# ── Workflow Optimizer Plugin Integration ────────────────────────────
add_executable(workflow_optimizer_plugin_integration_test
    integration/workflow_optimizer_plugin_integration_test.cpp
    ../apps/ecat-studio/plugins/workflowdesigner/WorkflowDesignerPlugin.cpp
    ../apps/ecat-studio/plugins/workflowdesigner/WorkflowDesignerPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/WorkflowOptimizationService.cpp
    ../apps/ecat-studio/services/WorkflowOptimizationService.h
    ../apps/ecat-studio/services/WorkflowSchedulingService.cpp
    ../apps/ecat-studio/services/WorkflowSchedulingService.h
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
    integration/workflow_dashboard_plugin_integration_test.cpp
    ../apps/ecat-studio/plugins/optimizationdashboard/OptimizationDashboardPlugin.cpp
    ../apps/ecat-studio/plugins/optimizationdashboard/OptimizationDashboardPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/WorkflowOptimizationService.cpp
    ../apps/ecat-studio/services/WorkflowOptimizationService.h
    ../apps/ecat-studio/services/WorkflowMonitoringService.cpp
    ../apps/ecat-studio/services/WorkflowMonitoringService.h
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

# ── WorkflowVisualizationService Test ─────────────────────────────
add_executable(workflow_visualization_service_test
    workflow_visualization_service_test.cpp
    ../apps/ecat-studio/services/WorkflowVisualizationService.cpp
    ../apps/ecat-studio/services/WorkflowVisualizationService.h
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
    ../apps/ecat-studio/services/WorkflowReportingService.cpp
    ../apps/ecat-studio/services/WorkflowReportingService.h
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
    ../apps/ecat-studio/services/WorkflowIntegrationService.cpp
    ../apps/ecat-studio/services/WorkflowIntegrationService.h
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
    ../apps/ecat-studio/services/WorkflowSecurityService.cpp
    ../apps/ecat-studio/services/WorkflowSecurityService.h
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
    ../apps/ecat-studio/services/WorkflowComplianceService.cpp
    ../apps/ecat-studio/services/WorkflowComplianceService.h
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
    ../apps/ecat-studio/services/WorkflowCertificationService.cpp
    ../apps/ecat-studio/services/WorkflowCertificationService.h
)
target_include_directories(workflow_certification_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_certification_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_certification_service_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_certification_service_test COMMAND workflow_certification_service_test)
set_tests_properties(workflow_certification_service_test PROPERTIES
    TIMEOUT 30
    LABELS "unit;certification"
)

# ── WorkflowSecurityManagerService ───────────────────────────────────
add_executable(workflow_security_manager_service_test
    workflow_security_manager_service_test.cpp
    ../apps/ecat-studio/services/WorkflowSecurityManagerService.cpp
    ../apps/ecat-studio/services/WorkflowSecurityManagerService.h
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
    ../apps/ecat-studio/services/WorkflowComplianceManagerService.cpp
    ../apps/ecat-studio/services/WorkflowComplianceManagerService.h
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
    ../apps/ecat-studio/services/WorkflowCertificationManagerService.cpp
    ../apps/ecat-studio/services/WorkflowCertificationManagerService.h
)
target_include_directories(workflow_certification_manager_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_certification_manager_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_certification_manager_service_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_certification_manager_service_test COMMAND workflow_certification_manager_service_test)
set_tests_properties(workflow_certification_manager_service_test PROPERTIES
    TIMEOUT 30
    LABELS "unit;certification"
)

# ── Workflow Deployment Service ─────────────────────────────────────
add_executable(workflow_deployment_service_test
    workflow_deployment_service_test.cpp
    ../apps/ecat-studio/services/WorkflowDeploymentService.cpp
    ../apps/ecat-studio/services/WorkflowDeploymentService.h
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
    ../apps/ecat-studio/services/WorkflowUpdateService.cpp
    ../apps/ecat-studio/services/WorkflowUpdateService.h
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
    ../apps/ecat-studio/services/WorkflowMaintenanceService.cpp
    ../apps/ecat-studio/services/WorkflowMaintenanceService.h
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
add_test(NAME workflow_maintenance_service_test COMMAND workflow_maintenance_service_test)
set_tests_properties(workflow_maintenance_service_test PROPERTIES
    TIMEOUT 30
    LABELS "unit;maintenance"
)

# ── Workflow Deployment Performance ─────────────────────────────────
add_executable(workflow_deployment_performance_test
    performance/workflow_deployment_performance_test.cpp
    ../apps/ecat-studio/services/WorkflowDeploymentService.cpp
    ../apps/ecat-studio/services/WorkflowDeploymentService.h
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
    performance/workflow_update_performance_test.cpp
    ../apps/ecat-studio/services/WorkflowUpdateService.cpp
    ../apps/ecat-studio/services/WorkflowUpdateService.h
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
    performance/workflow_maintenance_performance_test.cpp
    ../apps/ecat-studio/services/WorkflowMaintenanceService.cpp
    ../apps/ecat-studio/services/WorkflowMaintenanceService.h
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

# ── WorkflowIntegrationHubService Test ─────────────────────────────
add_executable(workflow_integration_hub_service_test
    workflow_integration_hub_service_test.cpp
    ../apps/ecat-studio/services/WorkflowIntegrationHubService.cpp
    ../apps/ecat-studio/services/WorkflowIntegrationHubService.h
)
target_include_directories(workflow_integration_hub_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_integration_hub_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_integration_hub_service_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_integration_hub_service_test COMMAND workflow_integration_hub_service_test)

# ── WorkflowSyncService Test ────────────────────────────────────────
add_executable(workflow_sync_service_test
    workflow_sync_service_test.cpp
    ../apps/ecat-studio/services/WorkflowSyncService.cpp
    ../apps/ecat-studio/services/WorkflowSyncService.h
)
target_include_directories(workflow_sync_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_sync_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_sync_service_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_sync_service_test COMMAND workflow_sync_service_test)

# ── WorkflowReplicationService Test ─────────────────────────────────
add_executable(workflow_replication_service_test
    workflow_replication_service_test.cpp
    ../apps/ecat-studio/services/WorkflowReplicationService.cpp
    ../apps/ecat-studio/services/WorkflowReplicationService.h
)
target_include_directories(workflow_replication_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_replication_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_replication_service_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_replication_service_test COMMAND workflow_replication_service_test)

# ── WorkflowCloudService Test ─────────────────────────────────
add_executable(workflow_cloud_service_test
    workflow_cloud_service_test.cpp
    ../apps/ecat-studio/services/WorkflowCloudService.cpp
    ../apps/ecat-studio/services/WorkflowCloudService.h
)
target_include_directories(workflow_cloud_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_cloud_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_cloud_service_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_cloud_service_test COMMAND workflow_cloud_service_test)

# ── WorkflowEdgeService Test ─────────────────────────────────
add_executable(workflow_edge_service_test
    workflow_edge_service_test.cpp
    ../apps/ecat-studio/services/WorkflowEdgeService.cpp
    ../apps/ecat-studio/services/WorkflowEdgeService.h
)
target_include_directories(workflow_edge_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_edge_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_edge_service_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_edge_service_test COMMAND workflow_edge_service_test)

# ── WorkflowAIService Test ─────────────────────────────────
add_executable(workflow_ai_service_test
    workflow_ai_service_test.cpp
    ../apps/ecat-studio/services/WorkflowAIService.cpp
    ../apps/ecat-studio/services/WorkflowAIService.h
)
target_include_directories(workflow_ai_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_ai_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_ai_service_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_ai_service_test COMMAND workflow_ai_service_test)

# ── WorkflowCloudService Performance Test ─────────────────────────
add_executable(workflow_cloud_service_performance_test
    performance/workflow_cloud_service_performance_test.cpp
    ../apps/ecat-studio/services/WorkflowCloudService.cpp
    ../apps/ecat-studio/services/WorkflowCloudService.h
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
    performance/workflow_edge_service_performance_test.cpp
    ../apps/ecat-studio/services/WorkflowEdgeService.cpp
    ../apps/ecat-studio/services/WorkflowEdgeService.h
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
    performance/workflow_ai_service_performance_test.cpp
    ../apps/ecat-studio/services/WorkflowAIService.cpp
    ../apps/ecat-studio/services/WorkflowAIService.h
)
target_include_directories(workflow_ai_service_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_ai_service_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_ai_service_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_ai_service_performance_test COMMAND workflow_ai_service_performance_test)

# ── WorkflowDigitalTwinService Test ──────────────────────────────
add_executable(workflow_digital_twin_service_test
    workflow_digital_twin_service_test.cpp
    ../apps/ecat-studio/services/WorkflowDigitalTwinService.cpp
    ../apps/ecat-studio/services/WorkflowDigitalTwinService.h
)
target_include_directories(workflow_digital_twin_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_digital_twin_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_digital_twin_service_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_digital_twin_service_test COMMAND workflow_digital_twin_service_test)
set_tests_properties(workflow_digital_twin_service_test PROPERTIES
    TIMEOUT 30
    LABELS "unit;digitaltwin"
)

# ── WorkflowBlockchainService Test ───────────────────────────────
add_executable(workflow_blockchain_service_test
    workflow_blockchain_service_test.cpp
    ../apps/ecat-studio/services/WorkflowBlockchainService.cpp
    ../apps/ecat-studio/services/WorkflowBlockchainService.h
)
target_include_directories(workflow_blockchain_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_blockchain_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_blockchain_service_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_blockchain_service_test COMMAND workflow_blockchain_service_test)
set_tests_properties(workflow_blockchain_service_test PROPERTIES
    TIMEOUT 30
    LABELS "unit;blockchain"
)

# ── WorkflowQuantumService Test ──────────────────────────────────
add_executable(workflow_quantum_service_test
    workflow_quantum_service_test.cpp
    ../apps/ecat-studio/services/WorkflowQuantumService.cpp
    ../apps/ecat-studio/services/WorkflowQuantumService.h
)
target_include_directories(workflow_quantum_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_quantum_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_quantum_service_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_quantum_service_test COMMAND workflow_quantum_service_test)
set_tests_properties(workflow_quantum_service_test PROPERTIES
    TIMEOUT 30
    LABELS "unit;quantum"
)
endif()

# ── Workflow Optimization Services Tests ───────────────────────────────
add_executable(workflow_visualization_studio_service_test
    workflow_visualization_studio_service_test.cpp
    ../apps/ecat-studio/services/WorkflowVisualizationStudioService.cpp
    ../apps/ecat-studio/services/WorkflowVisualizationStudioService.h
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
    ../apps/ecat-studio/services/WorkflowReportDesignerService.cpp
    ../apps/ecat-studio/services/WorkflowReportDesignerService.h
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
set_target_properties(workflow_report_designer_service_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_report_designer_service_test COMMAND workflow_report_designer_service_test)
set_tests_properties(workflow_report_designer_service_test PROPERTIES
    TIMEOUT 30
    LABELS "unit;workflow"
)

add_executable(workflow_documentation_browser_service_test
    workflow_documentation_browser_service_test.cpp
    ../apps/ecat-studio/services/WorkflowDocumentationBrowserService.cpp
    ../apps/ecat-studio/services/WorkflowDocumentationBrowserService.h
    ../apps/ecat-studio/services/WorkflowSecurityManagerService.cpp
    ../apps/ecat-studio/services/WorkflowSecurityManagerService.h
    ../apps/ecat-studio/services/WorkflowComplianceManagerService.cpp
    ../apps/ecat-studio/services/WorkflowComplianceManagerService.h
    ../apps/ecat-studio/services/WorkflowCertificationManagerService.cpp
    ../apps/ecat-studio/services/WorkflowCertificationManagerService.h
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

# ── Workflow Visualization Studio Performance Test ───────────────────
add_executable(workflow_visualization_studio_performance_test
    performance/workflow_visualization_studio_performance_test.cpp
    ../apps/ecat-studio/services/WorkflowVisualizationStudioService.cpp
    ../apps/ecat-studio/services/WorkflowVisualizationStudioService.h
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
    performance/workflow_report_designer_performance_test.cpp
    ../apps/ecat-studio/services/WorkflowReportDesignerService.cpp
    ../apps/ecat-studio/services/WorkflowReportDesignerService.h
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
    performance/workflow_documentation_browser_performance_test.cpp
    ../apps/ecat-studio/services/WorkflowDocumentationBrowserService.cpp
    ../apps/ecat-studio/services/WorkflowDocumentationBrowserService.h
    ../apps/ecat-studio/services/WorkflowSecurityManagerService.cpp
    ../apps/ecat-studio/services/WorkflowSecurityManagerService.h
    ../apps/ecat-studio/services/WorkflowComplianceManagerService.cpp
    ../apps/ecat-studio/services/WorkflowComplianceManagerService.h
    ../apps/ecat-studio/services/WorkflowCertificationManagerService.cpp
    ../apps/ecat-studio/services/WorkflowCertificationManagerService.h
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
    performance/workflow_security_manager_performance_test.cpp
    ../apps/ecat-studio/services/WorkflowSecurityManagerService.cpp
    ../apps/ecat-studio/services/WorkflowSecurityManagerService.h
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
    performance/workflow_compliance_manager_performance_test.cpp
    ../apps/ecat-studio/services/WorkflowComplianceManagerService.cpp
    ../apps/ecat-studio/services/WorkflowComplianceManagerService.h
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
    performance/workflow_certification_manager_performance_test.cpp
    ../apps/ecat-studio/services/WorkflowCertificationManagerService.cpp
    ../apps/ecat-studio/services/WorkflowCertificationManagerService.h
)
target_include_directories(workflow_certification_manager_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(workflow_certification_manager_performance_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(workflow_certification_manager_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME workflow_certification_manager_performance_test COMMAND workflow_certification_manager_performance_test)

# ── ESI Browser Plugin Test ──────────────────────────────────────────
add_executable(esi_browser_plugin_test
    esi_browser_plugin_test.cpp
    ../apps/ecat-studio/plugins/esi/EsiBrowserPlugin.cpp
    ../apps/ecat-studio/plugins/esi/EsiBrowserPlugin.h
    ../apps/ecat-studio/plugins/esi/EsiParser.cpp
    ../apps/ecat-studio/plugins/esi/EsiParser.h
    ../apps/ecat-studio/plugins/esi/EsiDeviceMatcher.cpp
    ../apps/ecat-studio/plugins/esi/EsiDeviceMatcher.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/EsiService.cpp
    ../apps/ecat-studio/services/EsiService.h
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

# ── Project Persistence Test ─────────────────────────────────────────
add_executable(project_persistence_test
    project_persistence_test.cpp
    ../apps/ecat-studio/services/ProjectManagerService.cpp
    ../apps/ecat-studio/services/ProjectManagerService.h
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

# ── DcSyncPrecisionPlugin + DriftMonitorWidget + JitterAnalysisWidget ──
add_executable(dcsyncprecision_plugin_test
    dcsyncprecision_plugin_test.cpp
    ../apps/ecat-studio/plugins/dcsyncprecision/DcSyncPrecisionPlugin.cpp
    ../apps/ecat-studio/plugins/dcsyncprecision/DcSyncPrecisionPlugin.h
    ../apps/ecat-studio/plugins/dcsyncprecision/DriftMonitorWidget.cpp
    ../apps/ecat-studio/plugins/dcsyncprecision/DriftMonitorWidget.h
    ../apps/ecat-studio/plugins/dcsyncprecision/JitterAnalysisWidget.cpp
    ../apps/ecat-studio/plugins/dcsyncprecision/JitterAnalysisWidget.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/DcSyncPrecisionService.cpp
    ../apps/ecat-studio/services/DcSyncPrecisionService.h
    ../apps/ecat-studio/services/DcSyncOptimizerService.cpp
    ../apps/ecat-studio/services/DcSyncOptimizerService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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
add_executable(freerunoptimization_plugin_test
    freerunoptimization_plugin_test.cpp
    ../apps/ecat-studio/plugins/freerunoptimization/FreeRunOptimizationPlugin.cpp
    ../apps/ecat-studio/plugins/freerunoptimization/FreeRunOptimizationPlugin.h
    ../apps/ecat-studio/plugins/freerunoptimization/CycleTimeOptimizerWidget.cpp
    ../apps/ecat-studio/plugins/freerunoptimization/CycleTimeOptimizerWidget.h
    ../apps/ecat-studio/plugins/freerunoptimization/DataMappingOptimizerWidget.cpp
    ../apps/ecat-studio/plugins/freerunoptimization/DataMappingOptimizerWidget.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/FreeRunOptimizationService.cpp
    ../apps/ecat-studio/services/FreeRunOptimizationService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
)

target_include_directories(freerunoptimization_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins/freerunoptimization
    ${CMAKE_SOURCE_DIR}/src/core
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
    ../apps/ecat-studio/plugins/sdooptimization/SdoOptimizationPlugin.cpp
    ../apps/ecat-studio/plugins/sdooptimization/SdoOptimizationPlugin.h
    ../apps/ecat-studio/plugins/sdooptimization/CacheOptimizerWidget.cpp
    ../apps/ecat-studio/plugins/sdooptimization/CacheOptimizerWidget.h
    ../apps/ecat-studio/plugins/sdooptimization/BatchOptimizerWidget.cpp
    ../apps/ecat-studio/plugins/sdooptimization/BatchOptimizerWidget.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/SdoOptimizationService.cpp
    ../apps/ecat-studio/services/SdoOptimizationService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
)

target_include_directories(sdooptimization_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins/sdooptimization
    ${CMAKE_SOURCE_DIR}/src/core
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
    ../apps/ecat-studio/plugins/pdomapping/PdoMappingEditorPlugin.cpp
    ../apps/ecat-studio/plugins/pdomapping/PdoMappingEditorPlugin.h
    ../apps/ecat-studio/plugins/pdomapping/PdoMappingCanvas.cpp
    ../apps/ecat-studio/plugins/pdomapping/PdoMappingCanvas.h
    ../apps/ecat-studio/plugins/pdomapping/PdoMappingValidator.cpp
    ../apps/ecat-studio/plugins/pdomapping/PdoMappingValidator.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/PdoMappingService.cpp
    ../apps/ecat-studio/services/PdoMappingService.h
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
    ../apps/ecat-studio/plugins/realtimeperf/RealtimePerformancePlugin.cpp
    ../apps/ecat-studio/plugins/realtimeperf/RealtimePerformancePlugin.h
    ../apps/ecat-studio/plugins/realtimeperf/LatencyMonitorWidget.cpp
    ../apps/ecat-studio/plugins/realtimeperf/LatencyMonitorWidget.h
    ../apps/ecat-studio/plugins/realtimeperf/ThroughputMonitorWidget.cpp
    ../apps/ecat-studio/plugins/realtimeperf/ThroughputMonitorWidget.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/RealtimePerformanceService.cpp
    ../apps/ecat-studio/services/RealtimePerformanceService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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
add_executable(realtimeoptimizer_plugin_test
    realtimeoptimizer_plugin_test.cpp
    ../apps/ecat-studio/plugins/realtimeoptimizer/RealtimeOptimizerPlugin.cpp
    ../apps/ecat-studio/plugins/realtimeoptimizer/RealtimeOptimizerPlugin.h
    ../apps/ecat-studio/plugins/realtimeoptimizer/LatencyOptimizerWidget.cpp
    ../apps/ecat-studio/plugins/realtimeoptimizer/LatencyOptimizerWidget.h
    ../apps/ecat-studio/plugins/realtimeoptimizer/ThroughputOptimizerWidget.cpp
    ../apps/ecat-studio/plugins/realtimeoptimizer/ThroughputOptimizerWidget.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/RealtimeOptimizerService.cpp
    ../apps/ecat-studio/services/RealtimeOptimizerService.h
)

target_include_directories(realtimeoptimizer_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
    ${CMAKE_SOURCE_DIR}/src/core
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
    ../apps/ecat-studio/plugins/multimaster/MultiMasterPlugin.cpp
    ../apps/ecat-studio/plugins/multimaster/MultiMasterPlugin.h
    ../apps/ecat-studio/plugins/multimaster/MasterComparisonWidget.cpp
    ../apps/ecat-studio/plugins/multimaster/MasterComparisonWidget.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/MultiMasterService.cpp
    ../apps/ecat-studio/services/MultiMasterService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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
    ../apps/ecat-studio/plugins/erroranalysis/AdvancedErrorAnalysisPlugin.cpp
    ../apps/ecat-studio/plugins/erroranalysis/AdvancedErrorAnalysisPlugin.h
    ../apps/ecat-studio/plugins/erroranalysis/ErrorTimelineWidget.cpp
    ../apps/ecat-studio/plugins/erroranalysis/ErrorTimelineWidget.h
    ../apps/ecat-studio/plugins/erroranalysis/ErrorCorrelationWidget.cpp
    ../apps/ecat-studio/plugins/erroranalysis/ErrorCorrelationWidget.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/AdvancedErrorAnalysisService.cpp
    ../apps/ecat-studio/services/AdvancedErrorAnalysisService.h
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
    ../apps/ecat-studio/plugins/hardwareverification/HardwareVerificationPlugin.cpp
    ../apps/ecat-studio/plugins/hardwareverification/HardwareVerificationPlugin.h
    ../apps/ecat-studio/plugins/hardwareverification/DeviceVerificationWidget.cpp
    ../apps/ecat-studio/plugins/hardwareverification/DeviceVerificationWidget.h
    ../apps/ecat-studio/plugins/hardwareverification/NetworkVerificationWidget.cpp
    ../apps/ecat-studio/plugins/hardwareverification/NetworkVerificationWidget.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/HardwareVerificationService.cpp
    ../apps/ecat-studio/services/HardwareVerificationService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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

# ── FreeRunConfigurationService Test ─────────────────────────────────
add_executable(free_run_configuration_service_test
    free_run_configuration_service_test.cpp
    ../apps/ecat-studio/services/FreeRunConfigurationService.cpp
    ../apps/ecat-studio/services/FreeRunConfigurationService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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
add_test(NAME free_run_configuration_service_test COMMAND free_run_configuration_service_test)

# ── FreeRunMonitoringService Test ─────────────────────────────────────
add_executable(free_run_monitoring_service_test
    free_run_monitoring_service_test.cpp
    ../apps/ecat-studio/services/FreeRunMonitoringService.cpp
    ../apps/ecat-studio/services/FreeRunMonitoringService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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
    ../apps/ecat-studio/services/PdoConfigurationService.cpp
    ../apps/ecat-studio/services/PdoConfigurationService.h
)
target_include_directories(pdo_configuration_service_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
)
target_link_libraries(pdo_configuration_service_test PRIVATE Qt6::Core Qt6::Test)
set_target_properties(pdo_configuration_service_test PROPERTIES AUTOMOC ON)
add_test(NAME pdo_configuration_service_test COMMAND pdo_configuration_service_test)

# ── OpStateService Test ─────────────────────────────────────────────
add_executable(op_state_service_test
    op_state_service_test.cpp
    ../apps/ecat-studio/services/OpStateService.cpp
    ../apps/ecat-studio/services/OpStateService.h
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
    ../apps/ecat-studio/services/PerformanceMonitorService.cpp
    ../apps/ecat-studio/services/PerformanceMonitorService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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

# ── PerformanceDashboardPlugin Test ─────────────────────────────────
add_executable(performance_dashboard_plugin_test
    performance_dashboard_plugin_test.cpp
    ../apps/ecat-studio/plugins/performancedashboard/PerformanceDashboardPlugin.cpp
    ../apps/ecat-studio/plugins/performancedashboard/PerformanceDashboardPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/PerformanceMonitorService.cpp
    ../apps/ecat-studio/services/PerformanceMonitorService.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
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

# ── Daemon Handler Test ──────────────────────────────────────────────
add_executable(daemon_handler_test
    daemon_handler_test.cpp
    ../apps/ecatd/EcatDaemon.cpp
    ../apps/ecatd/EcatDaemon.h
    ../apps/ecatd/CommandDispatcher.cpp
    ../apps/ecatd/CommandDispatcher.h
    ../apps/ecatd/FreeRunController.cpp
    ../apps/ecatd/FreeRunController.h
    ../apps/ecatd/RtTestController.cpp
    ../apps/ecatd/RtTestController.h
    ../apps/ecatd/handlers/AlEventHandler.cpp
    ../apps/ecatd/handlers/AlEventHandler.h
    ../apps/ecatd/handlers/DcSyncHandler.cpp
    ../apps/ecatd/handlers/DcSyncHandler.h
    ../apps/ecatd/handlers/AdapterHandler.cpp
    ../apps/ecatd/handlers/AdapterHandler.h
    ../apps/ecatd/handlers/SignalHandler.cpp
    ../apps/ecatd/handlers/SignalHandler.h
    ../apps/ecatd/handlers/FoEHandler.cpp
    ../apps/ecatd/handlers/FoEHandler.h
    ../src/core/JsonProtocol.cpp
    ../src/core/JsonProtocol.h
    ../src/core/EthercatTypes.cpp
    ../src/core/EthercatTypes.h
)
target_include_directories(daemon_handler_test PRIVATE
    ../apps/ecatd
    ../src/core
)
target_link_libraries(daemon_handler_test PRIVATE Qt6::Core Qt6::Network Qt6::Test ecat_igh ethercat)
set_target_properties(daemon_handler_test PROPERTIES AUTOMOC ON)
add_test(NAME daemon_handler_test COMMAND daemon_handler_test)

# ── GUI-Daemon Integration Test ──────────────────────────────────────
add_executable(gui_daemon_integration_test
    integration/gui_daemon_integration_test.cpp
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
    ../apps/ecatd/CommandDispatcher.cpp
    ../apps/ecatd/CommandDispatcher.h
    ../src/core/JsonProtocol.cpp
    ../src/core/JsonProtocol.h
    ../src/core/EthercatTypes.cpp
    ../src/core/EthercatTypes.h
)

target_include_directories(gui_daemon_integration_test PRIVATE
    ../apps/ecatd
    ../apps/ecat-studio/infra
    ../src/core
)
target_link_libraries(gui_daemon_integration_test PRIVATE Qt6::Core Qt6::Network)
set_target_properties(gui_daemon_integration_test PROPERTIES AUTOMOC ON)
add_test(NAME gui_daemon_integration_test COMMAND gui_daemon_integration_test)

# ── Core Performance Benchmarks ──────────────────────────────────────
add_executable(core_performance_test
    core_performance_test.cpp
    ../src/core/JsonProtocol.cpp
    ../src/core/JsonProtocol.h
    ../src/core/EthercatTypes.cpp
    ../src/core/EthercatTypes.h
    ../apps/ecat-studio/services/EventBus.cpp
    ../apps/ecat-studio/services/EventBus.h
)
target_include_directories(core_performance_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
)
target_link_libraries(core_performance_test PRIVATE Qt6::Core Qt6::Test ecat_core)
set_target_properties(core_performance_test PROPERTIES AUTOMOC ON)
add_test(NAME core_performance_test COMMAND core_performance_test)
