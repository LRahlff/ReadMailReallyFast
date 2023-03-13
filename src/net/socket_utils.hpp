#pragma once

#include "net/async_fd.hpp"

namespace rmrf::net {
    /**
     * @brief Append the O_NONBLOCK flag to an existing socket
     * @param socket The socket to modify
     */
     void make_socket_nonblocking(auto_fd& socket);
}
