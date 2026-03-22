#pragma once

#include "common/common.hpp"
#include "buffer.hpp"

/**
 * @brief Produces sales records and inserts them into the shared buffer.
 *
 * Each Producer runs on its own thread and is responsible for one store.
 * Producers share an atomic counter (total_produced) to collectively
 * generate exactly TOTAL_ITEMS records across all producer threads.
 *
 * Shared variables:
 *   - total_produced_  std::atomic<int> shared with all other producers.
 *                      Each producer atomically claims the next slot via
 *                      fetch_add; when the claimed slot >= TOTAL_ITEMS the
 *                      producer stops.  No mutex needed — atomics suffice.
 *   - buffer_          SharedBuffer shared with all producers and consumers.
 *                      Access is synchronized inside SharedBuffer itself.
 */
class Producer {
public:
    /// Total number of sales records to produce across ALL producer threads.
    /// Can be overridden at compile time via -DPC_TOTAL_ITEMS=N (used by tests
    /// to keep runtimes short without changing source).
#ifndef PC_TOTAL_ITEMS
#define PC_TOTAL_ITEMS 1000
#endif
    static constexpr int TOTAL_ITEMS = PC_TOTAL_ITEMS;

    /**
     * @brief Construct a Producer for a specific store.
     *
     * @param store_id        Unique store identifier (1-based, 1 to p).
     * @param buffer          Shared buffer to write records into.
     * @param total_produced  Atomic counter shared across all producers;
     *                        tracks how many records have been claimed so far.
     */
    Producer(int store_id, SharedBuffer& buffer, std::atomic<int>& total_produced);

    /**
     * @brief Entry point for the producer thread.
     *
     * Loops until all TOTAL_ITEMS slots are claimed. On each iteration:
     *   1. Atomically claims the next slot via fetch_add on total_produced_.
     *   2. If the slot is >= TOTAL_ITEMS, stops immediately.
     *   3. Generates a random SalesRecord for this store.
     *   4. Inserts the record into the shared buffer (blocks if buffer full).
     *   5. Sleeps for a random duration between 5 ms and 40 ms.
     */
    void run();

private:
    /**
     * @brief Generate a random SalesRecord for this store.
     *
     * Randomised fields:
     *   - day          uniform in [1, 30]
     *   - month        uniform in [1, 12]
     *   - year         fixed at 16
     *   - register_num uniform in [1, 6]
     *   - amount       uniform in [0.50, 999.99]
     *
     * @return A freshly generated SalesRecord.
     */
    SalesRecord generateRecord();

    int               store_id_;        ///< This producer's store ID (1-based)
    SharedBuffer&     buffer_;          ///< Shared buffer (not owned)
    std::atomic<int>& total_produced_;  ///< Shared slot counter (not owned)
    std::mt19937      rng_;             ///< Per-thread RNG seeded from random_device
};
