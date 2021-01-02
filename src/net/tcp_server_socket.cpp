/*
 * tcp_server_socket.cpp
 *
 *  Created on: 02.01.2021
 *      Author: doralitze
 */
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>

#include "tcp_server_socket.hpp"

namespace rmrf::net {

tcp_server_socket::tcp_server_socket(uint16_t port) : ss{nullptr} {
	auto raw_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(raw_socket_fd < 0) {
		// TODO implement propper error handling
		throw netio_exception("Failed to create socket fd.");
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY; // TODO FIXME
	if (bind(raw_socket_fd, (struct sockaddr *) &addr, sizeof(addr)) != 0) {
		throw netio_exception("Failed to bind to all addresses (FIXME)");
	}

	fcntl(raw_socket_fd, F_SETFL, fcntl(raw_socket_fd, F_GETFL, 0) | O_NONBLOCK);
	if (listen(raw_socket_fd, 5) == -1) {
		throw netio_exception("Failed to enable listenting mode for raw socket");
	}

	this->ss = std::make_shared<async_server_socket>(auto_fd(raw_socket_fd));
}

}
