/*
 * tcp_client.cpp
 *
 *  Created on: 03.01.2021
 *      Author: doralitze
 */

#include "net/tcp_client.hpp"

#include <ev++.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <utility>
#include <deque>

#include "net/netio_exception.hpp"
#include "net/socketaddress.hpp"

namespace rmrf::net {

tcp_client::tcp_client(
    const destructor_cb_type destructor_cb_,
    auto_fd &&socket_fd,
    std::string peer_address_,
    uint16_t port_
) :
    connection_client{},
    destructor_cb(destructor_cb_),
    peer_address(peer_address_), port(port_),
    net_socket(std::forward<auto_fd>(socket_fd)),
    io{}, write_queue{}
{
    io.set<tcp_client, &tcp_client::cb_ev>(this);
    io.start(this->net_socket.get(), ::ev::READ);
    // TODO log created client
}

tcp_client::tcp_client(
    const std::string &peer_address_,
    const std::string &service_or_port,
    int ip_addr_family
) :
    connection_client{},
    destructor_cb(nullptr),
    peer_address(peer_address_),
    port(0),
    net_socket(nullfd),
    io{},
    write_queue{}
{
    if (!(ip_addr_family == AF_INET || ip_addr_family == AF_INET6)) {
        throw netio_exception("Invalid IP address family.");
    }

    this->net_socket = auto_fd(socket(ip_addr_family, SOCK_STREAM, 0));

    if (!this->net_socket.valid()) {
        // TODO implement proper error handling
        throw netio_exception("Failed to request socket fd from kernel.");
    }

    // TODO Extract DNS/service resolution into separate library
    // TODO build another nice HL structure wrapper for outbound connections
    std::deque<socketaddr> connection_candidates;
    int status;
    {
        //Reduce scope of locally declared variables
        //May be candidate for extraction into own method
        addrinfo hints;
        addrinfo* servinfo = nullptr;
        memset(&hints, 0, sizeof hints);
        hints.ai_family = ip_addr_family;
        hints.ai_socktype = SOCK_STREAM;

        if ((status = getaddrinfo(peer_address_.c_str(), service_or_port.c_str(), &hints, &servinfo)) != 0) {
            throw netio_exception("Failed to resolve address '" + peer_address_ + "' with service '" + service_or_port + "': " + gai_strerror(status));
        }

        // TODO: Prefer IPv6 over IPv4
        for (auto p = servinfo; p != NULL; p = p->ai_next) {
            if (p->ai_family == AF_INET) {
                socketaddr sa{(sockaddr_in*)p->ai_addr};
                connection_candidates.push_back(sa);
            } else if (p->ai_family == AF_INET6) {
                socketaddr sa{(sockaddr_in6*)p->ai_addr};
                connection_candidates.push_front(sa);
            }
        }

        freeaddrinfo(servinfo);
    }

    status = 1;

    do {
        if (connection_candidates.empty()) {
            throw netio_exception("Unable to find suitable connection candidate.");
        }

        socketaddr socket_identifier = connection_candidates.front();
        connection_candidates.pop_front();
        auto_fd socket_candidate{socket(socket_identifier.family(), SOCK_STREAM, 0)};

        if (socket_candidate.valid()) {
            if (connect(socket_candidate.get(), socket_identifier.ptr(), socket_identifier.size()) == 0) {
                status = 0;
                this->net_socket = std::forward<auto_fd>(socket_candidate);
                fcntl(this->net_socket.get(), F_SETFL, fcntl(this->net_socket.get(), F_GETFL, 0) | O_NONBLOCK);

                // Hier bin ich mir nicht sicher, wie ich das am besten mache. Auch mit socketaddr und type cast ist das irgendwie doof.
                // Das Problem besteht darin, dass erst nach erfolgreichem connect der Port auf dieser Seite bekannt ist.
                socketaddr sa_local;
                socklen_t sa_local_len = sizeof(sockaddr_storage);

                if (getsockname(this->net_socket.get(), sa_local.ptr(), &sa_local_len)) {
                    // Update length field after the internal structure was modified
                    // TODO: Maybe make this an internal method in socketaddr to update the size
                    sa_local = sa_local.ptr();

                    //The pointer casts are safe due to operator overloading in socketaddr ...
                    switch (sa_local.family()) {
                    case AF_INET:
                        this->port = ntohs(((sockaddr_in*)sa_local)->sin_port);
                        break;

                    case AF_INET6:
                        this->port = ntohs(((sockaddr_in6*)sa_local)->sin6_port);
                        break;

                    default:
                        throw netio_exception("Invalid/unexpected local socket address type");
                    }
                }
            }
        }

        // We don't need to worry about closing broken fd as auto_fd handles this for us
    } while (status == 1);

    io.set<tcp_client, &tcp_client::cb_ev>(this);
    io.start(this->net_socket.get(), ::ev::READ);
    //TODO log connected client
}

tcp_client::tcp_client(
    const std::string &peer_address_,
    const std::string &service_or_port
) :
    tcp_client(peer_address_, service_or_port, AF_UNSPEC)
{}

tcp_client::tcp_client(
    const std::string &peer_address_,
    const uint16_t port_
) :
    tcp_client(peer_address_, std::to_string(port_))
{}

tcp_client::~tcp_client() {
    io.stop();

    if (destructor_cb) {
        destructor_cb(exit_status_t::NO_ERROR);
    }
}

void tcp_client::write_data(const std::string &data) {
    // Create NICBuffer from data
    this->write_queue.push_back(iorecord{data.c_str(), data.size()});
    this->io.set(::ev::READ | ::ev::WRITE);
}

inline std::string buffer_to_string(char* buffer, ssize_t bufflen) {
    return std::string(buffer, (size_t)bufflen);
}

void tcp_client::cb_ev(::ev::io &w, int events) {
    if (events & ::ev::ERROR) {
        // Handle errors
        // Log and throw?
        return;
    }

    if (events & ::ev::READ) {
        // notify incomming_data_cb
        char buffer[1024];

        ssize_t n_read_bytes = recv(w.fd, buffer, sizeof(buffer), 0);

        if (n_read_bytes < 0) {
            throw netio_exception("Failed to read from network socket.");
        }

        if (n_read_bytes == 0) {
            // TODO find a way to properly announce the closed connection
            delete this;
        } else {
            this->in_data_cb(buffer_to_string(buffer, n_read_bytes));
        }
    }

    if (events & ::ev::WRITE) {
        // Handle sending data
        push_write_queue(w);
    }

    if (write_queue.empty()) {
        io.set(::ev::READ);
    } else {
        io.set(::ev::READ | ::ev::WRITE);
    }
}

void tcp_client::push_write_queue(::ev::io &w) {
    if (this->write_queue.empty()) {
        io.set(::ev::READ);
        return;
    }

    this->data_write_active = true;
    iorecord buffer = this->write_queue.pop_front();
    ssize_t written = write(w.fd, buffer.ptr(), buffer.size());

    if (written >= 0) {
        buffer.advance((size_t)written);
    } else if (EAGAIN_WRAPPER) {
        throw netio_exception("Failed to write latest buffer content.");
    }

    this->write_queue.push_front(buffer);
    this->data_write_active = false;
}

inline std::string tcp_client::get_peer_address() {
    return this->peer_address;
}

inline uint16_t tcp_client::get_port() {
    return this->port;
}

inline bool tcp_client::is_write_queue_empty() {
    return this->write_queue.empty() && !this->data_write_active;
}

}
