#include "net/socketaddress.hpp"

namespace rmrf::net {
    
    [[nodiscard]] std::string socketaddr::str() const {
        std::ostringstream oss;
        oss << "SocketAddress: ";
        const int buffer_size = 1024;
        char buffer[buffer_size];

        switch (this->family()) {
        case AF_INET:
            inet_ntop(AF_INET, &((sockaddr_in*)&addr)->sin_addr, buffer, buffer_size);
            oss << "IPv4 " << buffer << ":" << ntohs(((sockaddr_in*)&addr)->sin_port);
            break;
        case AF_INET6:
            inet_ntop(AF_INET6, &((sockaddr_in6*)&addr)->sin6_addr, buffer, buffer_size);
            oss << "IPv6 ["<< buffer << "]:" << ntohs(((sockaddr_in6*)&addr)->sin6_port);
            break;
        case AF_UNIX:
            oss << "FileSocket " << ((sockaddr_un*)&addr)->sun_path;
            break;
#ifdef __linux__
	case AF_NETLINK: {
            const auto nl_sock_ptr = (sockaddr_nl*) &addr;
            oss << "Netlink g:" << nl_sock_ptr->nl_groups << " p:" << nl_sock_ptr->nl_pad << " pid:" << nl_sock_ptr->nl_pid;
            break;
        }
#endif // __linux__
        default:
            oss << "Unknown Socket Address Type";
            break;
        }

        return oss.str();
    }
}
