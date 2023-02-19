#pragma once

#include <list>
#include <stdexcept>
#include <string>

#include "net/socketaddress.hpp"

namespace rmrf::net {
    std::string format_network_error(int error);

    enum class socket_t {
        UDP,
        TCP
    };

    /**
     * This method constructs a new sockaddr object and prefers IPv6 while doing so. In case of multiple looked up
     * addresses for a host, it returns the first one.
     * @brief Construct a sockaddr object from string address
     * @param interface_description The human readable representation of an IP address or DNS name
     * @param port The port to connect to
     * @param socket_type Optional. The socket type to perform the lookup for. Defaults to TCP.
     * @return The constructed sockaddr object
     */
    socketaddr get_first_general_socketaddr(const std::string& interface_description, const uint16_t port, const socket_t socket_type = socket_t::TCP);

    /**
     * This method constructs a new sockaddr object and prefers IPv6 while doing so. In case of multiple looked up
     * addresses for a host, it returns the first one.
     * @brief Construct a sockaddr object from string address
     * @param interface_description The human readable representation of an IP address or DNS name
     * @param service_or_port The service to connect to
     * @param socket_type Optional. The socket type to perform the lookup for. Defaults to TCP.
     * @return The constructed sockaddr object
     */
    socketaddr get_first_general_socketaddr(const std::string& interface_description, const std::string& service_or_port, const socket_t socket_type = socket_t::TCP);

    /**
     * This method constructs a list of new sockaddr objects.
     *
     * @brief Construct a sockaddr object from string address
     * @param interface_description The human readable representation of an IP address or DNS name
     * @param port The port to connect to
     * @param socket_type Optional. The socket type to perform the lookup for. Defaults to TCP.
     * @return The constructed unordered list
     */
    std::list<socketaddr> get_socketaddr_list(const std::string& interface_description, const std::string& service_or_port, const socket_t socket_type);
}
