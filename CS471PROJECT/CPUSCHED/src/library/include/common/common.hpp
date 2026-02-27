#pragma once

#include <sys/types.h>

namespace cpusched {

struct Process {
    pid_t id;
    int arrivalTime;
    int burstTime;

    // Fields calculated by scheduler
    int completionTime = 0;
    int turnaroundTime = 0;
    int waitingTime = 0;
};

} // namespace cpusched

#include "filereader/file_reader.hpp"
