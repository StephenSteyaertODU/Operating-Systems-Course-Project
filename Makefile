# Makefile for CS471 Course Project
# Convenience wrapper around CMake commands

.PHONY: all build configure clean test run-tests run-cpu run-producer help

# Help target
help:
	@echo "CS471 Course Project - Available Make Targets:"
	@echo ""
	@echo "  make              - Build all projects (default)"
	@echo "  make configure    - Configure CMake (first time setup)"
	@echo "  make build        - Build all projects"
	@echo "  make cpu-sched    - Build CPUSCHED project only"
	@echo "  make producer-consumer - Build PRODUCER-CONSUMER project only"
	@echo ""
	@echo "  make test         - Run all tests"
	@echo "  make test-verbose - Run all tests with verbose output"
	@echo "  make test-cpu     - Run CPUSCHED tests only"
	@echo "  make test-producer - Run PRODUCER-CONSUMER tests only"
	@echo ""
	@echo "  make run-cpu      - Build and run CPUSCHED"
	@echo "  make run-producer - Build and run PRODUCER-CONSUMER"
	@echo ""
	@echo "  make clean        - Remove all build artifacts"
	@echo "  make rebuild      - Clean and rebuild everything"
	@echo "  make help         - Show this help message"

# Default target
all: build

# Configure CMake (first time setup)
configure:
	@echo "Configuring CMake..."
	cmake -S . -B build

# Build all projects
build:
	@echo "Building all projects..."
	cmake --build build

# Build specific targets
cpu-sched:
	@echo "Building CPUSCHED..."
	cmake --build build --target cpu_sched

producer-consumer:
	@echo "Building PRODUCER-CONSUMER..."
	cmake --build build --target producer_consumer

# Build tests only
build-tests:
	@echo "Building tests..."
	cmake --build build --target cpu_sched_tests
	cmake --build build --target producer_consumer_tests

# Run all tests
test: build
	@echo "Running all tests..."
	cd build && ctest --output-on-failure

# Run tests with verbose output
test-verbose: build
	@echo "Running tests (verbose)..."
	cd build && ctest -V

# Run specific test suites
test-cpu: build
	@echo "Running CPUSCHED tests..."
	cd CPUSCHED && ./cpu_sched_tests

test-producer: build
	@echo "Running PRODUCER-CONSUMER tests..."
	cd PRODUCER-CONSUMER && ./producer_consumer_tests

# Run executables
run-cpu: build
	@echo "Running CPUSCHED..."
	cd CPUSCHED && ./cpu_sched

run-producer: build
	@echo "Running PRODUCER-CONSUMER..."
	cd PRODUCER-CONSUMER && ./producer_consumer

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -rf build
	rm -f CPUSCHED/cpu_sched
	rm -f CPUSCHED/cpu_sched_tests
	rm -f PRODUCER-CONSUMER/producer_consumer
	rm -f PRODUCER-CONSUMER/producer_consumer_tests

# Full rebuild
rebuild: clean configure build
