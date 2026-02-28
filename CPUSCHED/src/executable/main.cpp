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

        // Read and parse process data from file
        auto processes = cpusched::readProcessFile(dataFilePath);

        std::cout << "Total processes read: " << processes.size() << std::endl;

        // Print first few to verify
        std::cout << "\nFirst 5 processes:" << std::endl;
        std::cout << "ID\tArrival\tBurst" << std::endl;
        for (size_t i = 0; i < std::min(size_t(5), processes.size()); ++i) {
            std::cout << processes[i].id << "\t"
                      << processes[i].arrivalTime << "\t"
                      << processes[i].burstTime << std::endl;
        }

        // TODO: Run FIFO scheduler
        // TODO: Run SJF scheduler
        // TODO: Output results

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
