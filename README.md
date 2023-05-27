# lockfree
![CMake](https://github.com/DNedic/lockfree/actions/workflows/.github/workflows/cmake.yml/badge.svg)

`lockfree` is a collection of lock-free data structures written in standard C++11 and suitable for all platforms - from deeply embedded to HPC.

## What are lock-free data structures?

Lock-free data structures are data structures that are thread and interrupt safe without having to use mutual exclusion mechanisms. Lock-free data structures are most useful for inter process communication, but due to the efficiency of `lockfree`, it can safely be used for single threaded uses as well, making it good for general purpose use.

## Why use `lockfree`
* Written in standard C++11, compatible with all platforms supporting it
* All data structures are thread and multicore safe in their respective usecases
* No dynamic allocation
* Optimized for high performance
* MIT Licensed
* Additional APIs for newer C++ versions

## What data structures are available?
### Single-producer single-consumer data structures
* [Queue](docs/spsc/queue.md) - Best for single element operations, extremely fast, simple API consisting of only 2 methods.
* [Ring Buffer](docs/spsc/ring_buf.md) - A more general data structure with the ability to handle multiple elements at a time, uses standard library copies making it very fast for bulk operations.
* [Bipartite Buffer](docs/spsc/bipartite_buf.md) - A variation of the ring buffer with the ability to always provide linear space in the buffer, enables in-buffer processing.
* [Priority Queue](docs/spsc/priority_queue.md) - A Variation of the queue with the ability to provide different priorities for elements, very useful for things like signals, events and communication packets.

These data structures are more performant and should generally be used whenever there is only one thread/interrupt pushing data and another one retrieving it.

### Multi-producer multi-consumer data structures
* [Queue](docs/mpmc/queue.md) - Best for single element operations, extremely fast, simple API consisting of only 2 methods.
* [Priority Queue](docs/spsc/priority_queue.md) - A Variation of the queue with the ability to provide different priorities for elements, very useful for things like signals, events and communication packets.

These data structures are more general, supporting multiple producers and consumers at the same time, however they have storage and performance overhead compared to single producer single consumer data structures.

## How to get
There are three main ways to get the library:
* Using CMake [FetchContent()](https://cmake.org/cmake/help/latest/module/FetchContent.html)
* As a [git submodule](https://git-scm.com/book/en/v2/Git-Tools-Submodules)
* By downloading a release from GitHub

## Configuration
For cache coherent systems the [False Sharing](https://en.wikipedia.org/wiki/False_sharing) phenomenom can reduce performance to some extent which is why passing ```LOCKFREE_CACHE_COHERENT``` as ```true``` is advisable. This aligns the indexes to ```LOCKFREE_CACHELINE_LENGTH```, ```64``` by default.

On embedded systems, ```LOCKFREE_CACHE_COHERENT``` should almost always be left as ```false```.

Some systems have a non-typical cacheline length (for instance the apple M1/M2 CPUs have a cacheline length of 128 bytes), and ```LOCKFREE_CACHELINE_LENGTH``` should be set accordingly in those cases.

## FAQ
### Why would I use this over locking data structures on a hosted machine?

The biggest reason you would want to use a lockfree data structure in such a scenario would be performance. Locking has a non-neglegible runtime cost on hosted systems as every lock requires a syscall.

Additional benefits would be performance from cache locality as lockfree data structures are array-based or code portability to non-POSIX environments.

### Why use this over RTOS-provided IPC mechanisms on an embedded system?

While locking usually isn't expensive on embedded systems such as microcontrollers, there is a wide variety of RTOS-es and no standardized API for locking. The fact that multiple architectures are present from 8051 to RISC-V means that architecture-specific locking methods are not standardized either.

`lockfree` provides a way to build portable embedded code with a neglegible performance cost as opposed to locking, code using `lockfree` can be compiled to run on any embedded platform supporting C++11. Additionally, the code can easily be tested on a host machine without the need for mocking.

### What advantages does the C++ version of the library bring?
* Type safety, as data structures are type and size templated
* Much simpler and less error-prone instantiation
* Higher performance due to compile-time known size and header-only implementation
* Encapsulation, the data buffer is a class member instead of being passed by a pointer

### Give me more theory
All structures in `lockfree` are **bounded**, **array-based** and **lockfree**, spsc data structures are also **waitfree** and **termination safe**. For more insight into lock-free programming, take a look at this [brilliant talk series](https://youtu.be/c1gO9aB9nbs) from Herb Sutter.
