#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// struct to hold each process info
struct Process {
    int pid;
    int arrivalTime;
    int burstTime;
    int startTime;
    int finishTime;
    int waitingTime;
    int turnaroundTime;
    int responseTime;
};

// just a helper to print the stats at the end
void printStats(vector<Process> processes, string algorithmName) {

    int numProcesses = processes.size();

    // calculate all the totals first
    int totalBurst = 0;
    int totalWaiting = 0;
    int totalTurnaround = 0;
    int totalResponse = 0;

    for (int i = 0; i < numProcesses; i++) {
        totalBurst += processes[i].burstTime;
        totalWaiting += processes[i].waitingTime;
        totalTurnaround += processes[i].turnaroundTime;
        totalResponse += processes[i].responseTime;
    }

    // find the first start time and last finish time
    int firstStart = processes[0].startTime;
    int lastFinish = processes[0].finishTime;

    for (int i = 1; i < numProcesses; i++) {
        if (processes[i].startTime < firstStart) {
            firstStart = processes[i].startTime;
        }
        if (processes[i].finishTime > lastFinish) {
            lastFinish = processes[i].finishTime;
        }
    }

    int totalElapsedTime = lastFinish - firstStart;

    double throughput = (double)totalBurst / numProcesses;
    double cpuUtilization = ((double)totalBurst / totalElapsedTime) * 100;
    double avgWaiting = (double)totalWaiting / numProcesses;
    double avgTurnaround = (double)totalTurnaround / numProcesses;
    double avgResponse = (double)totalResponse / numProcesses;

    // // print to terminal
    // cout << "\n==================================================" << endl;
    // cout << "Algorithm: " << algorithmName << endl;
    // cout << "==================================================" << endl;
    // cout << "Number of processes: " << numProcesses << endl;
    // cout << "Total elapsed time: " << totalElapsedTime << endl;
    // cout << "Throughput: " << throughput << endl;
    // cout << "CPU utilization: " << cpuUtilization << "%" << endl;
    // cout << "Average waiting time: " << avgWaiting << endl;
    // cout << "Average turnaround time: " << avgTurnaround << endl;
    // cout << "Average response time: " << avgResponse << endl;
    // cout << "==================================================" << endl;

    // also save the results to results.txt
    ofstream outputFile("results.txt");

    if (!outputFile) {
        cout << "could not create results.txt" << endl;
        return;
    }

    // outputFile << "\n==================================================" << endl;
    outputFile << "Algorithm: " << algorithmName << endl;
    // outputFile << "==================================================" << endl;
    outputFile << "Number of processes: " << numProcesses << endl;
    outputFile << "Total elapsed time: " << totalElapsedTime << endl;
    outputFile << "Throughput: " << throughput << endl;
    outputFile << "CPU utilization: " << cpuUtilization << "%" << endl;
    outputFile << "Average waiting time: " << avgWaiting << endl;
    outputFile << "Average turnaround time: " << avgTurnaround << endl;
    outputFile << "Average response time: " << avgResponse << endl;
    // outputFile << "==================================================" << endl;

    outputFile.close();
    cout << "\nResults saved to results.txt" << endl;
}

// FIFO scheduling - just run them in order they arrive
vector<Process> runFIFO(vector<Process> processes) {

    // sort by arrival time
    for (int i = 0; i < processes.size(); i++) {
        for (int j = i + 1; j < processes.size(); j++) {
            if (processes[j].arrivalTime < processes[i].arrivalTime) {
                Process temp = processes[i];
                processes[i] = processes[j];
                processes[j] = temp;
            }
        }
    }

    int currentTime = 0;

    for (int i = 0; i < processes.size(); i++) {

        // if cpu is idle wait for next process to arrive
        if (currentTime < processes[i].arrivalTime) {
            currentTime = processes[i].arrivalTime;
        }

        processes[i].startTime = currentTime;
        processes[i].waitingTime = currentTime - processes[i].arrivalTime;
        processes[i].responseTime = currentTime - processes[i].arrivalTime;

        currentTime = currentTime + processes[i].burstTime;

        processes[i].finishTime = currentTime;
        processes[i].turnaroundTime = processes[i].finishTime - processes[i].arrivalTime;
    }

    return processes;
}

// SJF - pick the shortest job that has already arrived
vector<Process> runSJF(vector<Process> processes) {

    // sort by arrival time first
    for (int i = 0; i < processes.size(); i++) {
        for (int j = i + 1; j < processes.size(); j++) {
            if (processes[j].arrivalTime < processes[i].arrivalTime) {
                Process temp = processes[i];
                processes[i] = processes[j];
                processes[j] = temp;
            }
        }
    }

    int currentTime = 0;
    int completed = 0;
    int n = processes.size();
    vector<bool> isDone(n, false);
    vector<Process> finishedProcesses;

    while (completed < n) {

        // find shortest job that has arrived and is not done yet
        int shortestIndex = -1;
        int shortestBurst = 99999;

        for (int i = 0; i < n; i++) {
            if (!isDone[i] && processes[i].arrivalTime <= currentTime) {
                if (processes[i].burstTime < shortestBurst) {
                    shortestBurst = processes[i].burstTime;
                    shortestIndex = i;
                }
            }
        }

        // if nothing is ready yet just move time forward
        if (shortestIndex == -1) {
            currentTime++;
            continue;
        }

        // run the selected process
        processes[shortestIndex].startTime = currentTime;
        processes[shortestIndex].waitingTime = currentTime - processes[shortestIndex].arrivalTime;
        processes[shortestIndex].responseTime = currentTime - processes[shortestIndex].arrivalTime;

        currentTime = currentTime + processes[shortestIndex].burstTime;

        processes[shortestIndex].finishTime = currentTime;
        processes[shortestIndex].turnaroundTime = processes[shortestIndex].finishTime - processes[shortestIndex].arrivalTime;

        isDone[shortestIndex] = true;
        completed++;
        finishedProcesses.push_back(processes[shortestIndex]);
    }

    return finishedProcesses;
}

int main() {

    // read the file
    ifstream inputFile("datafile.txt");

    if (!inputFile) {
        cout << "could not open datafile.txt" << endl;
        return 1;
    }
    //skip header line
    string header;
    getline(inputFile, header);


    vector<Process> allProcesses;
    int arrival, burst;
    int pidCounter = 1;

    while (inputFile >> arrival >> burst) {
        Process p;
        p.pid = pidCounter;
        p.arrivalTime = arrival;
        p.burstTime = burst;
        p.startTime = 0;
        p.finishTime = 0;
        p.waitingTime = 0;
        p.turnaroundTime = 0;
        p.responseTime = 0;
        allProcesses.push_back(p);
        pidCounter++;

        // stop after 500 processes
        if (allProcesses.size() == 500) {
            break;
        }
    }

    inputFile.close();

    cout << "Loaded " << allProcesses.size() << " processes" << endl;

    // ask user which scheduling algorithm to use
    cout << "\nWhich scheduling algorithm?" << endl;
    cout << "1. FIFO" << endl;
    cout << "2. SJF" << endl;
    cout << "Enter choice (1 or 2): ";

    int choice;
    cin >> choice;

    if (choice == 1) {
        vector<Process> result = runFIFO(allProcesses);
        printStats(result, "FIFO");
    } else if (choice == 2) {
        vector<Process> result = runSJF(allProcesses);
        printStats(result, "SJF (Non-Preemptive)");
    } else {
        cout << "invalid choice" << endl;
        return 1;
    }

    return 0;
}
