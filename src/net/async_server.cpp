/*
 * async_server.cpp
 *
 *  Created on: 02.01.2021
 *      Author: doralitze
 */
#include <ev++.h>
#include <utility>

#include "async_server.hpp"


namespace rmrf::net::asio {

async_server_socket::async_server_socket(auto_fd&& socket_fd) : socket(std::forward(socket_fd)) {
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

	if (events & ::ev::READ) {
		// Handle incoming clients
		auto ah = this->get_accept_handler();
	}

	if (events & ::ev::WRITE) {
		// Handle sending data which should be none here
	}

	if (events & ::ev::ERROR) {
		// Handle errors
		// Rebind socket if missed iov res else
		// Log and throw?
	}
}

}

