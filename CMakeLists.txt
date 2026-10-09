# Project CMakeLists
# Djordje Nedic 2023

cmake_minimum_required(VERSION 3.16)

project(lockfree
    VERSION 3.0.1
    LANGUAGES CXX
)

add_subdirectory(${PROJECT_NAME})

# Library configuration
option(LOCKFREE_CACHE_COHERENT
    "Pad data structures to eliminate false sharing" ON)
set(LOCKFREE_CACHELINE_LENGTH 64 CACHE STRING
    "Cacheline length in bytes used for padding when LOCKFREE_CACHE_COHERENT is on")
target_compile_definitions(${PROJECT_NAME}
INTERFACE
    LOCKFREE_CACHE_COHERENT=$<BOOL:${LOCKFREE_CACHE_COHERENT}>
    LOCKFREE_CACHELINE_LENGTH=${LOCKFREE_CACHELINE_LENGTH}
)

# Only build tests if we're actually working on the library,
# not when the library is being used in a project
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    enable_testing()
    add_subdirectory(tests)
endif()
