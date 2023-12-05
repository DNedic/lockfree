#include <algorithm>
#include <catch2/catch_test_macros.hpp>

#include "lockfree.hpp"

TEST_CASE("Get free with an empty buffer", "[rb_get_free_empty]") {
    lockfree::spsc::RingBuf<float, 1024U> const rb;
    const float test_data[120] = {2.71828F};

    REQUIRE(rb.GetFree() == 1024U - 1U);
}

TEST_CASE("Get free after a write", "[rb_get_free]") {
    lockfree::spsc::RingBuf<float, 1024U> rb;
    const float test_data[120] = {2.71828F};

    rb.Write(test_data, sizeof(test_data) / sizeof(test_data[0]));

    REQUIRE(rb.GetFree() ==
            1024U - 1U - sizeof(test_data) / sizeof(test_data[0]));
}

TEST_CASE("Get free when the buffer is full", "[rb_get_free_full]") {
    lockfree::spsc::RingBuf<float, 1024U> rb;
    const float test_data[1023] = {2.71828F};

    rb.Write(test_data, sizeof(test_data) / sizeof(test_data[0]));

    REQUIRE(rb.GetFree() == 0);
}

TEST_CASE("Get free after a wrapping write", "[rb_get_free_wrapped]") {
    lockfree::spsc::RingBuf<float, 1024U> rb;
    const float test_data[360] = {2.71828F};

    rb.Write(test_data, sizeof(test_data) / sizeof(test_data[0]));

    rb.Skip(sizeof(test_data) / sizeof(test_data[0]));

    const float test_data2[900] = {3.1416F};
    rb.Write(test_data2, sizeof(test_data2) / sizeof(test_data2[0]));

    /* After a wrapping write */
    REQUIRE(rb.GetFree() ==
            1024U - 1U - sizeof(test_data2) / sizeof(test_data2[0]));
}

TEST_CASE("Get available with an empty buffer", "[rb_get_available_empty]") {
    lockfree::spsc::RingBuf<double, 1024U> const rb;

    REQUIRE(rb.GetAvailable() == 0);
}

TEST_CASE("Get available after a write", "[rb_get_available]") {
    lockfree::spsc::RingBuf<double, 1024U> rb;
    const double test_data[120] = {123.123123};

    rb.Write(test_data, sizeof(test_data) / sizeof(test_data[0]));

    REQUIRE(rb.GetAvailable() == sizeof(test_data) / sizeof(test_data[0]));
}

TEST_CASE("Get available when the buffer is full", "[rb_get_available_full]") {
    lockfree::spsc::RingBuf<double, 1024U> rb;
    const double test_data[1023] = {123.123123};

    rb.Write(test_data, sizeof(test_data) / sizeof(test_data[0]));

    REQUIRE(rb.GetAvailable() == 1024U - 1U);
}

TEST_CASE("Get available after a wrapping write",
          "[rb_get_available_wrapped]") {
    lockfree::spsc::RingBuf<double, 1024U> rb;
    const double test_data[360] = {123.123123};

    rb.Write(test_data, sizeof(test_data) / sizeof(test_data[0]));

    rb.Skip(sizeof(test_data) / sizeof(test_data[0]));

    const double test_data2[900] = {3.1416F};
    rb.Write(test_data2, sizeof(test_data2) / sizeof(test_data2[0]));

    /* After a wrapping write */
    REQUIRE(rb.GetAvailable() == sizeof(test_data2) / sizeof(test_data2[0]));
}

TEST_CASE("Skip from the beginning", "[rb_skip]") {
    lockfree::spsc::RingBuf<int32_t, 100U> rb;
    const int32_t test_data[60] = {-125};

    rb.Write(test_data, sizeof(test_data) / sizeof(test_data[0]));
    bool const skip_success = rb.Skip(sizeof(test_data) / sizeof(test_data[0]));

    REQUIRE(skip_success);
    REQUIRE(rb.GetFree() == 100U - 1U);
}

