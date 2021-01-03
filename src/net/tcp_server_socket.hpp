/*
 * tcp_server_socket.hpp
 *
 *  Created on: 02.01.2021
 *      Author: doralitze
 */

#pragma once

#include <memory>

#include "net/async_server.hpp"
#include "net/netio_exception.hpp"
#include "net/tcp_client.hpp"

namespace rmrf::net {


class tcp_server_socket : public std::enable_shared_from_this<tcp_server_socket>{
public:
	typedef std::function<void(tcp_client)> incoming_client_listener_type;
private:
	async_server_socket::self_ptr_type ss;
	incoming_client_listener_type client_listener;
	int number_of_connected_clients;
public:
	tcp_server_socket(uint16_t port, incoming_client_listener_type client_listener_);
	int get_number_of_connected_clients() const;
private:
	void await_raw_socket_incomming(async_server_socket::self_ptr_type ass, const auto_fd& socket);
	void client_destructed_cb(tcp_client::exit_status exit_status);
};

}
