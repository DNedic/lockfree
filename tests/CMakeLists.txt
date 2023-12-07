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

if (NOT DEFINED TEST_MT_TRANSFER_CNT)
    set(TEST_MT_TRANSFER_CNT 10240)
endif()

target_compile_definitions(tests PRIVATE TEST_MT_TRANSFER_CNT=${TEST_MT_TRANSFER_CNT})

# Required in order to test the std::span API as well
target_compile_features(tests PRIVATE cxx_std_20)

target_compile_options(tests
PRIVATE
    -fno-omit-frame-pointer # For nicer stack traces with sanitizers
    -fsanitize=address
    -fsanitize=signed-integer-overflow
    -fsanitize=bounds
)

target_link_options(tests
PRIVATE
    -fsanitize=address
    -fsanitize=signed-integer-overflow
    -fsanitize=bounds
)

target_link_libraries(tests
PRIVATE
    ${PROJECT_NAME}
    Catch2::Catch2WithMain
)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(tests)
