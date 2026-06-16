add_executable(ecatd
    EcatDaemon.cpp
    EcatDaemon.h
    FreeRunController.cpp
    RtTestController.cpp
    RtTestController.h
    FreeRunController.h
    main.cpp
)

target_link_libraries(ecatd PRIVATE Qt6::Core Qt6::Network ecat_core ecat_igh ethercat)
