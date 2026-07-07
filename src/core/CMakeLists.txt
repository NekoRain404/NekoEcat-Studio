add_library(ecat_core STATIC
    EthercatTypes.cpp
    EthercatTypes.h
    JsonProtocol.cpp
    JsonProtocol.h
    EniGenerator.cpp
    EniGenerator.h
)

target_link_libraries(ecat_core PUBLIC Qt6::Core open62541)
target_include_directories(ecat_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${open62541_BINARY_DIR}
)

