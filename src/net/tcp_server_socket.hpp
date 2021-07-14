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
    std::atomic_uint32_t number_of_connected_clients;

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
    int get_number_of_connected_clients() const;

private:
    void await_raw_socket_incomming(async_server_socket::self_ptr_type ass, const auto_fd& socket);
    void client_destructed_cb(exit_status_t exit_status);
};

}
