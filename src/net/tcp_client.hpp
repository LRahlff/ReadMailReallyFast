/*
 * tcp_client.hpp
 *
 *  Created on: 03.01.2021
 *      Author: doralitze
 */

#pragma once

#include <ev++.h>

#include <functional>
#include <list>
#include <memory>
#include <string>
#include <unistd.h>

#include "net/async_fd.hpp"
#include "net/connection_client.hpp"
#include "net/ioqueue.hpp"

namespace rmrf::net {

enum class exit_status_t : uint16_t {
    NO_ERROR = 0,
    TIMEOUT = 1
};

/**
 * This class ressembles a TCP client.
 * @class tcp_client
 * @author doralitze BenBe
 * @brief A raw TCP client.
 */
class tcp_client : public connection_client<tcp_client> {
public:
    /**
      * A callback for the destructor must match this definition.
      */
    typedef std::function<void(exit_status_t)> destructor_cb_type;

private:
    const destructor_cb_type destructor_cb;
    const std::string peer_address;

    uint16_t port;
    auto_fd net_socket;
    ::ev::io io;
    ioqueue<iorecord> write_queue;
    bool data_write_active = false;

public:
    /**
     * Construct a new TCP client using an already existing socket. This might be particulary useful if you've
     * got a server and accept incoming connections.
     *
     * @brief Connect to a TCP server
     * @param destructor_cb_ The callback that should be issued when the client closes or looses it's connection
     * @param socket_fd A file descriptor for an already open socket to be wrapped by this client
     * @param peer_address_ The address the socket is bound to on the other end of the connection
     * @param port_ The bound port
     */
    tcp_client(const destructor_cb_type destructor_cb_, auto_fd&& socket_fd, std::string peer_address_, uint16_t port_);

    /**
     * Construct a new TCP client using an address and port pair.
     * @brief Connect to a TCP server
     * @param peer_address_ The address to connect to
     * @param port_ The remote port to connect to described as uint16
     */
    tcp_client(const std::string& peer_address_, const uint16_t port_);

    /**
     * Construct a new TCP client using an address and a service description.
     * @brief Connect to a TCP server
     * @param peer_address_ The address to connect to
     * @param service_or_port The service or port to connect with.
     */
    tcp_client(const std::string& peer_address_, const std::string& service_or_port);

    /**
     * Construct a new TCP client using an address, a service description and a socket family identifier.
     * @brief Connect to a TCP server
     * @param peer_address_ The address to connect to
     * @param service_or_port The service or port to connect with.
     * @param ip_addr_family The IP or service address family as defined in <sys/socket.h>
     */
    tcp_client(const std::string& peer_address_, const std::string& service_or_port, int ip_addr_family);

    /**
     * This descructor will handle the resource deallocation and frees the socket. It also stops the
     * event queue and will call the specified destructor_cb_ (if any).
     * @brief The descructor
     */
    virtual ~tcp_client();

    /**
     * This method sends data over the socket. Keep in mind that this method only enqueues the
     * data and requests it's transmission but does not send the data directly. While this ensures
     * asynchronous execution you need to check if your data has been send using the is_write_queue_empty
     * method.
     * @brief Send data down the socket.
     * @param data The data to send
     */
    virtual void write_data(const std::string& data);

    /**
     * Get the connected sockets address. Keep in mind that the lone existance of this address
     * does not guaruntee the integrity of the connection.
     * @brief Get the connected sockets address
     * @return The peers address.
     */
    std::string get_peer_address();

    /**
     * Use this method in order to retrieve the associated port. A port number of 0 encodes an invalid
     * port state. This either means that the choosen protocol familiy does not support the concepts
     * of ports or the socket is broken.
     * @brief Get the bound port
     * @return The port or 0
     */
    uint16_t get_port();

    /**
     * Use this method in order to check if all pending write requests have been successfully transmitted.
     * This method returns false if the write queue is not empty or the socket is currently transmitting data.
     * Note: If the socket connection crashes with an netio exception it might be possible that this
     * method returns true even if the last data send has been corrupted.
     * @brief Check if all pending data has been send.
     * @return true if all pending data has been send.
     */
    bool is_write_queue_empty();

private:
    /**
     * This method implements the io queue callback. It is responsible for the actual data transactions
     * @param w The io handle to use
     * @param events The event flag container
     */
    void cb_ev(::ev::io& w, int events);

    /**
     * This method pushes the front of the write_queue to the socket. It will automatically advance
     * the internal buffers and discard empty ones.
     * @param w The io handle to use.
     */
    void push_write_queue(::ev::io& w);
};

}
