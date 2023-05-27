#include <algorithm>
#include <math.h>

#include <catch2/catch_test_macros.hpp>

#include "lockfree.hpp"

TEST_CASE("Write to empty, lowest priority and read back",
          "[pq_write_empty_lowest]") {
    lockfree::spsc::PriorityQueue<int16_t, 20, 3> queue;

    bool const push_success = queue.Push(-1024, 0);
    REQUIRE(push_success);

    int16_t read = 0;
    bool const pop_success = queue.Pop(read);
    REQUIRE(pop_success);
    REQUIRE(read == -1024);
}

TEST_CASE("Write to empty, highest priority and read back",
          "[pq_write_empty_highest]") {
    lockfree::spsc::PriorityQueue<int16_t, 20, 3> queue;

    bool const push_success = queue.Push(-1024, 2);
    REQUIRE(push_success);

    int16_t read = 0;
    bool const pop_success = queue.Pop(read);
    REQUIRE(pop_success);
    REQUIRE(read == -1024);
}

TEST_CASE("Write multiple with different priority and read back ensuring "
          "proper sequence",
          "[pq_write_multiple_read_multiple]") {
    lockfree::spsc::PriorityQueue<uint64_t, 10, 4> queue;

    bool push_success = queue.Push(256, 2);
    REQUIRE(push_success);

    push_success = queue.Push(1024, 0);
    REQUIRE(push_success);

    push_success = queue.Push(128, 1);
    REQUIRE(push_success);

    push_success = queue.Push(512, 3);
    REQUIRE(push_success);

    uint64_t read = 0;
    bool pop_success = queue.Pop(read);
    REQUIRE(pop_success);
    REQUIRE(read == 512);

    pop_success = queue.Pop(read);
    REQUIRE(pop_success);
    REQUIRE(read == 256);

    pop_success = queue.Pop(read);
    REQUIRE(pop_success);
    REQUIRE(read == 128);

    pop_success = queue.Pop(read);
    REQUIRE(pop_success);
    REQUIRE(read == 1024);
}

TEST_CASE("Optional API", "[pq_optional_api]") {
    lockfree::spsc::PriorityQueue<int16_t, 20, 3> queue;

    bool const push_success = queue.Push(-1024, 0);
    REQUIRE(push_success);

    auto const read = queue.PopOptional();
    REQUIRE(read == -1024);
}
