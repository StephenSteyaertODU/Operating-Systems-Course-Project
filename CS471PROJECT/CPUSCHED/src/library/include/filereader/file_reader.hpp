#pragma once

#include <string>
#include <vector>

namespace cpusched {

/**
 * Reads lines from a file.
 *
 * @param filePath Path to the data file
 * @return Vector of strings, one per line
 * @throws std::runtime_error if file cannot be opened
 */
std::vector<std::string> readProcessFile(const std::string& filePath);

} // namespace cpusched
