# Queue

## When to use the Queue
Queue is the simplest data structure in the library, and it should be used when single element operations are dominant. It has the simplest API and lowest overhead per operation.

Additionally, the Queue uses the copy constructor unlike Ring Buffer and Bipartite Buffer, which are only meant for PODs.

## How to use
Shown here is an example of typical use:
* Initialization
```cpp
#include "lockfree.hpp"
// --snip--
lockfree::Queue<uint32_t, 128U> queue_adc;
```

* Producer thread/interrupt
```cpp
uint32_t adc_raw = ADC_GetData(adc_h);
bool write_success = queue_adc.Push(adc_raw);
```

* Consumer thread/interrupt
```cpp
uint32_t read;
bool read_success = queue_adc.Pop(read);

if (read_success) {
    DoStuffWithData(read);
}
```

There is also a `std::optional` API for the `Pop` method:
```c
auto read = queue_adc.Pop(read);

if (read) {
    DoStuffWithData(read);
}
```
