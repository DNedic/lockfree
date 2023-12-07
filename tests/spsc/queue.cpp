#include <algorithm>
#include <math.h>
#include <thread>

#include <catch2/catch_test_macros.hpp>

#include "lockfree.hpp"

TEST_CASE("Write to empty and read back", "[q_write_empty]") {
    lockfree::spsc::Queue<int16_t, 20> queue;

    bool const push_success = queue.Push(-1024);
    REQUIRE(push_success);

    int16_t read = 0;
    bool const pop_success = queue.Pop(read);
    REQUIRE(read == -1024);
    REQUIRE(pop_success);
}

TEST_CASE("Read empty", "[q_read_empty]") {
    lockfree::spsc::Queue<uint8_t, 20> queue;

    uint8_t read = 0;
    bool const pop_success = queue.Pop(read);
    REQUIRE(!pop_success);
}

TEST_CASE("Write full", "[q_write_full]") {
    lockfree::spsc::Queue<uint8_t, 5> queue;

    bool push_success = queue.Push(1U);
    push_success = queue.Push(1U);
    push_success = queue.Push(2U);
    push_success = queue.Push(3U);
    push_success = queue.Push(5U);
    REQUIRE(!push_success);
}

TEST_CASE("Write multiple to empty and read back", "[q_write_empty_multiple]") {
    lockfree::spsc::Queue<float, 20> queue;

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

TEST_CASE("Write with overflow and read back from start",
          "[q_write_overflow]") {
    lockfree::spsc::Queue<int32_t, 4> queue;

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

TEST_CASE("Optional API", "[q_optional_api]") {
    lockfree::spsc::Queue<uint64_t, 20> queue;

    REQUIRE(!queue.PopOptional());
    queue.Push(-1024);

    REQUIRE(queue.PopOptional() == -1024);
}

TEST_CASE("Multithreaded read/write", "[q_multithread]") {
    std::vector<std::thread> threads;
    lockfree::spsc::Queue<uint64_t, 1024U> queue;
    std::vector<uint64_t> written;
    std::vector<uint64_t> read;

    // consumer
    threads.emplace_back([&]() {
        uint64_t element = 0;
        do {
            bool read_success = queue.Pop(element);
            if (read_success) {
                read.push_back(element);
            }
        } while (element < TEST_MT_TRANSFER_CNT);
    });

    // producer
    threads.emplace_back([&]() {
        uint64_t element = 0;
        do {
            bool push_success = queue.Push(element);
            if (push_success) {
                written.push_back(element);
                element++;
            }
        } while (element < TEST_MT_TRANSFER_CNT + 1);
    });

    for (auto &t : threads) {
        t.join();
    }

    REQUIRE(
        std::equal(std::begin(written), std::end(written), std::begin(read)));
}
