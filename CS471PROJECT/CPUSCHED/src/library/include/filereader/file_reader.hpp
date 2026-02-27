#pragma once

#include <string>
#include <vector>
#include "common/common.hpp"

namespace cpusched {

/**
 * Reads and parses process data from a file.
 *
 * @param filePath Path to the data file (tab-separated: ArrivalTime, CPUBurstLength)
 * @return Vector of Process structs with sequential IDs starting from 1
 * @throws std::runtime_error if file cannot be opened or data format is invalid
 */
std::vector<Process> readProcessFile(const std::string& filePath);

} // namespace cpusched
