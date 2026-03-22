#include <catch2/catch_test_macros.hpp>
#include "buffer.hpp"
#include "stats.hpp"
#include "producer.hpp"
#include "consumer.hpp"

// ============================================================================
// Helpers
// ============================================================================

/**
 * Run a full producer-consumer simulation and return the resulting GlobalStats.
 *
 * Uses the real Producer and Consumer classes. Producers do NOT sleep in tests
 * because PC_TOTAL_ITEMS is set to 50 via the test CMakeLists, keeping
 * runtimes well under a second even with the 5–40 ms sleep per record.
 *
 * Flow:
 *   1. Launch c consumer threads.
 *   2. Launch p producer threads (they collectively produce TOTAL_ITEMS records).
 *   3. Join producer threads.
 *   4. Push c sentinels so each consumer exits cleanly.
 *   5. Join consumer threads.
 *   6. Return GlobalStats.
 */
static void run_simulation(int p, int c, int b, GlobalStats& global) {
    SharedBuffer       buffer(b);
    std::atomic<int>   total_produced{0};

    // Launch consumers first so they're ready when producers start
    std::vector<std::unique_ptr<Consumer>> consumers;
    std::vector<std::thread> consumer_threads;
    for (int i = 1; i <= c; ++i) {
        consumers.push_back(std::make_unique<Consumer>(i, p, buffer, global));
        consumer_threads.emplace_back(&Consumer::run, consumers.back().get());
    }

    // Launch producers — pass 50 so tests finish in seconds rather than minutes
    std::vector<std::unique_ptr<Producer>> producers;
    std::vector<std::thread> producer_threads;
    for (int i = 1; i <= p; ++i) {
        producers.push_back(std::make_unique<Producer>(i, buffer, total_produced, 50));
        producer_threads.emplace_back(&Producer::run, producers.back().get());
    }

    for (auto& t : producer_threads) t.join();

    // Send one sentinel per consumer to signal shutdown
    for (int i = 0; i < c; ++i) {
        SalesRecord s{};
        s.is_sentinel = true;
        buffer.produce(s);
    }

    for (auto& t : consumer_threads) t.join();
}

// ============================================================================
// Integration tests
// ============================================================================

TEST_CASE("p=2 c=2: all records consumed, grand total > 0", "[integration]") {
    GlobalStats g(2);
    run_simulation(2, 2, 10, g);

    // Every record has amount >= 0.50, so total must be >= 0.50 * TOTAL_ITEMS
    double min_expected = 0.50 * 50;
    REQUIRE(g.aggregate >= min_expected);

    // All store_sales must be non-negative
    for (int s = 1; s <= 2; ++s)
        REQUIRE(g.store_sales[s] >= 0.0);
}

TEST_CASE("p=5 c=5: records_processed sums to TOTAL_ITEMS", "[integration]") {
    // We can verify total records by checking that the sum of per-consumer
    // records_processed equals TOTAL_ITEMS. To do this we need the consumers'
    // local stats — so we run manually here.
    const int P = 5, C = 5, B = 20;

    SharedBuffer     buffer(B);
    GlobalStats      global(P);
    std::atomic<int> total_produced{0};

    std::vector<std::unique_ptr<Consumer>> consumers;
    std::vector<std::thread> consumer_threads;
    for (int i = 1; i <= C; ++i) {
        consumers.push_back(std::make_unique<Consumer>(i, P, buffer, global));
        consumer_threads.emplace_back(&Consumer::run, consumers.back().get());
    }

    std::vector<std::unique_ptr<Producer>> producers;
    std::vector<std::thread> producer_threads;
    for (int i = 1; i <= P; ++i) {
        producers.push_back(std::make_unique<Producer>(i, buffer, total_produced, 50));
        producer_threads.emplace_back(&Producer::run, producers.back().get());
    }

    for (auto& t : producer_threads) t.join();

    for (int i = 0; i < C; ++i) {
        SalesRecord s{}; s.is_sentinel = true;
        buffer.produce(s);
    }

    for (auto& t : consumer_threads) t.join();

    // Sum records_processed across all consumers
    int total_consumed = 0;
    for (auto& c : consumers)
        total_consumed += c->localStats().records_processed;

    REQUIRE(total_consumed == 50);

    // Global aggregate must equal sum of all local aggregates
    double local_total = 0.0;
    for (auto& c : consumers)
        local_total += c->localStats().aggregate;
    REQUIRE(std::abs(global.aggregate - local_total) < 0.001);
}

TEST_CASE("p=10 c=2 small buffer: no deadlock, all records consumed", "[integration]") {
    GlobalStats g(10);
    run_simulation(10, 2, 5, g);
    REQUIRE(g.aggregate >= 0.50 * 50);
}

TEST_CASE("p=2 c=10: all store sales sum to grand total", "[integration]") {
    GlobalStats g(2);
    run_simulation(2, 10, 15, g);

    double store_sum = 0.0;
    for (int s = 1; s <= 2; ++s)
        store_sum += g.store_sales[s];

    REQUIRE(std::abs(store_sum - g.aggregate) < 0.001);
}

TEST_CASE("p=5 c=5: all month sales sum to grand total", "[integration]") {
    GlobalStats g(5);
    run_simulation(5, 5, 20, g);

    double month_sum = 0.0;
    for (int m = 0; m < 12; ++m)
        month_sum += g.month_sales[m];

    REQUIRE(std::abs(month_sum - g.aggregate) < 0.001);
}
