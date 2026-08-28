# Unit Tests - models
# Auto-generated from monolithic tests/CMakeLists.txt

add_executable(sdo_evidence_model_test
    sdo_evidence_model_test.cpp
    ../../../apps/ecat-studio/models/SdoEvidenceModel.cpp
    ../../../apps/ecat-studio/models/SdoEvidenceModel.h
    ../../../apps/ecat-studio/utils/TextHelpers.cpp
    ../../../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(sdo_evidence_model_test PRIVATE
    ../../../apps/ecat-studio
)
target_link_libraries(sdo_evidence_model_test PRIVATE Qt6::Core)

add_test(NAME sdo_evidence_model_test COMMAND sdo_evidence_model_test)

add_executable(process_data_row_model_test
    process_data_row_model_test.cpp
    ../../../apps/ecat-studio/models/ProcessDataRowModel.cpp
    ../../../apps/ecat-studio/models/ProcessDataRowModel.h
    ../../../apps/ecat-studio/infra/ProcessDataTypes.h
    ../../../apps/ecat-studio/models/EvidenceModel.cpp
    ../../../apps/ecat-studio/models/EvidenceModel.h
    ../../../apps/ecat-studio/utils/TextHelpers.cpp
    ../../../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(process_data_row_model_test PRIVATE
    ../../../apps/ecat-studio
)
target_link_libraries(process_data_row_model_test PRIVATE Qt6::Core)

add_test(NAME process_data_row_model_test
    COMMAND process_data_row_model_test
)

add_executable(watch_startup_model_test
    watch_startup_model_test.cpp
    ../../../apps/ecat-studio/models/WatchStartupModel.cpp
    ../../../apps/ecat-studio/models/WatchStartupModel.h
    ../../../apps/ecat-studio/models/SdoEvidenceModel.cpp
    ../../../apps/ecat-studio/models/SdoEvidenceModel.h
    ../../../apps/ecat-studio/utils/TextHelpers.cpp
    ../../../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(watch_startup_model_test PRIVATE
    ../../../apps/ecat-studio
)
target_link_libraries(watch_startup_model_test PRIVATE Qt6::Core)

add_test(NAME watch_startup_model_test COMMAND watch_startup_model_test)

add_executable(consistency_route_model_test
    consistency_route_model_test.cpp
    ../../../apps/ecat-studio/detail/ConsistencyDetail.h
    ../../../apps/ecat-studio/models/ConsistencyModel.cpp
    ../../../apps/ecat-studio/models/ConsistencyModel.h
    ../../../apps/ecat-studio/utils/TextHelpers.cpp
    ../../../apps/ecat-studio/utils/TextHelpers.h
)

target_include_directories(consistency_route_model_test PRIVATE
    ../../../apps/ecat-studio
    ../../../src/core
)
target_link_libraries(consistency_route_model_test PRIVATE Qt6::Core)

add_test(NAME consistency_route_model_test
    COMMAND consistency_route_model_test
)

add_executable(cia402_drive_model_test
    cia402_drive_model_test.cpp
    ../../../apps/ecat-studio/models/Cia402DriveModel.cpp
    ../../../apps/ecat-studio/models/Cia402DriveModel.h
)

target_include_directories(cia402_drive_model_test PRIVATE
    ../../../apps/ecat-studio
)
target_link_libraries(cia402_drive_model_test PRIVATE Qt6::Core)

add_test(NAME cia402_drive_model_test COMMAND cia402_drive_model_test)

add_executable(commissioning_workflow_model_test
    commissioning_workflow_model_test.cpp
    ../../../apps/ecat-studio/models/CommissioningWorkflowModel.cpp
    ../../../apps/ecat-studio/models/CommissioningWorkflowModel.h
)

target_include_directories(commissioning_workflow_model_test PRIVATE
    ../../../apps/ecat-studio
)
target_link_libraries(commissioning_workflow_model_test PRIVATE Qt6::Core)

add_test(NAME commissioning_workflow_model_test
    COMMAND commissioning_workflow_model_test
)

add_executable(next_best_action_model_test
    next_best_action_model_test.cpp
    ../../../apps/ecat-studio/models/CommissioningWorkflowModel.h
    ../../../apps/ecat-studio/models/NextBestActionModel.cpp
    ../../../apps/ecat-studio/models/NextBestActionModel.h
)

target_include_directories(next_best_action_model_test PRIVATE
    ../../../apps/ecat-studio
)
target_link_libraries(next_best_action_model_test PRIVATE Qt6::Core)

add_test(NAME next_best_action_model_test
    COMMAND next_best_action_model_test
)

add_executable(session_brief_model_test
    session_brief_model_test.cpp
    ../../../apps/ecat-studio/models/SessionBriefModel.cpp
    ../../../apps/ecat-studio/models/SessionBriefModel.h
)

target_include_directories(session_brief_model_test PRIVATE
    ../../../apps/ecat-studio
)
target_link_libraries(session_brief_model_test PRIVATE Qt6::Core)

add_test(NAME session_brief_model_test COMMAND session_brief_model_test)

add_executable(slave_evidence_model_test
    slave_evidence_model_test.cpp
    ../../../apps/ecat-studio/models/SlaveEvidenceModel.cpp
    ../../../apps/ecat-studio/models/SlaveEvidenceModel.h
)

target_include_directories(slave_evidence_model_test PRIVATE
    ../../../apps/ecat-studio
)
target_link_libraries(slave_evidence_model_test PRIVATE Qt6::Core)

add_test(NAME slave_evidence_model_test COMMAND slave_evidence_model_test)

# Label every test in this directory as "unit" so `ctest -L unit` is meaningful
# (existing sub-labels are preserved via APPEND).
get_property(_unit_tests DIRECTORY PROPERTY TESTS)
set_property(TEST ${_unit_tests} APPEND PROPERTY LABELS "unit")
