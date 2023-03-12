#include <algorithm>

#include "net/iorecord.hpp"

namespace rmrf::net {

iorecord::iorecord() : offset{}, data{}, addr{} {}

iorecord::iorecord(const void *buf, const size_t size, const socketaddr& sa) :
    offset{0},
    data((const uint8_t*)buf, (const uint8_t*)buf + size),
    addr{sa}
{
    // Nothing special to do here ...
}

iorecord::iorecord(
    const void* buf,
    const size_t size
) : iorecord(buf, size, socketaddr{}) {}

iorecord::iorecord(const iorecord& other) : offset{other.offset}, data{other.data}, addr{other.addr} {
    // NOP
}

iorecord::iorecord(iorecord&& other) :
    offset(other.offset),
    data(std::forward<std::vector<uint8_t>>(other.data)),
    addr(other.addr)
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

[[nodiscard]] std::vector<std::string> iorecord::get_strings_in_record() const {
    std::vector<std::string> sv;
    sv.reserve(this->potential_strings_in_record());
    std::stringstream ss;
    for(auto& c : this->data) {
        if(c == 0) {
            sv.push_back(ss.str());
            ss.str(std::string());
        } else {
            ss.put((char) c);
        }
    }
    sv.push_back(ss.str());
    return sv;
}

void iorecord::reserve_space(size_t amount) {
    this->data.reserve(amount);
}

} // namespace rmrf::net

std::ostream& operator<<(std::ostream& os, const rmrf::net::iorecord& obj) {
    if (obj.potential_strings_in_record() == 1)
        return os << obj.str();
    else
        return os << obj.str() << "[...]";
}
