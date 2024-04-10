#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <thread>

#include "lockfree.hpp"

TEST_CASE("spsc::BipartiteBuf - Write to the beginning",
          "[bb_write_beginning]") {
    lockfree::spsc::BipartiteBuf<uint8_t, 512U> bb;
    const uint8_t test_data[320] = {0xE5U};

    auto *write_location =
        bb.WriteAcquire(sizeof(test_data) / sizeof(test_data[0]));
    REQUIRE(write_location != nullptr);

    std::copy(std::begin(test_data), std::end(test_data), write_location);

    bb.WriteRelease(sizeof(test_data) / sizeof(test_data[0]));

    auto read = bb.ReadAcquire();

    REQUIRE(read.first != nullptr);
    REQUIRE(read.second == sizeof(test_data));
    REQUIRE(std::equal(std::begin(test_data), std::end(test_data), read.first));
}

TEST_CASE("spsc::BipartiteBuf - Try to acquire too much data",
          "[bb_acquire_too_much]") {
    lockfree::spsc::BipartiteBuf<uint8_t, 512U> bb;
    auto *write_location = bb.WriteAcquire(512U + rand());
    REQUIRE(write_location == nullptr);
}

TEST_CASE("spsc::BipartiteBuf - Try to acquire read with an empty buffer",
          "[bb_read_empty]") {
    lockfree::spsc::BipartiteBuf<uint8_t, 512U> bb;

    auto read = bb.ReadAcquire();
    REQUIRE(read.first == nullptr);
    REQUIRE(read.second == 0);
}

TEST_CASE("spsc::BipartiteBuf - Write with overflow condition",
          "[bb_write_overflow]") {
    lockfree::spsc::BipartiteBuf<uint32_t, 512U> bb;
    const uint32_t test_data[320] = {0xE5A1D2C3U};

    /* Write to the start and read the data back */
    auto *write_location =
        bb.WriteAcquire(sizeof(test_data) / sizeof(test_data[0]));
    std::copy(std::begin(test_data), std::end(test_data), write_location);
    bb.WriteRelease(sizeof(test_data) / sizeof(test_data[0]));

    auto read = bb.ReadAcquire();
    bb.ReadRelease(sizeof(test_data) / sizeof(test_data[0]));

    /* Write again, this time the oveflow will trigger and should give us the
     * beginning again */
    const uint32_t test_data2[240] = {0xA3B2C1D0U};
    write_location =
        bb.WriteAcquire(sizeof(test_data2) / sizeof(test_data2[0]));
    REQUIRE(write_location != nullptr);
    std::copy(std::begin(test_data2), std::end(test_data2), write_location);
    bb.WriteRelease(sizeof(test_data2) / sizeof(test_data[0]));

    read = bb.ReadAcquire();
    REQUIRE(read.first != nullptr);
    REQUIRE(read.second == sizeof(test_data2) / sizeof(test_data2[0]));
    REQUIRE(
        std::equal(std::begin(test_data2), std::end(test_data2), read.first));
}

