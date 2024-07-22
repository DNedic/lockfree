# Changelog

## 2.0.9
- Fixed the initialization order in the [Bipartite Buffer](docs/spsc/bipartite_buf.md) constructor
- Fixed missing include for the `std::pair` use in the [Bipartite Buffer](docs/spsc/bipartite_buf.md)

## 2.0.8
- Added a performance optimization in the [Bipartite Buffer](docs/spsc/bipartite_buf.md), where the atomic invalidate index load can be avoided after writing wraps

## 2.0.7
- Fixed an omission where the invalidate index of the [Bipartite Buffer](docs/spsc/bipartite_buf.md) was sharing a cacheline with wrapping flags, leading to unnecessary performance loss

## 2.0.6
- Added a performance optimization in the [Bipartite Buffer](docs/spsc/bipartite_buf.md), where the atomic read index load can be avoided after reading wraps

## 2.0.5

- A linear space calculation bug in the [Bipartite Buffer](docs/spsc/bipartite_buf.md) that could cause data corruption in builds without asserts enabled was fixed

## 2.0.4

- Added the ability to configure the library through CMake

## 2.0.3

- Fixed MSVC C++ language standard detection

## 2.0.2

- Performance and code conciseness improvements in single-producer single-consumer [Queue](docs/spsc/queue.md), [Priority Queue](docs/spsc/priority_queue.md) and [Bipartite Buffer](docs/spsc/bipartite_buf.md)

## 2.0.1

- Added asserts in the [Bipartite Buffer](docs/spsc/bipartite_buf.md) to protect against buffer overruns
- Various README improvements

## 2.0.0

- Added multi-producer multi-consumer [Queue](docs/mpmc/queue.md) and [Priority Queue](docs/mpmc/priority_queue.md) data structures
- **Important**: SPSC data structures have been moved to the `spsc` namespace
- **Important**: Cacheline padding is now the default behaviour as analytics showed most people are using the library on cache coherent systems, for embedded systems it is necessary to set `LOCKFREE_CACHE_COHERENT` to `false` to avoid wasting memory

## 1.1.0
- Added the [Priority Queue](docs/spsc/priority_queue.md) data structure

## 1.0.2
- Made all class members accidentally public private
- Small doc fixes

## 1.0.1

- Added static asserts to verify the template type arguments are trivial types
- Added static asserts to verify the minimum size
- Moved all the `relaxed` atomic loads before the `acquire` loads, potentially increasing performance, there was no need for these to be inside fences.
- Updated the documentation in various places

## 1.0.0

- Initial release
