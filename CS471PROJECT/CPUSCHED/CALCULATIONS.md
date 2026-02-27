# CPU Scheduling Metrics Calculations <!-- omit in toc -->

This document explains how to calculate the various performance metrics for CPU scheduling algorithms.

- [Core Timing Metrics](#core-timing-metrics)
  - [Completion Time (Exit Time)](#completion-time-exit-time)
  - [Turnaround Time (TAT)](#turnaround-time-tat)
  - [Waiting Time (WT)](#waiting-time-wt)
  - [Response Time (RT)](#response-time-rt)
- [System-Wide Performance Metrics](#system-wide-performance-metrics)
  - [Total Elapsed Time](#total-elapsed-time)
  - [Throughput](#throughput)
  - [CPU Utilization](#cpu-utilization)
  - [Average Waiting Time](#average-waiting-time)
  - [Average Turnaround Time](#average-turnaround-time)
  - [Average Response Time](#average-response-time)
- [Quick Reference Table](#quick-reference-table)
- [Important Notes](#important-notes)


## Core Timing Metrics

### Completion Time (Exit Time)
The absolute time when a process finishes execution.

```
Completion Time = Start Time + CPU Burst Length
```

**Example:**
- Process arrives at time 268
- Starts executing at time 284
- Burst length is 14
- Completion Time = 284 + 14 = **298**

---

### Turnaround Time (TAT)
Total time from when a process arrives until it completes.

```
Turnaround Time = Completion Time - Arrival Time
```

**Example:**
- Arrival Time = 268
- Completion Time = 298
- Turnaround Time = 298 - 268 = **30**

**Meaning:** The process took 30 time units from arrival to completion.

---

### Waiting Time (WT)
Time spent in the ready queue waiting for the CPU (not executing).

```
Waiting Time = Turnaround Time - CPU Burst Length
```

or equivalently:

```
Waiting Time = Start Time - Arrival Time
```

**Example:**
- Turnaround Time = 30
- CPU Burst Length = 14
- Waiting Time = 30 - 14 = **16**

**Meaning:** The process waited 16 time units in the ready queue before executing.

---

### Response Time (RT)
Time from arrival until the process first gets the CPU.

```
Response Time = First CPU Access Time - Arrival Time
```

**For non-preemptive algorithms (FIFO, SJF):**
```
Response Time = Waiting Time
```
(Because the process runs completely once it starts)

**Example:**
- Arrival Time = 268
- First CPU Access = 284
- Response Time = 284 - 268 = **16**

---

## System-Wide Performance Metrics

### Total Elapsed Time
Total simulation time from start to when the last process finishes.

```
Total Elapsed Time = Last Process Completion Time - Simulation Start Time
```

**Example:**
- First process arrives at time 10
- Last process (P514) completes at time 15,500
- Total Elapsed Time = 15,500 - 0 = **15,500**

**Note:** Simulation typically starts at time 0, not at the first arrival time.

---

### Throughput
Number of processes completed per unit of time.

```
Throughput = Total Number of Processes / Total Elapsed Time
```

**Example:**
- 500 processes completed
- Total Elapsed Time = 15,500
- Throughput = 500 / 15,500 = **0.0323 processes per time unit**

**Alternative representation:** Average time per process
```
Average Time per Process = Total Elapsed Time / Total Number of Processes
Average Time per Process = 15,500 / 500 = 31 time units per process
```

---

### CPU Utilization
Percentage of time the CPU is actively executing processes (not idle).

```
CPU Utilization = (Total CPU Busy Time / Total Elapsed Time) × 100%
```

where:
```
Total CPU Busy Time = Sum of all CPU Burst Lengths
```

**Example:**
- Sum of all burst lengths = 10,000
- Total Elapsed Time = 15,500
- CPU Utilization = (10,000 / 15,500) × 100% = **64.5%**

**Meaning:** The CPU was idle 35.5% of the time (gaps when no processes were in the ready queue).

---

### Average Waiting Time
Mean waiting time across all processes.

```
Average Waiting Time = (Sum of all Waiting Times) / Total Number of Processes
```

**Example:**
- Process 1: WT = 0
- Process 2: WT = 0
- Process 3: WT = 16
- Process 4: WT = 30
- Average Waiting Time = (0 + 0 + 16 + 30) / 4 = **11.5**

---

### Average Turnaround Time
Mean turnaround time across all processes.

```
Average Turnaround Time = (Sum of all Turnaround Times) / Total Number of Processes
```

**Example:**
- Process 1: TAT = 22
- Process 2: TAT = 12
- Process 3: TAT = 30
- Process 4: TAT = 60
- Average Turnaround Time = (22 + 12 + 30 + 60) / 4 = **31**

---

### Average Response Time
Mean response time across all processes.

```
Average Response Time = (Sum of all Response Times) / Total Number of Processes
```

For non-preemptive schedulers:
```
Average Response Time = Average Waiting Time
```

---

## Quick Reference Table

| Metric | Formula |
|--------|---------|
| Completion Time | Start Time + Burst Length |
| Turnaround Time | Completion Time - Arrival Time |
| Waiting Time | Turnaround Time - Burst Length |
| Response Time | Start Time - Arrival Time |
| Total Elapsed Time | Last Completion - Start of Simulation |
| Throughput | Processes / Total Elapsed Time |
| CPU Utilization | (Sum of Bursts / Total Elapsed) × 100% |
| Avg Waiting Time | Sum(WT) / Number of Processes |
| Avg Turnaround Time | Sum(TAT) / Number of Processes |
| Avg Response Time | Sum(RT) / Number of Processes |

---

## Important Notes

1. **Time Units:** All times are in "burst units" as specified in the assignment.

2. **Idle Time:** CPU utilization < 100% indicates the CPU was idle during gaps when no processes were available in the ready queue.

3. **FIFO vs SJF:** The calculation formulas are the same for both algorithms. The difference is the order in which processes are selected from the ready queue.

4. **Non-Preemptive:** Both FIFO and SJF are non-preemptive, meaning once a process starts, it runs to completion without interruption.
