#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <ev++.h>
#include <memory>
#include <string>
#include <sstream>
#include <stdexcept>
#include <ostream>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>

#include "net/async_fd.hpp"
#include "net/connection_client.hpp"
#include "net/ioqueue.hpp"
#include "net/socketaddress.hpp"
#include "net/sock_address_factory.hpp"

#include <iostream>

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

class address_augmented_iorecord : public iorecord {
private:
    const socketaddr addr;

public:
    address_augmented_iorecord(const void* buf, size_t size, socketaddr _addr) : iorecord{buf, size}, addr(_addr) {}
    address_augmented_iorecord(const iorecord& ior, socketaddr _addr) : iorecord{ior}, addr(_addr) {}
    address_augmented_iorecord() : iorecord{}, addr{} {};
    address_augmented_iorecord(const address_augmented_iorecord& _other) = default;

    const socketaddr& get_address() const {
        return this->addr;
    }
};

/**
 * This class provides a UDP server / client wrapper. Please note that it is packet based and thus not based
 * on connection client on purpose.
 * @class udp_client
 * @author leondietrich
 * @date 04/12/21
 * @file udp_client.hpp
 * @brief A UDP socket wrapper class
 */
template<std::size_t pkg_size = 1024>
class udp_client : public std::enable_shared_from_this<udp_client<pkg_size>> {
public:
    static const unsigned int no_rate_limit = 0;
    typedef std::function<void(const udp_packet<pkg_size>& data, socketaddr& source)> in_packet_cb;

private:
    auto_fd net_socket;
    ::ev::io io;
    ioqueue<address_augmented_iorecord> write_queue;
    const in_packet_cb _in_cb;
    unsigned int rate_limit = no_rate_limit;
    unsigned int current_burst_count = 0;
    bool data_write_active = false;
    int send_flags = MSG_DONTWAIT;

public:
    udp_client(
        socketaddr _addr,
        in_packet_cb _incomming_cb = nullptr
    ) :
        net_socket(null_fd()),
        io{},
        write_queue{},
        _in_cb(_incomming_cb)
    {
        static_assert(pkg_size < 65536);

        const auto family = _addr.family();

        if (!(family == AF_INET || family == AF_INET6)) {
            std::stringstream ss;
            ss << "Invalid IP address family. (" <<  family << ")";
            throw netio_exception(ss.str());
        }

        this->net_socket = auto_fd(socket(family, SOCK_DGRAM, 0));

        if (!this->net_socket.valid()) {
            // TODO implement proper error handling
            throw netio_exception("Failed to request socket fd from kernel.");
        }

        if (_incomming_cb != nullptr) {
            if (auto error = bind(this->net_socket.get(), _addr.ptr(), _addr.size()); error != 0) {
                throw netio_exception("Failed to bind UDP socket: " + std::to_string(error));
            }
        }

        if (
            const auto existing_fd_flags = fcntl(this->net_socket.get(), F_GETFL, 0);
            existing_fd_flags == -1 || fcntl(this->net_socket.get(), F_SETFL, existing_fd_flags | O_NONBLOCK) == -1
        ) {
            throw netio_exception("Failed to set socket mode. fcntl resulted in error:" + std::to_string(errno));
        }

        io.set<udp_client, &udp_client::cb_ev>(this);
        const auto initial_state = _incomming_cb != nullptr ? ::ev::READ : 0;
        io.start(this->net_socket.get(), initial_state);

        //std::cout << "UDP socket created: state=" << initial_state << std::endl;
    }

    udp_client(
        const std::string& interface_to_bind,
        uint16_t port,
        in_packet_cb _incomming_cb = nullptr
    ) :
        udp_client{get_first_general_socketaddr(interface_to_bind, port, socket_t::UDP), _incomming_cb}
    {}

    ~udp_client() {
        io.stop();
    }

    /**
     * @brief Set a maximum number of packets to transmit per burst
     * @param new_limit The limit to set
     */
    inline void set_rate_limit(unsigned int new_limit) {
        this->rate_limit = new_limit;
    }

    inline unsigned int get_rate_limit() {
        return this->rate_limit;
    }

    void send_packet(const socketaddr& destination, const udp_packet<pkg_size>& data) {
        this->write_queue.push_back(address_augmented_iorecord{data.raw(), data.size(), destination});
        this->io.set(::ev::READ | ::ev::WRITE);
    }

    /**
     * Enable or disable UDP confirm mode. Disabled by default. If enabled every
     * device handling the send packet shall report if the path was viable or not. This
     * is useful for debugging but may negatively impact performance.
     * @brief Enable or disable UDP confirm mode
     * @param enabled If set to true the UDP confirm mode will be activated.
     */
    void enable_confirm_mode(bool enabled) {
        if (enabled) {
            this->send_flags |= MSG_CONFIRM;
        } else {
            this->send_flags &= ~MSG_CONFIRM;
        }
    };

private:
    inline int get_io_read_state() const {
        return this->_in_cb != nullptr ? ::ev::READ : 0;
    }

    /**
     * This method implements the io queue callback. It is responsible for the actual data transactions
     * @param w The io handle to use
     * @param events The event flag container
     */
    void cb_ev(::ev::io& w, int events) {
        if (events & ::ev::ERROR) {
            throw netio_exception("Error while decoding events from libev: state=" + std::to_string(events) + ", current_send_flags=" + std::to_string(this->send_flags));
        }

        if (events & ::ev::READ && this->_in_cb != nullptr) {
            read_from_buffer(w);
        }

        if (events & ::ev::WRITE && (this->current_burst_count < this->rate_limit || this->rate_limit == this->no_rate_limit)) {
            send_write_buffer(w);
        }

        const auto state = this->get_io_read_state();

        if (write_queue.empty()) {
            io.set(state);
        } else {
            io.set(state | ::ev::WRITE);
        }
    }

    void send_write_buffer(::ev::io& w) {
        // read packet buffer and dispatch it
        // TODO fix destination address
        // TODO register a timer to decrease rate limit if enabled.
        if (this->write_queue.empty()) {
            io.set(get_io_read_state());
            return;
        }

        this->data_write_active = true;
        auto buffer = this->write_queue.pop_front();
        const auto dest_addr = buffer.get_address();
        ssize_t written = sendto(w.fd, buffer.ptr(), buffer.size(), this->send_flags, dest_addr.ptr(), dest_addr.size());

        if (written >= 0) {
            buffer.advance((size_t)written);
        } else if (EAGAIN_WRAPPER) {
            throw netio_exception("Failed to write latest buffer content.");
        }

        this->write_queue.push_front(buffer);
        this->data_write_active = false;
        this->current_burst_count++;
    }

    void read_from_buffer(::ev::io& w) {
        sockaddr_storage source_addr_raw{};
        socklen_t srcaddr_len = sizeof(source_addr_raw);

        udp_packet<pkg_size> p;
        auto read_bytes_or_error = recvfrom(w.fd, p.raw(), p.max_size(), 0, (sockaddr*)&source_addr_raw, &srcaddr_len);
        if (read_bytes_or_error < 0) {
            throw netio_exception("Failed to read UDP packet. err:" + std::to_string(read_bytes_or_error));
        }

        p.advance(read_bytes_or_error);

        socketaddr source_address{source_addr_raw};
        this->_in_cb(p, source_address);
    }
};

}

std::ostream& operator<<(std::ostream& os, const rmrf::net::udp_packet<>& obj) {
    return os << "[UDP-Packet with size " << obj.size() << ']';
}
