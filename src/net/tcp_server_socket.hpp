/*
 * tcp_server_socket.hpp
 *
 *  Created on: 02.01.2021
 *      Author: doralitze
 */

#pragma once

#include <memory>

#include "net/async_server.hpp"
#include "net/netio_exception.hpp"
#include "net/socketaddress.hpp"
#include "net/tcp_client.hpp"

namespace rmrf::net {

/**
 * This class awaits incomming TCP connections and submits them to the provided client listener callback.
 * @class tcp_server_socket
 * @author doralitze
 * @date 14/01/21
 * @file tcp_server_socket.hpp
 * @brief A TCP server delivering incomming connections to you.
 */
class tcp_server_socket : public async_server_socket {
private:
    bool low_latency = false;

public:
    /**
     * This constructor accepts a port to bind to and the client listener that should be called when clients arrive.
     * This constructor automatically binds to all avaliable IPv6 interfaces on the specified port.
     * @brief Construct a TCP server that listens on all interfaces
     * @param port The port to bind to
     * @param client_listener_ The client listener to call when a client arrives.
     */
    tcp_server_socket(const uint16_t port, async_server_socket::accept_handler_type client_listener_);

    /**
     * This constructor accepts an interface description to bind to and the client listener that should be called when clients arrive.
     * @brief Construct a TCP server that listens on the specified interface
     * @param socket_identifier The socket to bind to
     * @param port The port to bind to
     * @param client_listener_ The client listener to call when a client arrives.
     */
    tcp_server_socket(const socketaddr& socket_identifier, async_server_socket::accept_handler_type client_listener_);

    /**
     * This constructor accepts an interface address and a port to bind to and the client listener that should be called when clients arrive.
     * This constructor tries to guess the correct address family and interface.
     * @brief Construct a TCP server that listens the specified interface
     * @param interface_description The interface to bind to
     * @param port The port to bind to
     * @param client_listener_ The client listener to call when a client arrives.
     */
    tcp_server_socket(const std::string& interface_description, const std::string& port, async_server_socket::accept_handler_type client_listener_);
    
    virtual ~tcp_server_socket() {}

    /**
     * Enable TCP low latency mode. It disables Nagle's algorithm on all platforms and
     * furthermore quickacks on linux. Please note that this will increase traffic.
     * @brief Enable or disable TCP low latency mode.
     * @param enabled enable or disable fast package dispatching.
     */
    inline void set_low_latency_mode(bool enabled) {
        this->low_latency = enabled;
    }

    /**
     * @brief Get the current low latency mode flag
     * @return True if the socket is in low latency mode. Otherwise false.
     */
    inline bool is_low_latency_mode_enabled() {
        return this->low_latency;
    }

protected:
    virtual std::shared_ptr<connection_client> await_raw_socket_incomming(const auto_fd& socket);
};

}
