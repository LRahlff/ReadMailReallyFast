#pragma once

#include <ev++.h>

#include <functional>
#include <memory>

#include <net/async_fd.hpp>

namespace rmrf::net {

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
    async_server_socket(auto_fd &&fd);
    ~async_server_socket();

	accept_handler_type get_accept_handler() const;
	void set_accept_handler(const accept_handler_type &value);

private:
    void cb_ev(::ev::io &w, int events);
};

}
