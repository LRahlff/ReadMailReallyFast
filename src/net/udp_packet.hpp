#pragma once

#include <algorithm>
#include <array>
#include <ostream>
#include <sstream>

namespace rmrf::net {

template<std::size_t pkg_size = 1024>
class udp_packet {
    typedef const uint8_t& const_reference;
    typedef uint8_t* iterator;

private:
    std::array<uint8_t, pkg_size> arr;
    size_t actual_length;

public:
    udp_packet() : arr{}, actual_length(0) { }

    inline constexpr size_t max_size() const {
        return arr.max_size();
    }

    inline size_t size() const {
        return actual_length;
    }

    inline size_t length() const {
        return actual_length;
    }

    inline constexpr uint8_t at(size_t pos) const {
        return arr.at(pos);
    }

    inline constexpr const_reference operator[](size_t pos) const noexcept {
        return arr.operator[](pos);
    }

    inline uint8_t& operator[](size_t pos) noexcept {
        return arr.operator[](pos);
    }

    inline bool advance(size_t i) {
        if (this->actual_length + i > this->max_size()) {
            return false;
        }
        this->actual_length += i;
        return true;
    }

    inline size_t append(const iterator begin, const iterator end) {
        const auto remaining_length = pkg_size - this->length();
        size_t i = 0;

        for (; i < remaining_length && begin + i < end; i++) {
            this->arr[this->length() + i] = begin[i];
        }

        this->advance(i);
        return i;
    }

    constexpr const uint8_t* raw() const noexcept {
        return this->arr.data();
    }

    constexpr uint8_t* raw() noexcept {
        return this->arr.data();
    }

    constexpr std::array<uint8_t, pkg_size>::iterator begin() noexcept {
        return this->arr.begin();
    }

    constexpr std::array<uint8_t, pkg_size>::iterator end() noexcept {
        return this->arr.end();
    }

    constexpr std::array<uint8_t, pkg_size>::const_iterator cbegin() const noexcept {
        return this->arr.cbegin();
    }

    constexpr std::array<uint8_t, pkg_size>::const_iterator cend() const noexcept {
        return this->arr.cend();
    }

    udp_packet<pkg_size>& operator<<(const std::string& obj) {
        const auto max_space = pkg_size - this->length();
        const auto str_length = obj.length();
        strncpy((char*) (this->raw() + this->length()), obj.c_str(), max_space);
        this->advance(std::min(str_length, max_space));
        return *this;
    }

    template<class T>
    udp_packet<pkg_size>& operator<<(const T& obj) {
        const auto size_to_copy = obj.end() - obj.begin();

        if (auto error = memcpy_s(this->raw() + this->length(), pkg_size - this->length(), obj.begin(), size_to_copy); error != 0) {
            std::stringstream ss;
            ss << "Unable to concat object. Errorcode: " << error << ". Is the memory structure too large for this packet?";
            throw std::invalid_argument(ss.str());
        }

        this->advance(size_to_copy);
        return *this;
    }
};

}

inline std::ostream& operator<<(std::ostream& os, const rmrf::net::udp_packet<>& obj) {
    return os << "[UDP-Packet with size " << obj.size() << ']';
}
