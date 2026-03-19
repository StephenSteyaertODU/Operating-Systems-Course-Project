#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <thread>
#include <vector>
#include "buffer.hpp"

// Helper: create a simple non-sentinel record
static SalesRecord makeRecord(int store_id, double amount) {
    return SalesRecord{1, 1, 16, store_id, 1, amount, false};
}

// ---------------------------------------------------------------------------
// Single-threaded behaviour
// ---------------------------------------------------------------------------

TEST_CASE("Buffer preserves FIFO order", "[buffer][unit]") {
    SharedBuffer buf(5);

    buf.produce(makeRecord(1, 10.00));
    buf.produce(makeRecord(2, 20.00));
    buf.produce(makeRecord(3, 30.00));

    REQUIRE(buf.consume().amount == 10.00);
    REQUIRE(buf.consume().amount == 20.00);
    REQUIRE(buf.consume().amount == 30.00);
}

TEST_CASE("Buffer stores and returns correct fields", "[buffer][unit]") {
    SharedBuffer buf(2);

    SalesRecord r{15, 6, 16, 3, 4, 123.45, false};
    buf.produce(r);
    SalesRecord out = buf.consume();

    REQUIRE(out.day         == 15);
    REQUIRE(out.month       == 6);
    REQUIRE(out.year        == 16);
    REQUIRE(out.store_id    == 3);
    REQUIRE(out.register_num == 4);
    REQUIRE(out.amount      == Catch::Approx(123.45));
    REQUIRE(out.is_sentinel == false);
}

TEST_CASE("Sentinel record passes through unchanged", "[buffer][unit]") {
    SharedBuffer buf(2);

    SalesRecord sentinel{};
    sentinel.is_sentinel = true;
    buf.produce(sentinel);

    SalesRecord out = buf.consume();
    REQUIRE(out.is_sentinel == true);
}

TEST_CASE("Buffer handles capacity=1 correctly", "[buffer][unit]") {
    SharedBuffer buf(1);

    buf.produce(makeRecord(1, 5.00));
    REQUIRE(buf.consume().amount == 5.00);

    buf.produce(makeRecord(1, 9.99));
    REQUIRE(buf.consume().amount == 9.99);
}

// ---------------------------------------------------------------------------
// Concurrent behaviour
// ---------------------------------------------------------------------------

TEST_CASE("Single producer single consumer transfers all records", "[buffer][threading]") {
    const int N = 200;
    SharedBuffer buf(10);

    std::thread producer([&]() {
        for (int i = 0; i < N; ++i)
            buf.produce(makeRecord(1, static_cast<double>(i)));
    });

    std::vector<double> received;
    std::thread consumer([&]() {
        for (int i = 0; i < N; ++i)
            received.push_back(buf.consume().amount);
    });

    producer.join();
    consumer.join();

    REQUIRE(received.size() == N);
    for (int i = 0; i < N; ++i)
        REQUIRE(received[i] == Catch::Approx(static_cast<double>(i)));
}

TEST_CASE("Multiple producers and consumers transfer all records", "[buffer][threading]") {
    const int TOTAL   = 1000;
    const int P       = 5;
    const int C       = 5;
    const int PER_P   = TOTAL / P;

    SharedBuffer buf(20);
    std::atomic<int> total_produced{0};
    std::atomic<double> total_amount{0.0};

    // Producers each emit PER_P records with amount = 1.0
    std::vector<std::thread> producers;
    for (int i = 0; i < P; ++i) {
        producers.emplace_back([&]() {
            for (int j = 0; j < PER_P; ++j) {
                buf.produce(makeRecord(1, 1.0));
                total_produced.fetch_add(1);
            }
        });
    }

    // Consumers run until they've collectively read TOTAL records
    std::atomic<int> total_consumed{0};
    std::vector<std::thread> consumers;
    for (int i = 0; i < C; ++i) {
        consumers.emplace_back([&]() {
            while (true) {
                int slot = total_consumed.fetch_add(1);
                if (slot >= TOTAL) break;
                SalesRecord r = buf.consume();
                total_amount.fetch_add(r.amount);
            }
        });
    }

    for (auto& t : producers) t.join();
    for (auto& t : consumers) t.join();

    REQUIRE(total_produced.load() == TOTAL);
    REQUIRE(total_amount.load()   == Catch::Approx(static_cast<double>(TOTAL)));
}

TEST_CASE("Sentinel stops consumers cleanly", "[buffer][threading]") {
    const int C = 4;
    SharedBuffer buf(10);

    // Push some real records then one sentinel per consumer
    for (int i = 0; i < 8; ++i)
        buf.produce(makeRecord(1, 1.0));

    std::atomic<int> records_consumed{0};
    std::atomic<int> sentinels_received{0};

    std::vector<std::thread> consumers;
    for (int i = 0; i < C; ++i) {
        consumers.emplace_back([&]() {
            while (true) {
                SalesRecord r = buf.consume();
                if (r.is_sentinel) { sentinels_received.fetch_add(1); break; }
                records_consumed.fetch_add(1);
            }
        });
    }

    // Send sentinels after real records are already in the buffer
    for (int i = 0; i < C; ++i) {
        SalesRecord s{}; s.is_sentinel = true;
        buf.produce(s);
    }

    for (auto& t : consumers) t.join();

    REQUIRE(records_consumed.load()   == 8);
    REQUIRE(sentinels_received.load() == C);
}
