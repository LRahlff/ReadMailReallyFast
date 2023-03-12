/*
 * tcp_server_socket.cpp
 *
 *  Created on: 02.01.2021
 *      Author: doralitze
 */
#include "net/tcp_server_socket.hpp"

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <functional>

#include "macros.hpp"
#include "net/client_factory.hpp"
#include "net/socketaddress.hpp"
#include "net/sock_address_factory.hpp"
#include "net/socket_utils.hpp"
#include "net/tcp_client.hpp"


namespace rmrf::net {
    
    auto_fd create_tcp_server_socket(const socketaddr& socket_identifier) {
        if (auto f = socket_identifier.family(); f != AF_INET6 && f != AF_INET) {
            throw netio_exception("For now, TCP is only supported over IP(v6).");
        }
        
        auto_fd socket_fd{socket(socket_identifier.family(), SOCK_STREAM, 0)};
        
        if (!socket_fd.valid()) {
            // TODO implement propper error handling
            throw netio_exception("Failed to create socket fd.");
        }
        
        if (auto error = bind(socket_fd.get(), socket_identifier.ptr(), socket_identifier.size()); error != 0) {
            std::string msg = "Failed to bind to all addresses (FIXME). Errorcode: " + std::to_string(error);

            if (socket_identifier.family() == AF_INET6) {
                sockaddr_in* inptr = (sockaddr_in*) socket_identifier.ptr();
                const auto port = ntohs(inptr->sin_port);

                if (port < 1024) {
                    msg += "\nYou tried to bind to a port smaller than 1024. Are you root?";
                }
            } else if (socket_identifier.family() == AF_INET) {
                sockaddr_in6* inptr = (sockaddr_in6*) socket_identifier.ptr();
                const auto port = ntohs(inptr->sin6_port);

                if (port < 1024) {
                    msg += "\nYou tried to bind to a port smaller than 1024. Are you root?";
                }
            }

            throw netio_exception(msg);
        }
        
        make_socket_nonblocking(socket_fd);
        
        if (listen(socket_fd.get(), 20) == -1) {
            throw netio_exception("Failed to enable listening mode for raw socket");
        }
        
        return socket_fd;
    }

tcp_server_socket::tcp_server_socket(
    const socketaddr& socket_identifier,
    async_server_socket::accept_handler_type client_listener
) :
    async_server_socket(std::move(create_tcp_server_socket(socket_identifier)))
{
    this->set_accept_handler(client_listener);
}

static inline socketaddr get_ipv6_socketaddr(const uint16_t port) {
    sockaddr_in6 addr;
    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(port);
    addr.sin6_addr = IN6ADDR_ANY_INIT;
    socketaddr sa{addr};
    return sa;
}

tcp_server_socket::tcp_server_socket(
    const uint16_t port,
    async_server_socket::accept_handler_type client_listener_
) :
    tcp_server_socket{get_ipv6_socketaddr(port), client_listener_}
{}

tcp_server_socket::tcp_server_socket(
    const std::string& interface_description,
    const std::string& port,
    async_server_socket::accept_handler_type client_listener_
) : tcp_server_socket{get_first_general_socketaddr(interface_description, port, socket_t::TCP), client_listener_} {}

std::shared_ptr<connection_client> tcp_server_socket::await_raw_socket_incomming(const auto_fd& server_socket) {

    struct sockaddr_storage client_addr_raw;
    socklen_t client_len = sizeof(client_addr_raw);
    int client_fd_raw = accept(server_socket.get(), (struct sockaddr*) &client_addr_raw, &client_len);
    const socketaddr client_address{client_addr_raw};

    if (client_fd_raw < 0) {
        throw netio_exception("Unable to bind incoming client");
    }
    
    auto client_socket = auto_fd(client_fd_raw);
    make_socket_nonblocking(client_socket);

    if (this->is_low_latency_mode_enabled()) {
        int one = 1;
        setsockopt(client_socket.get(), IPPROTO_TCP, TCP_NODELAY, &one, sizeof(one));
#ifdef __linux__
        setsockopt(client_socket.get(), IPPROTO_TCP, TCP_QUICKACK, &one, sizeof(one));
#endif
    }
    
    return std::make_shared<tcp_client>(
        this->get_locked_destructor_callback(),
        std::move(client_socket),
        get_own_address_after_connect(client_socket),
        client_address);
}


}
