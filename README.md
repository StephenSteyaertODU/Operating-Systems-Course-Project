# Operating Systems Course Project

This repository contains implementations for CS 471 Operating Systems course projects.

## Prerequisites

- **Make**
- **CMake** (version 3.16 or higher)
- **C++20 compatible compiler**
  - GCC 10+ or Clang 10+ on Linux/macOS
  - MSVC 2019+ on Windows (AppleClang 17+ for macOS)
- **Git** (for cloning and fetching dependencies)

## Project Structure

```
course-project/
├── CPUSCHED/                  # CPU Scheduling Algorithms (Project 1)
│   ├── src/
│   │   ├── library/           # Core scheduling logic
│   │   │   ├── include/
│   │   │   │   ├── common/    # Common types (Process, SchedulerMetrics)
│   │   │   │   ├── filereader/ # File parsing
│   │   │   │   └── scheduler/ # FIFO, SJF schedulers
│   │   │   └── *.cpp          # Implementation files
│   │   └── executable/        # Main program
│   ├── tests/                 # Unit tests with Catch2
│   ├── data/                  # Input data files
│   ├── cpu_sched              # Built executable (after build)
│   ├── cpu_sched_tests        # Built test executable (after build)
│   ├── CALCULATIONS.md        # CPU scheduling formulas reference
│   └── README.md              # Project-specific README
├── PRODUCER-CONSUMER/         # Producer-Consumer Problem (Project 2)
│   ├── src/
│   │   ├── library/           # Core logic
│   │   └── executable/        # Main program
│   ├── tests/                 # Unit tests
│   ├── producer_consumer      # Built executable (after build)
│   ├── producer_consumer_tests # Built test executable (after build)
│   └── README.md              # Project-specific README
├── build/                     # CMake build directory (generated)
├── CMakeLists.txt             # Root CMake configuration
├── README.md                  # This file
└── video.mp4                  # Our video recording to be added later
```

## Building

```bash
# Build everything
make

# Build specific projects
make build-cpu
make build-producer

# Clean all build artifacts
make clean

# Clean and rebuild
make rebuild
```

The build system will:
- Automatically configure CMake on first build
- Fetch Catch2 v3.8.0 testing framework automatically
- Build both CPUSCHED and PRODUCER-CONSUMER projects
- Place executables in their respective project folders
- Generate `compile_commands.json` for IDE support

## Running

```bash
make run-cpu          # Build and run CPUSCHED
make run-producer     # Build and run PRODUCER-CONSUMER
```

Or run executables directly (after building):

```bash
# CPU Scheduler
cd CPUSCHED
./cpu_sched

# Producer-Consumer
cd PRODUCER-CONSUMER
./producer_consumer
```

See individual project READMEs for specific usage instructions.

## Testing

```bash
make test               # Run all tests
make test-cpu           # Run CPUSCHED tests only
make test-producer      # Run PRODUCER-CONSUMER tests only
```

Or run test executables directly:

```bash
cd CPUSCHED
./cpu_sched_tests

cd PRODUCER-CONSUMER
./producer_consumer_tests
```

Tests use Catch2 framework with assertions to verify:
- File reading and parsing
- Process struct calculations
- Scheduler correctness
- Edge cases and error handling

## Project Status

### CPUSCHED (CPU Scheduling Algorithms)

**Completed:**
- ✅ Process data structure with timing fields
- ✅ File reader with parsing and validation
- ✅ Comprehensive test suite (25 assertions, 8 test cases)
- ✅ System-wide metrics calculation (avg times, throughput, CPU utilization)
- ✅ Common utilities and helper functions

**In Progress:**
- 🚧 FIFO (First In First Out) scheduler implementation
- 🚧 SJF (Shortest Job First) scheduler implementation
- 🚧 Output formatting and file generation

**Formulas Implemented:**
- Turnaround Time = Completion Time - Arrival Time
- Waiting Time = Turnaround Time - Burst Time
- Response Time = Waiting Time (for non-preemptive)
- CPU Utilization = (Total Burst Time / Total Elapsed Time) × 100%
- Throughput = Number of Processes / Total Elapsed Time

See [CPUSCHED/CALCULATIONS.md](CPUSCHED/CALCULATIONS.md) for detailed formulas.

### PRODUCER-CONSUMER

**Status:** Pending implementation

## Development

### Adding New Source Files

Source files are automatically discovered. Just add `.cpp` files to:
- `src/library/` for library code
- `src/executable/` for main programs
- `tests/` for test files

Then rebuild:

```bash
make
```

### Project Architecture

**CPUSCHED uses a modular design:**
- `common/common.hpp` - Umbrella header including all components
- `Process` struct - Core data type with calculated fields
- `SchedulerMetrics` struct - System-wide performance metrics
- `calculateMetrics()` - Computes averages and utilization
- Schedulers modify Process objects in-place, then calculate metrics

**Benefits:**
- Single header include for entire library
- Reusable metrics calculation across schedulers
- Clear separation of concerns
- Easy to test individual components

## Troubleshooting

### VSCode IntelliSense errors (red squiggles)

If you see `#include` errors in VSCode after building successfully:

1. Open Command Palette (`Cmd+Shift+P` or `Ctrl+Shift+P`)
2. Run: **"C/C++: Reset IntelliSense Database"**
3. Run: **"C/C++: Reload IntelliSense Database"**
4. Or run: **"Developer: Reload Window"**

The project is configured with:
- `compile_commands.json` symlink in root
- CMake Tools integration
- C/C++ extension configured to use CMake provider

### CMake/Catch2 build errors

If you encounter Catch2 header errors during build:

```bash
rm -rf build
make
```

This cleans the CMake cache and refetches Catch2 v3.8.0.

### CMake version error
```bash
cmake --version  # Check your version (needs 3.16+)
```

### Compiler not found
Ensure C++ compiler is in PATH:
```bash
g++ --version    # or clang++ --version
```

### Clean everything
```bash
make clean       # Remove build artifacts
rm -rf build     # Nuclear option: remove entire build directory
```

## IDE Support

### VSCode

The project includes `.vscode/settings.json` configured for:
- CMake Tools extension
- C/C++ IntelliSense
- Automatic compile commands detection
- Build directory configuration

Recommended extension:
- **C/C++ Extension Pack** (ms-vscode.cpptools-extension-pack)
