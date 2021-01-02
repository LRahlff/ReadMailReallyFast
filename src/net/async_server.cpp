/*
 * async_server.cpp
 *
 *  Created on: 02.01.2021
 *      Author: doralitze
 */
#include "async_server.hpp"

#include <ev++.h>

#include <utility>


namespace rmrf::net {

async_server_socket::async_server_socket(auto_fd&& socket_fd) :
		socket(std::forward(socket_fd)), on_accept{}, on_error{}, io{} {
    // This constructor got a constructed socket as an argument
    // and forwards it to libev
    io.set<async_server_socket, &async_server_socket::cb_ev>(this);
    io.start(this->socket, ::ev::READ);
}

async_server_socket::~async_server_socket() {
    // Remove this socket from libev ...
	io.stop();
}

void async_server_socket::cb_ev(::ev::io &w, int events) {
	(void) w;

	if (events & ::ev::ERROR) {
		// Handle errors
		// Rebind socket if missed iov res else
		// Log and throw?
		return;
	}

	if (events & ::ev::READ) {
		// Handle incoming clients
		auto ah = this->get_accept_handler();
	}

	if (events & ::ev::WRITE) {
		// Handle sending data which should be none here
	}
}

inline void async_server_socket::set_accept_handler(
		const accept_handler_type &value) {
	on_accept = value;
}

inline async_server_socket::accept_handler_type async_server_socket::get_accept_handler() const {
	return on_accept;
}

}
