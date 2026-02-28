# Writing Tests

This directory contains unit tests for the CPUSCHED project using the [Catch2](https://github.com/catchorg/Catch2) testing framework.

## How Tests Work

The build system automatically:

1. **Discovers all test files** - any `.cpp` file in this directory
2. **Creates a test executable** named `cpu_sched_tests`
3. **Links the executable** to:
   - `cpu_sched::library` (your project code)
   - `Catch2::Catch2WithMain` (test framework with auto-generated main)
4. **Registers tests** for running with `make test`

No manual configuration needed - just add `.cpp` files and run `make`!

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
#include "scheduler.hpp"
#include "process.hpp"

TEST_CASE("Process creation", "[process]") {
    Process proc(1, 0, 10, 5);  // pid, arrival, burst, priority

    REQUIRE(proc.getPid() == 1);
    REQUIRE(proc.getArrivalTime() == 0);
    REQUIRE(proc.getBurstTime() == 10);
    REQUIRE(proc.getPriority() == 5);
}

TEST_CASE("FCFS scheduling", "[scheduler][fcfs]") {
    Scheduler scheduler(SchedulerType::FCFS);

    scheduler.addProcess(Process(1, 0, 5, 0));
    scheduler.addProcess(Process(2, 1, 3, 0));
    scheduler.addProcess(Process(3, 2, 8, 0));

    auto schedule = scheduler.run();

    REQUIRE(schedule.size() == 3);
    REQUIRE(schedule[0].pid == 1);  // First in, first out
}

TEST_CASE("SJF scheduling", "[scheduler][sjf]") {
    Scheduler scheduler(SchedulerType::SJF);

    scheduler.addProcess(Process(1, 0, 10, 0));
    scheduler.addProcess(Process(2, 0, 5, 0));   // Shortest
    scheduler.addProcess(Process(3, 0, 8, 0));

    auto schedule = scheduler.run();

    REQUIRE(schedule[0].pid == 2);  // Shortest job first
}

TEST_CASE("Priority scheduling", "[scheduler][priority]") {
    Scheduler scheduler(SchedulerType::Priority);

    scheduler.addProcess(Process(1, 0, 10, 3));
    scheduler.addProcess(Process(2, 0, 5, 1));   // Highest priority
    scheduler.addProcess(Process(3, 0, 8, 2));

    auto schedule = scheduler.run();

    REQUIRE(schedule[0].pid == 2);  // Highest priority first
}

TEST_CASE("Round Robin scheduling", "[scheduler][rr]") {
    const int TIME_QUANTUM = 2;
    Scheduler scheduler(SchedulerType::RoundRobin, TIME_QUANTUM);

    scheduler.addProcess(Process(1, 0, 5, 0));
    scheduler.addProcess(Process(2, 0, 5, 0));

    auto schedule = scheduler.run();

    // Processes should alternate every TIME_QUANTUM
    REQUIRE(schedule.size() > 2);  // Multiple context switches
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

REQUIRE(avg_waiting_time == Catch::Approx(15.5).epsilon(0.01));
REQUIRE(cpu_utilization == Catch::Approx(0.85).margin(0.05));
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
./cpu_sched_tests "[quick]"     # Run only quick tests
./cpu_sched_tests "[fcfs]"      # Run only FCFS tests
./cpu_sched_tests "~[slow]"     # Skip slow tests
```

### Using Sections

Sections share setup code:

```cpp
TEST_CASE("Scheduler calculations", "[scheduler][metrics]") {
    Scheduler scheduler(SchedulerType::FCFS);
    scheduler.addProcess(Process(1, 0, 10, 0));
    scheduler.addProcess(Process(2, 5, 5, 0));

    SECTION("Waiting time") {
        auto metrics = scheduler.calculateMetrics();
        REQUIRE(metrics.avgWaitingTime > 0);
    }

    SECTION("Turnaround time") {
        auto metrics = scheduler.calculateMetrics();
        REQUIRE(metrics.avgTurnaroundTime > 0);
    }

    SECTION("Response time") {
        auto metrics = scheduler.calculateMetrics();
        REQUIRE(metrics.avgResponseTime >= 0);
    }
}
```

### Fixtures for Shared Setup

```cpp
class SchedulerFixture {
protected:
    Scheduler fcfs{SchedulerType::FCFS};
    Scheduler sjf{SchedulerType::SJF};
    std::vector<Process> testProcesses{
        Process(1, 0, 10, 1),
        Process(2, 1, 5, 2),
        Process(3, 2, 8, 3)
    };
};

TEST_CASE_METHOD(SchedulerFixture, "Compare algorithms", "[integration]") {
    for (const auto& proc : testProcesses) {
        fcfs.addProcess(proc);
        sjf.addProcess(proc);
    }

    auto fcfs_metrics = fcfs.run();
    auto sjf_metrics = sjf.run();

    // SJF should generally have better average waiting time
    REQUIRE(sjf_metrics.avgWaitingTime <= fcfs_metrics.avgWaitingTime);
}
```

## Testing Scheduling Algorithms

### Testing Process State Transitions

```cpp
TEST_CASE("Process state transitions", "[process][state]") {
    Process proc(1, 0, 10, 0);

    REQUIRE(proc.getState() == ProcessState::NEW);

    proc.setState(ProcessState::READY);
    REQUIRE(proc.getState() == ProcessState::READY);

    proc.setState(ProcessState::RUNNING);
    REQUIRE(proc.getState() == ProcessState::RUNNING);

    proc.setState(ProcessState::TERMINATED);
    REQUIRE(proc.getState() == ProcessState::TERMINATED);
}
```

### Testing Metrics Calculations

```cpp
TEST_CASE("Waiting time calculation", "[metrics]") {
    // Process: PID=1, Arrival=0, Burst=5
    // Starts at time 0, completes at time 5
    // Waiting time = Start - Arrival = 0 - 0 = 0

    Process p1(1, 0, 5, 0);
    p1.setStartTime(0);
    p1.setCompletionTime(5);

    REQUIRE(p1.getWaitingTime() == 0);
    REQUIRE(p1.getTurnaroundTime() == 5);
}

TEST_CASE("Turnaround time calculation", "[metrics]") {
    // Process: PID=2, Arrival=3, Burst=4
    // Starts at time 5, completes at time 9
    // Turnaround = Completion - Arrival = 9 - 3 = 6

    Process p2(2, 3, 4, 0);
    p2.setStartTime(5);
    p2.setCompletionTime(9);

    REQUIRE(p2.getTurnaroundTime() == 6);
    REQUIRE(p2.getWaitingTime() == 2);  // 5 - 3
}
```

### Testing Edge Cases

```cpp
TEST_CASE("Empty process queue", "[scheduler][edge]") {
    Scheduler scheduler(SchedulerType::FCFS);

    auto schedule = scheduler.run();

    REQUIRE(schedule.empty());
}

TEST_CASE("Single process", "[scheduler][edge]") {
    Scheduler scheduler(SchedulerType::FCFS);
    scheduler.addProcess(Process(1, 0, 10, 0));

    auto schedule = scheduler.run();

    REQUIRE(schedule.size() == 1);
    REQUIRE(schedule[0].waitingTime == 0);
}

TEST_CASE("Processes with same arrival time", "[scheduler][edge]") {
    Scheduler scheduler(SchedulerType::FCFS);

    scheduler.addProcess(Process(1, 0, 5, 0));
    scheduler.addProcess(Process(2, 0, 3, 0));
    scheduler.addProcess(Process(3, 0, 8, 0));

    auto schedule = scheduler.run();

    REQUIRE(schedule.size() == 3);
}

TEST_CASE("Zero burst time", "[process][edge]") {
    REQUIRE_THROWS_AS(Process(1, 0, 0, 0), std::invalid_argument);
}

TEST_CASE("Negative values", "[process][edge]") {
    REQUIRE_THROWS_AS(Process(-1, 0, 5, 0), std::invalid_argument);
    REQUIRE_THROWS_AS(Process(1, -1, 5, 0), std::invalid_argument);
    REQUIRE_THROWS_AS(Process(1, 0, -5, 0), std::invalid_argument);
}
```

### Testing Algorithm Correctness

```cpp
TEST_CASE("FCFS maintains order", "[fcfs][correctness]") {
    Scheduler scheduler(SchedulerType::FCFS);

    scheduler.addProcess(Process(1, 0, 5, 0));
    scheduler.addProcess(Process(2, 1, 3, 0));
    scheduler.addProcess(Process(3, 2, 2, 0));

    auto schedule = scheduler.run();

    // Verify FCFS order
    REQUIRE(schedule[0].pid == 1);
    REQUIRE(schedule[1].pid == 2);
    REQUIRE(schedule[2].pid == 3);

    // Verify no gaps in execution
    REQUIRE(schedule[0].startTime == 0);
    REQUIRE(schedule[1].startTime == 5);
    REQUIRE(schedule[2].startTime == 8);
}

TEST_CASE("SJF selects shortest job", "[sjf][correctness]") {
    Scheduler scheduler(SchedulerType::SJF);

    scheduler.addProcess(Process(1, 0, 8, 0));
    scheduler.addProcess(Process(2, 0, 4, 0));  // Shortest
    scheduler.addProcess(Process(3, 0, 6, 0));

    auto schedule = scheduler.run();

    REQUIRE(schedule[0].pid == 2);  // Shortest first
    REQUIRE(schedule[1].pid == 3);  // Middle
    REQUIRE(schedule[2].pid == 1);  // Longest last
}

TEST_CASE("Round Robin time quantum", "[rr][correctness]") {
    const int QUANTUM = 3;
    Scheduler scheduler(SchedulerType::RoundRobin, QUANTUM);

    scheduler.addProcess(Process(1, 0, 10, 0));
    scheduler.addProcess(Process(2, 0, 10, 0));

    auto events = scheduler.getExecutionEvents();

    // Verify each execution slice is at most QUANTUM
    for (const auto& event : events) {
        REQUIRE(event.duration <= QUANTUM);
    }
}
```

## Building and Running Tests

### Build and Run Tests

From the repository root:

```bash
make test-cpu           # Build and run CPUSCHED tests
make test-verbose       # Run all tests with verbose output
make test               # Run all tests (both projects)
```

The test executable will be at: `CS471PROJECT/CPUSCHED/cpu_sched_tests`

### Run Tests Directly

After building, you can run the test executable directly:

```bash
cd CS471PROJECT/CPUSCHED
./cpu_sched_tests
```

### Run Specific Tests

```bash
# By test name
./cpu_sched_tests "FCFS scheduling"

# By tag
./cpu_sched_tests "[fcfs]"
./cpu_sched_tests "[scheduler]"
./cpu_sched_tests "[quick]"

# List all tests
./cpu_sched_tests --list-tests

# List all tags
./cpu_sched_tests --list-tags
```

### Verbose Output

```bash
./cpu_sched_tests -s   # Show successful assertions too
./cpu_sched_tests -d   # Show test durations
```

## Test File Examples

### Example: `test_process.cpp`

```cpp
#include <catch2/catch_test_macros.hpp>
#include "process.hpp"

TEST_CASE("Process initialization", "[process][unit]") {
    Process proc(1, 0, 10, 5);

    REQUIRE(proc.getPid() == 1);
    REQUIRE(proc.getArrivalTime() == 0);
    REQUIRE(proc.getBurstTime() == 10);
    REQUIRE(proc.getPriority() == 5);
    REQUIRE(proc.getRemainingTime() == 10);
}

TEST_CASE("Process execution", "[process][unit]") {
    Process proc(1, 0, 10, 0);

    SECTION("Execute partial time") {
        proc.execute(3);
        REQUIRE(proc.getRemainingTime() == 7);
        REQUIRE_FALSE(proc.isComplete());
    }

    SECTION("Execute full time") {
        proc.execute(10);
        REQUIRE(proc.getRemainingTime() == 0);
        REQUIRE(proc.isComplete());
    }

    SECTION("Cannot execute more than remaining") {
        REQUIRE_THROWS(proc.execute(15));
    }
}
```

### Example: `test_fcfs.cpp`

```cpp
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "scheduler.hpp"

TEST_CASE("FCFS with multiple processes", "[fcfs][integration]") {
    Scheduler scheduler(SchedulerType::FCFS);

    // Add processes: (PID, Arrival, Burst, Priority)
    scheduler.addProcess(Process(1, 0, 8, 0));
    scheduler.addProcess(Process(2, 1, 4, 0));
    scheduler.addProcess(Process(3, 2, 9, 0));
    scheduler.addProcess(Process(4, 3, 5, 0));

    auto results = scheduler.run();
    auto metrics = scheduler.getMetrics();

    SECTION("Process order") {
        REQUIRE(results[0].pid == 1);
        REQUIRE(results[1].pid == 2);
        REQUIRE(results[2].pid == 3);
        REQUIRE(results[3].pid == 4);
    }

    SECTION("Start times") {
        REQUIRE(results[0].startTime == 0);
        REQUIRE(results[1].startTime == 8);
        REQUIRE(results[2].startTime == 12);
        REQUIRE(results[3].startTime == 21);
    }

    SECTION("Completion times") {
        REQUIRE(results[0].completionTime == 8);
        REQUIRE(results[1].completionTime == 12);
        REQUIRE(results[2].completionTime == 21);
        REQUIRE(results[3].completionTime == 26);
    }

    SECTION("Metrics are calculated") {
        REQUIRE(metrics.avgWaitingTime >= 0);
        REQUIRE(metrics.avgTurnaroundTime > 0);
        REQUIRE(metrics.cpuUtilization > 0);
        REQUIRE(metrics.cpuUtilization <= 1.0);
    }
}
```

### Example: `test_metrics.cpp`

```cpp
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "scheduler.hpp"

TEST_CASE("Metrics calculation accuracy", "[metrics][unit]") {
    Scheduler scheduler(SchedulerType::FCFS);

    // Simple known case
    scheduler.addProcess(Process(1, 0, 5, 0));  // WT=0, TAT=5
    scheduler.addProcess(Process(2, 0, 5, 0));  // WT=5, TAT=10

    scheduler.run();
    auto metrics = scheduler.getMetrics();

    SECTION("Average waiting time") {
        // (0 + 5) / 2 = 2.5
        REQUIRE(metrics.avgWaitingTime == Catch::Approx(2.5));
    }

    SECTION("Average turnaround time") {
        // (5 + 10) / 2 = 7.5
        REQUIRE(metrics.avgTurnaroundTime == Catch::Approx(7.5));
    }

    SECTION("CPU utilization") {
        // Total burst time / Total time = 10 / 10 = 1.0 (100%)
        REQUIRE(metrics.cpuUtilization == Catch::Approx(1.0));
    }
}
```

## How It Works

The build system automatically:
- Discovers all `.cpp` files in this directory
- Compiles them into the `cpu_sched_tests` executable
- Links with Catch2 testing framework and your library code
- Provides a `main()` function automatically (via `Catch2WithMain`)

Just add new test files and run `make` - no manual configuration needed!

## Best Practices

1. **One concept per test case** - Keep tests focused
2. **Use descriptive names** - "FCFS maintains arrival order" not "test1"
3. **Use tags** - Organize by algorithm, speed, type ([fcfs], [sjf], [rr], [unit], [integration])
4. **Test edge cases** - Empty queues, single process, identical arrival times
5. **Test error conditions** - Invalid inputs should throw/fail gracefully
6. **Keep tests fast** - Use `[slow]` tag for long-running tests
7. **Avoid test interdependence** - Each test should run independently
8. **Test metrics accuracy** - Verify calculations with known examples
9. **Test all algorithms** - Each scheduling algorithm should have comprehensive tests

## Additional Resources

- [Catch2 Tutorial](https://github.com/catchorg/Catch2/blob/devel/docs/tutorial.md)
- [Catch2 Assertion Macros](https://github.com/catchorg/Catch2/blob/devel/docs/assertions.md)
- [Catch2 Command Line](https://github.com/catchorg/Catch2/blob/devel/docs/command-line.md)
- [CPU Scheduling Algorithms](https://www.geeksforgeeks.org/cpu-scheduling-in-operating-systems/)
