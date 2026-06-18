add_executable(ecatd
    CommandDispatcher.cpp
    CommandDispatcher.h
    EcatDaemon.cpp
    EcatDaemon.h
    FreeRunController.cpp
    RtTestController.cpp
    RtTestController.h
    FreeRunController.h
    handlers/AdapterHandler.h
    handlers/AdapterHandler.cpp
    handlers/AlEventHandler.h
    handlers/AlEventHandler.cpp
    handlers/DcSyncHandler.h
    handlers/DcSyncHandler.cpp
    main.cpp
)

target_include_directories(ecatd PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(ecatd PRIVATE Qt6::Core Qt6::Network ecat_core ecat_igh ethercat)
