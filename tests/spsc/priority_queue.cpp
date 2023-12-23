#include <algorithm>
#include <math.h>
#include <thread>

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

TEST_CASE("Multithreaded read/write", "[pq_multithreaded]") {
    lockfree::spsc::PriorityQueue<uint64_t, 10, 4> queue;
    std::vector<std::thread> threads;
    std::vector<uint64_t> written;
    std::vector<uint64_t> read;

    // consumer
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

    // producer
    threads.emplace_back([&]() {
        uint64_t cnt = 0;
        uint64_t value = 0;
        uint8_t prio = 0;
        do {
          value = cnt << 2 + prio;
          bool push_success = queue.Push(value, prio);
          if (push_success) {
            written.push_back(value);
            prio = (prio + 1) % 4;
            cnt++;
          }
        } while (cnt < TEST_MT_TRANSFER_CNT + 1);
    });
    for (auto &t : threads) {
        t.join();
    }
    // check at all times no higher value is present in the `written` vector
    std::vector<bool> consumed(written.size(), false);    
    uint64_t value1, value2;
    uint8_t prio1, prio2;
    size_t start = 0;
    bool all_consumed;
    for(size_t idx=0; idx<read.size(); idx++) {
      // the value was read
      value1 = read[idx];
      prio1 = value1 & ((1<<2) - 1);

      all_consumed = true;
      for(size_t idx2=start; idx2<written.size(); idx2++) {
        if(consumed[idx2]) {
          if(all_consumed){
            start++;
          }
          continue;
        }
        all_consumed = false;
        // find when the value was written
        if(written[idx2] == value1) {
            consumed[idx2] = true;
            for(size_t idx3=start; idx3 < idx2; idx3++) {
              if(consumed[idx3]) {
                continue;
              }
              value2 = written[idx3];
              prio2 = value2 & ((1<<2) - 1);
              REQUIRE(prio2 <= prio1);
            }
            break;
        }
      }
    }
}

TEST_CASE("Optional API", "[pq_optional_api]") {
    lockfree::spsc::PriorityQueue<int16_t, 20, 3> queue;

    bool const push_success = queue.Push(-1024, 0);
    REQUIRE(push_success);

    auto const read = queue.PopOptional();
    REQUIRE(read == -1024);
}
