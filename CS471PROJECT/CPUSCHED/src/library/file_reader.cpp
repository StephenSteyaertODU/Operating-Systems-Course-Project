#include "common/common.hpp"
#include <fstream>
#include <stdexcept>

namespace cpusched {

std::vector<std::string> readProcessFile(const std::string& filePath) {
    std::ifstream inputFile(filePath);

    if (!inputFile.is_open()) {
        throw std::runtime_error("Could not open file: " + filePath);
    }

    std::vector<std::string> lines;
    std::string line;

    while (std::getline(inputFile, line)) {
        lines.push_back(line);
    }

    inputFile.close();

    return lines;
}

} // namespace cpusched
