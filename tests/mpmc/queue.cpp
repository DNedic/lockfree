#include <algorithm>
#include <math.h>

#include <catch2/catch_test_macros.hpp>

#include "lockfree.hpp"

TEST_CASE("mpmc::Queue - Write to empty and read back",
          "[mpmc_q_write_empty]") {
    lockfree::mpmc::Queue<int16_t, 20> queue;

    bool const push_success = queue.Push(-1024);
    REQUIRE(push_success);

    int16_t read = 0;
    bool const pop_success = queue.Pop(read);
    REQUIRE(read == -1024);
    REQUIRE(pop_success);
}

TEST_CASE("mpmc::Queue - Read empty", "[mpmc_q_read_empty]") {
    lockfree::mpmc::Queue<uint8_t, 20> queue;

    uint8_t read = 0;
    bool const pop_success = queue.Pop(read);
    REQUIRE(!pop_success);
}

TEST_CASE("mpmc::Queue - Write full", "[mpmc_q_write_full]") {
    lockfree::mpmc::Queue<uint8_t, 5> queue;

    bool push_success = queue.Push(1U);
    push_success = queue.Push(1U);
    push_success = queue.Push(2U);
    push_success = queue.Push(3U);
    push_success = queue.Push(5U);
    push_success = queue.Push(6U);
    REQUIRE(!push_success);
}

TEST_CASE("mpmc::Queue - Write multiple to empty and read back",
          "[mpmc_q_write_empty_multiple]") {
    lockfree::mpmc::Queue<float, 20> queue;

    bool push_success = queue.Push(2.7183F);
    REQUIRE(push_success);

    push_success = queue.Push(-1.0F);
    REQUIRE(push_success);

    push_success = queue.Push(3.1416F);
    REQUIRE(push_success);

    push_success = queue.Push(10.0F);
    REQUIRE(push_success);

    float read = NAN;
    bool const pop_success = queue.Pop(read);
    REQUIRE(read == 2.7183F);
    REQUIRE(pop_success);
}

TEST_CASE("mpmc::Queue - Write with overflow and read back from start",
          "[mpmc_q_write_overflow]") {
    lockfree::mpmc::Queue<int32_t, 4> queue;

    bool push_success = queue.Push(-1024);
    push_success = queue.Push(111);
    push_success = queue.Push(654);

    int32_t read = 0;
    bool pop_success = queue.Pop(read);
    pop_success = queue.Pop(read);
    pop_success = queue.Pop(read);

    push_success = queue.Push(5);
    push_success = queue.Push(1000);
    REQUIRE(push_success);

    pop_success = queue.Pop(read);
    pop_success = queue.Pop(read);
    REQUIRE(read == 1000);
}

TEST_CASE("mpmc::Queue - Optional API", "[mpmc_q_optional_api]") {
    lockfree::mpmc::Queue<uint64_t, 20> queue;

    REQUIRE(!queue.PopOptional());
    queue.Push(-1024);

    REQUIRE(queue.PopOptional() == -1024);
}
