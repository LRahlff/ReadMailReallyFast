#include "unix_socket_server.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include "net/netio_exception.hpp"
#include "net/socket_utils.hpp"

auto_fd construct_server_fd(const socketaddr& addr) {
    if(addr.family() != AF_UNIX) {
        throw netio_exception("Expected a UNIX socket file path.");
    }

    // man 7 unix suggests the ussage of SOCK_SEQPACKET, but we'd loose the ability to distinguish multiple clients if we do so
    auto_fd socket_fd{socket(socket_identifier.family(), SOCK_STREAM, 0)};
    
    if(!socket_fd.valid()) {
        throw netio_exception("Failed to create UNIX socket. Do you have the permissions to do this?");
    }
    
    if (auto error = bind(socket_fd.get(), socket_identifier.ptr(), socket_identifier.size()); error != 0) {
        throw netio_exception("Failed to bin to socket" + addr.str());
    }
    
    make_socket_nonblocking(socket_fd);
    
    if (listen(socket_fd.get(), 5) == -1) {
        throw netio_exception("Failed to enable listening mode for raw socket");
    }
        
    return socket_fd;
}

unix_socket_server::unix_socket_server(
    const socketaddr& socket_identifier,
    async_server_socket::accept_handler_type client_listener_
) : async_server_socket{construct_server_fd(socket_identifier)} {}

unix_socket_server::~unix_socket_server() : {}

std::shared_ptr<connection_client> unix_socket_server::await_raw_socket_incomming(const auto_fd& socket) {
    auto client_socket = auto_fd{accept(socket.get(), nullptr, nullptr)};
    
    if(!client_socket.valid()) {
        throw netio_exception("Failed to accept incomming client to unix socket.");
    }
    
    make_socket_nonblocking(client_socket);
    
    const socketaddr own_address{}, peer_address{};
    
    return std::make_shared<tcp_client>(
        this->get_locked_destructor_callback(),
        std::move(client_socket),
        own_address,
        peer_address);
}
