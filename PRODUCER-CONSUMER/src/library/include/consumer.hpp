#pragma once

#include "common/common.hpp"
#include "buffer.hpp"
#include "stats.hpp"

/**
 * @brief Consumes sales records from the shared buffer and accumulates statistics.
 *
 * Each Consumer runs on its own thread. It loops — blocking on the buffer
 * when empty — until it receives a sentinel record, at which point it prints
 * its local statistics, merges them into the global stats, and exits.
 *
 * Shared variables:
 *   - buffer_       SharedBuffer shared with all producers and consumers.
 *                   Access is synchronized inside SharedBuffer itself.
 *   - global_stats_ GlobalStats shared with all other consumers.
 *                   Access is serialized via GlobalStats::merge_and_print(),
 *                   which holds global_stats_.mutex_ for the duration.
 *
 * Thread-local variables:
 *   - local_stats_  Owned exclusively by this consumer; no locking needed.
 */
class Consumer {
public:
    /**
     * @brief Construct a Consumer thread.
     *
     * @param id           1-based consumer identifier.
     * @param num_stores   Total number of stores/producers (p); used to size
     *                     the per-store sales array in LocalStats.
     * @param buffer       Shared buffer to read records from.
     * @param global_stats Shared statistics object to merge into when done.
     */
    Consumer(int id, int num_stores, SharedBuffer& buffer, GlobalStats& global_stats);

    /**
     * @brief Entry point for the consumer thread.
     *
     * Loops until a sentinel record is received:
     *   1. Calls buffer_.consume() — blocks if the buffer is empty.
     *   2. If the record is a sentinel, exits the loop.
     *   3. Otherwise, accumulates the record into local_stats_.
     *
     * After the loop:
     *   4. Calls global_stats_.merge_and_print(local_stats_) which, under
     *      a single lock, prints this consumer's local stats then folds
     *      them into the shared global totals.
     */
    void run();

    /**
     * @brief Read-only access to this consumer's local statistics.
     *
     * Safe to call from the main thread only after the consumer thread
     * has been joined.
     *
     * @return Const reference to the consumer's LocalStats.
     */
    const LocalStats& localStats() const;

private:
    SharedBuffer& buffer_;       ///< Shared buffer (not owned)
    GlobalStats&  global_stats_; ///< Shared global statistics (not owned)
    LocalStats    local_stats_;  ///< Thread-local stats (owned by this consumer)
};
