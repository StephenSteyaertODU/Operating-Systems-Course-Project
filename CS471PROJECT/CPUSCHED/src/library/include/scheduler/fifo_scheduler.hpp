#pragma once

#include "common/common.hpp"
#include <vector>

namespace cpusched {

/**
 * Runs the FIFO (First In First Out) scheduling algorithm.
 *
 * FIFO executes processes in the order they arrive. Each process runs to completion
 * before the next one starts.
 *
 * This function modifies the Process structs in-place, calculating:
 * - completionTime: When the process finishes execution
 * - turnaroundTime: completionTime - arrivalTime
 * - waitingTime: turnaroundTime - burstTime
 *
 * @param processes Vector of Process structs to schedule (will be modified)
 *
 * @note The processes should already be sorted by arrival time.
 *       If not sorted, the function will sort them first.
 */
void runFIFO(std::vector<Process>& processes);

} // namespace cpusched
