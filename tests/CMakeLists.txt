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

add_executable(io_variable_detail_ui_state_test
    io_variable_detail_ui_state_test.cpp
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

target_include_directories(io_variable_detail_ui_state_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(io_variable_detail_ui_state_test PRIVATE Qt6::Core)

add_test(NAME io_variable_detail_ui_state_test
    COMMAND io_variable_detail_ui_state_test
)

add_executable(free_run_entry_detail_ui_state_test
    free_run_entry_detail_ui_state_test.cpp
    ../apps/ecat-studio/detail/FreeRunEntryDetail.cpp
    ../apps/ecat-studio/detail/FreeRunEntryDetail.h
    ../apps/ecat-studio/infra/ProcessDataTypes.h
)

target_include_directories(free_run_entry_detail_ui_state_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(free_run_entry_detail_ui_state_test PRIVATE Qt6::Core)

add_test(NAME free_run_entry_detail_ui_state_test
    COMMAND free_run_entry_detail_ui_state_test
)

add_executable(pdo_map_detail_ui_state_test
    pdo_map_detail_ui_state_test.cpp
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

target_include_directories(pdo_map_detail_ui_state_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(pdo_map_detail_ui_state_test PRIVATE Qt6::Core)

add_test(NAME pdo_map_detail_ui_state_test
    COMMAND pdo_map_detail_ui_state_test
)

add_executable(io_variable_filter_model_test
    io_variable_filter_model_test.cpp
    ../apps/ecat-studio/models/IoVariableFilterModel.cpp
    ../apps/ecat-studio/models/IoVariableFilterModel.h
    ../apps/ecat-studio/models/ProcessDataRowModel.cpp
    ../apps/ecat-studio/models/ProcessDataRowModel.h
    ../apps/ecat-studio/infra/ProcessDataTypes.h
    ../apps/ecat-studio/models/EvidenceModel.cpp
    ../apps/ecat-studio/models/EvidenceModel.h
    ../apps/ecat-studio/utils/TextHelpers.cpp
    ../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(io_variable_filter_model_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(io_variable_filter_model_test PRIVATE Qt6::Core)

add_test(NAME io_variable_filter_model_test
    COMMAND io_variable_filter_model_test
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

add_executable(io_variable_handoff_model_test
    io_variable_handoff_model_test.cpp
    ../apps/ecat-studio/models/IoVariableHandoffModel.cpp
    ../apps/ecat-studio/models/IoVariableHandoffModel.h
    ../apps/ecat-studio/models/ProcessDataRowModel.cpp
    ../apps/ecat-studio/models/ProcessDataRowModel.h
    ../apps/ecat-studio/infra/ProcessDataTypes.h
    ../apps/ecat-studio/models/EvidenceModel.cpp
    ../apps/ecat-studio/models/EvidenceModel.h
    ../apps/ecat-studio/utils/TextHelpers.cpp
    ../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(io_variable_handoff_model_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(io_variable_handoff_model_test PRIVATE Qt6::Core)

add_test(NAME io_variable_handoff_model_test
    COMMAND io_variable_handoff_model_test
)

add_executable(io_variable_bulk_naming_model_test
    io_variable_bulk_naming_model_test.cpp
    ../apps/ecat-studio/models/IoVariableBulkNamingModel.cpp
    ../apps/ecat-studio/models/IoVariableBulkNamingModel.h
    ../apps/ecat-studio/models/IoVariableHandoffModel.cpp
    ../apps/ecat-studio/models/IoVariableHandoffModel.h
    ../apps/ecat-studio/models/ProcessDataRowModel.cpp
    ../apps/ecat-studio/models/ProcessDataRowModel.h
    ../apps/ecat-studio/infra/ProcessDataTypes.h
    ../apps/ecat-studio/models/EvidenceModel.cpp
    ../apps/ecat-studio/models/EvidenceModel.h
    ../apps/ecat-studio/utils/TextHelpers.cpp
    ../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(io_variable_bulk_naming_model_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(io_variable_bulk_naming_model_test PRIVATE Qt6::Core)

add_test(NAME io_variable_bulk_naming_model_test
    COMMAND io_variable_bulk_naming_model_test
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

add_executable(sdo_history_row_detail_ui_state_test
    sdo_history_row_detail_ui_state_test.cpp
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

target_include_directories(sdo_history_row_detail_ui_state_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(sdo_history_row_detail_ui_state_test PRIVATE
    Qt6::Core
    Qt6::Widgets
)

add_test(NAME sdo_history_row_detail_ui_state_test
    COMMAND sdo_history_row_detail_ui_state_test
)

add_executable(object_bookmark_detail_ui_state_test
    object_bookmark_detail_ui_state_test.cpp
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

target_include_directories(object_bookmark_detail_ui_state_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(object_bookmark_detail_ui_state_test PRIVATE
    Qt6::Core
    Qt6::Widgets
)

add_test(NAME object_bookmark_detail_ui_state_test
    COMMAND object_bookmark_detail_ui_state_test
)

add_executable(sdo_target_trail_detail_ui_state_test
    sdo_target_trail_detail_ui_state_test.cpp
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

target_include_directories(sdo_target_trail_detail_ui_state_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(sdo_target_trail_detail_ui_state_test PRIVATE
    Qt6::Core
    Qt6::Widgets
)

add_test(NAME sdo_target_trail_detail_ui_state_test
    COMMAND sdo_target_trail_detail_ui_state_test
)

add_executable(sdo_target_panel_route_model_test
    sdo_target_panel_route_model_test.cpp
    ../apps/ecat-studio/models/SdoTargetPanelRouteModel.cpp
    ../apps/ecat-studio/models/SdoTargetPanelRouteModel.h
)

target_include_directories(sdo_target_panel_route_model_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(sdo_target_panel_route_model_test PRIVATE Qt6::Core)

add_test(NAME sdo_target_panel_route_model_test
    COMMAND sdo_target_panel_route_model_test
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

add_executable(watch_row_detail_ui_state_test
    watch_row_detail_ui_state_test.cpp
    ../apps/ecat-studio/detail/WatchRowDetail.cpp
    ../apps/ecat-studio/detail/WatchRowDetail.h
    ../apps/ecat-studio/models/WatchStartupModel.h
    ../apps/ecat-studio/utils/TextHelpers.cpp
    ../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(watch_row_detail_ui_state_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(watch_row_detail_ui_state_test PRIVATE Qt6::Core)

add_test(NAME watch_row_detail_ui_state_test
    COMMAND watch_row_detail_ui_state_test
)

add_executable(startup_sdo_row_detail_ui_state_test
    startup_sdo_row_detail_ui_state_test.cpp
    ../apps/ecat-studio/detail/StartupSdoRowDetail.cpp
    ../apps/ecat-studio/detail/StartupSdoRowDetail.h
    ../apps/ecat-studio/models/WatchStartupModel.h
)

target_include_directories(startup_sdo_row_detail_ui_state_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(startup_sdo_row_detail_ui_state_test PRIVATE Qt6::Core)

add_test(NAME startup_sdo_row_detail_ui_state_test
    COMMAND startup_sdo_row_detail_ui_state_test
)

add_executable(topology_baseline_model_test
    topology_baseline_model_test.cpp
    ../apps/ecat-studio/models/TopologyModel.cpp
    ../apps/ecat-studio/models/TopologyModel.h
)

target_include_directories(topology_baseline_model_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(topology_baseline_model_test PRIVATE ecat_core Qt6::Core)

add_test(NAME topology_baseline_model_test COMMAND topology_baseline_model_test)

add_executable(topology_change_model_test
    topology_change_model_test.cpp
    ../apps/ecat-studio/models/TopologyModel.cpp
    ../apps/ecat-studio/models/TopologyModel.h
)

target_include_directories(topology_change_model_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(topology_change_model_test PRIVATE ecat_core Qt6::Core)

add_test(NAME topology_change_model_test COMMAND topology_change_model_test)

add_executable(state_recommendation_model_test
    state_recommendation_model_test.cpp
    ../apps/ecat-studio/models/EvidenceModel.cpp
    ../apps/ecat-studio/models/EvidenceModel.h
)

target_include_directories(state_recommendation_model_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(state_recommendation_model_test PRIVATE Qt6::Core)

add_test(NAME state_recommendation_model_test COMMAND state_recommendation_model_test)

add_executable(state_machine_row_detail_ui_state_test
    state_machine_row_detail_ui_state_test.cpp
    ../apps/ecat-studio/detail/StateMachineRowDetail.cpp
    ../apps/ecat-studio/detail/StateMachineRowDetail.h
    ../apps/ecat-studio/adapters/StateMachineTableAdapter.h
)

target_include_directories(state_machine_row_detail_ui_state_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(state_machine_row_detail_ui_state_test PRIVATE Qt6::Core)

add_test(NAME state_machine_row_detail_ui_state_test
    COMMAND state_machine_row_detail_ui_state_test
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

add_executable(consistency_detail_ui_state_test
    consistency_detail_ui_state_test.cpp
    ../apps/ecat-studio/detail/ConsistencyDetail.cpp
    ../apps/ecat-studio/detail/ConsistencyDetail.h
)

target_include_directories(consistency_detail_ui_state_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(consistency_detail_ui_state_test PRIVATE Qt6::Core)

add_test(NAME consistency_detail_ui_state_test
    COMMAND consistency_detail_ui_state_test
)

add_executable(consistency_evidence_route_model_test
    consistency_evidence_route_model_test.cpp
    ../apps/ecat-studio/detail/ConsistencyDetail.h
    ../apps/ecat-studio/models/ConsistencyModel.cpp
    ../apps/ecat-studio/models/ConsistencyModel.h
    ../apps/ecat-studio/utils/TextHelpers.cpp
    ../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(consistency_evidence_route_model_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(consistency_evidence_route_model_test PRIVATE Qt6::Core)

add_test(NAME consistency_evidence_route_model_test
    COMMAND consistency_evidence_route_model_test
)

add_executable(consistency_gate_model_test
    consistency_gate_model_test.cpp
    ../apps/ecat-studio/models/ConsistencyModel.cpp
    ../apps/ecat-studio/models/ConsistencyModel.h
)

target_include_directories(consistency_gate_model_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(consistency_gate_model_test PRIVATE Qt6::Core)

add_test(NAME consistency_gate_model_test COMMAND consistency_gate_model_test)

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

add_executable(evidence_status_model_test
    evidence_status_model_test.cpp
    ../apps/ecat-studio/models/EvidenceModel.cpp
    ../apps/ecat-studio/models/EvidenceModel.h
)

target_include_directories(evidence_status_model_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(evidence_status_model_test PRIVATE Qt6::Core)

add_test(NAME evidence_status_model_test COMMAND evidence_status_model_test)

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

add_executable(studio_table_helpers_test
    studio_table_helpers_test.cpp
    ../apps/ecat-studio/utils/TableHelpers.cpp
    ../apps/ecat-studio/utils/TableHelpers.h
    ../apps/ecat-studio/utils/TextHelpers.cpp
    ../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(studio_table_helpers_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(studio_table_helpers_test PRIVATE
    Qt6::Core
    Qt6::Widgets
)

add_test(NAME studio_table_helpers_test COMMAND studio_table_helpers_test)
set_tests_properties(studio_table_helpers_test PROPERTIES
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

add_executable(commissioning_workflow_ui_state_test
    commissioning_workflow_ui_state_test.cpp
    ../apps/ecat-studio/models/CommissioningWorkflowModel.cpp
    ../apps/ecat-studio/models/CommissioningWorkflowModel.h
    ../apps/ecat-studio/detail/CommissioningWorkflowDetail.cpp
    ../apps/ecat-studio/detail/CommissioningWorkflowDetail.h
)

target_include_directories(commissioning_workflow_ui_state_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(commissioning_workflow_ui_state_test PRIVATE Qt6::Core)

add_test(NAME commissioning_workflow_ui_state_test
    COMMAND commissioning_workflow_ui_state_test
)

add_executable(commissioning_workflow_step_detail_ui_state_test
    commissioning_workflow_step_detail_ui_state_test.cpp
    ../apps/ecat-studio/models/CommissioningWorkflowModel.cpp
    ../apps/ecat-studio/models/CommissioningWorkflowModel.h
    ../apps/ecat-studio/detail/CommissioningWorkflowStepDetail.cpp
    ../apps/ecat-studio/detail/CommissioningWorkflowStepDetail.h
    ../apps/ecat-studio/adapters/CommissioningWorkflowTableAdapter.h
    ../apps/ecat-studio/detail/CommissioningWorkflowDetail.cpp
    ../apps/ecat-studio/detail/CommissioningWorkflowDetail.h
)

target_include_directories(commissioning_workflow_step_detail_ui_state_test
    PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(commissioning_workflow_step_detail_ui_state_test
    PRIVATE
    Qt6::Core
)

add_test(NAME commissioning_workflow_step_detail_ui_state_test
    COMMAND commissioning_workflow_step_detail_ui_state_test
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

add_executable(next_best_action_ui_state_test
    next_best_action_ui_state_test.cpp
    ../apps/ecat-studio/models/CommissioningWorkflowModel.h
    ../apps/ecat-studio/models/NextBestActionModel.cpp
    ../apps/ecat-studio/models/NextBestActionModel.h
    ../apps/ecat-studio/detail/NextBestActionDetail.cpp
    ../apps/ecat-studio/detail/NextBestActionDetail.h
)

target_include_directories(next_best_action_ui_state_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(next_best_action_ui_state_test PRIVATE Qt6::Core)

add_test(NAME next_best_action_ui_state_test
    COMMAND next_best_action_ui_state_test
)

add_executable(host_health_ui_state_test
    host_health_ui_state_test.cpp
    ../apps/ecat-studio/detail/HostHealthDetail.cpp
    ../apps/ecat-studio/detail/HostHealthDetail.h
)

target_include_directories(host_health_ui_state_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(host_health_ui_state_test PRIVATE Qt6::Core)

add_test(NAME host_health_ui_state_test
    COMMAND host_health_ui_state_test
)

add_executable(diagnostics_event_ui_state_test
    diagnostics_event_ui_state_test.cpp
    ../apps/ecat-studio/detail/DiagnosticsEventDetail.cpp
    ../apps/ecat-studio/detail/DiagnosticsEventDetail.h
)

target_include_directories(diagnostics_event_ui_state_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(diagnostics_event_ui_state_test PRIVATE Qt6::Core)

add_test(NAME diagnostics_event_ui_state_test
    COMMAND diagnostics_event_ui_state_test
)

add_executable(workspace_tab_badge_ui_state_test
    workspace_tab_badge_ui_state_test.cpp
    ../apps/ecat-studio/detail/WorkspaceTabBadgeDetail.cpp
    ../apps/ecat-studio/detail/WorkspaceTabBadgeDetail.h
)

target_include_directories(workspace_tab_badge_ui_state_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(workspace_tab_badge_ui_state_test PRIVATE Qt6::Core)

add_test(NAME workspace_tab_badge_ui_state_test
    COMMAND workspace_tab_badge_ui_state_test
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

add_executable(workspace_boundary_ui_state_test
    workspace_boundary_ui_state_test.cpp
    ../apps/ecat-studio/detail/WorkspaceBoundaryDetail.cpp
    ../apps/ecat-studio/detail/WorkspaceBoundaryDetail.h
)

target_include_directories(workspace_boundary_ui_state_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(workspace_boundary_ui_state_test PRIVATE Qt6::Core)

add_test(NAME workspace_boundary_ui_state_test
    COMMAND workspace_boundary_ui_state_test
)

add_executable(commissioning_workflow_table_adapter_test
    commissioning_workflow_table_adapter_test.cpp
    ../apps/ecat-studio/adapters/CommissioningWorkflowTableAdapter.cpp
    ../apps/ecat-studio/adapters/CommissioningWorkflowTableAdapter.h
    ../apps/ecat-studio/utils/TableHelpers.cpp
    ../apps/ecat-studio/utils/TableHelpers.h
    ../apps/ecat-studio/utils/TextHelpers.cpp
    ../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(commissioning_workflow_table_adapter_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(commissioning_workflow_table_adapter_test PRIVATE
    Qt6::Core
    Qt6::Widgets
)

add_test(NAME commissioning_workflow_table_adapter_test
    COMMAND commissioning_workflow_table_adapter_test
)
set_tests_properties(commissioning_workflow_table_adapter_test PROPERTIES
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

add_executable(session_brief_ui_state_test
    session_brief_ui_state_test.cpp
    ../apps/ecat-studio/detail/SessionBriefDetail.cpp
    ../apps/ecat-studio/detail/SessionBriefDetail.h
    ../apps/ecat-studio/models/SessionBriefModel.h
)

target_include_directories(session_brief_ui_state_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(session_brief_ui_state_test PRIVATE Qt6::Core)

add_test(NAME session_brief_ui_state_test COMMAND session_brief_ui_state_test)

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

add_executable(selected_drive_summary_ui_state_test
    selected_drive_summary_ui_state_test.cpp
    ../apps/ecat-studio/models/Cia402DriveModel.cpp
    ../apps/ecat-studio/models/Cia402DriveModel.h
    ../apps/ecat-studio/models/EvidenceModel.cpp
    ../apps/ecat-studio/models/EvidenceModel.h
    ../apps/ecat-studio/detail/SelectedDriveSummaryDetail.cpp
    ../apps/ecat-studio/detail/SelectedDriveSummaryDetail.h
    ../apps/ecat-studio/models/WatchStartupModel.h
)

target_include_directories(selected_drive_summary_ui_state_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(selected_drive_summary_ui_state_test PRIVATE Qt6::Core)

add_test(NAME selected_drive_summary_ui_state_test
    COMMAND selected_drive_summary_ui_state_test
)

add_executable(selected_slave_evidence_summary_ui_state_test
    selected_slave_evidence_summary_ui_state_test.cpp
    ../apps/ecat-studio/detail/SelectedSlaveEvidenceSummaryDetail.cpp
    ../apps/ecat-studio/detail/SelectedSlaveEvidenceSummaryDetail.h
    ../apps/ecat-studio/models/SlaveEvidenceModel.h
)

target_include_directories(selected_slave_evidence_summary_ui_state_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(selected_slave_evidence_summary_ui_state_test PRIVATE
    Qt6::Core
)

add_test(NAME selected_slave_evidence_summary_ui_state_test
    COMMAND selected_slave_evidence_summary_ui_state_test
)

add_executable(slave_evidence_ui_state_test
    slave_evidence_ui_state_test.cpp
    ../apps/ecat-studio/detail/SlaveEvidenceDetail.cpp
    ../apps/ecat-studio/detail/SlaveEvidenceDetail.h
    ../apps/ecat-studio/models/SlaveEvidenceModel.cpp
    ../apps/ecat-studio/models/SlaveEvidenceModel.h
)

target_include_directories(slave_evidence_ui_state_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(slave_evidence_ui_state_test PRIVATE Qt6::Core)

add_test(NAME slave_evidence_ui_state_test COMMAND slave_evidence_ui_state_test)

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

set(RELEASE_SMOKE_TESTS
    sdo_evidence_model_test
    process_data_row_model_test
    evidence_status_model_test
    cia402_drive_model_test
    watch_startup_model_test
    selected_drive_summary_ui_state_test
    selected_slave_evidence_summary_ui_state_test
    workspace_tab_badge_ui_state_test
    workspace_boundary_ui_state_test
    session_brief_model_test
    slave_evidence_model_test
    consistency_gate_model_test
    state_recommendation_model_test
    topology_baseline_model_test
    topology_change_model_test
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
        evidence_status_model_test
        cia402_drive_model_test
        watch_startup_model_test
        selected_drive_summary_ui_state_test
        selected_slave_evidence_summary_ui_state_test
        workspace_tab_badge_ui_state_test
        workspace_boundary_ui_state_test
        session_brief_model_test
        slave_evidence_model_test
        consistency_gate_model_test
        state_recommendation_model_test
        topology_baseline_model_test
        topology_change_model_test
    USES_TERMINAL
)

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
target_link_libraries(workspace_widgets_test PRIVATE Qt6::Core Qt6::Test)

set_target_properties(workspace_widgets_test PROPERTIES AUTOMOC ON)

add_test(NAME workspace_widgets_test COMMAND workspace_widgets_test)

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
