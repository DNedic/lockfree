# Project CMakeLists
# Djordje Nedic 2023

cmake_minimum_required(VERSION 3.16)

project(lockfree
    VERSION 2.0.0
    LANGUAGES CXX
)

add_subdirectory(${PROJECT_NAME})

# Only build tests if we're actually working on the library,
# not when the library is being used in a project
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    add_subdirectory(tests)
endif()
