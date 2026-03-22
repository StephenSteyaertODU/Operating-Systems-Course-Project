#include "consumer.hpp"

Consumer::Consumer(int id, int num_stores, SharedBuffer& buffer, GlobalStats& global_stats)
    : id_(id)
    , buffer_(buffer)
    , global_stats_(global_stats)
    , local_stats_(id, num_stores)
{}

void Consumer::run() {
    while (true) {
        SalesRecord record = buffer_.consume();  // blocks if buffer is empty

        if (record.is_sentinel) break;           // main sent this — time to stop

        local_stats_.accumulate(record);
    }

    // Print local stats and merge into global under a single lock
    // (prevents output from concurrent consumers from interleaving)
    global_stats_.merge_and_print(local_stats_);
}

const LocalStats& Consumer::localStats() const {
    return local_stats_;
}
