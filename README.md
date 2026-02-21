# Operating Systems Course Project

This repository contains implementations for CS 471 Operating Systems course projects.

## Prerequisites

- **Make**
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

```bash
# Build everything
make

# Build specific projects
make cpu-sched
make producer-consumer

# Clean all build artifacts
make clean

# Clean and rebuild
make rebuild

# See all available commands
make help
```

The Makefile will:
- Automatically configure CMake on first build
- Fetch Catch2 testing framework automatically
- Build both CPUSCHED and PRODUCER-CONSUMER projects
- Place executables in their respective project folders

## Running

```bash
make run-cpu          # Build and run CPUSCHED
make run-producer     # Build and run PRODUCER-CONSUMER
```

Or run executables directly (after building):

```bash
# CPU Scheduler
cd CS471PROJECT/CPUSCHED
./cpu_sched [arguments]

# Producer-Consumer
cd CS471PROJECT/PRODUCER-CONSUMER
./producer_consumer [arguments]
```

See individual project READMEs for specific usage instructions and sample inputs/outputs.

## Testing

```bash
make test               # Run all tests
make test-verbose       # Run all tests with verbose output
make test-cpu           # Run CPUSCHED tests only
make test-producer      # Run PRODUCER-CONSUMER tests only
```

Or run test executables directly:

```bash
cd CS471PROJECT/CPUSCHED
./cpu_sched_tests

cd CS471PROJECT/PRODUCER-CONSUMER
./producer_consumer_tests
```

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

## Troubleshooting

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
make clean
```
