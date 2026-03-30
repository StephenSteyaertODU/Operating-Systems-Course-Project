#include "producer.hpp"

Producer::Producer(int store_id, SharedBuffer& buffer, 
                   std::atomic<int>& total_produced,
                   int total_items)
    : store_id_(store_id)
    , buffer_(buffer)
    , total_produced_(total_produced)
    , total_items_(total_items)
    , rng_(std::random_device{}())  // seed each thread's RNG independently
{}

SalesRecord Producer::generateRecord() {
    std::uniform_int_distribution<> day_dist(1, 30);
    std::uniform_int_distribution<> month_dist(1, 12);
    std::uniform_int_distribution<> reg_dist(1, 6);
    std::uniform_real_distribution<> amount_dist(0.50, 999.99);

    return SalesRecord{
        .day          = day_dist(rng_),
        .month        = month_dist(rng_),
        .year         = 16,
        .store_id     = store_id_,
        .register_num = reg_dist(rng_),
        .amount       = amount_dist(rng_),
        .is_sentinel  = false
    };
}

void Producer::run() {
    std::uniform_int_distribution<> sleep_dist(5, 40);

    while (true) {
        // Atomically claim the next production slot.
        // Slots 0–(total_items_-1) map to actual records; anything >= total_items_ means done.
        int slot = total_produced_.fetch_add(1);
        if (slot >= total_items_) break;

        buffer_.produce(generateRecord());

        // Sleep between 5–40 ms before producing the next record
        std::this_thread::sleep_for(std::chrono::milliseconds(
            sleep_dist(rng_)));
    }
}
