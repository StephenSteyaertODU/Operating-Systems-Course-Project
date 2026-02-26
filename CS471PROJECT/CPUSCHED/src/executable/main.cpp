#include "common/common.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    // Default data file path
    std::string dataFilePath = "data/datafile.txt";

    // Allow user to specify data file via command line
    if (argc > 1) {
        dataFilePath = argv[1];
    }

    try {
        std::cout << "Reading from: " << dataFilePath << std::endl;

        // Read all lines from file
        auto lines = cpusched::readProcessFile(dataFilePath); 

        std::cout << "Total lines read: " << lines.size() << std::endl;

        // Print first few to verify
        std::cout << "\nFirst 5 lines:" << std::endl;
        for (size_t i = 0; i < std::min(size_t(5), lines.size()); ++i) {
            std::cout << lines[i] << std::endl;
        }

        // TODO: Parse the data into Process structs
        // TODO: Run FIFO scheduler
        // TODO: Run SJF scheduler
        // TODO: Output results

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
