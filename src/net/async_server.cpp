/*
 * async_server.cpp
 *
 *  Created on: 02.01.2021
 *      Author: doralitze
 */
#include "net/async_server.hpp"

#include <ev++.h>

#include <utility>


namespace rmrf::net {

async_server_socket::async_server_socket(
    auto_fd &&socket_fd
) :
    socket(std::forward<auto_fd>(socket_fd)),
    number_of_connected_clients{0},
    max_number_of_simulataneusly_allowed_clients{0},
    on_accept{},
    on_overflow{},
    on_error{},
    io{}
{
    // This constructor got a constructed socket as an argument
    // and forwards it to libev
    io.set<async_server_socket, &async_server_socket::cb_ev>(this);
    io.start(this->socket.get(), ::ev::READ);
}

async_server_socket::~async_server_socket() {
    // Remove this socket from libev ...
    io.stop();
}

void async_server_socket::cb_ev(::ev::io &w, int events) {
    (void) w;

    if (events & ::ev::ERROR) {
        this->on_error(this->shared_from_this());
        return;
    }

    if (events & ::ev::READ) {
        // Handle incoming clients
        if(auto client = this->await_raw_socket_incomming(this->socket); client) {
            if (this->max_number_of_simulataneusly_allowed_clients == 0 || this->get_number_of_connected_clients() <= this->max_number_of_simulataneusly_allowed_clients) {
                this->number_of_connected_clients++;
                this->get_accept_handler()(this->shared_from_this(), client);
            } else if (this->on_overflow != nullptr) {
                this->on_overflow(this->shared_from_this(), client);
            }
        }
    }

    if (events & ::ev::WRITE) {
        // Handle sending data which should be none here
    }
}

void async_server_socket::client_destructed_cb(exit_status_t exit_status) {
    MARK_UNUSED(exit_status);

    this->number_of_connected_clients--;
}

connection_client::destructor_cb_type async_server_socket::get_locked_destructor_callback() {
    const auto weak_this = this->weak_from_this();
    const connection_client::destructor_cb_type cb = [weak_this](exit_status_t status) {
        auto ref_this = weak_this.lock();

        if (!ref_this) {
            return;
        }

        ref_this->client_destructed_cb(status);
    };
    return cb;
}
}
