add_library(ecat_core STATIC
    EthercatTypes.cpp
    EthercatTypes.h
    JsonProtocol.cpp
    JsonProtocol.h
)

target_link_libraries(ecat_core PUBLIC Qt6::Core)
target_include_directories(ecat_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

