#include <catch2/catch_test_macros.hpp>

TEST_CASE("Basic arithmetic works", "[math]") {
    REQUIRE(1 + 1 == 2);
    REQUIRE(5 - 3 == 2);
}

TEST_CASE("String operations", "[string]") {
    std::string str = "hello";
    REQUIRE(str.length() == 5);
    REQUIRE(str + " world" == "hello world");
}