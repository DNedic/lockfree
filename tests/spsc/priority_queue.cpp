#include <algorithm>
#include <math.h>
#include <thread>

#include <catch2/catch_test_macros.hpp>

#include "lockfree.hpp"

TEST_CASE("spsc::PriorityQueue - Write to empty, lowest priority and read back",
          "[pq_write_empty_lowest]") {
    lockfree::spsc::PriorityQueue<int16_t, 20, 3> queue;

    bool const push_success = queue.Push(-1024, 0);
    REQUIRE(push_success);

    int16_t read = 0;
    bool const pop_success = queue.Pop(read);
    REQUIRE(pop_success);
    REQUIRE(read == -1024);
}

TEST_CASE(
    "spsc::PriorityQueue - Write to empty, highest priority and read back",
    "[pq_write_empty_highest]") {
    lockfree::spsc::PriorityQueue<int16_t, 20, 3> queue;

    bool const push_success = queue.Push(-1024, 2);
    REQUIRE(push_success);

    int16_t read = 0;
    bool const pop_success = queue.Pop(read);
    REQUIRE(pop_success);
    REQUIRE(read == -1024);
}

TEST_CASE("spsc::PriorityQueue - Write multiple with different priority and "
          "read back ensuring "
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

TEST_CASE("spsc::PriorityQueue - Multithreaded read/write",
          "[pq_multithreaded]") {
    lockfree::spsc::PriorityQueue<uint64_t, 10, 4> queue;
    std::vector<std::thread> threads;
    std::vector<uint64_t> written;
    std::vector<uint64_t> read;

    // consumer, it just pops values from the queue and stores them in the
    // main thread vector
    threads.emplace_back([&]() {
        uint64_t value = 0;
        uint64_t cnt = 0;
        do {
            bool pop_success = queue.Pop(value);
            if (pop_success) {
                read.push_back(value);
                cnt++;
            }
        } while (cnt < TEST_MT_TRANSFER_CNT);
    });

    // producer, uses alternative priorities and pushes a counter shifted to
    // accommodate the priority on its lower bits.
    threads.emplace_back([&]() {
        uint64_t cnt = 0;
        uint64_t value = 0;
        uint8_t prio = 0;
        do {
            value = cnt << 2 + prio;
            bool push_success = queue.Push(value, prio);
            if (push_success) {
                written.push_back(value);
                prio = (prio + 1) % 4; // this could be also randomly generated
                cnt++;
            }
        } while (cnt < TEST_MT_TRANSFER_CNT + 1);
    });

    for (auto &t : threads) {
        t.join();
    }
    /* The following code checks that at all times no higher priority value was
       present in the `written` vector.

       It needs to keep track which values were already read, it does that with
       the help of the `consumed` Boolean vector.
    */
    std::vector<bool> consumed(written.size(), false);

    for (size_t read_idx = 0; read_idx < read.size(); read_idx++) {
        const uint64_t read_value = read[read_idx];
        const uint64_t read_priority = read_value & ((1 << 2) - 1);

        bool found_value = false;
        for (size_t write_idx = 0; write_idx < written.size(); write_idx++) {
            if (consumed[write_idx]) { // consumed values are skipped
                continue;
            }
            // find when the value was written
            const uint64_t written_value = written[write_idx];
            const uint64_t written_priority = written_value & ((1 << 2) - 1);
            if (written[write_idx] == read_value) {
                consumed[write_idx] = true; // this value is now accounted for
                found_value = true;
                break;
            } else { // intermediate value, should be lower priority
                REQUIRE(written_priority <= read_priority);
            }
        }
        REQUIRE(found_value);
    }
}

TEST_CASE("spsc::PriorityQueue - Optional API", "[pq_optional_api]") {
    lockfree::spsc::PriorityQueue<int16_t, 20, 3> queue;

    bool const push_success = queue.Push(-1024, 0);
    REQUIRE(push_success);

    auto const read = queue.PopOptional();
    REQUIRE(read == -1024);
}
