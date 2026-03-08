# CS471 Course Project TODO List

**Due Date:** May 4, 2026
**Total Points:** 100 (50 per problem)
**Video Requirement:** Max 10 minutes demonstration (20% penalty if missing)

---

## Problem 1: CPU Scheduling (50 points)

### Requirements
- Simulate CPU scheduler for **500 processes**
- Process format: `<Arrival time, CPU burst units>`
- Implement two algorithms:
  1. **FIFO** (First In First Out)
  2. **SJF** (Shortest Job First - non-preemptive)

### Scheduler Implementation
- [ ] Implement FIFO scheduler (`fifo_scheduler.cpp`)
  - [ ] Sort processes by arrival time
  - [ ] Simulate execution (track current time, handle idle time)
  - [ ] Calculate completion times for each process
  - [ ] Call `calculateTimes()` for each process
  - [ ] Test with sample data
- [ ] Write tests for FIFO scheduler
  - [ ] Test with simple 3-process scenario
  - [ ] Test with processes that have gaps (idle time)
  - [ ] Test with 500 processes from data file
  - [ ] Verify all metrics are calculated correctly
- [ ] Implement SJF scheduler (`sjf_scheduler.hpp` and `sjf_scheduler.cpp`)
  - [ ] Create header file with function signature
  - [ ] Sort ready processes by burst time
  - [ ] Handle arrival time ordering
  - [ ] Simulate execution with SJF selection
  - [ ] Calculate completion times
- [ ] Write tests for SJF scheduler
  - [ ] Test basic SJF ordering
  - [ ] Test with same arrival times (pure SJF)
  - [ ] Test with different arrival times
  - [ ] Test with 500 processes from data file

### Output Requirements
Must print these statistics for each run:
- [ ] Implement output formatting
  - [ ] Number of processes: 500
  - [ ] Total elapsed time (simulation start to last completion)
  - [ ] Throughput (processes executed per CPU burst unit)
  - [ ] CPU utilization (%)
  - [ ] Average waiting time
  - [ ] Average turnaround time
  - [ ] Average response time

### Data & Testing
- [x] Sample input data file (500 processes) - **DONE** ([data/datafile.txt](CPUSCHED/data/datafile.txt))
- [ ] Generate sample output for FIFO
- [ ] Generate sample output for SJF
- [ ] Verify all formulas match specification

### Documentation
- [ ] Update CPUSCHED README with:
  - [ ] Copy-paste commands to build
  - [ ] Copy-paste commands to run
  - [ ] Explanation of output format
  - [ ] Example run with interpretation

---

## Problem 2: Producer-Consumer (50 points)

### Requirements
- **p producers** and **c consumers** (separate threads)
- **Shared buffer** of size b
- Sales records: `<DD/MM/YY, Store ID, Register#, Sale Amount>`
- Generate **1000 total items** across all producers
- Each producer sleeps randomly 5-40ms between records

### Record Format
- **DD:** 1-30 (day)
- **MM:** 01-12 (month)
- **YY:** 16 (fixed year)
- **Store ID:** 1 to p (one per producer)
- **Register#:** 1-6 (random per store)
- **Sale Amount:** $0.50 to $999.99 (random)

### Implementation Tasks
- [ ] Design thread-safe buffer structure
  - [ ] Define SalesRecord struct
  - [ ] Create circular buffer with size b
  - [ ] Add thread synchronization primitives
- [ ] Implement producer threads (random sales record generation)
  - [ ] Random DD (1-30), MM (01-12), YY (16)
  - [ ] Random register# (1-6)
  - [ ] Random sale amount ($0.50-$999.99)
  - [ ] Random sleep (5-40ms)
  - [ ] Track total items produced (shared counter)
- [ ] Implement consumer threads (statistics computation)
  - [ ] Read sales records from buffer
  - [ ] Compute local statistics (store-wide, month-wise, aggregate)
  - [ ] Wait for completion flag
  - [ ] Add to global statistics
  - [ ] Print local statistics
