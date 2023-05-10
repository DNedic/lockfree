# Ring Buffer

## When to use the Ring Buffer
* Whenever there is a need for adding or removing more than one element at a time
* For byte streams where variably sized packets need to be enqueued but sent `n` bytes at a time
* When operations on data can fail or only some of the data is used

## How to use
There are three API types that can be used:
* A raw pointer and element count API
* A `std::array` based API
* A `std::span` based API for C++20 or higher

Shown here is an example of raw pointer API use:
* Initialization
```cpp
#include "lockfree.hpp"
// --snip--
lockfree::RingBuf<uint8_t, 1024U> rb_uart;
```

* Producer thread/interrupt
```cpp
uint8_t buf[size];
some_data.Serialize(buf);
bool write_success = rb_uart.Write(buf, sizeof(buf) / sizeof(buf[0]));
```

* Consumer thread/interrupt
```cpp
size_t bytes_to_send = uart.HardWareFifoFree();
if (rb_uart.GetAvailable() >= bytes_to_send) {
    rb_uart.Read(buf, bytes_to_send / sizeof(buf[0]));
    uart.Transmit(buf, bytes_to_send);
}
```

Alternatively, methods `Peek()` and `Skip()` provide a useful combination when the operations on data can fail or only some of the data can be used:
```cpp
if (rb_uart.Peek(buf, sizeof(buf) / sizeof(buf[0]))) {
    size_t sent = uart.Transmit(buf, sizeof(buf));
    if (sent) {
        rb_uart.Skip(sent / sizeof(buf[0]));
    }
}
```
