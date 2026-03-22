#include <catch2/catch_test_macros.hpp>
#include "stats.hpp"

// Helper: build a record with specific fields
static SalesRecord make(int store, int month, double amount) {
    return SalesRecord{1, month, 16, store, 1, amount, false};
}

// ============================================================================
// LocalStats
// ============================================================================

TEST_CASE("LocalStats starts fully zeroed", "[stats][unit]") {
    LocalStats s(1, 3);

    REQUIRE(s.records_processed == 0);
    REQUIRE(s.aggregate         == 0.0);

    for (int i = 1; i <= 3; ++i)
        REQUIRE(s.store_sales[i] == 0.0);

    for (int m = 0; m < 12; ++m)
        REQUIRE(s.month_sales[m] == 0.0);
}

TEST_CASE("LocalStats accumulates a single record correctly", "[stats][unit]") {
    LocalStats s(1, 2);
    s.accumulate(make(1, 3, 50.00));

    REQUIRE(s.records_processed  == 1);
    REQUIRE(s.aggregate          == 50.00);
    REQUIRE(s.store_sales[1]     == 50.00);
    REQUIRE(s.store_sales[2]     == 0.00);
    REQUIRE(s.month_sales[2]     == 50.00);  // month 3 → index 2
    REQUIRE(s.month_sales[0]     == 0.00);
}

TEST_CASE("LocalStats accumulates multiple records across stores and months", "[stats][unit]") {
    LocalStats s(1, 3);

    s.accumulate(make(1,  1, 100.00));  // store 1, Jan
    s.accumulate(make(2,  1,  50.00));  // store 2, Jan
    s.accumulate(make(3,  6, 200.00));  // store 3, Jun
    s.accumulate(make(1, 12,  25.00));  // store 1, Dec

    REQUIRE(s.records_processed == 4);
    REQUIRE(s.aggregate         == 375.00);

    REQUIRE(s.store_sales[1]    == 125.00);
    REQUIRE(s.store_sales[2]    ==  50.00);
    REQUIRE(s.store_sales[3]    == 200.00);

    REQUIRE(s.month_sales[0]    == 150.00);  // Jan
    REQUIRE(s.month_sales[5]    == 200.00);  // Jun
    REQUIRE(s.month_sales[11]   ==  25.00);  // Dec
    REQUIRE(s.month_sales[1]    ==   0.00);  // Feb (untouched)
}

TEST_CASE("LocalStats records_processed increments correctly", "[stats][unit]") {
    LocalStats s(1, 1);
    for (int i = 0; i < 10; ++i)
        s.accumulate(make(1, 1, 1.00));

    REQUIRE(s.records_processed == 10);
    REQUIRE(s.aggregate         == 10.00);
}

// ============================================================================
// GlobalStats
// ============================================================================

TEST_CASE("GlobalStats starts fully zeroed", "[stats][unit]") {
    GlobalStats g(3);

    REQUIRE(g.aggregate == 0.0);
    for (int s = 1; s <= 3; ++s)
        REQUIRE(g.store_sales[s] == 0.0);
    for (int m = 0; m < 12; ++m)
        REQUIRE(g.month_sales[m] == 0.0);
}

TEST_CASE("GlobalStats merges a single consumer correctly", "[stats][unit]") {
    GlobalStats g(2);

    LocalStats local(1, 2);
    local.accumulate(make(1, 3, 100.00));
    local.accumulate(make(2, 6,  50.00));

    g.merge_and_print(local);

    REQUIRE(g.aggregate        == 150.00);
    REQUIRE(g.store_sales[1]   == 100.00);
    REQUIRE(g.store_sales[2]   ==  50.00);
    REQUIRE(g.month_sales[2]   == 100.00);  // Mar
    REQUIRE(g.month_sales[5]   ==  50.00);  // Jun
}

TEST_CASE("GlobalStats merges multiple consumers and sums correctly", "[stats][unit]") {
    GlobalStats g(2);

    LocalStats c1(1, 2);
    c1.accumulate(make(1, 1, 200.00));
    c1.accumulate(make(2, 2, 100.00));

    LocalStats c2(2, 2);
    c2.accumulate(make(1, 1,  50.00));
    c2.accumulate(make(2, 3,  75.00));

    g.merge_and_print(c1);
    g.merge_and_print(c2);

    REQUIRE(g.aggregate       == 425.00);
    REQUIRE(g.store_sales[1]  == 250.00);
    REQUIRE(g.store_sales[2]  == 175.00);
    REQUIRE(g.month_sales[0]  == 250.00);  // Jan
    REQUIRE(g.month_sales[1]  == 100.00);  // Feb
    REQUIRE(g.month_sales[2]  ==  75.00);  // Mar
}

TEST_CASE("GlobalStats merge is thread-safe under concurrent consumers", "[stats][threading]") {
    const int NUM_CONSUMERS = 8;
    const int NUM_STORES    = 4;
    const double AMOUNT     = 10.00;

    GlobalStats g(NUM_STORES);

    // Each consumer contributes one record per store
    std::vector<std::thread> threads;
    for (int i = 1; i <= NUM_CONSUMERS; ++i) {
        threads.emplace_back([&, i]() {
            LocalStats local(i, NUM_STORES);
            for (int s = 1; s <= NUM_STORES; ++s)
                local.accumulate(make(s, 1, AMOUNT));
            g.merge_and_print(local);
        });
    }

    for (auto& t : threads) t.join();

    double expected_total = NUM_CONSUMERS * NUM_STORES * AMOUNT;
    REQUIRE(g.aggregate == expected_total);

    for (int s = 1; s <= NUM_STORES; ++s)
        REQUIRE(g.store_sales[s] == NUM_CONSUMERS * AMOUNT);
}
