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
#include "net/socketaddress.hpp"
#include "net/tcp_client.hpp"


namespace rmrf::net {

tcp_server_socket::tcp_server_socket(
    const socketaddr &socket_identifier,
    incoming_client_listener_type client_listener_
) :
    ss{nullptr},
    client_listener(client_listener_),
    overflow_client_listener(nullptr),
    number_of_connected_clients(0),
    max_number_of_simulataneusly_allowed_clients(0)
{
    auto_fd socket_fd{socket(socket_identifier.family(), SOCK_STREAM, 0)};

    if (!socket_fd.valid()) {
        // TODO implement propper error handling
        throw netio_exception("Failed to create socket fd.");
    }

    if (bind(socket_fd.get(), socket_identifier.ptr(), socket_identifier.size()) != 0) {
        std::string msg = "Failed to bind to all addresses (FIXME)";

        if (socket_identifier.family() == AF_INET6 || socket_identifier.family() == AF_INET) {
            // TODO find a nice way to check for the port
            /*
            if (port < 1024) {
                msg += "\nYou tried to bind to a port smaller than 1024. Are you root?";
            }
            */
        }

        throw netio_exception(msg);
    }

    // Append the non blocking flag to the file state of the socket fd.
    // TODO This might be linux only. We should check that
    fcntl(socket_fd.get(), F_SETFL, fcntl(socket_fd.get(), F_GETFL, 0) | O_NONBLOCK);

    if (listen(socket_fd.get(), 5) == -1) {
        throw netio_exception("Failed to enable listening mode for raw socket");
    }

    this->ss = std::make_shared<async_server_socket>(std::forward<auto_fd>(socket_fd));

    using namespace std::placeholders;
    this->ss->set_accept_handler(std::bind(&tcp_server_socket::await_raw_socket_incomming, this, _1, _2));
    this->set_low_latency_mode(false);
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
    incoming_client_listener_type client_listener_
) :
    tcp_server_socket{get_ipv6_socketaddr(port), client_listener_}
{}

void tcp_server_socket::await_raw_socket_incomming(
    async_server_socket::self_ptr_type ass,
    const auto_fd &socket
) {
    MARK_UNUSED(ass);

    struct sockaddr_storage client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd_raw = accept(socket.get(), (struct sockaddr*)&client_addr, &client_len);
    socketaddr address{client_addr};

    if (client_fd_raw < 0) {
        throw netio_exception("Unable to bind incoming client");
    }

    auto flags = O_NONBLOCK;
    if (
        const auto existing_fd_flags = fcntl(client_fd_raw, F_GETFL, 0);
        existing_fd_flags == -1 || fcntl(client_fd_raw, F_SETFL, existing_fd_flags | flags) == -1
    ) {
        throw netio_exception("Failed to set socket mode. fcntl resulted in error:" + std::to_string(errno));
    }

    // Generate client object from fd and announce it
    this->number_of_connected_clients++;
    using namespace std::placeholders;

    if (this->is_low_latency_mode_enabled()) {
        int one = 1;
        setsockopt(socket.get(), IPPROTO_TCP, TCP_NODELAY, &one, sizeof(one));
#ifdef __linux__
        setsockopt(socket.get(), IPPROTO_TCP, TCP_QUICKACK, &one, sizeof(one));
#endif
    }

    auto weak_this = this->weak_from_this();
    tcp_client::destructor_cb_type cb = [weak_this](exit_status_t status) {
        auto ref_this = weak_this.lock();

        if (!ref_this) {
            return;
        }

        ref_this->client_destructed_cb(status);
    };
    auto client = std::make_shared<tcp_client>(cb, auto_fd(client_fd_raw), address);

    if (this->max_number_of_simulataneusly_allowed_clients == 0 || this->get_number_of_connected_clients() <= this->max_number_of_simulataneusly_allowed_clients) {
        this->client_listener(client);
    } else if (this->overflow_client_listener != nullptr) {
        this->overflow_client_listener(client);
    }
}

unsigned int tcp_server_socket::get_number_of_connected_clients() const {
    return this->number_of_connected_clients;
}

void tcp_server_socket::client_destructed_cb(exit_status_t exit_status) {
    MARK_UNUSED(exit_status);

    this->number_of_connected_clients--;
}

void tcp_server_socket::set_client_overflow_handler(incoming_client_listener_type overflow_client_listener_) {
    this->overflow_client_listener = overflow_client_listener_;
}

void tcp_server_socket::set_maximum_concurrent_connections(unsigned int max_connections) {
    this->max_number_of_simulataneusly_allowed_clients = max_connections;
}

}
