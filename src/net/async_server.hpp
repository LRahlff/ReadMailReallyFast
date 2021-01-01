#pragma once

#include <functional>
#include <memory>

#include <net/async_fd.hpp>

namespace rmrf::net::asio {

class async_server_socket : public std::enable_shared_from_this<async_server_socket> {
public:
    typedef std::shared_ptr<async_server_socket> self_ptr_type;

    typedef std::function<void(self_ptr_type&, const auto_fd &)> accept_handler_type;
    typedef std::function<void(self_ptr_type&)> error_handler_type;

private:
    auto_fd socket;

    accept_handler_type on_accept;
    error_handler_type on_error;

public:
    async_server_socket(auto_fd&& fd) : socket(std::forward(fd)) {
        // Add this socket to libev ...
    }
    ~async_server_socket() {
        // Remove this socket from libev ...
    }

public:
    accept_handler_type get_accept_handler() const {
        return on_accept;
    }
    void set_accept_handler(const accept_handler_type& value) {
        on_accept = value;
    }
};

}