TEST_CASE(
    "spsc::BipartiteBuf - Read data written after overflow condition write",
    "[bb_read_after_overflow_write]") {
    lockfree::spsc::BipartiteBuf<int16_t, 512U> bb;
    const int16_t test_data[320] = {-222};

    /* Write to the start and read the data back */
    auto *write_location =
        bb.WriteAcquire(sizeof(test_data) / sizeof(test_data[0]));
    std::copy(std::begin(test_data), std::end(test_data), write_location);
    bb.WriteRelease(sizeof(test_data) / sizeof(test_data[0]));

    auto read = bb.ReadAcquire();
    bb.ReadRelease(sizeof(test_data) / sizeof(test_data[0]));

    /* Write again, this time the oveflow will trigger and should give us the
     * beginning again */
    const int16_t test_data2[240] = {-66};
    write_location =
        bb.WriteAcquire(sizeof(test_data2) / sizeof(test_data2[0]));
    std::copy(std::begin(test_data2), std::end(test_data2), write_location);
    bb.WriteRelease(sizeof(test_data2) / sizeof(test_data2[0]));

    read = bb.ReadAcquire();
    bb.ReadRelease(sizeof(test_data2) / sizeof(test_data2[0]));

    /* Write again, without overflow and read back */
    const uint8_t test_data3[120] = {0xBCU};
    write_location =
        bb.WriteAcquire(sizeof(test_data3) / sizeof(test_data3[0]));
    std::copy(std::begin(test_data3), std::end(test_data3), write_location);
    bb.WriteRelease(sizeof(test_data3) / sizeof(test_data3[0]));

    read = bb.ReadAcquire();
    REQUIRE(read.first != nullptr);
    REQUIRE(read.second == sizeof(test_data3));
    REQUIRE(
        std::equal(std::begin(test_data3), std::end(test_data3), read.first));
}

TEST_CASE("spsc::BipartiteBuf - Interleaved write and read with enough space",
          "[bb_interleaved_success]") {
    lockfree::spsc::BipartiteBuf<double, 512U> bb;
    const double test_data[320] = {42.4242};

    /* 1. Complete write */
    auto *write_location =
        bb.WriteAcquire(sizeof(test_data) / sizeof(test_data[0]));
    std::copy(std::begin(test_data), std::end(test_data), write_location);
    bb.WriteRelease(sizeof(test_data) / sizeof(test_data[0]));

    /* 2. Read acquire, the linear space previously written is reserved for
     * reading now */
    auto read = bb.ReadAcquire();

    /* 3. Write acquire, a linear space after the read linear space is reserved
     * for writing and is copied to*/
    const double test_data2[120] = {-123.123};
    write_location =
        bb.WriteAcquire(sizeof(test_data2) / sizeof(test_data2[0]));
    REQUIRE(write_location != nullptr);
    std::copy(std::begin(test_data2), std::end(test_data2), write_location);

    /* 4. Compare the data */
    REQUIRE(std::equal(std::begin(test_data), std::end(test_data), read.first));
}

TEST_CASE("spsc::BipartiteBuf - Interleaved write and read with enough space 2",
          "[bb_interleaved_success2]") {
    lockfree::spsc::BipartiteBuf<char, 512U> bb;
    const char test_data[320] = {'A'};

    /* 1. Complete write */
    auto *write_location =
        bb.WriteAcquire(sizeof(test_data) / sizeof(test_data[0]));
    std::copy(std::begin(test_data), std::end(test_data), write_location);
    bb.WriteRelease(sizeof(test_data) / sizeof(test_data[0]));

    /* 2. Write acquire, a linear space after the read linear space is reserved
     * for writing and is copied to*/
    const char test_data2[120] = {'b'};
    write_location = bb.WriteAcquire(sizeof(test_data2) / sizeof(test_data2));
    std::copy(std::begin(test_data2), std::end(test_data2), write_location);

    /* 3. Read acquire, the linear space previously written is reserved for
     * reading now */
    auto read = bb.ReadAcquire();
    REQUIRE(read.first != nullptr);
    REQUIRE(std::equal(std::begin(test_data), std::end(test_data), read.first));
}

TEST_CASE(
    "spsc::BipartiteBuf - Interleaved write and read without enough space",
    "[bb_interleaved_fail]") {
    lockfree::spsc::BipartiteBuf<uint8_t, 512U> bb;
    const uint8_t test_data[320] = {0xE5U};

    /* 1. Complete write */
    auto *write_location =
        bb.WriteAcquire(sizeof(test_data) / sizeof(test_data[0]));
    std::copy(std::begin(test_data), std::end(test_data), write_location);
    bb.WriteRelease(sizeof(test_data) / sizeof(test_data[0]));

    /* 2. Read acquire, the linear space previously written is reserved for
     * reading now */
    auto read = bb.ReadAcquire();

    /* 3. Write acquire, attempt to acquire more linear space than available */
    const uint8_t test_data2[240] = {0xA3U};
    write_location =
        bb.WriteAcquire(sizeof(test_data2) / sizeof(test_data2[0]));
    REQUIRE(write_location == nullptr);
}

