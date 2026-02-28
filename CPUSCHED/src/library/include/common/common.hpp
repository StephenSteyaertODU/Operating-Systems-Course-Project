#pragma once

#include <sys/types.h>
#include <vector>
#include <algorithm>

namespace cpusched {

struct Process {
    pid_t id;
    int arrivalTime;
    int burstTime;

    // Fields calculated by scheduler
    int completionTime = 0;
    int turnaroundTime = 0;
    int waitingTime = 0;
    int responseTime = 0;  // For non-preemptive: same as waitingTime

    /**
     * Calculate turnaround, waiting, and response times based on completion time.
     * Must be called after completionTime has been set by the scheduler.
     *
     * Formulas:
     * - Turnaround Time = Completion Time - Arrival Time
     * - Waiting Time = Turnaround Time - Burst Time
     * - Response Time = Waiting Time (for non-preemptive algorithms)
     */
    void calculateTimes() {
        turnaroundTime = completionTime - arrivalTime;
        waitingTime = turnaroundTime - burstTime;
        responseTime = waitingTime;  // Non-preemptive: response = waiting
    }
};

/**
 * System-wide performance metrics for a scheduling algorithm.
 */
struct SchedulerMetrics {
    double avgWaitingTime = 0.0;
    double avgTurnaroundTime = 0.0;
    double avgResponseTime = 0.0;
    int totalElapsedTime = 0;
    double throughput = 0.0;
    double cpuUtilization = 0.0;
};

/**
 * Calculate system-wide performance metrics from a completed schedule.
 *
 * @param processes Vector of processes with calculated times
 * @return SchedulerMetrics containing all system-wide performance metrics
 */
inline SchedulerMetrics calculateMetrics(const std::vector<Process>& processes) {
    if (processes.empty()) {
        return SchedulerMetrics{};
    }

    SchedulerMetrics metrics;

    // Calculate averages
    double totalWaiting = 0.0;
    double totalTurnaround = 0.0;
    double totalResponse = 0.0;
    int totalBurstTime = 0;
    int maxCompletionTime = 0;

    for (const auto& p : processes) {
        totalWaiting += p.waitingTime;
        totalTurnaround += p.turnaroundTime;
        totalResponse += p.responseTime;
        totalBurstTime += p.burstTime;
        maxCompletionTime = std::max(maxCompletionTime, p.completionTime);
    }

    size_t n = processes.size();
    metrics.avgWaitingTime = totalWaiting / n;
    metrics.avgTurnaroundTime = totalTurnaround / n;
    metrics.avgResponseTime = totalResponse / n;

    // Total elapsed time (simulation starts at time 0)
    metrics.totalElapsedTime = maxCompletionTime;

    // Throughput (processes per time unit)
    metrics.throughput = static_cast<double>(n) / maxCompletionTime;

    // CPU Utilization (percentage of time CPU was busy)
    metrics.cpuUtilization = (static_cast<double>(totalBurstTime) / maxCompletionTime) * 100.0;

    return metrics;
}

} // namespace cpusched

#include "filereader/file_reader.hpp"
#include "scheduler/fifo_scheduler.hpp"
