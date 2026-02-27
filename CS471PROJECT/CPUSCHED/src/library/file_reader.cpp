#include "common/common.hpp"
#include <fstream>
#include <stdexcept>
#include <sstream>

namespace cpusched {

std::vector<Process> readProcessFile(const std::string& filePath) {
    std::ifstream inputFile(filePath);

    if (!inputFile.is_open()) {
        throw std::runtime_error("Could not open file: " + filePath);
    }

    std::vector<Process> processes;
    std::string line;

    // Skip header line
    std::getline(inputFile, line);

    pid_t processId = 1;

    while (std::getline(inputFile, line)) {
        if (line.empty()) {
            continue;  // Skip empty lines
        }

        std::istringstream iss(line);
        Process process;
        process.id = processId++;

        if (!(iss >> process.arrivalTime >> process.burstTime)) {
            throw std::runtime_error("Invalid data format in line: " + line);
        }

        processes.emplace_back(process);
    }

    inputFile.close();

    return processes;
}

} // namespace cpusched