TEST_CASE("spsc::BipartiteBuf - Test keeping the chunk of data when write ends "
          "exactly in the end "
          "of the buffer.",
          "[bb_exact_end_write_release_proper_invalidation_test]") {
    constexpr size_t size = 8;
    lockfree::spsc::BipartiteBuf<uint8_t, size * 2> bb;

    // First half, no overflow.
    auto *base = bb.WriteAcquire(size);
    bb.WriteRelease(size);
    bb.ReadAcquire();
    bb.ReadRelease(size);

    // Second half, write overflow.
    auto *write_buf_second_half = bb.WriteAcquire(size);
    bb.WriteRelease(size);

    // Second half, read overflow.
    auto *read_buf_second_half = bb.ReadAcquire().first;
    REQUIRE((read_buf_second_half - base) == (write_buf_second_half - base));
}

TEST_CASE("spsc::BipartiteBuf - std::span API test", "[bb_std_span_api]") {
    lockfree::spsc::BipartiteBuf<double, 512U> bb;

    auto *write_ptr = bb.WriteAcquire(320);
    bb.WriteRelease(0);

    auto write_span = bb.WriteAcquireSpan(320);

    REQUIRE(write_ptr == write_span.data());

    bb.WriteRelease(write_span);

    auto read_pair = bb.ReadAcquire();
    bb.ReadRelease(0);

    auto read_span = bb.ReadAcquireSpan();

    REQUIRE(read_pair.first == read_span.data());

    bb.ReadRelease(read_span);

    write_ptr = bb.WriteAcquire(120);
    bb.WriteRelease(0);

    write_span = bb.WriteAcquireSpan(120);

    REQUIRE(write_ptr == write_span.data());

    bb.WriteRelease(write_span);

    read_pair = bb.ReadAcquire();
    bb.ReadRelease(0);
    read_span = bb.ReadAcquireSpan();

    REQUIRE(read_pair.first == read_span.data());
}

TEST_CASE("spsc::BipartiteBuf - Multithreaded read/write multiple",
          "[bb_multithread_multiple]") {
    std::vector<std::thread> threads;
    lockfree::spsc::BipartiteBuf<unsigned int, 1024U> bb;
    std::vector<unsigned int> written;
    std::vector<unsigned int> read;

    const size_t data_size = 59; // Intentionally a prime number

    // consumer
    threads.emplace_back([&]() {
        size_t read_count = 0;
        do {
            std::pair<unsigned int *, size_t> read_region = bb.ReadAcquire();
            if (read_region.second) {
                read.insert(read.end(), &read_region.first[0],
                            &read_region.first[read_region.second]);
                bb.ReadRelease(read_region.second);
                read_count += read_region.second;
            }
        } while (read_count < TEST_MT_TRANSFER_CNT);
    });

    // producer
    threads.emplace_back([&]() {
        unsigned int data[data_size] = {0};
        for (unsigned int i = 0; i < data_size; i++) {
            data[i] = rand();
        }

        size_t write_count = 0;
        do {
            unsigned int *write_region = bb.WriteAcquire(data_size);
            if (write_region != nullptr) {
                std::copy(&data[0], &data[data_size], write_region);
                bb.WriteRelease(data_size);
                written.insert(written.end(), &data[0], &data[data_size]);
                write_count += data_size;
            }
        } while (write_count < TEST_MT_TRANSFER_CNT);
    });

    for (auto &t : threads) {
        t.join();
    }

    REQUIRE(
        std::equal(std::begin(written), std::end(written), std::begin(read)));
}
