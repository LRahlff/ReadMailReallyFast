#include "net/client_factory.hpp"

#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "net/async_fd.hpp"
#include "net/tcp_client.hpp"
#include "net/sock_address_factory.hpp"
#include "net/socket_utils.hpp"
#include "net/udp_client.hpp"

namespace rmrf::net {
    
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
    
    [[nodiscard]] std::unique_ptr<udp_client> client_factory_construct_udp_client(const socketaddr& socket_identifier, connection_client::incomming_data_cb cb) {
        const auto family = socket_identifier.family();

        // TODO maybe implement also unix sockets with SOCK_SEQPACKET if we need them
        if (!(family == AF_INET || family == AF_INET6)) {
            std::stringstream ss;
            ss << "Invalid IP address family. (" <<  family << ")";
            throw netio_exception(ss.str());
        }

        auto net_socket = auto_fd(socket(family, SOCK_DGRAM, 0));

        if (!net_socket.valid()) {
            return nullptr;
        }

        make_socket_nonblocking(net_socket);

        auto c = std::make_unique<udp_client>(std::move(net_socket), socket_identifier);
        if(cb) {
            c->set_incomming_data_callback(cb);
        }
        return c;
    }
    
    [[nodiscard]] std::unique_ptr<tcp_client> client_factory_construct_tcp_client(const socketaddr& socket_identifier, connection_client::incomming_data_cb cb) {
        auto_fd socket_candidate{socket(socket_identifier.family(), SOCK_STREAM, 0)};
        
        if (socket_candidate.valid()) {
            if (connect(socket_candidate.get(), socket_identifier.ptr(), socket_identifier.size()) == 0) {
                make_socket_nonblocking(socket_candidate);
                const auto own_address = get_own_address_after_connect(socket_candidate);
                
                // TODO create client object using socket_candidate, own_address and socket_identifier as remote address
                auto c = std::make_unique<tcp_client>(nullptr, std::move(socket_candidate), own_address, socket_identifier);
                if(cb) {
                    c->set_incomming_data_callback(cb);
                }
                return c;
            }
        }
        return nullptr;
    }
    
    [[nodiscard]] std::unique_ptr<connection_client> connect(const socketaddr& address, const socket_t& type) {
        switch(type) {
            case socket_t::TCP:
                return client_factory_construct_tcp_client(address);
            case socket_t::UDP:
                return client_factory_construct_udp_client(address);
            case socket_t::UNIX:
                // TODO implement
                return nullptr;
            default:
                return nullptr;
        }
    }
    
    socket_t guess_socket_type_from_address(const socketaddr& address) {
        switch(address.family()) {
            case AF_INET:
            case AF_INET6:
                return socket_t::TCP;
            case AF_UNIX:
                return socket_t::UNIX;
#ifdef __linux__
            case AF_NETLINK:
                throw netio_exception("Connecting to netlink interfaces isn't supported yet.");
#endif
            default:
                throw netio_exception("Trying to assign unknown address family");
        }
    }

    [[nodiscard]] std::unique_ptr<connection_client> connect(const socketaddr& address) {
        return connect(address, guess_socket_type_from_address(address));
    }

    [[nodiscard]] std::unique_ptr<connection_client> connect(const std::string& peer_address, const std::string& service, int ip_addr_family) {
        const auto candidates = get_socketaddr_list(peer_address, service);
        
        if (candidates.empty()) {
            throw netio_exception("Unable to find suitable connection candidate.");
        }
        
        if (ip_addr_family == AF_UNSPEC) {
            ip_addr_family = candidates.front().family();
        }

        if (!(ip_addr_family == AF_INET || ip_addr_family == AF_INET6 || ip_addr_family == AF_UNIX)) {
            throw netio_exception("Invalid IP address family.");
        }
        
        std::unique_ptr<connection_client> latest_client;
        
        for(const auto& current_connection_candidate : candidates) {
            if(latest_client = connect(current_connection_candidate, guess_socket_type_from_address(current_connection_candidate));
                latest_client) {
                break;
            }
        }
        
        if (!latest_client) {
            throw netio_exception("Unable to find suitable connection candidate.");
        }
        
        return latest_client;
    }
 }
