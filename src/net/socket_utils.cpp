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
    
    [[nodiscard]] socketaddr get_own_address_after_connect(const auto_fd& socket) {
        socketaddr own_address;
        socklen_t sa_local_len = sizeof(sockaddr_storage);

        if (getsockname(socket.get(), own_address.ptr(), &sa_local_len) == 0) {
            // Update length field after the internal structure was modified
            // TODO: Maybe make this an internal method in socketaddr to update the size
            own_address = own_address.ptr();
        } else {
            switch(errno) {
                case EBADF:
                case ENOTSOCK:
                    throw netio_exception("Invalid file descriptor provided to obtain own address. ERRNO: " + std::to_string(errno));
                case EFAULT:
                case EINVAL:
                    throw netio_exception("Invlid data structure for information retrival of own socket address provided. ERRNO: " + std::to_string(errno));
                case ENOBUFS:
                    throw netio_exception("Kernel temporarily out of buffer space to store own address informatio.n ERRNO:." + std::to_string(errno));
                default:
                    throw netio_exception("Unexpected error while requesting own socket address. ERRNO: " + std::to_string(errno));
            }
        }
        return own_address;
    }
}
