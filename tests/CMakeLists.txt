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
    ../apps/ecat-studio/services/ServiceContainer.h
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
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
)
target_link_libraries(plugin_integration_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(plugin_integration_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
set_target_properties(plugin_integration_test PROPERTIES AUTOMOC ON)
add_test(NAME plugin_integration_test COMMAND plugin_integration_test)
set_tests_properties(plugin_integration_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# ── Service Integration Test ─────────────────────────────────────────
add_executable(service_integration_test
    service_integration_test.cpp
    ../apps/ecat-studio/services/ServiceContainer.cpp
    ../apps/ecat-studio/services/ServiceContainer.h
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
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
)
target_link_libraries(service_integration_test PRIVATE Qt6::Core Qt6::Network Qt6::Widgets Qt6::Test ecat_core)
target_include_directories(service_integration_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/src/core
)
set_target_properties(service_integration_test PROPERTIES AUTOMOC ON)
add_test(NAME service_integration_test COMMAND service_integration_test)
set_tests_properties(service_integration_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
