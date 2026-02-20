# Writing Tests

This directory contains unit tests for the Producer-Consumer project using the [Catch2](https://github.com/catchorg/Catch2) testing framework.

## How Tests Work with CMake

The [CMakeLists.txt](CMakeLists.txt) in this directory automatically:

1. **Discovers all test files** using `GLOB_RECURSE` - any `.cpp` file in this directory
2. **Creates a test executable** named `producer_consumer_tests`
3. **Links the executable** to:
   - `producer_consumer::library` (your project code)
   - `Catch2::Catch2WithMain` (test framework with auto-generated main)
4. **Registers tests with CTest** using `catch_discover_tests()`

No manual configuration needed - just add `.cpp` files and rebuild!

## Writing Your First Test

### Basic Test Structure

Create a file like `test_basic.cpp`:

```cpp
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Basic arithmetic works", "[math]") {
    REQUIRE(1 + 1 == 2);
    REQUIRE(5 - 3 == 2);
}

TEST_CASE("String operations", "[string]") {
    std::string str = "hello";
    REQUIRE(str.length() == 5);
    REQUIRE(str + " world" == "hello world");
}
```

### Testing Your Library Code

Assuming you have code in `src/library/include/`:

```cpp
#include <catch2/catch_test_macros.hpp>
#include "common/common.hpp"  // Your library headers
#include "your_class.hpp"

TEST_CASE("Producer creates items", "[producer]") {
    Producer producer(/* args */);

    // Test producer functionality
    REQUIRE(producer.produce() != nullptr);
    REQUIRE(producer.getCount() == 1);
}

TEST_CASE("Consumer processes items", "[consumer]") {
    Consumer consumer(/* args */);

    // Test consumer functionality
    REQUIRE(consumer.consume() == expected_value);
}

TEST_CASE("Thread safety", "[threading]") {
    // Test concurrent access to shared buffer
    // ...
}
```

## Common Catch2 Assertions

### Basic Assertions

```cpp
REQUIRE(expression);          // Test fails if false
CHECK(expression);            // Test continues if false
REQUIRE_FALSE(expression);    // Test fails if true
```

### Comparisons

```cpp
REQUIRE(value == expected);
REQUIRE(value != unexpected);
REQUIRE(value < 10);
REQUIRE(value >= 0);
```

### Floating Point

```cpp
#include <catch2/catch_approx.hpp>

REQUIRE(value == Catch::Approx(3.14).epsilon(0.01));
```

### Exceptions

```cpp
REQUIRE_THROWS(function_that_throws());
REQUIRE_THROWS_AS(function(), std::runtime_error);
REQUIRE_NOTHROW(safe_function());
```

### String Matching

```cpp
#include <catch2/matchers/catch_matchers_string.hpp>
using Catch::Matchers::Contains;
using Catch::Matchers::StartsWith;
using Catch::Matchers::EndsWith;

REQUIRE_THAT(str, Contains("substring"));
REQUIRE_THAT(str, StartsWith("prefix"));
REQUIRE_THAT(str, EndsWith("suffix"));
```

## Test Organization

### Using Tags

Tags help organize and filter tests:

```cpp
TEST_CASE("Quick test", "[quick][unit]") {
    // Fast test
}

TEST_CASE("Slow integration test", "[slow][integration]") {
    // Slower test
}
```

Run specific tags:
```bash
./producer_consumer_tests "[quick]"     # Run only quick tests
./producer_consumer_tests "[unit]"      # Run only unit tests
./producer_consumer_tests "~[slow]"     # Skip slow tests
```

### Using Sections

Sections share setup code:

```cpp
TEST_CASE("Buffer operations", "[buffer]") {
    Buffer buffer(10);  // Shared setup

    SECTION("Adding items") {
        buffer.add(5);
        REQUIRE(buffer.size() == 1);
    }

    SECTION("Removing items") {
        buffer.add(5);
        buffer.remove();
        REQUIRE(buffer.empty());
    }
}
```

### Fixtures for Shared Setup

```cpp
class BufferFixture {
protected:
    Buffer buffer{10};
    Producer producer{&buffer};
    Consumer consumer{&buffer};
};

TEST_CASE_METHOD(BufferFixture, "Producer-Consumer interaction", "[integration]") {
    producer.produce();
    REQUIRE(buffer.size() == 1);

    consumer.consume();
    REQUIRE(buffer.empty());
}
```

## Testing Threading/Concurrency

For Producer-Consumer specifically:

```cpp
#include <thread>
#include <vector>

TEST_CASE("Multiple producers and consumers", "[threading][integration]") {
    const int NUM_PRODUCERS = 3;
    const int NUM_CONSUMERS = 3;
    const int ITEMS_PER_PRODUCER = 100;

    SharedBuffer buffer(50);
    std::vector<std::thread> threads;

    // Launch producers
    for (int i = 0; i < NUM_PRODUCERS; ++i) {
        threads.emplace_back([&buffer, ITEMS_PER_PRODUCER]() {
            Producer p(&buffer);
            for (int j = 0; j < ITEMS_PER_PRODUCER; ++j) {
                p.produce();
            }
        });
    }

    // Launch consumers
    for (int i = 0; i < NUM_CONSUMERS; ++i) {
        threads.emplace_back([&buffer, ITEMS_PER_PRODUCER, NUM_PRODUCERS]() {
            Consumer c(&buffer);
            for (int j = 0; j < (ITEMS_PER_PRODUCER * NUM_PRODUCERS / NUM_CONSUMERS); ++j) {
                c.consume();
            }
        });
    }

    // Wait for all threads
    for (auto& t : threads) {
        t.join();
    }

    REQUIRE(buffer.empty());
}
```

## Building and Running Tests

### Build Tests

From the repository root:

```bash
cd CS471PROJECT
cmake -S . -B build
cmake --build build
```

The test executable will be at: `CS471PROJECT/PRODUCER-CONSUMER/producer_consumer_tests`

### Run All Tests

```bash
# Using CTest
cd CS471PROJECT/build
ctest -V

# Or run directly
cd CS471PROJECT/PRODUCER-CONSUMER
./producer_consumer_tests
```

### Run Specific Tests

```bash
# By test name
./producer_consumer_tests "Producer creates items"

# By tag
./producer_consumer_tests "[producer]"
./producer_consumer_tests "[quick]"

# List all tests
./producer_consumer_tests --list-tests

# List all tags
./producer_consumer_tests --list-tags
```

### Verbose Output

```bash
./producer_consumer_tests -s   # Show successful assertions too
./producer_consumer_tests -d   # Show test durations
```

## Test File Examples

### Example: `test_buffer.cpp`

```cpp
#include <catch2/catch_test_macros.hpp>
#include "buffer.hpp"

TEST_CASE("Buffer initialization", "[buffer][unit]") {
    SECTION("Empty buffer") {
        Buffer buf(10);
        REQUIRE(buf.empty());
        REQUIRE(buf.capacity() == 10);
    }

    SECTION("Zero capacity throws") {
        REQUIRE_THROWS_AS(Buffer(0), std::invalid_argument);
    }
}

TEST_CASE("Buffer operations", "[buffer][unit]") {
    Buffer buf(5);

    SECTION("Add and remove") {
        buf.add(42);
        REQUIRE_FALSE(buf.empty());
        REQUIRE(buf.size() == 1);

        int value = buf.remove();
        REQUIRE(value == 42);
        REQUIRE(buf.empty());
    }

    SECTION("FIFO order") {
        buf.add(1);
        buf.add(2);
        buf.add(3);

        REQUIRE(buf.remove() == 1);
        REQUIRE(buf.remove() == 2);
        REQUIRE(buf.remove() == 3);
    }
}
```

### Example: `test_synchronization.cpp`

```cpp
#include <catch2/catch_test_macros.hpp>
#include <thread>
#include <mutex>
#include "semaphore.hpp"

TEST_CASE("Semaphore operations", "[semaphore][unit]") {
    Semaphore sem(1);

    SECTION("Wait decrements") {
        sem.wait();
        // Semaphore should now be 0
    }

    SECTION("Signal increments") {
        sem.wait();
        sem.signal();
        // Semaphore back to 1
    }
}

TEST_CASE("Mutex protects critical section", "[mutex][threading]") {
    int shared_counter = 0;
    std::mutex mtx;
    const int ITERATIONS = 1000;

    auto increment = [&]() {
        for (int i = 0; i < ITERATIONS; ++i) {
            std::lock_guard<std::mutex> lock(mtx);
            ++shared_counter;
        }
    };

    std::thread t1(increment);
    std::thread t2(increment);

    t1.join();
    t2.join();

    REQUIRE(shared_counter == 2 * ITERATIONS);
}
```

## CMakeLists.txt Explanation

The [CMakeLists.txt](CMakeLists.txt) in this directory does the following:

```cmake
include(CTest)                          # Enable CTest integration

file(GLOB_RECURSE TEST_SOURCES          # Find all .cpp files
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_executable(producer_consumer_tests) # Create test executable

target_sources(                         # Add test files to executable
    producer_consumer_tests
    PRIVATE
    ${TEST_SOURCES}
)

target_link_libraries(producer_consumer_tests
    PRIVATE producer_consumer::library  # Link your library
    PRIVATE Catch2::Catch2WithMain      # Link Catch2 (with main)
)

target_compile_features(producer_consumer_tests PRIVATE cxx_std_20)

include(Catch)                          # Include Catch2 CMake module

catch_discover_tests(producer_consumer_tests)  # Auto-discover tests for CTest
```

### CONFIGURE_DEPENDS

Tells CMake to recheck for new files on each build. If you add a new test file, just run `cmake --build build` - no need to reconfigure.

### Catch2::Catch2WithMain

This provides a `main()` function automatically, so you don't need to write one. If you need a custom main (for example, to initialize global state), use `Catch2::Catch2` instead and provide your own main.

## Best Practices

1. **One concept per test case** - Keep tests focused
2. **Use descriptive names** - "Producer adds item to buffer" not "test1"
3. **Use tags** - Organize by feature, speed, type ([unit], [integration], etc.)
4. **Test edge cases** - Empty buffers, full buffers, boundary conditions
5. **Test error conditions** - Invalid inputs should throw/fail gracefully
6. **Keep tests fast** - Use `[slow]` tag for long-running tests
7. **Avoid test interdependence** - Each test should run independently
8. **Test threading carefully** - Use appropriate synchronization, may need multiple runs

## Additional Resources

- [Catch2 Tutorial](https://github.com/catchorg/Catch2/blob/devel/docs/tutorial.md)
- [Catch2 Assertion Macros](https://github.com/catchorg/Catch2/blob/devel/docs/assertions.md)
- [Catch2 Command Line](https://github.com/catchorg/Catch2/blob/devel/docs/command-line.md)
- [Testing Concurrent Code](https://github.com/catchorg/Catch2/blob/devel/docs/benchmarks.md)
