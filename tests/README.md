# Tests

The library contains tests for all data structures and their respective features.
Each data structure has it's own test file, split into `spsc` and `mpmc` folders.

## Building and running

In order to build tests, simply run CMake in the library root:
```
cmake -B build
cmake --build build
```

> **Note:** Due to `std::span` tests, a compiler with C++20 support is required for building tests

After that, you can run tests either with `ctest`:
```
ctest --output-on-failure --test-dir build/tests
```
or by executing the `build/tests/tests` binary.

## Writing tests
If adding a new feature, or fixing a bug, it is necessary to add tests in order to avoid future regressions.
You can take a look at existing tests for examples.

[Catch2](https://github.com/catchorg/Catch2) is used as the testing framework, you can read the documentation of the library [here](https://github.com/catchorg/Catch2/blob/devel/docs/tutorial.md#writing-tests).
