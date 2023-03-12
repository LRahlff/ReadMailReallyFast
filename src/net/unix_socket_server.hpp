#pragma once

#include "net/async_server.hpp" // Base class: rmrf::net::async_server_socket
#include "net/socketaddress.hpp"

namespace rmrf::net {
/**
 * @class unix_socket_server
 * @author doralitze
 * @date 12/03/23
 * @file unix_socket_server.hpp
 * @brief A unix file socket server
 */
class unix_socket_server : public async_server_socket
{
public:
    unix_socket_server(const socketaddr& socket_identifier, async_server_socket::accept_handler_type client_listener_);
    virtual ~unix_socket_server();

public:
    virtual std::shared_ptr<connection_client> await_raw_socket_incomming(const auto_fd& socket);
};

}
