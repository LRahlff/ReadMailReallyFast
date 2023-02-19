#pragma once

#include <ev++.h>

#include <functional>
#include <memory>

#include <net/async_fd.hpp>

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

    typedef std::function<void(self_ptr_type, const auto_fd &)> accept_handler_type;
    typedef std::function<void(self_ptr_type)> error_handler_type;

private:
    auto_fd socket;

    accept_handler_type on_accept;
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
    ~async_server_socket();

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

private:
    void cb_ev(::ev::io &w, int events);
};

}
