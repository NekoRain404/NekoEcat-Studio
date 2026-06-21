add_library(ecat_igh STATIC
    EthercatCliBackend.cpp
    EthercatCliBackend.h
    EthercatNativeBackend.cpp
    EthercatNativeBackend.h
)

target_link_libraries(ecat_igh PUBLIC Qt6::Core ecat_core)
target_include_directories(ecat_igh PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Check for IgH EtherCAT Master headers
include(CheckIncludeFileCXX)
check_include_file_cxx(ecrt.h HAVE_IGH)
if(HAVE_IGH)
    target_compile_definitions(ecat_igh PUBLIC HAVE_IGH=1)
endif()

