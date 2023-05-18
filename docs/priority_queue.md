# Priority Queue

## When to use the Priority Queue
The Priority Queue should be used when there are distinct priorities between elements to be enqueued, for instance different urgency signals between threads or packets of different priorities.

> Note: At the moment, the Queue is only meant to be used for [trivial](https://en.cppreference.com/w/cpp/language/classes#Trivial_class) types.

## How to use
Shown here is an example of typical use:
* Initialization
```cpp
#include "lockfree.hpp"
// --snip--
lockfree::PriorityQueue<Event, 64, 3> queue_events;
```

* Producer thread/interrupt
```cpp
Event event = actor1.Run();
// --snip--
while(!queue_events.Push(event, priority)) {}
```

* Consumer thread/interrupt
```cpp
Event event_in;
bool read_success = queue_events.PopHighest(read);
if (read_success) {
    actor2.ProcessEvent(event_in);
}
```

There is also a `std::optional` API for the `PopHighest` and `PopLowest` methods:
```c
auto read = queue_events.PopHighestOptional();

if (read) {
    actor2.ProcessEvent(read);
}
```

## Performance and memory use

This implementation has `O(1)` time complexity for `Push` and `O(current_max_priority)` or `O(current_min_priority)` for `Pop` variants making it extremely fast. 

On the other hand the memory usage is a function of `size * priority_count`, so adequately chosing the number of priorities is necessary.
