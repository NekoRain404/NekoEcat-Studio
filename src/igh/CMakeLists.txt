add_library(ecat_igh STATIC
    EthercatCliBackend.cpp
    EthercatCliBackend.h
)

target_link_libraries(ecat_igh PUBLIC Qt6::Core ecat_core)
target_include_directories(ecat_igh PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

