cmake_minimum_required(VERSION 3.20)

project(NekoEcatStudio VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# ── Parallel Compilation ─────────────────────────────────────────────
include(ProcessorCount)
ProcessorCount(NPROC)
if(NOT NPROC EQUAL 0)
    set(CMAKE_BUILD_PARALLEL_LEVEL ${NPROC})
endif()

# ── Build Caching ────────────────────────────────────────────────────
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

# ── Default Build Type ──────────────────────────────────────────────
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Network Widgets Test LinguistTools)
find_package(Qt6 COMPONENTS Qml QUIET)

# ── open62541 (OPC UA) ───────────────────────────────────────────────
# Currently unused — no code includes open62541 headers.
# Uncomment the block below when OPC UA client/server support is implemented.
# include(FetchContent)
# FetchContent_Declare(
#     open62541
#     GIT_REPOSITORY https://github.com/open62541/open62541.git
#     GIT_TAG        v1.4.10
#     GIT_SHALLOW    TRUE
# )
# set(UA_ENABLE_AMALGAMATION OFF CACHE BOOL "" FORCE)
# set(UA_ENABLE_METHODCALLS ON CACHE BOOL "" FORCE)
# set(UA_ENABLE_SUBSCRIPTIONS ON CACHE BOOL "" FORCE)
# set(UA_ENABLE_SUBSCRIPTIONS_EVENTS ON CACHE BOOL "" FORCE)
# set(UA_ENABLE_HISTORIZING OFF CACHE BOOL "" FORCE)
# set(UA_ENABLE_ENCRYPTION OFF CACHE BOOL "" FORCE)
# set(UA_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
# set(UA_BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
# set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
# FetchContent_MakeAvailable(open62541)

# Shared libraries
add_subdirectory(src/core)    # EthercatTypes, JsonProtocol
add_subdirectory(src/igh)     # IgH ethercat CLI adapter

# Applications
add_subdirectory(apps/ecatd)       # Runtime daemon
add_subdirectory(apps/ecat-studio) # Qt6 desktop GUI

# Tests
enable_testing()
add_subdirectory(tests)

# ── Packaging ─────────────────────────────────────────────────────────
set(CPACK_PACKAGE_NAME "NekoEcatStudio")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "EtherCAT commissioning and diagnostics workstation")
set(CPACK_PACKAGE_VENDOR "NekoRain")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/NekoRain/NekoEcatStudio")
set(CPACK_PACKAGE_LICENSE "GPL-3.0")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_CONTACT "NekoRain <nekorain@example.com>")

# Linux archive generator (works out of the box, no extra tools)
set(CPACK_GENERATOR "TGZ;DEB;RPM")
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY ON)
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-linux-x64")

# DEB package settings
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6core6 (>= 6.5), libqt6network6, libqt6widgets6, libethercat")
set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}_amd64.deb")

# RPM package settings
set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
set(CPACK_RPM_PACKAGE_REQUIRES "qt6-core >= 6.5, qt6-network, qt6-widgets, ethercat")
set(CPACK_RPM_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}.x86_64.rpm")

# Install rules
install(TARGETS ecatd ecat-studio DESTINATION bin)
install(DIRECTORY translations/ DESTINATION share/nekoecat/translations)
install(FILES README.md LICENSE CHANGELOG.md ARCHITECTURE.md
        DESTINATION share/doc/nekoecat)

set(CPACK_STRIP_FILES TRUE)

include(CPack)
