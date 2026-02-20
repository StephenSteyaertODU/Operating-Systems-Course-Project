# Operating Systems Course Project

This repository contains implementations for CS 471 Operating Systems course projects.

## Prerequisites

- **CMake** (version 3.16 or higher)
- **C++20 compatible compiler**
  - GCC 10+ or Clang 10+ on Linux/macOS
  - MSVC 2019+ on Windows
- **Git** (for cloning and fetching dependencies)

## Project Structure

```
course-project/
├── CS471PROJECT/
│   ├── CPUSCHED/              # CPU Scheduling Algorithms
│   │   ├── src/               # Source code
│   │   ├── tests/             # Unit tests
│   │   ├── cpu_sched          # Built executable (after build)
│   │   └── README.md          # Project-specific README
│   └── PRODUCER-CONSUMER/     # Producer-Consumer Problem
│       ├── src/               # Source code
│       ├── tests/             # Unit tests
│       ├── producer_consumer  # Built executable (after build)
│       └── README.md          # Project-specific README
└── README.md                  # This file
```

## Building

### Build All Projects

From the repository root:

```bash
cd CS471PROJECT
cmake -S . -B build
cmake --build build
```

This will:
- Fetch Catch2 testing framework automatically
- Build both CPUSCHED and PRODUCER-CONSUMER projects
- Place executables in their respective project folders

### Build Individual Projects

To build only one project:

```bash
cd CS471PROJECT
cmake -S . -B build
cmake --build build --target cpu_sched              # CPUSCHED executable
cmake --build build --target producer_consumer      # PRODUCER-CONSUMER executable
```

### Clean Build

To start fresh:

```bash
cd CS471PROJECT
rm -rf build
cmake -S . -B build
cmake --build build
```

## Running

After building, executables are located in each project's folder:

### CPU Scheduler
```bash
cd CS471PROJECT/CPUSCHED
./cpu_sched [arguments]
```

### Producer-Consumer
```bash
cd CS471PROJECT/PRODUCER-CONSUMER
./producer_consumer [arguments]
```

See individual project READMEs for specific usage instructions and sample inputs/outputs.

## Testing

### Run All Tests

```bash
cd CS471PROJECT/build
ctest
```

Or run test executables directly:

```bash
cd CS471PROJECT/CPUSCHED
./cpu_sched_tests

cd CS471PROJECT/PRODUCER-CONSUMER
./producer_consumer_tests
```

### Run Specific Tests

```bash
cd CS471PROJECT/build
ctest -R cpu_sched        # Run only CPUSCHED tests
ctest -R producer         # Run only PRODUCER-CONSUMER tests
ctest -V                  # Verbose output
```

## Development

### IDE Setup

After building, `compile_commands.json` is generated in the build directory for IDE integration:

```bash
# Link to root for IDE discovery
ln -s CS471PROJECT/build/compile_commands.json .
```

### Adding New Source Files

Source files are automatically discovered using `GLOB_RECURSE`. Just add `.cpp` files to:
- `src/library/` for library code
- `src/executable/` for main programs
- `tests/` for test files

Then rebuild:

```bash
cmake --build CS471PROJECT/build
```

## Troubleshooting

### CMake version error
```bash
cmake --version  # Check your version
# Upgrade if needed
```

### Compiler not found
Ensure C++ compiler is in PATH:
```bash
g++ --version    # or clang++ --version
```

### Clean everything
```bash
cd CS471PROJECT
rm -rf build cpu_sched cpu_sched_tests producer_consumer producer_consumer_tests
```
