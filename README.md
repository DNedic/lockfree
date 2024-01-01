# lockfree
![CMake](https://github.com/DNedic/lockfree/actions/workflows/.github/workflows/cmake.yml/badge.svg)

`lockfree` is a collection of lock-free data structures written in standard C++11 and suitable for all platforms - from deeply embedded to HPC.

## What are lock-free data structures?

Lock-free data structures are data structures that are thread and interrupt safe for concurrent use without having to use mutual exclusion mechanisms. They are most useful for inter process communication, and often scale much better than lock-based structures with the number of operations and threads.

## Why use `lockfree`
* Written in standard C++11, compatible with all platforms supporting it
* All data structures are thread and interrupt safe in their respective usecases
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
* [Priority Queue](docs/mpmc/priority_queue.md) - A Variation of the queue with the ability to provide different priorities for elements, very useful for things like signals, events and communication packets.

These data structures are more general, supporting multiple producers and consumers at the same time, however they have storage and performance overhead compared to single producer single consumer data structures. They also require atomic instructions which can be missing from some low-end microcontrollers.

## How to get
There are three main ways to get the library:
* Using CMake [FetchContent()](https://cmake.org/cmake/help/latest/module/FetchContent.html)
* As a [git submodule](https://git-scm.com/book/en/v2/Git-Tools-Submodules)
* By downloading a release from GitHub

## Configuration
`lockfree` uses cacheline alignment for indexes to avoid the [False Sharing](https://en.wikipedia.org/wiki/False_sharing) phenomenon by default, avoiding the performance loss of cacheline invalidation  on cache coherent systems.  This aligns the indexes to ```LOCKFREE_CACHELINE_LENGTH```, ```64``` by default.

On embedded systems, ```LOCKFREE_CACHE_COHERENT``` should almost always be set as ```false``` to avoid wasting memory.

Additionally, some systems have a non-typical cacheline length (for instance the apple M1/M2 CPUs have a cacheline length of 128 bytes), and ```LOCKFREE_CACHELINE_LENGTH``` should be set accordingly in those cases.

## How It Works 🌟
* **[MPMC Queues](lockfree/mpmc):** Multi-producer, multi-consumer queues that allow multiple threads to simultaneously perform enqueue and dequeue operations.
Multi-producer multi-consumer (MPMC) queues in the lockfree repository are designed to handle concurrent enqueue (push) and dequeue (pop) operations from multiple producers and consumers. This design ensures thread safety without the need for locking mechanisms, which enhances performance in multi-threaded environments. The queue typically uses atomic operations to manage the head and tail indices, ensuring that multiple threads can safely access and modify the queue. 

  **A Multi-Producer Multi-Consumer queue differs from a standard queue in several key ways:**
  * **Concurrency Support:** It is designed to support multiple producers and consumers accessing the queue concurrently, without causing race conditions or data corruption.

  * **Lock-Free Mechanism:** Utilizes atomic operations to manage access, ensuring thread safety without traditional locking methods like mutexes, thus reducing potential bottlenecks in a multi-threaded environment. 

  * **Efficiency in Multi-Threading:** Optimized for performance in scenarios where several threads are simultaneously pushing to and popping from the queue.

This design is crucial for applications requiring high-concurrency and low-latency data exchange between threads.

  ```cpp
  lockfree::mpmc::Queue<int, 20> queue;
  queue.Push(10); // Enqueue
  int value;
  queue.Pop(value); // Dequeue
  ```
  
* **[Priority Queues](lockfree/mpmc/priority_queue.hpp):** Efficient handling of data with different priority levels, ensuring high-priority items are processed first.
  * Priority Queues, as implemented in the lockfree repository, are designed to handle elements with varying levels of priority. Unlike standard queues, where elements are processed in a first-come-first-served basis, priority queues ensure that elements with higher priority are processed first.
  * This is typically achieved by organizing the elements in a way that allows efficient retrieval of the highest-priority item.
  * These queues are particularly useful in scenarios where tasks need to be processed according to their urgency or importance, rather than their order of arrival.
  * A Priority Queue differs from an ordinary queue in that it orders elements based on priority. Instead of a first-in-first-out (FIFO) approach, items with higher priority are processed first. This allows for more critical tasks to be addressed sooner. In multi-threaded environments, this can significantly improve efficiency and responsiveness by ensuring important tasks aren't delayed behind less urgent ones. This is especially useful in applications like scheduling systems, where task prioritization is essential.

  ```cpp
  lockfree::mpmc::PriorityQueue<int, 20, 3> pQueue;
  pQueue.Push(5, 1); // Enqueue with priority
  int value;
  pQueue.Pop(value); // Dequeue
  ```
  
  Imagine a Priority Queue handling tasks where each task has a priority level (e.g., 1 to 5, with 1 being the highest priority). When tasks are added to the queue, they are organized based on their priority level.
    1. Task A (Priority 3) is added.
    2. Task B (Priority 1) is added.
    3. Task C (Priority 2) is added.

  When dequeuing, the queue returns the tasks in the order of B, C, A, despite A being added first. This is because B and C have higher priorities than A. This differs from a standard queue, where tasks would be processed in the order they were added, regardless of priority.

  ```cpp
  #include <iostream>
  #include <queue>
  #include <vector>
  #include <functional>
  
  int main() {
      // Creating a Priority Queue where smaller numbers have higher priority
      std::priority_queue<int, std::vector<int>, std::greater<int>> pq;
  
      // Adding elements with different priorities
      pq.push(5); // Lowest priority
      pq.push(1); // Highest priority
      pq.push(3); // Medium priority
  
      // Dequeuing elements
      while (!pq.empty()) {
          std::cout << "Processing task with priority: " << pq.top() << std::endl;
          pq.pop();
      }
  
      return 0;
  }

  ```

  In this example, tasks with lower numerical values have higher priority. The queue will first process task 1, then task 3, and finally task 5. This behavior is different from a standard queue, where tasks would be processed in the order they were added (5, 1, 3 in this case).
  Threfore, the priority of elements in the queue is determined by the comparator std::greater<int>. This comparator makes the priority queue function as a min-heap, where smaller numerical values are given higher priority. Therefore, in this context, the number 5 has the lowest priority because it's the largest number among the elements pushed to the queue (5, 1, 3). The queue will process elements in ascending order, starting with the smallest number, which is considered the highest priority.
  
*  **[Bipartite Buffers](lockfree/spc):** Ideal for scenarios with one producer and one consumer, these buffers optimize data transfer with minimal locking. Bipartite Buffers, as implemented in the lockfree repository, are designed for efficient data transfer between a single producer and a single consumer. They use two buffer parts to minimize locking. When the producer writes data, it uses one part of the buffer. Once the data is written, the roles of the buffer parts are switched, allowing the consumer to read from the part where data was just written while the producer writes to the other part. This design reduces the need for synchronization mechanisms, enhancing performance in single-producer-single-consumer scenarios.

    ```cpp
    lockfree::spsc::BipartiteBuf<int, 512> bb;
    auto* writePtr = bb.WriteAcquire(100); // Acquire write pointer
    // ... write data ...
    bb.WriteRelease(100); // Release after writing
    auto read = bb.ReadAcquire(); // Acquire read access
    // ... read data ...
    bb.ReadRelease(read.second); // Release after reading
    
    ```

   They differs from ordinary buffers in its unique approach to handling read and write operations in single-producer-single-consumer scenarios. An ordinary buffer typically allows linear reads and writes, often requiring synchronization mechanisms to prevent data corruption in concurrent environments. In contrast, the Bipartite Buffer is designed with two separate sections. When the producer writes data to one section, the consumer can simultaneously read from the other, effectively minimizing the need for synchronization. This dual-part system allows seamless and efficient data transfer without the typical contention found in ordinary buffers. This method is particularly effective in scenarios where data is produced and consumed at variable rates.

   **To exemplify how a Bipartite Buffer differs from an ordinary buffer using code, consider these two scenarios:**
   
     **1 - Ordinary Buffer (Simple Queue) Example:**
     ```ccp
         class SimpleBuffer {
              int data[SIZE];
              int writeIndex = 0;
              int readIndex = 0;
          
              void write(int value) {
                  data[writeIndex++] = value;
                  // Handle wrapping and synchronization
              }
          
              int read() {
                  int value = data[readIndex++];
                  // Handle wrapping and synchronization
                  return value;
              }
          };

     ```
     **2 - Bipartite Buffer Example:**
     ```ccp
       class BipartiteBuffer {
            int data[SIZE];
            std::atomic<int> readIndex;
            std::atomic<int> writeIndex;
        
            int* writeAcquire(int size) {
                // Determine the write position and handle wrapping
                // Return pointer to write location
            }
        
            void writeRelease(int size) {
                // Update writeIndex after writing
            }
        
            std::pair<int*, size_t> readAcquire() {
                // Determine the read position and size available for reading
                // Return pointer to read location and size
            }
        
            void readRelease(int size) {
                // Update readIndex after reading
            }
        };

     ```
     In the Bipartite Buffer, the writeAcquire and readAcquire methods handle the logic of wrapping and provide direct access to the buffer part. This is different from a simple queue where each write and read operation is sequential and might need locking for synchronization. The Bipartite Buffer's approach allows simultaneous reading and writing to different parts of the buffer, optimizing performance in single-producer-single-consumer scenarios.
   
## Unit Tests 🧪
  **Robust Testing:** Comprehensive unit tests cover a wide range of scenarios, ensuring reliability and correctness of the data structures in concurrent environments.
  Multithreaded Scenarios: Tests include multithreaded read/write operations, highlighting the thread safety and efficiency of the structures.

## Concurrency and Performance 🚀
  **Designed for Concurrency:** These data structures excel in high-concurrency environments, providing efficient, lock-free operations.
  Performance Optimized: Carefully crafted to reduce overhead and latency, making them suitable for high-performance computing applications.

## FAQ
### Why would I use this over locking data structures on a hosted machine?

The biggest reason you would want to use a lock-free data structure on hosted environments would be avoiding issues surrounding locking such as deadlocks, priority inversion and nondeterministic access latency. When used properly, lock-free data structures can also improve performance in some scenarios.

Additionally, `lockfree` provides a way to build applications and libraries that can be compiled to work on both POSIX and non-POSIX environments without `#ifdef`s or polymorphism.

### Why use this over RTOS-provided IPC mechanisms on an embedded system?

While locking usually isn't expensive on embedded systems such as microcontrollers, there is a wide variety of RTOS-es and no standardized API for locking. The fact that multiple architectures are present from 8051 to RISC-V means that architecture-specific locking methods are not standardized either.

`lockfree` provides a way to build portable embedded code with a negligible performance cost as opposed to locking, code using `lockfree` can be compiled to run on any embedded platform supporting C++11. Additionally, the code can easily be tested on a host machine without the need for mocking.

### What advantages does using C++ over C provide for the library?
* Type safety, as data structures are type and size templated
* Much simpler and less error-prone instantiation
* Higher performance due to compile-time known size and header-only implementation
* Encapsulation, the data buffer is a class member instead of being passed by a pointer

### What is the formal classification of the data structures in `lockfree`?
All structures in `lockfree` are **bounded**, **array-based** and **lock-free**, spsc data structures are also **waitfree** and **termination safe**. 

## Theory and references
For more insight into lock-free programming, take a look at:
* This [brilliant talk series](https://youtu.be/c1gO9aB9nbs) from Herb Sutter
* [Live Lock-Free or Deadlock](https://youtu.be/lVBvHbJsg5Y) talk series from Fedor Pikus
* Dmitry Vyukov's excellent [blog](https://www.1024cores.net/home/lock-free-algorithms/introduction)
