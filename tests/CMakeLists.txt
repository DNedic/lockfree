# Tests CMakeLists
# Djordje Nedic 2023

cmake_minimum_required(VERSION 3.16)

Include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.3.2
)

FetchContent_MakeAvailable(Catch2)

add_executable(tests
    spsc/queue.cpp
    spsc/ring_buf.cpp
    spsc/bipartite_buf.cpp
    spsc/priority_queue.cpp
    mpmc/queue.cpp
    mpmc/priority_queue.cpp
)

# Required in order to test the std::span API as well
target_compile_features(tests PRIVATE cxx_std_20)

target_link_libraries(tests
PRIVATE
    ${PROJECT_NAME}
    Catch2::Catch2WithMain
)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(tests)