- [ ] Implement semaphore/mutex synchronization
  - [ ] Mutex for buffer access
  - [ ] Semaphore for empty slots
  - [ ] Semaphore for filled slots
  - [ ] Mutex for shared item counter
  - [ ] Mutex for global statistics
- [ ] Implement local statistics per consumer
- [ ] Implement global statistics aggregation
- [ ] Implement special flag for completion signal

### Statistics Required
Each consumer maintains local stats, then adds to global:
- [ ] Store-wide total sales (per store)
- [ ] Month-wise total sales (12 months, all stores)
- [ ] Aggregate sales (grand total)
- [ ] Total simulation time (begin to end)

### Testing Runs (9 combinations)
Run with all combinations of p (producers) and c (consumers):
- [ ] p=2, c=2
- [ ] p=2, c=5
- [ ] p=2, c=10
- [ ] p=5, c=2
- [ ] p=5, c=5
- [ ] p=5, c=10
- [ ] p=10, c=2
- [ ] p=10, c=5
- [ ] p=10, c=10

### Documentation
- [ ] Write PRODUCER-CONSUMER README with:
  - [ ] Copy-paste commands to build
  - [ ] Copy-paste commands to run with different p/c values
  - [ ] Explanation of synchronization approach
  - [ ] Table comparing all 9 run results
- [ ] Clearly document all shared variables
- [ ] Clearly document all semaphores/mutexes
- [ ] Generate sample output files for all 9 runs

---

## Submission Requirements

### Directory Structure (REQUIRED)
```
root/
├── video.mp4                    # Max 10 minutes, shows execution
├── CPUSCHED/
│   ├── README.md                # With copy-paste commands
│   ├── src/                     # Well documented source code
│   ├── cpu_sched                # Executable
│   ├── data/                    # Sample input (500 processes)
│   └── output/                  # Sample outputs (FIFO & SJF)
└── PRODUCER-CONSUMER/
    ├── README.md                # With copy-paste commands
    ├── src/                     # Well documented source code
    ├── producer_consumer        # Executable
    └── output/                  # Sample outputs (9 runs)
```

### Submission Checklist
- [ ] README files have actual copy-paste commands
- [ ] Source code is well documented
- [ ] Executables build and run successfully
- [ ] Sample input files included
- [ ] Sample output files included
- [ ] Video recorded (max 10 min)
- [ ] Video shows code execution
- [ ] Video explains implementation

---

## Current Progress

### CPUSCHED Foundation Complete
- Process data structure with timing fields
- File reader parsing 513 processes
- Comprehensive test suite (25 assertions, 8 test cases)
- System-wide metrics calculation functions
- Helper functions (calculateMetrics, calculateTimes)
- FIFO scheduler - **IN PROGRESS** (header done, need cpp)
- SJF scheduler - **PENDING**
- Output generation - **PENDING**

### PRODUCER-CONSUMER Not Started
- Design phase
- Implementation
- Testing (9 combinations)

---

## Formulas Reference (from specification)

### CPU Scheduling Metrics (Problem 1)
```
Total elapsed time = Last process completion time - Simulation start (0)

Throughput = Total burst time / Total number of processes

CPU Utilization = (Total burst time / Total elapsed time) × 100%

Waiting time = Turnaround time - Burst time

Turnaround time = Exit time - Arrival time

Response time = First CPU access time - Arrival time
(For non-preemptive: Response time = Waiting time)
```

### Implementation Notes
- All times are in CPU burst units, not actual milliseconds
- Can read all 500 processes into memory at once
- Process format: `<Arrival time, CPU burst units>` (both integers)
- Need to handle idle time (when no process is ready)

---

## Next Steps (Priority Order)

1. **Implement FIFO scheduler cpp file** **(CURRENT TASK)**
2. Test FIFO with 500 process sample data
3. Generate FIFO sample output
4. Implement SJF scheduler (hpp and cpp)
5. Test SJF and generate sample output
6. Update CPUSCHED README with commands
7. Start Producer-Consumer design
8. Implement Producer-Consumer
9. Run all 9 test combinations (p/c variations)
10. Record video demonstration
11. Package final submission
