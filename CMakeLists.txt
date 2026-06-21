cmake_minimum_required(VERSION 3.20)

project(NekoEcatStudio VERSION 3.8.0 LANGUAGES CXX)

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

# Shared libraries
add_subdirectory(src/core)    # EthercatTypes, JsonProtocol
add_subdirectory(src/igh)     # IgH ethercat CLI adapter

# Applications
add_subdirectory(apps/ecatd)       # Runtime daemon
add_subdirectory(apps/ecat-studio) # Qt6 desktop GUI

# Tests
enable_testing()
add_subdirectory(tests)
