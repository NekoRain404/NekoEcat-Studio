add_executable(sdo_evidence_model_test
    sdo_evidence_model_test.cpp
    ../apps/ecat-studio/SdoEvidenceModel.cpp
    ../apps/ecat-studio/SdoEvidenceModel.h
    ../apps/ecat-studio/StudioTextHelpers.cpp
    ../apps/ecat-studio/StudioTextHelpers.h
)

target_include_directories(sdo_evidence_model_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(sdo_evidence_model_test PRIVATE Qt6::Core)

add_test(NAME sdo_evidence_model_test COMMAND sdo_evidence_model_test)

add_executable(process_data_row_model_test
    process_data_row_model_test.cpp
    ../apps/ecat-studio/ProcessDataRowModel.cpp
    ../apps/ecat-studio/ProcessDataRowModel.h
    ../apps/ecat-studio/ProcessDataTypes.h
    ../apps/ecat-studio/EvidenceStatusModel.cpp
    ../apps/ecat-studio/EvidenceStatusModel.h
    ../apps/ecat-studio/StudioTextHelpers.cpp
    ../apps/ecat-studio/StudioTextHelpers.h
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
    ../apps/ecat-studio/IoVariableDetailUiState.cpp
    ../apps/ecat-studio/IoVariableDetailUiState.h
    ../apps/ecat-studio/ProcessDataRowModel.cpp
    ../apps/ecat-studio/ProcessDataRowModel.h
    ../apps/ecat-studio/ProcessDataTypes.h
    ../apps/ecat-studio/EvidenceStatusModel.cpp
    ../apps/ecat-studio/EvidenceStatusModel.h
    ../apps/ecat-studio/StudioTextHelpers.cpp
    ../apps/ecat-studio/StudioTextHelpers.h
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
    ../apps/ecat-studio/FreeRunEntryDetailUiState.cpp
    ../apps/ecat-studio/FreeRunEntryDetailUiState.h
    ../apps/ecat-studio/ProcessDataTypes.h
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
    ../apps/ecat-studio/PdoMapDetailUiState.cpp
    ../apps/ecat-studio/PdoMapDetailUiState.h
    ../apps/ecat-studio/ProcessDataRowModel.cpp
    ../apps/ecat-studio/ProcessDataRowModel.h
    ../apps/ecat-studio/ProcessDataTypes.h
    ../apps/ecat-studio/EvidenceStatusModel.cpp
    ../apps/ecat-studio/EvidenceStatusModel.h
    ../apps/ecat-studio/StudioTextHelpers.cpp
    ../apps/ecat-studio/StudioTextHelpers.h
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
    ../apps/ecat-studio/IoVariableFilterModel.cpp
    ../apps/ecat-studio/IoVariableFilterModel.h
    ../apps/ecat-studio/ProcessDataRowModel.cpp
    ../apps/ecat-studio/ProcessDataRowModel.h
    ../apps/ecat-studio/ProcessDataTypes.h
    ../apps/ecat-studio/EvidenceStatusModel.cpp
    ../apps/ecat-studio/EvidenceStatusModel.h
    ../apps/ecat-studio/StudioTextHelpers.cpp
    ../apps/ecat-studio/StudioTextHelpers.h
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
    ../apps/ecat-studio/ProcessDataTableAdapter.cpp
    ../apps/ecat-studio/ProcessDataTableAdapter.h
    ../apps/ecat-studio/ProcessDataRowModel.cpp
    ../apps/ecat-studio/ProcessDataRowModel.h
    ../apps/ecat-studio/ProcessDataTypes.h
    ../apps/ecat-studio/EvidenceStatusModel.cpp
    ../apps/ecat-studio/EvidenceStatusModel.h
    ../apps/ecat-studio/StudioTableHelpers.cpp
    ../apps/ecat-studio/StudioTableHelpers.h
    ../apps/ecat-studio/StudioTextHelpers.cpp
    ../apps/ecat-studio/StudioTextHelpers.h
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
    ../apps/ecat-studio/IoVariableHandoffModel.cpp
    ../apps/ecat-studio/IoVariableHandoffModel.h
    ../apps/ecat-studio/ProcessDataRowModel.cpp
    ../apps/ecat-studio/ProcessDataRowModel.h
    ../apps/ecat-studio/ProcessDataTypes.h
    ../apps/ecat-studio/EvidenceStatusModel.cpp
    ../apps/ecat-studio/EvidenceStatusModel.h
    ../apps/ecat-studio/StudioTextHelpers.cpp
    ../apps/ecat-studio/StudioTextHelpers.h
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
    ../apps/ecat-studio/IoVariableBulkNamingModel.cpp
    ../apps/ecat-studio/IoVariableBulkNamingModel.h
    ../apps/ecat-studio/IoVariableHandoffModel.cpp
    ../apps/ecat-studio/IoVariableHandoffModel.h
    ../apps/ecat-studio/ProcessDataRowModel.cpp
    ../apps/ecat-studio/ProcessDataRowModel.h
    ../apps/ecat-studio/ProcessDataTypes.h
    ../apps/ecat-studio/EvidenceStatusModel.cpp
    ../apps/ecat-studio/EvidenceStatusModel.h
    ../apps/ecat-studio/StudioTextHelpers.cpp
    ../apps/ecat-studio/StudioTextHelpers.h
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
    ../apps/ecat-studio/SdoDictionaryTableAdapter.cpp
    ../apps/ecat-studio/SdoDictionaryTableAdapter.h
    ../apps/ecat-studio/StudioTableHelpers.cpp
    ../apps/ecat-studio/StudioTableHelpers.h
    ../apps/ecat-studio/StudioTextHelpers.cpp
    ../apps/ecat-studio/StudioTextHelpers.h
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
    ../apps/ecat-studio/SdoEvidenceModel.cpp
    ../apps/ecat-studio/SdoEvidenceModel.h
    ../apps/ecat-studio/SdoEvidenceTableAdapter.cpp
    ../apps/ecat-studio/SdoEvidenceTableAdapter.h
    ../apps/ecat-studio/StudioTableHelpers.cpp
    ../apps/ecat-studio/StudioTableHelpers.h
    ../apps/ecat-studio/StudioTextHelpers.cpp
    ../apps/ecat-studio/StudioTextHelpers.h
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
    ../apps/ecat-studio/SdoHistoryRowDetailUiState.cpp
    ../apps/ecat-studio/SdoHistoryRowDetailUiState.h
    ../apps/ecat-studio/SdoEvidenceTableAdapter.cpp
    ../apps/ecat-studio/SdoEvidenceTableAdapter.h
    ../apps/ecat-studio/SdoEvidenceModel.cpp
    ../apps/ecat-studio/SdoEvidenceModel.h
    ../apps/ecat-studio/StudioTableHelpers.cpp
    ../apps/ecat-studio/StudioTableHelpers.h
    ../apps/ecat-studio/StudioTextHelpers.cpp
    ../apps/ecat-studio/StudioTextHelpers.h
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
    ../apps/ecat-studio/ObjectBookmarkDetailUiState.cpp
    ../apps/ecat-studio/ObjectBookmarkDetailUiState.h
    ../apps/ecat-studio/SdoEvidenceTableAdapter.cpp
    ../apps/ecat-studio/SdoEvidenceTableAdapter.h
    ../apps/ecat-studio/SdoEvidenceModel.cpp
    ../apps/ecat-studio/SdoEvidenceModel.h
    ../apps/ecat-studio/StudioTableHelpers.cpp
    ../apps/ecat-studio/StudioTableHelpers.h
    ../apps/ecat-studio/StudioTextHelpers.cpp
    ../apps/ecat-studio/StudioTextHelpers.h
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
    ../apps/ecat-studio/SdoTargetTrailDetailUiState.cpp
    ../apps/ecat-studio/SdoTargetTrailDetailUiState.h
    ../apps/ecat-studio/SdoEvidenceTableAdapter.cpp
    ../apps/ecat-studio/SdoEvidenceTableAdapter.h
    ../apps/ecat-studio/SdoEvidenceModel.cpp
    ../apps/ecat-studio/SdoEvidenceModel.h
    ../apps/ecat-studio/StudioTableHelpers.cpp
    ../apps/ecat-studio/StudioTableHelpers.h
    ../apps/ecat-studio/StudioTextHelpers.cpp
    ../apps/ecat-studio/StudioTextHelpers.h
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
    ../apps/ecat-studio/SdoTargetPanelRouteModel.cpp
    ../apps/ecat-studio/SdoTargetPanelRouteModel.h
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
    ../apps/ecat-studio/WatchStartupModel.cpp
    ../apps/ecat-studio/WatchStartupModel.h
    ../apps/ecat-studio/SdoEvidenceModel.cpp
    ../apps/ecat-studio/SdoEvidenceModel.h
    ../apps/ecat-studio/StudioTextHelpers.cpp
    ../apps/ecat-studio/StudioTextHelpers.h
)

target_include_directories(watch_startup_model_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(watch_startup_model_test PRIVATE Qt6::Core)

add_test(NAME watch_startup_model_test COMMAND watch_startup_model_test)

add_executable(watch_row_detail_ui_state_test
    watch_row_detail_ui_state_test.cpp
    ../apps/ecat-studio/WatchRowDetailUiState.cpp
    ../apps/ecat-studio/WatchRowDetailUiState.h
    ../apps/ecat-studio/WatchStartupModel.h
    ../apps/ecat-studio/StudioTextHelpers.cpp
    ../apps/ecat-studio/StudioTextHelpers.h
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
    ../apps/ecat-studio/StartupSdoRowDetailUiState.cpp
    ../apps/ecat-studio/StartupSdoRowDetailUiState.h
    ../apps/ecat-studio/WatchStartupModel.h
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
    ../apps/ecat-studio/TopologyBaselineModel.cpp
    ../apps/ecat-studio/TopologyBaselineModel.h
)

target_include_directories(topology_baseline_model_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(topology_baseline_model_test PRIVATE ecat_core Qt6::Core)

add_test(NAME topology_baseline_model_test COMMAND topology_baseline_model_test)

add_executable(topology_change_model_test
    topology_change_model_test.cpp
    ../apps/ecat-studio/TopologyChangeModel.cpp
    ../apps/ecat-studio/TopologyChangeModel.h
)

target_include_directories(topology_change_model_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(topology_change_model_test PRIVATE ecat_core Qt6::Core)

add_test(NAME topology_change_model_test COMMAND topology_change_model_test)

add_executable(state_recommendation_model_test
    state_recommendation_model_test.cpp
    ../apps/ecat-studio/StateRecommendationModel.cpp
    ../apps/ecat-studio/StateRecommendationModel.h
)

target_include_directories(state_recommendation_model_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(state_recommendation_model_test PRIVATE Qt6::Core)

add_test(NAME state_recommendation_model_test COMMAND state_recommendation_model_test)

add_executable(state_machine_row_detail_ui_state_test
    state_machine_row_detail_ui_state_test.cpp
    ../apps/ecat-studio/StateMachineRowDetailUiState.cpp
    ../apps/ecat-studio/StateMachineRowDetailUiState.h
    ../apps/ecat-studio/StateMachineTableAdapter.h
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
    ../apps/ecat-studio/StateMachineTableAdapter.cpp
    ../apps/ecat-studio/StateMachineTableAdapter.h
    ../apps/ecat-studio/StudioTableHelpers.cpp
    ../apps/ecat-studio/StudioTableHelpers.h
    ../apps/ecat-studio/StudioTextHelpers.cpp
    ../apps/ecat-studio/StudioTextHelpers.h
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
    ../apps/ecat-studio/ConsistencyDetailUiState.cpp
    ../apps/ecat-studio/ConsistencyDetailUiState.h
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
    ../apps/ecat-studio/ConsistencyDetailUiState.h
    ../apps/ecat-studio/ConsistencyEvidenceRouteModel.cpp
    ../apps/ecat-studio/ConsistencyEvidenceRouteModel.h
    ../apps/ecat-studio/StudioTextHelpers.cpp
    ../apps/ecat-studio/StudioTextHelpers.h
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
    ../apps/ecat-studio/ConsistencyGateModel.cpp
    ../apps/ecat-studio/ConsistencyGateModel.h
)

target_include_directories(consistency_gate_model_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(consistency_gate_model_test PRIVATE Qt6::Core)

add_test(NAME consistency_gate_model_test COMMAND consistency_gate_model_test)

add_executable(consistency_table_adapter_test
    consistency_table_adapter_test.cpp
    ../apps/ecat-studio/ConsistencyGateModel.cpp
    ../apps/ecat-studio/ConsistencyGateModel.h
    ../apps/ecat-studio/ConsistencyTableAdapter.cpp
    ../apps/ecat-studio/ConsistencyTableAdapter.h
    ../apps/ecat-studio/StudioTableHelpers.cpp
    ../apps/ecat-studio/StudioTableHelpers.h
    ../apps/ecat-studio/StudioTextHelpers.cpp
    ../apps/ecat-studio/StudioTextHelpers.h
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
    ../apps/ecat-studio/EvidenceStatusModel.cpp
    ../apps/ecat-studio/EvidenceStatusModel.h
)

target_include_directories(evidence_status_model_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(evidence_status_model_test PRIVATE Qt6::Core)

add_test(NAME evidence_status_model_test COMMAND evidence_status_model_test)

add_executable(cia402_drive_model_test
    cia402_drive_model_test.cpp
    ../apps/ecat-studio/Cia402DriveModel.cpp
    ../apps/ecat-studio/Cia402DriveModel.h
)

target_include_directories(cia402_drive_model_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(cia402_drive_model_test PRIVATE Qt6::Core)

add_test(NAME cia402_drive_model_test COMMAND cia402_drive_model_test)

add_executable(studio_table_helpers_test
    studio_table_helpers_test.cpp
    ../apps/ecat-studio/StudioTableHelpers.cpp
    ../apps/ecat-studio/StudioTableHelpers.h
    ../apps/ecat-studio/StudioTextHelpers.cpp
    ../apps/ecat-studio/StudioTextHelpers.h
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
    ../apps/ecat-studio/CommissioningWorkflowModel.cpp
    ../apps/ecat-studio/CommissioningWorkflowModel.h
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
    ../apps/ecat-studio/CommissioningWorkflowModel.cpp
    ../apps/ecat-studio/CommissioningWorkflowModel.h
    ../apps/ecat-studio/CommissioningWorkflowUiState.cpp
    ../apps/ecat-studio/CommissioningWorkflowUiState.h
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
    ../apps/ecat-studio/CommissioningWorkflowModel.cpp
    ../apps/ecat-studio/CommissioningWorkflowModel.h
    ../apps/ecat-studio/CommissioningWorkflowStepDetailUiState.cpp
    ../apps/ecat-studio/CommissioningWorkflowStepDetailUiState.h
    ../apps/ecat-studio/CommissioningWorkflowTableAdapter.h
    ../apps/ecat-studio/CommissioningWorkflowUiState.cpp
    ../apps/ecat-studio/CommissioningWorkflowUiState.h
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
    ../apps/ecat-studio/CommissioningWorkflowModel.h
    ../apps/ecat-studio/NextBestActionModel.cpp
    ../apps/ecat-studio/NextBestActionModel.h
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
    ../apps/ecat-studio/CommissioningWorkflowModel.h
    ../apps/ecat-studio/NextBestActionModel.cpp
    ../apps/ecat-studio/NextBestActionModel.h
    ../apps/ecat-studio/NextBestActionUiState.cpp
    ../apps/ecat-studio/NextBestActionUiState.h
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
    ../apps/ecat-studio/HostHealthUiState.cpp
    ../apps/ecat-studio/HostHealthUiState.h
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
    ../apps/ecat-studio/DiagnosticsEventUiState.cpp
    ../apps/ecat-studio/DiagnosticsEventUiState.h
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
    ../apps/ecat-studio/WorkspaceTabBadgeUiState.cpp
    ../apps/ecat-studio/WorkspaceTabBadgeUiState.h
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
    ../apps/ecat-studio/ConsistencyGateModel.cpp
    ../apps/ecat-studio/ConsistencyGateModel.h
    ../apps/ecat-studio/ConsistencyTableAdapter.cpp
    ../apps/ecat-studio/ConsistencyTableAdapter.h
    ../apps/ecat-studio/DiagnosticsEventUiState.cpp
    ../apps/ecat-studio/DiagnosticsEventUiState.h
    ../apps/ecat-studio/EvidenceStatusModel.cpp
    ../apps/ecat-studio/EvidenceStatusModel.h
    ../apps/ecat-studio/ProcessDataRowModel.cpp
    ../apps/ecat-studio/ProcessDataRowModel.h
    ../apps/ecat-studio/ProcessDataTableAdapter.cpp
    ../apps/ecat-studio/ProcessDataTableAdapter.h
    ../apps/ecat-studio/ProcessDataTypes.h
    ../apps/ecat-studio/SdoEvidenceModel.cpp
    ../apps/ecat-studio/SdoEvidenceModel.h
    ../apps/ecat-studio/SlaveEvidenceTableAdapter.cpp
    ../apps/ecat-studio/SlaveEvidenceTableAdapter.h
    ../apps/ecat-studio/SlaveEvidenceModel.cpp
    ../apps/ecat-studio/SlaveEvidenceModel.h
    ../apps/ecat-studio/StudioTableHelpers.cpp
    ../apps/ecat-studio/StudioTableHelpers.h
    ../apps/ecat-studio/StudioTextHelpers.cpp
    ../apps/ecat-studio/StudioTextHelpers.h
    ../apps/ecat-studio/WatchStartupModel.cpp
    ../apps/ecat-studio/WatchStartupModel.h
    ../apps/ecat-studio/WatchStartupTableAdapter.cpp
    ../apps/ecat-studio/WatchStartupTableAdapter.h
    ../apps/ecat-studio/WorkspaceTabBadgeTableAdapter.cpp
    ../apps/ecat-studio/WorkspaceTabBadgeTableAdapter.h
    ../apps/ecat-studio/WorkspaceTabBadgeUiState.h
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
    ../apps/ecat-studio/WorkspaceBoundaryUiState.cpp
    ../apps/ecat-studio/WorkspaceBoundaryUiState.h
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
    ../apps/ecat-studio/CommissioningWorkflowTableAdapter.cpp
    ../apps/ecat-studio/CommissioningWorkflowTableAdapter.h
    ../apps/ecat-studio/StudioTableHelpers.cpp
    ../apps/ecat-studio/StudioTableHelpers.h
    ../apps/ecat-studio/StudioTextHelpers.cpp
    ../apps/ecat-studio/StudioTextHelpers.h
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
    ../apps/ecat-studio/SessionBriefModel.cpp
    ../apps/ecat-studio/SessionBriefModel.h
)

target_include_directories(session_brief_model_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(session_brief_model_test PRIVATE Qt6::Core)

add_test(NAME session_brief_model_test COMMAND session_brief_model_test)

add_executable(session_brief_table_adapter_test
    session_brief_table_adapter_test.cpp
    ../apps/ecat-studio/SessionBriefTableAdapter.cpp
    ../apps/ecat-studio/SessionBriefTableAdapter.h
    ../apps/ecat-studio/StudioTableHelpers.cpp
    ../apps/ecat-studio/StudioTableHelpers.h
    ../apps/ecat-studio/StudioTextHelpers.cpp
    ../apps/ecat-studio/StudioTextHelpers.h
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
    ../apps/ecat-studio/SessionBriefUiState.cpp
    ../apps/ecat-studio/SessionBriefUiState.h
    ../apps/ecat-studio/SessionBriefModel.h
)

target_include_directories(session_brief_ui_state_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(session_brief_ui_state_test PRIVATE Qt6::Core)

add_test(NAME session_brief_ui_state_test COMMAND session_brief_ui_state_test)

add_executable(slave_evidence_model_test
    slave_evidence_model_test.cpp
    ../apps/ecat-studio/SlaveEvidenceModel.cpp
    ../apps/ecat-studio/SlaveEvidenceModel.h
)

target_include_directories(slave_evidence_model_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(slave_evidence_model_test PRIVATE Qt6::Core)

add_test(NAME slave_evidence_model_test COMMAND slave_evidence_model_test)

add_executable(selected_drive_summary_ui_state_test
    selected_drive_summary_ui_state_test.cpp
    ../apps/ecat-studio/Cia402DriveModel.cpp
    ../apps/ecat-studio/Cia402DriveModel.h
    ../apps/ecat-studio/EvidenceStatusModel.cpp
    ../apps/ecat-studio/EvidenceStatusModel.h
    ../apps/ecat-studio/SelectedDriveSummaryUiState.cpp
    ../apps/ecat-studio/SelectedDriveSummaryUiState.h
    ../apps/ecat-studio/WatchStartupModel.h
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
    ../apps/ecat-studio/SelectedSlaveEvidenceSummaryUiState.cpp
    ../apps/ecat-studio/SelectedSlaveEvidenceSummaryUiState.h
    ../apps/ecat-studio/SlaveEvidenceModel.h
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
    ../apps/ecat-studio/SlaveEvidenceUiState.cpp
    ../apps/ecat-studio/SlaveEvidenceUiState.h
    ../apps/ecat-studio/SlaveEvidenceModel.cpp
    ../apps/ecat-studio/SlaveEvidenceModel.h
)

target_include_directories(slave_evidence_ui_state_test PRIVATE
    ../apps/ecat-studio
)
target_link_libraries(slave_evidence_ui_state_test PRIVATE Qt6::Core)

add_test(NAME slave_evidence_ui_state_test COMMAND slave_evidence_ui_state_test)

add_executable(slave_evidence_table_adapter_test
    slave_evidence_table_adapter_test.cpp
    ../apps/ecat-studio/SlaveEvidenceTableAdapter.cpp
    ../apps/ecat-studio/SlaveEvidenceTableAdapter.h
    ../apps/ecat-studio/SlaveEvidenceModel.cpp
    ../apps/ecat-studio/SlaveEvidenceModel.h
    ../apps/ecat-studio/EvidenceStatusModel.cpp
    ../apps/ecat-studio/EvidenceStatusModel.h
    ../apps/ecat-studio/StudioTableHelpers.cpp
    ../apps/ecat-studio/StudioTableHelpers.h
    ../apps/ecat-studio/StudioTextHelpers.cpp
    ../apps/ecat-studio/StudioTextHelpers.h
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
