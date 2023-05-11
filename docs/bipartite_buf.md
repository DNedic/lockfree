# Bipartite Buffer

## When to use the Bipartite Buffer
A bipartite buffer should be used everywhere the Ring Buffer is used if you want:
* To offload transfers to DMA increasing the transfer speed and freeing up CPU time
* To avoid creating intermediate buffers for APIs that require contigous data
* To process data inside the buffer without dequeing it
* When operations on data can fail or only some of the data is used

> Note: At the moment, the Bipartite Buffer is only meant to be used for [trivial](https://en.cppreference.com/w/cpp/language/classes#Trivial_class) types.

## How to use
Shown here is an example of typical use:
* Initialization
```cpp
#include "lockfree.hpp"
// --snip--
lockfree::BipartiteBuf<uint32_t, 1024U> bb_adc;
```

* Consumer thread/interrupt
```cpp
auto read = bb_adc.ReadAcquire();

if (read.first != nullptr) {
    size_t data_used = DoStuffWithData(read);
    bb_adc.ReadRelease(data_used);
}
```

* Producer thread/interrupt
```cpp
if (!write_started) {
    auto *write_ptr = bb_adc.WriteAcquire(data.size());
    if (write_ptr != nullptr) {
        ADC_StartDma(&adc_dma_h, write_ptr, sizeof(data));
        write_started = true;
    }
} else {
    if (ADC_PollDmaComplete(&adc_dma_h) {
        bb_adc.WriteRelease(data.size());
        write_started = false;
    }
}
```

There is also a `std::span` based API for those using C++20 and up:
* Consumer thread/interrupt
```cpp
auto read = bb_adc.ReadAcquireSpan();

if (!read.empty())) {
    auto span_used = DoStuffWithData(read);
    bb_adc.ReadRelease(span_used);
}
```

* Producer thread/interrupt
```cpp
if (!write_started) {
    auto write_span = bb_adc.WriteAcquireSpan(data.size());
    if (!write_span.empty()) {
        ADC_StartDma(&adc_dma_h, write_span.data(), write_span.size_bytes());
        write_started = true;
    }
} else {
    if (ADC_PollDmaComplete(&adc_dma_h) {
        bb_adc.WriteRelease(data.size());
        write_started = false;
    }
}
```

## Dealing with caches on embedded systems
When using the library with DMA or asymmetric multicore on embedded systems with cache it is necessary to perform manual cache synchronization in one of the following ways:
* Using platform specific data synchronization barriers (```DSB``` on ARM)
* By manually invalidating cache
* By setting the MPU/MMU up to not cache the data buffer
