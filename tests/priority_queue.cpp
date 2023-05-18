#include <algorithm>
#include <math.h>

#include <catch2/catch_test_macros.hpp>

#include "lockfree.hpp"

TEST_CASE("Write to empty, lowest priority and read back highest priority",
          "[pq_write_empty_lowest_read_highest]") {
    lockfree::PriorityQueue<int16_t, 20, 3> queue;

    bool const push_success = queue.Push(-1024, 0);
    REQUIRE(push_success);

    int16_t read = 0;
    bool const pop_success = queue.PopHighest(read);
    REQUIRE(pop_success);
    REQUIRE(read == -1024);
}

TEST_CASE("Write to empty, highest priority and read back highest priority",
          "[pq_write_empty_highest_read_highest]") {
    lockfree::PriorityQueue<int16_t, 20, 3> queue;

    bool const push_success = queue.Push(-1024, 2);
    REQUIRE(push_success);

    int16_t read = 0;
    bool const pop_success = queue.PopHighest(read);
    REQUIRE(pop_success);
    REQUIRE(read == -1024);
}

TEST_CASE("Write to empty and read back lowest priority",
          "[pq_write_empty_lowest_read_lowest]") {
    lockfree::PriorityQueue<int16_t, 20, 3> queue;

    bool const push_success = queue.Push(256, 0);
    REQUIRE(push_success);

    int16_t read = 0;
    bool const pop_success = queue.PopLowest(read);
    REQUIRE(pop_success);
    REQUIRE(read == 256);
}

TEST_CASE("Write to empty and read back lowest priority",
          "[pq_write_empty_highest_read_lowest]") {
    lockfree::PriorityQueue<int16_t, 20, 3> queue;

    bool const push_success = queue.Push(256, 2);
    REQUIRE(push_success);

    int16_t read = 0;
    bool const pop_success = queue.PopLowest(read);
    REQUIRE(pop_success);
    REQUIRE(read == 256);
}

TEST_CASE("Write multiple with different priority and read back highest "
          "priority ensuring proper sequence",
          "[pq_write_multiple_read_multiple_highest]") {
    lockfree::PriorityQueue<uint64_t, 10, 4> queue;

    bool push_success = queue.Push(256, 2);
    REQUIRE(push_success);

    push_success = queue.Push(1024, 0);
    REQUIRE(push_success);

    push_success = queue.Push(128, 1);
    REQUIRE(push_success);

    push_success = queue.Push(512, 3);
    REQUIRE(push_success);

    uint64_t read = 0;
    bool pop_success = queue.PopHighest(read);
    REQUIRE(pop_success);
    REQUIRE(read == 512);

    pop_success = queue.PopHighest(read);
    REQUIRE(pop_success);
    REQUIRE(read == 256);

    pop_success = queue.PopHighest(read);
    REQUIRE(pop_success);
    REQUIRE(read == 128);

    pop_success = queue.PopHighest(read);
    REQUIRE(pop_success);
    REQUIRE(read == 1024);
}

TEST_CASE("Write multiple with different priority and read back lowest "
          "priority ensuring proper sequence",
          "[pq_write_multiple_read_multiple_lowest]") {
    lockfree::PriorityQueue<uint64_t, 10, 4> queue;

    bool push_success = queue.Push(256, 2);
    REQUIRE(push_success);

    push_success = queue.Push(1024, 0);
    REQUIRE(push_success);

    push_success = queue.Push(128, 1);
    REQUIRE(push_success);

    push_success = queue.Push(512, 3);
    REQUIRE(push_success);

    uint64_t read = 0;
    bool pop_success = queue.PopLowest(read);
    REQUIRE(pop_success);
    REQUIRE(read == 1024);

    pop_success = queue.PopLowest(read);
    REQUIRE(pop_success);
    REQUIRE(read == 128);

    pop_success = queue.PopLowest(read);
    REQUIRE(pop_success);
    REQUIRE(read == 256);

    pop_success = queue.PopLowest(read);
    REQUIRE(pop_success);
    REQUIRE(read == 512);
}

TEST_CASE("Write multiple with different priority and read back interleaved "
          "priorities ensuring proper sequence",
          "[pq_write_multiple_read_multiple_interleaved]") {
    lockfree::PriorityQueue<uint64_t, 10, 4> queue;

    bool push_success = queue.Push(256, 2);
    REQUIRE(push_success);

    push_success = queue.Push(1024, 0);
    REQUIRE(push_success);

    push_success = queue.Push(128, 1);
    REQUIRE(push_success);

    push_success = queue.Push(512, 3);
    REQUIRE(push_success);

    uint64_t read = 0;
    bool pop_success = queue.PopLowest(read);
    REQUIRE(pop_success);
    REQUIRE(read == 1024);

    pop_success = queue.PopHighest(read);
    REQUIRE(pop_success);
    REQUIRE(read == 512);

    pop_success = queue.PopHighest(read);
    REQUIRE(pop_success);
    REQUIRE(read == 256);

    pop_success = queue.PopLowest(read);
    REQUIRE(pop_success);
    REQUIRE(read == 128);
}

TEST_CASE("Optional API for PopHighest", "[pq_optional_api_highest]") {
    lockfree::PriorityQueue<int16_t, 20, 3> queue;

    bool const push_success = queue.Push(-1024, 0);
    REQUIRE(push_success);

    auto const read = queue.PopHighestOptional();
    REQUIRE(read == -1024);
}

TEST_CASE("Optional API for PopLowest", "[pq_optional_api_highest]") {
    lockfree::PriorityQueue<int16_t, 20, 3> queue;

    bool const push_success = queue.Push(-1024, 0);
    REQUIRE(push_success);

    auto const read = queue.PopLowestOptional();
    REQUIRE(read == -1024);
}
