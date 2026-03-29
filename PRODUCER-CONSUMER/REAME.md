## Building the Program

From the project root, build just the producer-consumer project:

```
make producer-consumer
```

Or build everything:

```
make
```

First time setup (if the `build` directory doesn't exist yet):

```
make configure
make
```

---

## Running the Program

### Using Make (recommended)

From the project root:

```
make run-producer
```

This builds and runs all 9 required producer/consumer combinations.

---

### Running Directly

Navigate to the `PRODUCER-CONSUMER` directory:

```
cd PRODUCER-CONSUMER
```

#### Running All 9 Required Combinations

If no arguments are provided, the program runs all 9 required producer/consumer combinations (p=2,5,10 × c=2,5,10) with a buffer size of 10:

```
./producer_consumer
```

Example output:
```
========================================
producers=2  consumers=2  buffer=10
========================================

--- Consumer 1 ---
  Records processed : 500
  Sales by store:
    Store 1 : $133769.73
    Store 2 : $112032.17
  Sales by month:
    Month  1 : $24146.15
    ...
  Aggregate         : $245801.90

=== Global Statistics ===
Sales by store:
  Store 1 : $255476.99
  Store 2 : $245195.46
Sales by month:
  Month  1 : $44566.71
  ...
Grand total : $500672.46

Total simulation time: 12.813s

...

--- Timing Comparison (seconds) ---
       p \ c       c=2       c=5      c=10
------------------------------------------
         p=2    12.813    13.038    12.510
         p=5     5.074     5.085     5.120
        p=10     2.615     2.536     2.617
```

---

#### Running a Single Custom Simulation

You may provide exactly three arguments — producers, consumers, and buffer size:

```
./producer_consumer <producers> <consumers> <buffer_size>
```

Example:
```
./producer_consumer 4 3 20
```

All three arguments must be positive integers.

---

### Invalid Usage

If the wrong number of arguments is provided, the program will display usage and exit:

```
Usage: ./producer_consumer <producers> <consumers> <buffer_size>
       ./producer_consumer   (no args — runs all 9 required combinations)
```

---

## Running Tests

From the project root:

```
make test-producer
```

Or with verbose output:

```
make test-verbose
```

---

## Output File Behavior

| Mode | Output File |
|------|-------------|
| No arguments (all 9 runs) | `report.txt` |
| Single custom run | `report_single.txt` |

Both files are saved in the `PRODUCER-CONSUMER` directory. Output is also printed to the terminal simultaneously. If the file already exists it will be overwritten with the new results.
