/*
 * tcp_server_socket.cpp
 *
 *  Created on: 02.01.2021
 *      Author: doralitze
 */
#include "net/tcp_server_socket.hpp"

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>

#include <functional>

#include "macros.hpp"
#include "net/socketaddress.hpp"


namespace rmrf::net {

tcp_server_socket::tcp_server_socket(uint16_t port, incoming_client_listener_type client_listener_) :
		ss{nullptr}, client_listener(client_listener_), number_of_connected_clients(0) {
	auto_fd raw_socket_fd{socket(AF_INET6, SOCK_STREAM, 0)};
	if(!raw_socket_fd.valid()) {
		// TODO implement propper error handling
		throw netio_exception("Failed to create socket fd.");
	}

	sockaddr_in6 addr;
	addr.sin6_family = AF_INET6;
	addr.sin6_port = htons(port);
	addr.sin6_addr = IN6ADDR_ANY_INIT;
	socketaddr sa{addr};

	if (bind(raw_socket_fd.get(), sa.ptr(), sa.size()) != 0) {
		std::string msg = "Failed to bind to all addresses (FIXME)";
		if (port < 1024) {
			msg += "\nYou tried to bind to a port smaller than 1024. Are you root?";
		}

		throw netio_exception(msg);
	}

	// Append the non blocking flag to the file state of the socket fd.
	// This might be linux only. We should check that
	fcntl(raw_socket_fd.get(), F_SETFL, fcntl(raw_socket_fd.get(), F_GETFL, 0) | O_NONBLOCK);
	if (listen(raw_socket_fd.get(), 5) == -1) {
		throw netio_exception("Failed to enable listening mode for raw socket");
	}

	this->ss = std::make_shared<async_server_socket>(std::forward<auto_fd>(raw_socket_fd));

	using namespace std::placeholders;
	this->ss->set_accept_handler(std::bind(&tcp_server_socket::await_raw_socket_incomming, this, _1, _2));
}


// As we're not depending on the actual async server object we need to suppress the warning that we're not using it.

void tcp_server_socket::await_raw_socket_incomming(async_server_socket::self_ptr_type ass, const auto_fd& socket) {
	MARK_UNUSED(ass);

	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int client_fd_raw = accept(socket.get(), (struct sockaddr *)&client_addr, &client_len);

	if(client_fd_raw < 0) {
		throw netio_exception("Unable to bind incoming client");
	}

	fcntl(client_fd_raw, F_SETFL, fcntl(client_fd_raw, F_GETFL, 0) | O_NONBLOCK);

	const std::string address = inet_ntoa(client_addr.sin_addr);
	const uint16_t port = ntohs(client_addr.sin_port);

	// Generate client object from fd and announce it
	this->number_of_connected_clients++;
	using namespace std::placeholders;
	this->client_listener(tcp_client(std::bind(&tcp_server_socket::client_destructed_cb, this, _1), auto_fd(client_fd_raw), address, port));
}

int tcp_server_socket::get_number_of_connected_clients() {
	return this->number_of_connected_clients;
}

void tcp_server_socket::client_destructed_cb(tcp_client::exit_status exit_status) {
	MARK_UNUSED(exit_status);

	this->number_of_connected_clients--;
}

}