TEST_CASE("Skip after wrapping", "[rb_skip_wrapping]") {
    lockfree::spsc::RingBuf<int32_t, 100U> rb;
    const int32_t test_data[60] = {-125};

    rb.Write(test_data, sizeof(test_data) / sizeof(test_data[0]));
    rb.Skip(sizeof(test_data) / sizeof(test_data[0]));

    rb.Write(test_data, sizeof(test_data) / sizeof(test_data[0]));
    bool const skip_success = rb.Skip(sizeof(test_data) / sizeof(test_data[0]));

    REQUIRE(skip_success);
    REQUIRE(rb.GetFree() == 100U - 1U);
}

TEST_CASE("Try to skip with an empty buffer", "[rb_skip_empty]") {
    lockfree::spsc::RingBuf<int32_t, 100U> rb;

    bool const skip_success = rb.Skip(1);

    REQUIRE(!skip_success);
}

TEST_CASE("Write to the beginning", "[rb_write_beginning]") {
    lockfree::spsc::RingBuf<uint8_t, 512U> rb;
    const uint8_t test_data[320] = {0xE5U};

    bool const write_success =
        rb.Write(test_data, sizeof(test_data) / sizeof(test_data[0]));
    REQUIRE(write_success);
}

TEST_CASE("Write with wrapping", "[rb_write_wrapping]") {
    lockfree::spsc::RingBuf<uint8_t, 512U> rb;
    const uint8_t test_data[320] = {0xE5U};

    rb.Write(test_data, sizeof(test_data) / sizeof(test_data[0]));

    rb.Skip(sizeof(test_data) / sizeof(test_data[0]));

    bool const write_success =
        rb.Write(test_data, sizeof(test_data) / sizeof(test_data[0]));
    REQUIRE(write_success);
}

TEST_CASE("Write when there is not enough space", "[rb_write_no_space]") {
    lockfree::spsc::RingBuf<uint8_t, 512U> rb;
    const uint8_t test_data[320] = {0xE5U};

    rb.Write(test_data, sizeof(test_data) / sizeof(test_data[0]));

    bool const write_success =
        rb.Write(test_data, sizeof(test_data) / sizeof(test_data[0]));
    REQUIRE(!write_success);
}

TEST_CASE("Write full capacity", "[rb_write_max_size]") {
    lockfree::spsc::RingBuf<uint8_t, 512U> rb;
    const uint8_t test_data[511] = {0xE5U};

    bool const write_success =
        rb.Write(test_data, sizeof(test_data) / sizeof(test_data[0]));
    REQUIRE(write_success);
}

TEST_CASE("Write std::array", "[rb_write_std_array]") {
    lockfree::spsc::RingBuf<uint8_t, 512U> rb;
    const std::array<uint8_t, 320> test_data = {0xE5U};

    bool const write_success = rb.Write(test_data);
    REQUIRE(write_success);
}

TEST_CASE("Write std::span", "[rb_write_span]") {
    lockfree::spsc::RingBuf<uint8_t, 512U> rb;
    const uint8_t test_data[320] = {0xE5U};

    bool const write_success = rb.Write(test_data);

    REQUIRE(write_success);
}

TEST_CASE("Read from the beginning", "[rb_read_beginning]") {
    lockfree::spsc::RingBuf<uint64_t, 512U> rb;
    const uint64_t test_data[320] = {0xE5U};

    rb.Write(test_data, sizeof(test_data) / sizeof(test_data[0]));

    uint64_t test_data_read[320] = {0};
    bool const read_success = rb.Read(
        test_data_read, sizeof(test_data_read) / sizeof(test_data_read[0]));

    REQUIRE(read_success);
    REQUIRE(std::equal(std::begin(test_data), std::end(test_data),
                       std::begin(test_data_read)));
}

TEST_CASE("Read wrapping", "[rb_read_wrapping]") {
    lockfree::spsc::RingBuf<uint64_t, 512U> rb;
    const uint64_t test_data[320] = {0xE5U};

    rb.Write(test_data, sizeof(test_data) / sizeof(test_data[0]));

    rb.Skip(sizeof(test_data) / sizeof(test_data[0]));

    const uint64_t test_data2[360] = {11111U};
    rb.Write(test_data2, sizeof(test_data2) / sizeof(test_data2[0]));

    uint64_t test_data_read[360] = {0};
    bool const read_success = rb.Read(
        test_data_read, sizeof(test_data_read) / sizeof(test_data_read[0]));

    REQUIRE(read_success);
    REQUIRE(std::equal(std::begin(test_data2), std::end(test_data2),
                       std::begin(test_data_read)));
}

