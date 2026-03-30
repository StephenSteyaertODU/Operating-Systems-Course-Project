#pragma once

#include "common/common.hpp"

/// Convenience alias — semaphore with the platform's maximum count
using Semaphore = std::counting_semaphore
                    <std::numeric_limits<std::ptrdiff_t>::max()>;

/**
 * @brief Thread-safe circular buffer shared between producer and consumer threads.
 *
 * Synchronization primitives:
 *   - empty_slots_  counting semaphore, initialised to capacity.
 *                   Producers acquire (decrement) before writing;
 *                   consumers release (increment) after reading.
 *   - filled_slots_ counting semaphore, initialised to 0.
 *                   Consumers acquire (decrement) before reading;
 *                   producers release (increment) after writing.
 *   - mutex_        mutex that protects head_, tail_, and buf_
 *                   during the actual read/write inside the critical section.
 *
 * Both produce() and consume() block automatically when the buffer is
 * full or empty respectively, with no busy-waiting.
 */
class SharedBuffer {
public:
    /**
     * @brief Construct a SharedBuffer with the given capacity.
     * @param capacity  Maximum number of SalesRecords the buffer can hold (b).
     */
    explicit SharedBuffer(int capacity);

    /**
     * @brief Insert a record into the buffer.
     *
     * Blocks if the buffer is full until a consumer frees a slot.
     * Called by producer threads (and by main for sentinel records).
     *
     * @param record  The SalesRecord to enqueue.
     */
    void produce(const SalesRecord& record);

    /**
     * @brief Remove and return the next record from the buffer.
     *
     * Blocks if the buffer is empty until a producer adds a record.
     * Called by consumer threads.
     *
     * @return The oldest SalesRecord in the buffer (FIFO order).
     */
    SalesRecord consume();

private:
    int capacity_;                   ///< Maximum number of slots
    std::vector<SalesRecord> buf_;   ///< Underlying storage (circular)
    int head_{0};                    ///< Next index to read from
    int tail_{0};                    ///< Next index to write to

    std::mutex  mutex_;              ///< Protects head_, tail_, buf_
    Semaphore   empty_slots_;        ///< Counts available write slots
    Semaphore   filled_slots_;       ///< Counts available read slots
};
