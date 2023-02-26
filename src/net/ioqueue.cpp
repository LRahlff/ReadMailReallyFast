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

} // namespace rmrf::net
