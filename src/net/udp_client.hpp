#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <stdexcept>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "net/async_fd.hpp"
#include "net/connection_client.hpp"
#include "net/ioqueue.hpp"
#include "net/socketaddress.hpp"
#include "net/sock_address_factory.hpp"
#include "net/udp_packet.hpp"

#include <iostream>

namespace rmrf::net {

/**
 * This class provides a UDP server / client wrapper. Please note that it is packet based and thus not based
 * on connection client on purpose.
 * @class udp_client
 * @author leondietrich
 * @date 04/12/21
 * @file udp_client.hpp
 * @brief A UDP socket wrapper class
 */
class udp_client : public connection_client, std::enable_shared_from_this<udp_client> {
private:
    int send_flags = MSG_DONTWAIT;
    bool client_is_bound = false;

public:

    udp_client(auto_fd&& socket_fd, const socketaddr& own_address_) :
        connection_client{std::forward<auto_fd>(socket_fd), socketaddr{}, nullptr}
    {
        this->own_address = own_address_;
    }

    virtual ~udp_client() {}

    /**
     * This method copies the content of data on the write queue and schedules its transmission to destination.
     * It needs to know the packet size of the udp packet, provided by the template.
     * @brief Send a udp packet
     * @param destination The destination to send it to
     * @param data The data to be send
     */
    template<size_t packet_size>
    inline void send_packet(const socketaddr& destination, const udp_packet<packet_size>& data) {
        this->write_data(std::forward<iorecord>(iorecord{data.raw(), data.size(), destination}));
    }

    /**
     * Use this method in order to register your callback function that should be
     * called when the client got data to process. If this method is called for the first
     * time it binds the socket and will throw an error if it is not possible.
     * @param cb The callback function to register [void(iorecord& data)]
     */
    virtual void set_incomming_data_callback(const incomming_data_cb& cb) {
        connection_client::set_incomming_data_callback(cb);
        if(cb && !client_is_bound) {
            if (auto error = bind(this->net_socket.get(), this->own_address.ptr(), this->own_address.size()); error != 0) {
                throw netio_exception("Failed to bind UDP socket: " + std::to_string(error));
            }
            this->client_is_bound = true;
        }
    }

    /**
     * Enable or disable UDP confirm mode. Disabled by default. If enabled every
     * device handling the send packet shall report if the path was viable or not. This
     * is useful for debugging but may negatively impact performance.
     * @brief Enable or disable UDP confirm mode
     * @note This is only enabled if MSG_CONFIRM is enabled on your platform.
     * @param enabled If set to true the UDP confirm mode will be activated.
     */
    void enable_confirm_mode(bool enabled) {
#ifdef MSG_CONFIRM
        if (enabled) {
            this->send_flags |= MSG_CONFIRM;
        } else {
            this->send_flags &= ~MSG_CONFIRM;
        }
#else
	MARK_UNUSED(enabled);
#endif
    };
    
protected:

    virtual void read_from_socket(::ev::io& w) {
        sockaddr_storage source_addr_raw{};
        socklen_t srcaddr_len = sizeof(source_addr_raw);
        
        uint8_t buffer[1024];
        auto read_bytes_or_error = recvfrom(w.fd, buffer, sizeof(buffer), 0, (sockaddr*) &source_addr_raw, &srcaddr_len);
        if (read_bytes_or_error < 0) {
            throw netio_exception("Failed to read UDP packet. err:" + std::to_string(read_bytes_or_error));
        }

        const socketaddr source_address{source_addr_raw};
        // The case below is safe baecause we checked it earlyier to be greater or equal to zero
        this->in_data_cb(iorecord{buffer, (size_t) read_bytes_or_error, source_address});
    }

    virtual ssize_t push_write_queue(::ev::io& w, iorecord& buffer) {
        const auto dest_addr = buffer.get_address();
        const ssize_t written = sendto(w.fd, buffer.ptr(), buffer.size(), this->send_flags, dest_addr.ptr(), dest_addr.size());
        return written;
    }
};

}