TEST_CASE("Try to read from an empty buffer", "[rb_read_empty]") {
    lockfree::spsc::RingBuf<uint64_t, 512U> rb;

    uint64_t test_data_read[320] = {0};
    bool const read_success = rb.Read(
        test_data_read, sizeof(test_data_read) / sizeof(test_data_read[0]));

    REQUIRE(!read_success);
}

TEST_CASE("Read std::array", "[rb_read_std_array]") {
    lockfree::spsc::RingBuf<uint64_t, 512U> rb;
    const std::array<uint64_t, 320> test_data = {0xE5U};

    rb.Write(test_data);

    std::array<uint64_t, 320> test_data_read = {0};
    bool const read_success = rb.Read(test_data_read);

    REQUIRE(read_success);
    REQUIRE(
        std::equal(test_data.begin(), test_data.end(), test_data_read.begin()));
}

TEST_CASE("Read std::span", "[rb_read_span]") {
    lockfree::spsc::RingBuf<uint64_t, 512U> rb;
    const uint64_t test_data[320] = {0xE5U};

    rb.Write(test_data);

    uint64_t test_data_read[320] = {0};
    bool const peek_success = rb.Read(test_data_read);

    REQUIRE(peek_success);
    REQUIRE(std::equal(std::begin(test_data), std::end(test_data),
                       std::begin(test_data_read)));
}

TEST_CASE("Peek from the beginning", "[rb_peek_beginning]") {
    lockfree::spsc::RingBuf<uint64_t, 512U> rb;
    const uint64_t test_data[320] = {0xE5U};

    rb.Write(test_data, sizeof(test_data) / sizeof(test_data[0]));

    uint64_t test_data_read[320] = {0};
    bool const peek_success = rb.Peek(
        test_data_read, sizeof(test_data_read) / sizeof(test_data_read[0]));

    REQUIRE(peek_success);
    REQUIRE(std::equal(std::begin(test_data), std::end(test_data),
                       std::begin(test_data_read)));
}

TEST_CASE("Peek wrapping", "[rb_peek_wrapping]") {
    lockfree::spsc::RingBuf<uint64_t, 512U> rb;
    const uint64_t test_data[320] = {0xE5U};

    rb.Write(test_data, sizeof(test_data) / sizeof(test_data[0]));

    rb.Skip(sizeof(test_data) / sizeof(test_data[0]));

    const uint64_t test_data2[360] = {11111U};
    rb.Write(test_data2, sizeof(test_data2) / sizeof(test_data2[0]));

    uint64_t test_data_read[360] = {0};
    bool const peek_success = rb.Peek(
        test_data_read, sizeof(test_data_read) / sizeof(test_data_read[0]));

    REQUIRE(peek_success);
    REQUIRE(std::equal(std::begin(test_data2), std::end(test_data2),
                       std::begin(test_data_read)));
}

TEST_CASE("Try to peek from an empty buffer", "[rb_peek_empty]") {
    lockfree::spsc::RingBuf<uint64_t, 512U> const rb;

    uint64_t test_data_read[320] = {0};
    bool const peek_success = rb.Peek(
        test_data_read, sizeof(test_data_read) / sizeof(test_data_read[0]));

    REQUIRE(!peek_success);
}

TEST_CASE("Peek std::array", "[rb_peek_std_array]") {
    lockfree::spsc::RingBuf<uint64_t, 512U> rb;
    const std::array<uint64_t, 320> test_data = {0xE5U};

    rb.Write(test_data);

    std::array<uint64_t, 320> test_data_read = {0};
    bool const read_success = rb.Peek(test_data_read);

    REQUIRE(read_success);
    REQUIRE(
        std::equal(test_data.begin(), test_data.end(), test_data_read.begin()));
}

TEST_CASE("Peek std::span", "[rb_peek_span]") {
    lockfree::spsc::RingBuf<uint64_t, 512U> rb;
    const uint64_t test_data[320] = {0xE5U};

    rb.Write(test_data);

    uint64_t test_data_read[320] = {0};
    bool const peek_success = rb.Peek(test_data_read);

    REQUIRE(peek_success);
    REQUIRE(std::equal(std::begin(test_data), std::end(test_data),
                       std::begin(test_data_read)));
}
