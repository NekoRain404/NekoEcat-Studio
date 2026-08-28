# Standalone library target for the pure-C nekoecat client.
# This is not wired into the top-level build (the daemon consumes the same
# sources directly); it exists so the client can be built/consumed on its own.
cmake_minimum_required(VERSION 3.20)
project(nekoecat_client LANGUAGES C)

add_library(nekoecat_client STATIC
    nekoecat_client.c
    nekoecat_client.h
    shm_layout.h
    ../src/core/nekoecat_shm.h
)
target_include_directories(nekoecat_client PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core
)
target_link_libraries(nekoecat_client PRIVATE pthread)
