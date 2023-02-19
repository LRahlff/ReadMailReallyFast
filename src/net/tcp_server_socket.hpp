/*
 * tcp_server_socket.hpp
 *
 *  Created on: 02.01.2021
 *      Author: doralitze
 */

#pragma once

#include <atomic>
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
class tcp_server_socket : public std::enable_shared_from_this<tcp_server_socket>{
public:
    typedef std::function<void(tcp_client)> incoming_client_listener_type;

private:
    async_server_socket::self_ptr_type ss;
    incoming_client_listener_type client_listener;
    incoming_client_listener_type overflow_client_listener;
    std::atomic_uint32_t number_of_connected_clients;
    unsigned int max_number_of_simulataneusly_allowed_clients;
    bool low_latency = false;

public:
    /**
     * This constructor accepts a port to bind to and the client listener that should be called when clients arrive.
     * This constructor automatically binds to all avaliable IPv6 interfaces on the specified port.
     * @brief Construct a TCP server that listens on all interfaces
     * @param port The port to bind to
     * @param client_listener_ The client listener to call when a client arrives.
     */
    tcp_server_socket(const uint16_t port, incoming_client_listener_type client_listener_);

    /**
     * This constructor accepts an interface description to bind to and the client listener that should be called when clients arrive.
     * @brief Construct a TCP server that listens on the specified interface
     * @param socket_identifier The socket to bind to
     * @param port The port to bind to
     * @param client_listener_ The client listener to call when a client arrives.
     */
    tcp_server_socket(const socketaddr& socket_identifier, incoming_client_listener_type client_listener_);

    /**
     * This method provides you with the current number of connected clients. When a client
     * disconnects this number will be reduced. When a new client arrives this number will be incremented.
     * @brief Get the current number of connected clients.
     * @return  The number of connected clients
     */
    unsigned int get_number_of_connected_clients() const;

    /**
     * This method sets the overflow handler to use if the maximum number of allowed clients was reached.
     * The purpose of said handler is to inform the connected client that the connection cant be established
     * due to isufficient resources. It should transmit the appropriate error message and drop the client.
     * @brief Set the client overflow handler
     * @param overflow_client_listener The listener that handles clients that couldn't be accepted
     */
    void set_client_overflow_handler(incoming_client_listener_type overflow_client_listener);

    /**
     * Use this method in order to set the maximum number of allowed connections. Set it to
     * 0 in order to disable the limit. If anything other than 0 is set it is highly recommended
     * to also set an overflow handler.
     * @brief Set the maximum allowed simultaneus connections.
     * @param max_connections The maximum number of allowed connections.
     */
    void set_maximum_concurrent_connections(unsigned int max_connections);

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

private:
    void await_raw_socket_incomming(async_server_socket::self_ptr_type ass, const auto_fd& socket);
    void client_destructed_cb(exit_status_t exit_status);
};

}
