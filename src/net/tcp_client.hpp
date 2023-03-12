/*
 * tcp_client.hpp
 *
 *  Created on: 03.01.2021
 *      Author: doralitze
 */

#pragma once

#include <functional>
#include <list>
#include <memory>
#include <string>
#include <unistd.h>

#include "net/connection_client.hpp"
#include "net/iorecord.hpp"

namespace rmrf::net {

/**
 * This class ressembles a TCP client.
 * @class tcp_client
 * @author doralitze BenBe
 * @brief A raw TCP client.
 */
class tcp_client : public connection_client, std::enable_shared_from_this<tcp_client> {
public:

    /**
     * Construct a new TCP client using an already existing socket. This might be particulary useful if you've
     * got a server and accept incoming connections.
     *
     * @brief Connect to a TCP server
     * @param destructor_cb_ The callback that should be issued when the client closes or looses it's connection
     * @param socket_fd A file descriptor for an already open socket to be wrapped by this client
     * @param peer_address The address the socket is bound to on the other end of the connection
     */
    tcp_client(const destructor_cb_type destructor_cb_, auto_fd&& socket_fd, const socketaddr& own_address, const socketaddr& peer_address);

    /**
     * This descructor will handle the resource deallocation and frees the socket. It also stops the
     * event queue and will call the specified destructor_cb_ (if any).
     * @brief The descructor
     */
    virtual ~tcp_client();

    /**
     * Use this method in order to retrieve the associated port. A port number of 0 encodes an invalid
     * port state. This either means that the choosen protocol familiy does not support the concepts
     * of ports or the socket is broken.
     * @brief Get the bound port
     * @return The port or 0
     */
    uint16_t get_port();

protected:
    virtual void read_from_socket(::ev::io& w);
    virtual ssize_t push_write_queue(::ev::io& w, iorecord& buffer);
private:
    auto_fd get_connection(const std::string& peer_address_, const std::string& service_or_port, int ip_addr_family);
};

}
