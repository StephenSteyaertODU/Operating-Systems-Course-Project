CPU Scheduling Simulator – README
Overview
This program simulates a CPU scheduler using two algorithms:
First-In-First-Out (FIFO)
Shortest Job First (SJF, non-preemptive)
It reads process data from an input file and computes scheduling statistics including total elapsed time, throughput, CPU utilization, average waiting time, average turnaround time, and average response time.

Directory Contents
Place the following files in the same directory:
main.cpp (source code)
scheduler or scheduler.exe (compiled executable)
datafile.txt (input file)
results.txt (sample output file)

Input File Format
The input file must be named datafile.txt.
Each line should contain two integers:
<arrival_time> <burst_time>

Example:
10 22
68 12

If a header line is present, it will be ignored by the program.
The program reads up to 500 processes.

Running the Program
Option 1: Run using the provided executable
On Mac/Linux:
./scheduler

On Windows:
scheduler.exe


Option 2: Compile and run manually
If the executable does not run on your system, compile the program using:
g++ main.cpp -o scheduler

Then run:
./scheduler


Program Execution
After starting the program, you will be prompted to choose a scheduling algorithm:
1. FIFO
2. SJF
Enter choice (1 or 2):

Enter:
1 to run FIFO
2 to run SJF (non-preemptive)

Output
The program will:
Display results in the terminal
Write results to results.txt
If results.txt already exists, it will be overwritten.

Statistics Reported
Number of processes
Total elapsed time
Throughput (total burst time divided by number of processes)
CPU utilization (total burst time divided by total elapsed time)
Average waiting time
Average turnaround time
Average response time

Notes
All time values are simulated using CPU burst units.
SJF is non-preemptive, meaning a selected process runs to completion.
The program stops after processing 500 completed processes.

