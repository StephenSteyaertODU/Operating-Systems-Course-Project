#pragma once

#include "common/common.hpp"

// ============================================================================
// LocalStats
// ============================================================================

/**
 * @brief Per-consumer statistics accumulated while processing records.
 *
 * Each consumer thread owns one LocalStats instance. Stats are updated
 * inline as records are consumed — no locking needed since only one thread
 * ever writes to a given instance.
 *
 * Shared variables (none): LocalStats is entirely thread-local.
 */
struct LocalStats {
    int consumer_id;      ///< ID of the owning consumer thread (1-based)
    int num_stores;       ///< Total number of producer/store threads (p)

    /// store_sales[s] = total sales for store s (1-indexed; index 0 unused)
    std::vector<double> store_sales;

    /// month_sales[m] = total sales for month m+1  (index 0 = January, etc.)
    std::array<double, 12> month_sales{};

    double aggregate{0.0};       ///< Grand total of all sales seen by this consumer
    int    records_processed{0}; ///< Number of non-sentinel records processed

    /**
     * @brief Construct zeroed-out stats for a consumer.
     * @param consumer_id  1-based consumer index.
     * @param num_stores   Number of stores (= number of producers, p).
     */
    LocalStats(int consumer_id, int num_stores)
        : consumer_id(consumer_id)
        , num_stores(num_stores)
        , store_sales(num_stores + 1, 0.0)  // 1-indexed; slot 0 unused
    {}

    /**
     * @brief Accumulate a single sales record into local stats.
     *
     * Updates store_sales, month_sales, aggregate, and records_processed.
     * Must only be called from the owning consumer thread.
     *
     * @param r  The sales record to accumulate.
     */
    void accumulate(const SalesRecord& r) {
        store_sales[r.store_id]      += r.amount;
        month_sales[r.month - 1]     += r.amount;
        aggregate                    += r.amount;
        ++records_processed;
    }

    /**
     * @brief Print this consumer's local statistics to stdout.
     *
     * Called once per consumer after it receives its sentinel.
     * Output is serialized by GlobalStats::merge_and_print() so lines
     * from different consumers never interleave.
     */
    void print() const {
        std::cout << "\n--- Consumer " << consumer_id << " ---\n";
        std::cout << "  Records processed : " << records_processed << "\n";

        std::cout << "  Sales by store:\n";
        for (int s = 1; s <= num_stores; ++s) {
            std::cout << "    Store " << s << " : $"
                      << std::fixed << std::setprecision(2)
                      << store_sales[s] << "\n";
        }

        std::cout << "  Sales by month:\n";
        for (int m = 0; m < 12; ++m) {
            std::cout << "    Month " << std::setw(2) << (m + 1) << " : $"
                      << std::fixed << std::setprecision(2)
                      << month_sales[m] << "\n";
        }

        std::cout << "  Aggregate         : $"
                  << std::fixed << std::setprecision(2) << aggregate << "\n";
    }
};

// ============================================================================
// GlobalStats
// ============================================================================

/**
 * @brief Shared statistics accumulated across all consumer threads.
 *
 * Shared variables:
 *   - store_sales   protected by mutex_
 *   - month_sales   protected by mutex_
 *   - aggregate     protected by mutex_
 *
 * Semaphores / mutexes:
 *   - mutex_  std::mutex that serialises both the merge step and the local
 *             stats print so consumer output lines never interleave.
 */
struct GlobalStats {
    int num_stores; ///< Number of stores (= number of producers, p)

    /// store_sales[s] = total sales for store s across all consumers (1-indexed)
    std::vector<double> store_sales;

    /// month_sales[m] = total sales for month m+1 across all consumers
    std::array<double, 12> month_sales{};

    double aggregate{0.0}; ///< Grand total of all sales across all consumers

    std::mutex mutex_; ///< Serialises merge and per-consumer print output

    /**
     * @brief Construct zeroed-out global stats.
     * @param num_stores  Number of stores/producers (p).
     */
    explicit GlobalStats(int num_stores)
        : num_stores(num_stores)
        , store_sales(num_stores + 1, 0.0)
    {}

    /**
     * @brief Print a consumer's local stats then fold them into global totals.
     *
     * Holds mutex_ for the entire operation so output from concurrent
     * consumers does not interleave and the merge is atomic.
     *
     * Called by each consumer thread after receiving its sentinel.
     *
     * @param local  The finished LocalStats of the calling consumer.
     */
    void merge_and_print(const LocalStats& local) {
        std::lock_guard<std::mutex> lock(mutex_);

        local.print();  // serialised print — no interleaving

        for (int s = 1; s <= num_stores; ++s)
            store_sales[s] += local.store_sales[s];

        for (int m = 0; m < 12; ++m)
            month_sales[m] += local.month_sales[m];

        aggregate += local.aggregate;
    }

    /**
     * @brief Print the aggregated global statistics to stdout.
     *
     * Called from the main thread after all consumer threads have joined,
     * so no locking is required.
     */
    void print() const {
        std::cout << "\n=== Global Statistics ===\n";

        std::cout << "Sales by store:\n";
        for (int s = 1; s <= num_stores; ++s) {
            std::cout << "  Store " << s << " : $"
                      << std::fixed << std::setprecision(2)
                      << store_sales[s] << "\n";
        }

        std::cout << "Sales by month:\n";
        for (int m = 0; m < 12; ++m) {
            std::cout << "  Month " << std::setw(2) << (m + 1) << " : $"
                      << std::fixed << std::setprecision(2)
                      << month_sales[m] << "\n";
        }

        std::cout << "Grand total : $"
                  << std::fixed << std::setprecision(2) << aggregate << "\n";
    }
};
