#include "net/socket_utils.hpp"

#include <fcntl.h>

#include "net/netio_exception.hpp"

namespace rmrf::net {
     void make_socket_nonblocking(auto_fd& socket) {
        if (const auto existing_fd_flags = fcntl(socket.get(), F_GETFL, 0);
            existing_fd_flags == -1 || fcntl(socket.get(), F_SETFL, existing_fd_flags | O_NONBLOCK) == -1) {
            throw netio_exception("Failed to set socket mode. fcntl resulted in error:" + std::to_string(errno));
        }
    }
}
