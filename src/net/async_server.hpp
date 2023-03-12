#pragma once

#include <ev++.h>

#include <atomic>
#include <functional>
#include <memory>

#include <net/async_fd.hpp>
#include "net/connection_client.hpp"

namespace rmrf::net {

/**
 * This class handles raw server sockets and wrapps them with automatic
 * resource management functions. This class is an internal helper class
 * utilized by classes like the TCP Server and thus shouldn't be used directly.
 * @class async_server_socket
 * @author leondietrich
 * @date 14/07/21
 * @file async_server.hpp
 * @brief An asynchronous server socket handler.
 * @see rmrf::net::tcp_server_socket
 */
class async_server_socket : public std::enable_shared_from_this<async_server_socket> {
public:
    typedef std::shared_ptr<async_server_socket> self_ptr_type;

    typedef std::function<void(self_ptr_type, std::shared_ptr<connection_client>)> accept_handler_type;
    typedef std::function<void(self_ptr_type)> error_handler_type;

private:
    auto_fd socket;
    std::atomic_uint32_t number_of_connected_clients;
    unsigned int max_number_of_simulataneusly_allowed_clients;

    accept_handler_type on_accept;
    accept_handler_type on_overflow;
    error_handler_type on_error;

    ::ev::io io;

public:

    /**
      * This constructor accepts your given file descriptor to the operating systems socket.
      * @param fd A handle to the socket in form of an auto_fd
      */
    async_server_socket(auto_fd &&fd);

    /**
     * This deconstructor automatically unregisteres the socket from libev which in turn automatically removes it from
     * the watch list of active sockets. However this does not close the socket in the kernels perspective so you must
     * use always auto_fd.
     * @brief Automatically unregister the socket from libev
     */
    virtual ~async_server_socket();

    /**
     * This method will return the currently used connection acceptance handler.
     * Be aware that it will return an invalid state unless you used set_accept_handler
     * prior to calling this method.
     * @brief get the current registered accept handler
     * @return The current accept handler
     */
    inline accept_handler_type get_accept_handler() const {
        return on_accept;
    }

    /**
     * Use this method in order to set a new incomming socket acceptance handler.
     * It's best to call this method right after the constructor call of this class.
     * @brief Set a new acceptance handler
     * @param value The new handler to set
     */
    inline void set_accept_handler(const accept_handler_type& value) {
        on_accept = value;
    }

    /**
     * Use this method in order to set a new error handler. It will be called in case of fd errors reported by libev.
     * @brief Set a new error handler
     * @param value The error handler to set
     */
    inline void set_error_handler(const error_handler_type& value) {
        this->on_error = value;
    }

    /**
     * This method sets the overflow handler to use if the maximum number of allowed clients was reached.
     * The purpose of said handler is to inform the connected client that the connection cant be established
     * due to isufficient resources. It should transmit the appropriate error message and drop the client.
     * @brief Set the client overflow handler
     * @param value The listener that handles clients that couldn't be accepted
     */
    void set_overflow_handler(const accept_handler_type& value) {
        on_overflow = value;
    }
    
    /**
     * This method provides you with the current number of connected clients. When a client
     * disconnects this number will be reduced. When a new client arrives this number will be incremented.
     * @brief Get the current number of connected clients.
     * @return  The number of connected clients
     */
    [[nodiscard]] inline unsigned int get_number_of_connected_clients() const {
        return this->number_of_connected_clients;
    }
    
    /**
     * Use this method in order to set the maximum number of allowed connections. Set it to
     * 0 in order to disable the limit. If anything other than 0 is set it is highly recommended
     * to also set an overflow handler.
     * @brief Set the maximum allowed simultaneus connections.
     * @param max_connections The maximum number of allowed connections.
     */
    inline void set_maximum_concurrent_connections(unsigned int max_connections) {
        this->max_number_of_simulataneusly_allowed_clients = max_connections;
    }

protected:
    virtual std::shared_ptr<connection_client> await_raw_socket_incomming(const auto_fd& socket) = 0;
    connection_client::destructor_cb_type get_locked_destructor_callback();

private:
    void cb_ev(::ev::io &w, int events);
    void client_destructed_cb(exit_status_t exit_status);
};

}
