# Unit Tests - detail
# Auto-generated from monolithic tests/CMakeLists.txt

add_executable(io_variable_detail_test
    io_variable_detail_test.cpp
    ../../../apps/ecat-studio/detail/IoVariableDetail.cpp
    ../../../apps/ecat-studio/detail/IoVariableDetail.h
    ../../../apps/ecat-studio/models/ProcessDataRowModel.cpp
    ../../../apps/ecat-studio/models/ProcessDataRowModel.h
    ../../../apps/ecat-studio/infra/ProcessDataTypes.h
    ../../../apps/ecat-studio/models/EvidenceModel.cpp
    ../../../apps/ecat-studio/models/EvidenceModel.h
    ../../../apps/ecat-studio/utils/TextHelpers.cpp
    ../../../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(io_variable_detail_test PRIVATE
    ../../../apps/ecat-studio
)
target_link_libraries(io_variable_detail_test PRIVATE Qt6::Core)

add_test(NAME io_variable_detail_test
    COMMAND io_variable_detail_test
)

add_executable(free_run_entry_detail_test
    free_run_entry_detail_test.cpp
    ../../../apps/ecat-studio/detail/FreeRunEntryDetail.cpp
    ../../../apps/ecat-studio/detail/FreeRunEntryDetail.h
    ../../../apps/ecat-studio/infra/ProcessDataTypes.h
)

target_include_directories(free_run_entry_detail_test PRIVATE
    ../../../apps/ecat-studio
)
target_link_libraries(free_run_entry_detail_test PRIVATE Qt6::Core)

add_test(NAME free_run_entry_detail_test
    COMMAND free_run_entry_detail_test
)

add_executable(pdo_map_detail_test
    pdo_map_detail_test.cpp
    ../../../apps/ecat-studio/detail/PdoMapDetail.cpp
    ../../../apps/ecat-studio/detail/PdoMapDetail.h
    ../../../apps/ecat-studio/models/ProcessDataRowModel.cpp
    ../../../apps/ecat-studio/models/ProcessDataRowModel.h
    ../../../apps/ecat-studio/infra/ProcessDataTypes.h
    ../../../apps/ecat-studio/models/EvidenceModel.cpp
    ../../../apps/ecat-studio/models/EvidenceModel.h
    ../../../apps/ecat-studio/utils/TextHelpers.cpp
    ../../../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(pdo_map_detail_test PRIVATE
    ../../../apps/ecat-studio
)
target_link_libraries(pdo_map_detail_test PRIVATE Qt6::Core)

add_test(NAME pdo_map_detail_test
    COMMAND pdo_map_detail_test
)

add_executable(sdo_history_row_detail_test
    sdo_history_row_detail_test.cpp
    ../../../apps/ecat-studio/detail/SdoHistoryRowDetail.cpp
    ../../../apps/ecat-studio/detail/SdoHistoryRowDetail.h
    ../../../apps/ecat-studio/adapters/SdoEvidenceTableAdapter.cpp
    ../../../apps/ecat-studio/adapters/SdoEvidenceTableAdapter.h
    ../../../apps/ecat-studio/models/SdoEvidenceModel.cpp
    ../../../apps/ecat-studio/models/SdoEvidenceModel.h
    ../../../apps/ecat-studio/utils/TableHelpers.cpp
    ../../../apps/ecat-studio/utils/TableHelpers.h
    ../../../apps/ecat-studio/utils/TextHelpers.cpp
    ../../../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(sdo_history_row_detail_test PRIVATE
    ../../../apps/ecat-studio
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
    ../../../apps/ecat-studio/detail/ObjectBookmarkDetail.cpp
    ../../../apps/ecat-studio/detail/ObjectBookmarkDetail.h
    ../../../apps/ecat-studio/adapters/SdoEvidenceTableAdapter.cpp
    ../../../apps/ecat-studio/adapters/SdoEvidenceTableAdapter.h
    ../../../apps/ecat-studio/models/SdoEvidenceModel.cpp
    ../../../apps/ecat-studio/models/SdoEvidenceModel.h
    ../../../apps/ecat-studio/utils/TableHelpers.cpp
    ../../../apps/ecat-studio/utils/TableHelpers.h
    ../../../apps/ecat-studio/utils/TextHelpers.cpp
    ../../../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(object_bookmark_detail_test PRIVATE
    ../../../apps/ecat-studio
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
    ../../../apps/ecat-studio/detail/SdoTargetTrailDetail.cpp
    ../../../apps/ecat-studio/detail/SdoTargetTrailDetail.h
    ../../../apps/ecat-studio/adapters/SdoEvidenceTableAdapter.cpp
    ../../../apps/ecat-studio/adapters/SdoEvidenceTableAdapter.h
    ../../../apps/ecat-studio/models/SdoEvidenceModel.cpp
    ../../../apps/ecat-studio/models/SdoEvidenceModel.h
    ../../../apps/ecat-studio/utils/TableHelpers.cpp
    ../../../apps/ecat-studio/utils/TableHelpers.h
    ../../../apps/ecat-studio/utils/TextHelpers.cpp
    ../../../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(sdo_target_trail_detail_test PRIVATE
    ../../../apps/ecat-studio
)
target_link_libraries(sdo_target_trail_detail_test PRIVATE
    Qt6::Core
    Qt6::Widgets
)

add_test(NAME sdo_target_trail_detail_test
    COMMAND sdo_target_trail_detail_test
)

add_executable(sdo_target_panel_route_test
    ../../../apps/ecat-studio/models/SdoEvidenceModel.cpp
    ../../../apps/ecat-studio/models/SdoEvidenceModel.h
    ../../../apps/ecat-studio/utils/TextHelpers.cpp
    ../../../apps/ecat-studio/utils/TextHelpers.h
    sdo_target_panel_route_test.cpp
)

target_include_directories(sdo_target_panel_route_test PRIVATE
    ../../../apps/ecat-studio
)
target_link_libraries(sdo_target_panel_route_test PRIVATE Qt6::Core)

add_test(NAME sdo_target_panel_route_test
    COMMAND sdo_target_panel_route_test
)

add_executable(watch_row_detail_test
    watch_row_detail_test.cpp
    ../../../apps/ecat-studio/detail/WatchRowDetail.cpp
    ../../../apps/ecat-studio/detail/WatchRowDetail.h
    ../../../apps/ecat-studio/models/WatchStartupModel.h
    ../../../apps/ecat-studio/utils/TextHelpers.cpp
    ../../../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(watch_row_detail_test PRIVATE
    ../../../apps/ecat-studio
)
target_link_libraries(watch_row_detail_test PRIVATE Qt6::Core)

add_test(NAME watch_row_detail_test
    COMMAND watch_row_detail_test
)

add_executable(startup_sdo_row_detail_test
    startup_sdo_row_detail_test.cpp
    ../../../apps/ecat-studio/detail/StartupSdoRowDetail.cpp
    ../../../apps/ecat-studio/detail/StartupSdoRowDetail.h
    ../../../apps/ecat-studio/models/WatchStartupModel.h
)

target_include_directories(startup_sdo_row_detail_test PRIVATE
    ../../../apps/ecat-studio
)
target_link_libraries(startup_sdo_row_detail_test PRIVATE Qt6::Core)

add_test(NAME startup_sdo_row_detail_test
    COMMAND startup_sdo_row_detail_test
)

add_executable(state_machine_row_detail_test
    state_machine_row_detail_test.cpp
    ../../../apps/ecat-studio/detail/StateMachineRowDetail.cpp
    ../../../apps/ecat-studio/detail/StateMachineRowDetail.h
    ../../../apps/ecat-studio/adapters/StateMachineTableAdapter.h
)

target_include_directories(state_machine_row_detail_test PRIVATE
    ../../../apps/ecat-studio
)
target_link_libraries(state_machine_row_detail_test PRIVATE Qt6::Core)

add_test(NAME state_machine_row_detail_test
    COMMAND state_machine_row_detail_test
)

add_executable(consistency_detail_test
    consistency_detail_test.cpp
    ../../../apps/ecat-studio/detail/ConsistencyDetail.cpp
    ../../../apps/ecat-studio/detail/ConsistencyDetail.h
)

target_include_directories(consistency_detail_test PRIVATE
    ../../../apps/ecat-studio
)
target_link_libraries(consistency_detail_test PRIVATE Qt6::Core)

add_test(NAME consistency_detail_test
    COMMAND consistency_detail_test
)

add_executable(consistency_gate_test
    consistency_gate_test.cpp
    ../../../apps/ecat-studio/models/ConsistencyModel.cpp
    ../../../apps/ecat-studio/models/ConsistencyModel.h
    ../../../apps/ecat-studio/utils/TextHelpers.cpp
    ../../../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(consistency_gate_test PRIVATE
    ../../../apps/ecat-studio
    ../../../src/core
)
target_link_libraries(consistency_gate_test PRIVATE Qt6::Core)

add_test(NAME consistency_gate_test COMMAND consistency_gate_test)

add_executable(commissioning_workflow_detail_test
    commissioning_workflow_detail_test.cpp
    ../../../apps/ecat-studio/models/CommissioningWorkflowModel.cpp
    ../../../apps/ecat-studio/models/CommissioningWorkflowModel.h
    ../../../apps/ecat-studio/detail/CommissioningWorkflowDetail.cpp
    ../../../apps/ecat-studio/detail/CommissioningWorkflowDetail.h
)

target_include_directories(commissioning_workflow_detail_test PRIVATE
    ../../../apps/ecat-studio
)
target_link_libraries(commissioning_workflow_detail_test PRIVATE Qt6::Core)

add_test(NAME commissioning_workflow_detail_test
    COMMAND commissioning_workflow_detail_test
)

add_executable(workflow_step_detail_test
    workflow_step_detail_test.cpp
    ../../../apps/ecat-studio/models/CommissioningWorkflowModel.cpp
    ../../../apps/ecat-studio/models/CommissioningWorkflowModel.h
    ../../../apps/ecat-studio/detail/WorkflowStepDetail.cpp
    ../../../apps/ecat-studio/detail/WorkflowStepDetail.h
    ../../../apps/ecat-studio/adapters/WorkflowTableAdapter.h
    ../../../apps/ecat-studio/detail/CommissioningWorkflowDetail.cpp
    ../../../apps/ecat-studio/detail/CommissioningWorkflowDetail.h
)

target_include_directories(workflow_step_detail_test
    PRIVATE
    ../../../apps/ecat-studio
)
target_link_libraries(workflow_step_detail_test
    PRIVATE
    Qt6::Core
)

add_test(NAME workflow_step_detail_test
    COMMAND workflow_step_detail_test
)

add_executable(next_best_action_detail_test
    next_best_action_detail_test.cpp
    ../../../apps/ecat-studio/models/CommissioningWorkflowModel.h
    ../../../apps/ecat-studio/models/NextBestActionModel.cpp
    ../../../apps/ecat-studio/models/NextBestActionModel.h
    ../../../apps/ecat-studio/detail/NextBestActionDetail.cpp
    ../../../apps/ecat-studio/detail/NextBestActionDetail.h
)

target_include_directories(next_best_action_detail_test PRIVATE
    ../../../apps/ecat-studio
)
target_link_libraries(next_best_action_detail_test PRIVATE Qt6::Core)

add_test(NAME next_best_action_detail_test
    COMMAND next_best_action_detail_test
)

add_executable(host_health_detail_test
    host_health_detail_test.cpp
    ../../../apps/ecat-studio/detail/HostHealthDetail.cpp
    ../../../apps/ecat-studio/detail/HostHealthDetail.h
)

target_include_directories(host_health_detail_test PRIVATE
    ../../../apps/ecat-studio
)
target_link_libraries(host_health_detail_test PRIVATE Qt6::Core)

add_test(NAME host_health_detail_test
    COMMAND host_health_detail_test
)

add_executable(diagnostics_event_detail_test
    diagnostics_event_detail_test.cpp
    ../../../apps/ecat-studio/detail/DiagnosticsEventDetail.cpp
    ../../../apps/ecat-studio/detail/DiagnosticsEventDetail.h
)

target_include_directories(diagnostics_event_detail_test PRIVATE
    ../../../apps/ecat-studio
)
target_link_libraries(diagnostics_event_detail_test PRIVATE Qt6::Core)

add_test(NAME diagnostics_event_detail_test
    COMMAND diagnostics_event_detail_test
)

add_executable(workspace_tab_badge_detail_test
    workspace_tab_badge_detail_test.cpp
    ../../../apps/ecat-studio/detail/WorkspaceTabBadgeDetail.cpp
    ../../../apps/ecat-studio/detail/WorkspaceTabBadgeDetail.h
)

target_include_directories(workspace_tab_badge_detail_test PRIVATE
    ../../../apps/ecat-studio
)
target_link_libraries(workspace_tab_badge_detail_test PRIVATE Qt6::Core)

add_test(NAME workspace_tab_badge_detail_test
    COMMAND workspace_tab_badge_detail_test
)

add_executable(workspace_boundary_detail_test
    workspace_boundary_detail_test.cpp
    ../../../apps/ecat-studio/detail/WorkspaceBoundaryDetail.cpp
    ../../../apps/ecat-studio/detail/WorkspaceBoundaryDetail.h
)

target_include_directories(workspace_boundary_detail_test PRIVATE
    ../../../apps/ecat-studio
)
target_link_libraries(workspace_boundary_detail_test PRIVATE Qt6::Core)

add_test(NAME workspace_boundary_detail_test
    COMMAND workspace_boundary_detail_test
)

add_executable(session_brief_detail_test
    session_brief_detail_test.cpp
    ../../../apps/ecat-studio/detail/SessionBriefDetail.cpp
    ../../../apps/ecat-studio/detail/SessionBriefDetail.h
    ../../../apps/ecat-studio/models/SessionBriefModel.h
)

target_include_directories(session_brief_detail_test PRIVATE
    ../../../apps/ecat-studio
)
target_link_libraries(session_brief_detail_test PRIVATE Qt6::Core)

add_test(NAME session_brief_detail_test COMMAND session_brief_detail_test)

add_executable(selected_drive_summary_detail_test
    selected_drive_summary_detail_test.cpp
    ../../../apps/ecat-studio/models/Cia402DriveModel.cpp
    ../../../apps/ecat-studio/models/Cia402DriveModel.h
    ../../../apps/ecat-studio/models/EvidenceModel.cpp
    ../../../apps/ecat-studio/models/EvidenceModel.h
    ../../../apps/ecat-studio/detail/SelectedDriveSummaryDetail.cpp
    ../../../apps/ecat-studio/detail/SelectedDriveSummaryDetail.h
    ../../../apps/ecat-studio/models/WatchStartupModel.h
)

target_include_directories(selected_drive_summary_detail_test PRIVATE
    ../../../apps/ecat-studio
)
target_link_libraries(selected_drive_summary_detail_test PRIVATE Qt6::Core)

add_test(NAME selected_drive_summary_detail_test
    COMMAND selected_drive_summary_detail_test
)

add_executable(slave_evidence_summary_detail_test
    slave_evidence_summary_detail_test.cpp
    ../../../apps/ecat-studio/detail/SlaveEvidenceSummaryDetail.cpp
    ../../../apps/ecat-studio/detail/SlaveEvidenceSummaryDetail.h
    ../../../apps/ecat-studio/models/SlaveEvidenceModel.h
)

target_include_directories(slave_evidence_summary_detail_test PRIVATE
    ../../../apps/ecat-studio
)
target_link_libraries(slave_evidence_summary_detail_test PRIVATE
    Qt6::Core
)

add_test(NAME slave_evidence_summary_detail_test
    COMMAND slave_evidence_summary_detail_test
)

add_executable(slave_evidence_detail_test
    slave_evidence_detail_test.cpp
    ../../../apps/ecat-studio/detail/SlaveEvidenceDetail.cpp
    ../../../apps/ecat-studio/detail/SlaveEvidenceDetail.h
    ../../../apps/ecat-studio/models/SlaveEvidenceModel.cpp
    ../../../apps/ecat-studio/models/SlaveEvidenceModel.h
)

target_include_directories(slave_evidence_detail_test PRIVATE
    ../../../apps/ecat-studio
)
target_link_libraries(slave_evidence_detail_test PRIVATE Qt6::Core)

add_test(NAME slave_evidence_detail_test COMMAND slave_evidence_detail_test)

# Label every test in this directory as "unit" so `ctest -L unit` is meaningful
# (existing sub-labels are preserved via APPEND).
get_property(_unit_tests DIRECTORY PROPERTY TESTS)
set_property(TEST ${_unit_tests} APPEND PROPERTY LABELS "unit")
