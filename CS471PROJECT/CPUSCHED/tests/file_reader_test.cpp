// NOTE: These tests will need to be updated when file_reader.cpp is modified
// to parse the data into Process structs instead of returning raw strings.

#include <catch2/catch_test_macros.hpp>
#include "common/common.hpp"
#include <fstream>
#include <filesystem>

TEST_CASE("readProcessFile reads valid file", "[file_reader]") {
    // Create a temporary test file
    const std::string testFilePath = "test_data.txt";

    {
        std::ofstream testFile(testFilePath);
        testFile << "Line 1\n";
        testFile << "Line 2\n";
        testFile << "Line 3\n";
    }

    // Read the file
    auto lines = cpusched::readProcessFile(testFilePath);

    // Verify
    REQUIRE(lines.size() == 3);
    REQUIRE(lines[0] == "Line 1");
    REQUIRE(lines[1] == "Line 2");
    REQUIRE(lines[2] == "Line 3");

    // Cleanup
    std::filesystem::remove(testFilePath);
}

TEST_CASE("readProcessFile handles empty file", "[file_reader]") {
    // Create an empty test file
    const std::string testFilePath = "test_empty.txt";

    {
        std::ofstream testFile(testFilePath);
        // Don't write anything
    }

    // Read the file
    auto lines = cpusched::readProcessFile(testFilePath);

    // Verify it's empty
    REQUIRE(lines.empty());

    // Cleanup
    std::filesystem::remove(testFilePath);
}

TEST_CASE("readProcessFile throws exception for non-existent file", "[file_reader]") {
    const std::string nonExistentFile = "this_file_does_not_exist_12345.txt";

    // Verify it throws an exception
    REQUIRE_THROWS_AS(cpusched::readProcessFile(nonExistentFile), std::runtime_error);
}

TEST_CASE("readProcessFile handles file with trailing newline", "[file_reader]") {
    const std::string testFilePath = "test_trailing.txt";

    {
        std::ofstream testFile(testFilePath);
        testFile << "Line 1\n";
        testFile << "Line 2\n";
        testFile << "Line 3\n\n";  // Extra newline at end
    }

    auto lines = cpusched::readProcessFile(testFilePath);

    // Should have 4 lines (including the empty line from trailing newline)
    REQUIRE(lines.size() == 4);
    REQUIRE(lines[0] == "Line 1");
    REQUIRE(lines[1] == "Line 2");
    REQUIRE(lines[2] == "Line 3");
    REQUIRE(lines[3] == "");

    // Cleanup
    std::filesystem::remove(testFilePath);
}

TEST_CASE("readProcessFile handles file with tabs and spaces", "[file_reader]") {
    const std::string testFilePath = "test_whitespace.txt";

    {
        std::ofstream testFile(testFilePath);
        testFile << "Column1\tColumn2\tColumn3\n";
        testFile << "10     \t20     \t30\n";
    }

    auto lines = cpusched::readProcessFile(testFilePath);

    REQUIRE(lines.size() == 2);
    REQUIRE(lines[0] == "Column1\tColumn2\tColumn3");
    REQUIRE(lines[1] == "10     \t20     \t30");

    // Cleanup
    std::filesystem::remove(testFilePath);
}
