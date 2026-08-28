# Unit Tests - adapters
# Auto-generated from monolithic tests/CMakeLists.txt

add_executable(process_data_table_adapter_test
    process_data_table_adapter_test.cpp
    ../../../apps/ecat-studio/adapters/ProcessDataTableAdapter.cpp
    ../../../apps/ecat-studio/adapters/ProcessDataTableAdapter.h
    ../../../apps/ecat-studio/models/ProcessDataRowModel.cpp
    ../../../apps/ecat-studio/models/ProcessDataRowModel.h
    ../../../apps/ecat-studio/infra/ProcessDataTypes.h
    ../../../apps/ecat-studio/models/EvidenceModel.cpp
    ../../../apps/ecat-studio/models/EvidenceModel.h
    ../../../apps/ecat-studio/utils/TableHelpers.cpp
    ../../../apps/ecat-studio/utils/TableHelpers.h
    ../../../apps/ecat-studio/utils/TextHelpers.cpp
    ../../../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(process_data_table_adapter_test PRIVATE
    ../../../apps/ecat-studio
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

add_executable(sdo_dictionary_table_adapter_test
    sdo_dictionary_table_adapter_test.cpp
    ../../../apps/ecat-studio/adapters/SdoDictionaryTableAdapter.cpp
    ../../../apps/ecat-studio/adapters/SdoDictionaryTableAdapter.h
    ../../../apps/ecat-studio/utils/TableHelpers.cpp
    ../../../apps/ecat-studio/utils/TableHelpers.h
    ../../../apps/ecat-studio/utils/TextHelpers.cpp
    ../../../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(sdo_dictionary_table_adapter_test PRIVATE
    ../../../apps/ecat-studio
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
    ../../../apps/ecat-studio/models/SdoEvidenceModel.cpp
    ../../../apps/ecat-studio/models/SdoEvidenceModel.h
    ../../../apps/ecat-studio/adapters/SdoEvidenceTableAdapter.cpp
    ../../../apps/ecat-studio/adapters/SdoEvidenceTableAdapter.h
    ../../../apps/ecat-studio/utils/TableHelpers.cpp
    ../../../apps/ecat-studio/utils/TableHelpers.h
    ../../../apps/ecat-studio/utils/TextHelpers.cpp
    ../../../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(sdo_evidence_table_adapter_test PRIVATE
    ../../../apps/ecat-studio
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

add_executable(state_machine_table_adapter_test
    state_machine_table_adapter_test.cpp
    ../../../apps/ecat-studio/adapters/StateMachineTableAdapter.cpp
    ../../../apps/ecat-studio/adapters/StateMachineTableAdapter.h
    ../../../apps/ecat-studio/utils/TableHelpers.cpp
    ../../../apps/ecat-studio/utils/TableHelpers.h
    ../../../apps/ecat-studio/utils/TextHelpers.cpp
    ../../../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(state_machine_table_adapter_test PRIVATE
    ../../../apps/ecat-studio
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

add_executable(consistency_table_adapter_test
    consistency_table_adapter_test.cpp
    ../../../apps/ecat-studio/models/ConsistencyModel.cpp
    ../../../apps/ecat-studio/models/ConsistencyModel.h
    ../../../apps/ecat-studio/adapters/ConsistencyTableAdapter.cpp
    ../../../apps/ecat-studio/adapters/ConsistencyTableAdapter.h
    ../../../apps/ecat-studio/utils/TableHelpers.cpp
    ../../../apps/ecat-studio/utils/TableHelpers.h
    ../../../apps/ecat-studio/utils/TextHelpers.cpp
    ../../../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(consistency_table_adapter_test PRIVATE
    ../../../apps/ecat-studio
    ../../../src/core
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

add_executable(workspace_tab_badge_table_adapter_test
    workspace_tab_badge_table_adapter_test.cpp
    ../../../apps/ecat-studio/models/ConsistencyModel.cpp
    ../../../apps/ecat-studio/models/ConsistencyModel.h
    ../../../apps/ecat-studio/adapters/ConsistencyTableAdapter.cpp
    ../../../apps/ecat-studio/adapters/ConsistencyTableAdapter.h
    ../../../apps/ecat-studio/detail/DiagnosticsEventDetail.cpp
    ../../../apps/ecat-studio/detail/DiagnosticsEventDetail.h
    ../../../apps/ecat-studio/models/EvidenceModel.cpp
    ../../../apps/ecat-studio/models/EvidenceModel.h
    ../../../apps/ecat-studio/models/ProcessDataRowModel.cpp
    ../../../apps/ecat-studio/models/ProcessDataRowModel.h
    ../../../apps/ecat-studio/adapters/ProcessDataTableAdapter.cpp
    ../../../apps/ecat-studio/adapters/ProcessDataTableAdapter.h
    ../../../apps/ecat-studio/infra/ProcessDataTypes.h
    ../../../apps/ecat-studio/models/SdoEvidenceModel.cpp
    ../../../apps/ecat-studio/models/SdoEvidenceModel.h
    ../../../apps/ecat-studio/adapters/SlaveEvidenceTableAdapter.cpp
    ../../../apps/ecat-studio/adapters/SlaveEvidenceTableAdapter.h
    ../../../apps/ecat-studio/models/SlaveEvidenceModel.cpp
    ../../../apps/ecat-studio/models/SlaveEvidenceModel.h
    ../../../apps/ecat-studio/utils/TableHelpers.cpp
    ../../../apps/ecat-studio/utils/TableHelpers.h
    ../../../apps/ecat-studio/utils/TextHelpers.cpp
    ../../../apps/ecat-studio/utils/TextHelpers.h
    ../../../apps/ecat-studio/models/WatchStartupModel.cpp
    ../../../apps/ecat-studio/models/WatchStartupModel.h
    ../../../apps/ecat-studio/adapters/WatchStartupTableAdapter.cpp
    ../../../apps/ecat-studio/adapters/WatchStartupTableAdapter.h
    ../../../apps/ecat-studio/adapters/WorkspaceTabBadgeTableAdapter.cpp
    ../../../apps/ecat-studio/adapters/WorkspaceTabBadgeTableAdapter.h
    ../../../apps/ecat-studio/detail/WorkspaceTabBadgeDetail.h
)

target_include_directories(workspace_tab_badge_table_adapter_test PRIVATE
    ../../../apps/ecat-studio
    ../../../src/core
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

add_executable(workflow_table_adapter_test
    workflow_table_adapter_test.cpp
    ../../../apps/ecat-studio/adapters/WorkflowTableAdapter.cpp
    ../../../apps/ecat-studio/adapters/WorkflowTableAdapter.h
    ../../../apps/ecat-studio/utils/TableHelpers.cpp
    ../../../apps/ecat-studio/utils/TableHelpers.h
    ../../../apps/ecat-studio/utils/TextHelpers.cpp
    ../../../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(workflow_table_adapter_test PRIVATE
    ../../../apps/ecat-studio
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

add_executable(session_brief_table_adapter_test
    session_brief_table_adapter_test.cpp
    ../../../apps/ecat-studio/adapters/SessionBriefTableAdapter.cpp
    ../../../apps/ecat-studio/adapters/SessionBriefTableAdapter.h
    ../../../apps/ecat-studio/utils/TableHelpers.cpp
    ../../../apps/ecat-studio/utils/TableHelpers.h
    ../../../apps/ecat-studio/utils/TextHelpers.cpp
    ../../../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(session_brief_table_adapter_test PRIVATE
    ../../../apps/ecat-studio
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

add_executable(slave_evidence_table_adapter_test
    slave_evidence_table_adapter_test.cpp
    ../../../apps/ecat-studio/adapters/SlaveEvidenceTableAdapter.cpp
    ../../../apps/ecat-studio/adapters/SlaveEvidenceTableAdapter.h
    ../../../apps/ecat-studio/models/SlaveEvidenceModel.cpp
    ../../../apps/ecat-studio/models/SlaveEvidenceModel.h
    ../../../apps/ecat-studio/models/EvidenceModel.cpp
    ../../../apps/ecat-studio/models/EvidenceModel.h
    ../../../apps/ecat-studio/utils/TableHelpers.cpp
    ../../../apps/ecat-studio/utils/TableHelpers.h
    ../../../apps/ecat-studio/utils/TextHelpers.cpp
    ../../../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(slave_evidence_table_adapter_test PRIVATE
    ../../../apps/ecat-studio
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

# Label every test in this directory as "unit" so `ctest -L unit` is meaningful
# (existing sub-labels are preserved via APPEND).
get_property(_unit_tests DIRECTORY PROPERTY TESTS)
set_property(TEST ${_unit_tests} APPEND PROPERTY LABELS "unit")
