#include "net/sock_address_factory.hpp"

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>

#include <net/address.hpp>

namespace rmrf::net {

    std::string format_network_error(int error) {
        switch (error) {
        case EAI_ADDRFAMILY:
            return "There was no compatible address for the requested families. (EAI_ADDRFAMILY)";
        case EAI_AGAIN:
            return "The consulted DNS server reported a temporary lookup failure. Try again later. (EAI_AGAIN)";
        case EAI_BADFLAGS:
            return "The supplied host information did not suffice to identify a host. (EAI_BADFLAGS)";
        case EAI_FAIL:
            return "The DNS server crashed. (EAI_FAIL)";
        case EAI_FAMILY:
            return "The required protocol family is not supported by this host. (EAI_FAMILY)";
        case EAI_MEMORY:
            return "Out of Memory while performing DNS lookup. (EAI_MEMORY)";
        case EAI_NODATA:
            return "The requested DNS entry does not contain an A or AAAA entry. (EAI_NODATA)";
        case EAI_NONAME:
            return "There was no matching nodename, service tuple. (EAI_NONAME)";
        case EAI_SERVICE:
            return "The requested service entry is not avaiable with the requested socket type. (EAI_SERVICE)";
        case EAI_SOCKTYPE:
            return "There was a mismatch of the requested socket type (TCP and UDP are mutually exclusive). (EAI_SOCKTYPE)";
        default:
            return std::to_string(error);
        }
    }

    bool decode_address(std::list<socketaddr> &l, addrinfo* looked_up_addrs, const int port) {
        if (looked_up_addrs->ai_family == AF_UNIX) {
            const sockaddr_un* unix_addr = (sockaddr_un*) looked_up_addrs->ai_addr;
            const sockaddr_un addr{*unix_addr};
            socketaddr sa{addr};
            l.push_back(sa);
        } else if (looked_up_addrs->ai_family == AF_INET6) {
            const sockaddr_in6* ip6_addr = (sockaddr_in6*) looked_up_addrs->ai_addr;
            sockaddr_in6 addr{*ip6_addr};

            if (port != -1) {
                addr.sin6_port = htons((uint16_t) port);
            }

            socketaddr sa{addr};
            l.push_back(sa);
        } else if (looked_up_addrs->ai_family == AF_INET) {
            const sockaddr_in* ip6_addr = (sockaddr_in*) looked_up_addrs->ai_addr;
            sockaddr_in addr{*ip6_addr};

            if (port != -1) {
                addr.sin_port = htons((uint16_t) port);
            }

            socketaddr sa{addr};
            l.push_back(sa);
        } else {
            throw std::invalid_argument("The resulting address family should exist. Instead it is " + std::to_string(looked_up_addrs->ai_family));
        }

        return true;
    }

    bool is_plain_ip_address(const std::string &interface_description) {
        if (
            interface_description.starts_with("[") &&
            interface_description.ends_with("]") &&
            is_valid_ip6addr(
                interface_description.substr(1, interface_description.length() - 2)
            )
        ) {
            return true;
        }

        return is_valid_ip4addr(interface_description) || is_valid_ip6addr(interface_description);
    }

    socketaddr parse_plain_ip_address(const std::string &interface_description, const uint16_t port, const socket_t socket_type) {
        (void)socket_type;

        if (interface_description.find(':') != std::string::npos) {
            // Assume IPv6
            struct in6_addr addr;

            if (interface_description.starts_with("[") && interface_description.ends_with("]")) {
                const std::string tmp = interface_description.substr(1, interface_description.length() - 2);

                if (::inet_pton(AF_INET6, tmp.c_str(), &addr) != 1) {
                    return {};
                }
            } else {
                if (::inet_pton(AF_INET6, interface_description.c_str(), &addr) != 1) {
                    return {};
                }
            }

            sockaddr_in6 addr_ip6;
            addr_ip6.sin6_family = AF_INET6;
            addr_ip6.sin6_addr = addr;
            addr_ip6.sin6_port = htons(port);
            socketaddr sa{addr_ip6};
            return sa;
        } else {
            // Assume IPv4
            struct in_addr addr;

            if (::inet_pton(AF_INET, interface_description.c_str(), &addr) != 1) {
                return {};
            }

            sockaddr_in addr_ip4;
            addr_ip4.sin_family = AF_INET;
            addr_ip4.sin_addr = addr;
            addr_ip4.sin_port = htons(port);
            socketaddr sa{addr_ip4};
            return sa;
        }
    }
    
    [[nodiscard]] inline int get_socket_type_hint(const socket_t& type) {
        switch(type) {
            default:
            case socket_t::UNIX:
            case socket_t::TCP:
                return SOCK_STREAM;
            case socket_t::UDP:
                return SOCK_DGRAM;
        }
    }
    
    [[nodiscard]] inline int get_socket_protocol_hint(const socket_t& type) {
        switch(type) {
            default:
            case socket_t::UNIX:
                return 0;
            case socket_t::TCP:
                return IPPROTO_TCP;
            case socket_t::UDP:
                return IPPROTO_UDP;
        }
    }

    std::list<socketaddr> get_socketaddr_list(const std::string &interface_description, const std::string &service_or_port, const socket_t socket_type) {
        
        if (socket_type == socket_t::UNIX) {
            sockaddr_un storage;
            strncpy(storage.sun_path, interface_description.c_str(), sizeof(storage.sun_path));
            std::list<socketaddr> l = {socketaddr{storage}};
            return l;
        }
        
        int port = -1;

        try {
            port = (uint16_t) std::stoi(service_or_port);
        } catch (const std::invalid_argument &_) {
            // Could not parse port. Continue as service
        }

        // TODO sort to prioritize local addresses over more remote ones

        if (port != -1 && is_plain_ip_address(interface_description)) {
            std::list<socketaddr> l = {parse_plain_ip_address(interface_description, (uint16_t) port, socket_type)};
            return l;
        }

        // Attempt DNS lookup
        struct addrinfo hints = {};
        struct addrinfo* addrs = nullptr;

        hints.ai_family = AF_INET6;
        hints.ai_socktype = get_socket_type_hint(socket_type);
        hints.ai_protocol =  get_socket_protocol_hint(socket_type);

        if (auto dns_error = getaddrinfo(interface_description.c_str(), port == -1 ? service_or_port.c_str() : NULL, &hints, &addrs); dns_error != 0) {
            hints.ai_family = AF_UNSPEC;
            dns_error = getaddrinfo(interface_description.c_str(), NULL, &hints, &addrs);

            if (dns_error != 0) {
                if(addrs != nullptr) {
                    freeaddrinfo(addrs);
                }
                throw std::invalid_argument("Something went wrong with the DNS lookup. Error:" + format_network_error(dns_error));
            }
        }

        std::list<socketaddr> l;

        for (auto result_ptr = addrs; result_ptr != NULL; result_ptr = result_ptr->ai_next) {
            decode_address(l, result_ptr, port);
        }

        freeaddrinfo(addrs);
        return l;
    }

    socketaddr get_first_general_socketaddr(const std::string &interface_description, const std::string &service, const socket_t socket_type) {
        const auto l = get_socketaddr_list(interface_description, service, socket_type);

        if (l.size() > 0) {
            return l.front();
        } else {
            throw std::invalid_argument("No suitable socket address was found.");
        }
    }

    socketaddr get_first_general_socketaddr(const std::string &interface_description, const uint16_t port, const socket_t socket_type) {
        return get_first_general_socketaddr(interface_description, std::to_string(port), socket_type);
    }

}
