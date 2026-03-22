#include "buffer.hpp"

SharedBuffer::SharedBuffer(int capacity)
    : capacity_(capacity)
    , buf_(capacity)
    , empty_slots_(capacity)   // all slots are free at start
    , filled_slots_(0)         // no records available at start
{}

void SharedBuffer::produce(const SalesRecord& record) {
    empty_slots_.acquire();          // wait for a free slot
    {
        std::lock_guard<std::mutex> lock(mutex_);
        buf_[tail_] = record;
        tail_ = (tail_ + 1) % capacity_;
    }
    filled_slots_.release();         // signal that a record is ready
}

SalesRecord SharedBuffer::consume() {
    filled_slots_.acquire();         // wait for a record to be available
    SalesRecord record;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        record = buf_[head_];
        head_ = (head_ + 1) % capacity_;
    }
    empty_slots_.release();          // signal that a slot is free
    return record;
}
