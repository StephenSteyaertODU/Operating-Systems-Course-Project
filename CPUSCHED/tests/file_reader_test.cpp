#include <catch2/catch_test_macros.hpp>
#include "common/common.hpp"
#include <fstream>
#include <filesystem>

TEST_CASE("readProcessFile parses valid process data", "[file_reader]") {
    const std::string testFilePath = "test_data.txt";

    {
        std::ofstream testFile(testFilePath);
        testFile << "ArrivalTime\tCPUBurstLength\n";
        testFile << "10\t22\n";
        testFile << "68\t12\n";
        testFile << "98\t34\n";
    }

    auto processes = cpusched::readProcessFile(testFilePath);

    REQUIRE(processes.size() == 3);

    // Check first process
    REQUIRE(processes[0].id == 1);
    REQUIRE(processes[0].arrivalTime == 10);
    REQUIRE(processes[0].burstTime == 22);

    // Check second process
    REQUIRE(processes[1].id == 2);
    REQUIRE(processes[1].arrivalTime == 68);
    REQUIRE(processes[1].burstTime == 12);

    // Check third process
    REQUIRE(processes[2].id == 3);
    REQUIRE(processes[2].arrivalTime == 98);
    REQUIRE(processes[2].burstTime == 34);

    std::filesystem::remove(testFilePath);
}

TEST_CASE("readProcessFile skips header line", "[file_reader]") {
    const std::string testFilePath = "test_header.txt";

    {
        std::ofstream testFile(testFilePath);
        testFile << "ArrivalTime\tCPUBurstLength\n";
        testFile << "5\t10\n";
    }

    auto processes = cpusched::readProcessFile(testFilePath);

    REQUIRE(processes.size() == 1);
    REQUIRE(processes[0].arrivalTime == 5);
    REQUIRE(processes[0].burstTime == 10);

    std::filesystem::remove(testFilePath);
}

TEST_CASE("readProcessFile assigns sequential IDs", "[file_reader]") {
    const std::string testFilePath = "test_ids.txt";

    {
        std::ofstream testFile(testFilePath);
        testFile << "ArrivalTime\tCPUBurstLength\n";
        testFile << "1\t5\n";
        testFile << "2\t3\n";
        testFile << "3\t8\n";
        testFile << "4\t6\n";
    }

    auto processes = cpusched::readProcessFile(testFilePath);

    REQUIRE(processes.size() == 4);
    for (size_t i = 0; i < processes.size(); ++i) {
        REQUIRE(processes[i].id == static_cast<pid_t>(i + 1));
    }

    std::filesystem::remove(testFilePath);
}

TEST_CASE("readProcessFile handles empty file with only header", "[file_reader]") {
    const std::string testFilePath = "test_empty.txt";

    {
        std::ofstream testFile(testFilePath);
        testFile << "ArrivalTime\tCPUBurstLength\n";
    }

    auto processes = cpusched::readProcessFile(testFilePath);

    REQUIRE(processes.empty());

    std::filesystem::remove(testFilePath);
}

TEST_CASE("readProcessFile throws exception for non-existent file", "[file_reader]") {
    const std::string nonExistentFile = "this_file_does_not_exist_12345.txt";

    REQUIRE_THROWS_AS(cpusched::readProcessFile(nonExistentFile), std::runtime_error);
}

TEST_CASE("readProcessFile throws exception for invalid data format", "[file_reader]") {
    const std::string testFilePath = "test_invalid.txt";

    {
        std::ofstream testFile(testFilePath);
        testFile << "ArrivalTime\tCPUBurstLength\n";
        testFile << "not_a_number\t50\n";
    }

    REQUIRE_THROWS_AS(cpusched::readProcessFile(testFilePath), std::runtime_error);

    std::filesystem::remove(testFilePath);
}

TEST_CASE("readProcessFile skips empty lines", "[file_reader]") {
    const std::string testFilePath = "test_empty_lines.txt";

    {
        std::ofstream testFile(testFilePath);
        testFile << "ArrivalTime\tCPUBurstLength\n";
        testFile << "10\t20\n";
        testFile << "\n";
        testFile << "30\t40\n";
    }

    auto processes = cpusched::readProcessFile(testFilePath);

    REQUIRE(processes.size() == 2);
    REQUIRE(processes[0].arrivalTime == 10);
    REQUIRE(processes[1].arrivalTime == 30);

    std::filesystem::remove(testFilePath);
}
