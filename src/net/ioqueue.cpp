#include "net/ioqueue.hpp"

namespace rmrf::net {

iorecord::iorecord() : offset{}, data{} {}

iorecord::iorecord(
    const void* buf,
    size_t size
) :
    offset{0},
    data((const uint8_t*)buf, (const uint8_t*)buf + size)
{
    // Nothing special to do here ...
}

iorecord::iorecord(const iorecord &other) : offset{other.offset}, data{other.data} {
    // NOP
}

iorecord::iorecord(iorecord &&other) :
    offset(other.offset),
    data(std::forward<std::vector<uint8_t>>(other.data))
{
    // Nothing special to do here ...
}

size_t iorecord::size() const {
    return this->data.size() - this->offset;
}

bool iorecord::empty() const {
    return !this->size();
}

void* iorecord::ptr() const {
    return (void*)(this->data.data() + this->offset);
}

void iorecord::advance(size_t amount) {
    this->offset += std::min(amount, this->size());
}

ioqueue::ioqueue() : queue{} {
    // NOP
}

ioqueue::~ioqueue() {
    // NOP
}

bool ioqueue::empty() const {
    return this->queue.empty();
}

void ioqueue::push_back(const iorecord &data) {
    if (!data.empty()) {
        this->queue.push_back(data);
    }
}

void ioqueue::push_back(iorecord &&data) {
    if (!data.empty()) {
        this->queue.emplace_back(std::forward<iorecord>(data));
    }
}

void ioqueue::push_front(const iorecord &data) {
    if (!data.empty()) {
        this->queue.push_front(data);
    }
}

void ioqueue::push_front(iorecord &&data) {
    if (!data.empty()) {
        this->queue.emplace_front(std::forward<iorecord>(data));
    }
}

iorecord ioqueue::pop_front() {
    if (this->empty()) {
        return iorecord{};
    }

    iorecord result = this->queue.front();
    this->queue.pop_front();
    return result;
}

} // namespace rmrf::net
