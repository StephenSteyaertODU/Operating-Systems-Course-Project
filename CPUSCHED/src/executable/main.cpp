#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <climits>

using namespace std;

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

// UPDATED: now takes filename
void printStats(vector<Process> processes, string algorithmName, string outputFileName) {

    int numProcesses = processes.size();

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

    ofstream outputFile(outputFileName);

    if (!outputFile) {
        cout << "Could not create " << outputFileName << endl;
        return;
    }

    outputFile << "\n==================================================" << endl;
    outputFile << "Algorithm: " << algorithmName << endl;
    outputFile << "==================================================" << endl;
    outputFile << "Number of processes: " << numProcesses << endl;
    outputFile << "Total elapsed time: " << totalElapsedTime << endl;
    outputFile << "Throughput: " << throughput << endl;
    outputFile << "CPU utilization: " << cpuUtilization << "%" << endl;
    outputFile << "Average waiting time: " << avgWaiting << endl;
    outputFile << "Average turnaround time: " << avgTurnaround << endl;
    outputFile << "Average response time: " << avgResponse << endl;
    outputFile << "==================================================" << endl;

    outputFile.close();
    // extract just the filename (remove ../../)
string displayName = outputFileName;

// find last slash and trim everything before it
size_t pos = displayName.find_last_of("/\\");
if (pos != string::npos) {
    displayName = displayName.substr(pos + 1);
}

cout << "\nResults saved to " << displayName 
     << " in the CPUSCHED directory." << endl;
}

vector<Process> runFIFO(vector<Process> processes) {

    sort(processes.begin(), processes.end(),
         [](Process a, Process b) { return a.arrivalTime < b.arrivalTime; });

    int currentTime = 0;

    for (int i = 0; i < processes.size(); i++) {

        if (currentTime < processes[i].arrivalTime) {
            currentTime = processes[i].arrivalTime;
        }

        processes[i].startTime = currentTime;
        processes[i].waitingTime = currentTime - processes[i].arrivalTime;
        processes[i].responseTime = currentTime - processes[i].arrivalTime;

        currentTime += processes[i].burstTime;

        processes[i].finishTime = currentTime;
        processes[i].turnaroundTime = processes[i].finishTime - processes[i].arrivalTime;
    }

    return processes;
}

vector<Process> runSJF(vector<Process> processes) {

    sort(processes.begin(), processes.end(),
         [](Process a, Process b) { return a.arrivalTime < b.arrivalTime; });

    int currentTime = 0;
    int completed = 0;
    int n = processes.size();

    vector<bool> isDone(n, false);
    vector<Process> finishedProcesses;

    while (completed < n) {

        int shortestIndex = -1;
        int shortestBurst = INT_MAX;

        for (int i = 0; i < n; i++) {
            if (!isDone[i] && processes[i].arrivalTime <= currentTime) {
                if (processes[i].burstTime < shortestBurst) {
                    shortestBurst = processes[i].burstTime;
                    shortestIndex = i;
                }
            }
        }

        if (shortestIndex == -1) {
            currentTime++;
            continue;
        }

        processes[shortestIndex].startTime = currentTime;
        processes[shortestIndex].waitingTime = currentTime - processes[shortestIndex].arrivalTime;
        processes[shortestIndex].responseTime = currentTime - processes[shortestIndex].arrivalTime;

        currentTime += processes[shortestIndex].burstTime;

        processes[shortestIndex].finishTime = currentTime;
        processes[shortestIndex].turnaroundTime =
            processes[shortestIndex].finishTime - processes[shortestIndex].arrivalTime;

        isDone[shortestIndex] = true;
        completed++;
        finishedProcesses.push_back(processes[shortestIndex]);
    }

    return finishedProcesses;
}

int main(int argc, char* argv[]) {

    string filePath;

    // ARGUMENT HANDLING
    if (argc == 1) {
        filePath = "../../data/datafile.txt";
    }
    else if (argc == 2) {
        filePath = "../../data/" + string(argv[1]);
    }
    else {
        cout << "Usage: scheduler.exe [optional_input_file]" << endl;
        return 1;
    }

    ifstream inputFile(filePath);

    if (!inputFile) {
        cout << "Error: Could not find file: " << filePath << endl;
        cout << "Make sure the file is inside the /data directory." << endl;
        return 1;
    }

    string header;
    getline(inputFile, header);

    vector<Process> allProcesses;
    int arrival, burst;
    int pidCounter = 1;

    while (inputFile >> arrival >> burst) {
        Process p;
        p.pid = pidCounter++;
        p.arrivalTime = arrival;
        p.burstTime = burst;
        p.startTime = 0;
        p.finishTime = 0;
        p.waitingTime = 0;
        p.turnaroundTime = 0;
        p.responseTime = 0;
        allProcesses.push_back(p);

        if (allProcesses.size() == 500) break;
    }

    inputFile.close();

   // extract just the filename for clean display
string displayFile = filePath;

size_t pos = displayFile.find_last_of("/\\");
if (pos != string::npos) {
    displayFile = displayFile.substr(pos + 1);
}

    cout << "Loaded " << allProcesses.size() << " processes from " << displayFile << endl;  
    // UPDATED MENU
    cout << "\nWhich scheduling algorithm?" << endl;
    cout << "1. FIFO" << endl;
    cout << "2. SJF" << endl;
    cout << "3. BOTH" << endl;
    cout << "Enter choice (1, 2, or 3): ";

    int choice;
    cin >> choice;

    if (choice == 1) {
        auto result = runFIFO(allProcesses);
        printStats(result, "FIFO", "../../FIFOresults.txt");
    }
    else if (choice == 2) {
        auto result = runSJF(allProcesses);
        printStats(result, "SJF (Non-Preemptive)", "../../SJFresults.txt");
    }
    else if (choice == 3) {
        auto fifoResult = runFIFO(allProcesses);
        printStats(fifoResult, "FIFO", "../../FIFOresults.txt");

        auto sjfResult = runSJF(allProcesses);
        printStats(sjfResult, "SJF (Non-Preemptive)", "../../SJFresults.txt");
    }
    else {
        cout << "Invalid choice." << endl;
        return 1;
    }

    return 0;
}