cmake_minimum_required(VERSION 3.20)

project(NekoEcatStudio VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network Widgets)

add_subdirectory(src/core)
add_subdirectory(src/igh)
add_subdirectory(apps/ecatd)
add_subdirectory(apps/ecat-studio)

enable_testing()
add_subdirectory(tests)
