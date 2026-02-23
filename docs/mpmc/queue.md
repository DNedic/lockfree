# Queue

## When to use the Queue
The Queue is the simplest data structure in the library, and it should be used when single element operations are dominant. It has the simplest API and lowest overhead per operation.

## How to use
Shown here is an example of typical use:
* Initialization
```cpp
#include "lockfree.hpp"
// --snip--
lockfree::mpmc::Queue<uint32_t, 128U> queue_jobs;
```

* Producer threads
```cpp
Job job = connection.GetWorkItems();
bool write_success = queue_jobs.Push(job);
```

* Consumer threads/interrupts
```cpp
Job job;
bool read_success = queue_jobs.Pop(job);

if (read_success) {
    worker.ProcessJob(read);
}
```

There is also a `std::optional` API for the `Pop` method:
```c
auto job = queue_jobs.Pop();

if (job) {
    worker.ProcessJob(read);
}
```
