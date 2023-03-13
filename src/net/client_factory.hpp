#pragma once

#include <memory>

#include "net/connection_client.hpp"
#include "net/tcp_client.hpp"
#include "net/sock_address_factory.hpp"
#include "net/udp_client.hpp"

namespace rmrf::net {
    
    [[nodiscard]] std::unique_ptr<udp_client> client_factory_construct_udp_client(const socketaddr& socket_identifier, connection_client::incomming_data_cb cb = nullptr);
    [[nodiscard]] std::unique_ptr<tcp_client> client_factory_construct_stream_client(const socketaddr& socket_identifier, connection_client::incomming_data_cb cb = nullptr);
    
    /**
     * This method directly connects to the given address and returns an invalid pointer if it fails.
     * @brief Connect to a remote destination
     * @param address The specific address to use
     * @return A unique pointer to a connection client or nullptr if the operation failed.
     */
    [[nodiscard]] std::unique_ptr<connection_client> connect(const socketaddr& address);

    /**
     * Use this method to get a new client. Pass a default constructed socketaddr to address if the parameter is not applicable.
     * @brief Construct a client
     * @param address The address to use
     * @param type The type of client to use
     * @return A unique pointer to a connection client or nullptr if the operation failed.
     */
    [[nodiscard]] std::unique_ptr<connection_client> connect(const socketaddr& address, const socket_t& type);
    
    /**
     * This method looks up the requested service and returns a client using the most suitable method.
     * @brief Connect to a remote destination
     * @param peer_address The address to connect to
     * @param service The service hint to use
     * @param ip_addr_family Optional. Force the ussage of a specific address family
     * @throws netio exception in case of an impossible request
     */
    [[nodiscard]] std::unique_ptr<connection_client> connect(const std::string& peer_address, const std::string& service, int ip_addr_family = AF_UNSPEC);
}
