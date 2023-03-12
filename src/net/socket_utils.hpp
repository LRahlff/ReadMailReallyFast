#pragma once

#include "net/async_fd.hpp"
#include "net/socketaddress.hpp"

namespace rmrf::net {
    /**
     * @brief Append the O_NONBLOCK flag to an existing socket
     * @param socket The socket to modify
     */
     void make_socket_nonblocking(auto_fd& socket);
     
     /**
     * This method queries the remote address of a connected client. Please note that only TCP sockets are supported
     * at the moment. This operation will fail if an unsupported socket type is provided.
     * @brief Get the address of the connected remote client
     * @param socket The socket representing the client
     * @return The remote socketaddr pair
     */
    [[nodiscard]] socketaddr get_own_address_after_connect(const auto_fd& socket);
}
