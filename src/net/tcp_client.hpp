/*
 * tcp_client.hpp
 *
 *  Created on: 03.01.2021
 *      Author: doralitze
 */

#pragma once

#include <ev++.h>

#include <functional>
#include <list>
#include <memory>
#include <string>
#include <unistd.h>

#include "net/async_fd.hpp"
#include "net/connection_client.hpp"
#include "net/ioqueue.hpp"

namespace rmrf::net {

enum class exit_status_t : uint16_t {
	NO_ERROR = 0,
	TIMEOUT = 1
};

class tcp_client : public connection_client, std::enable_shared_from_this<tcp_client> {
public:
	typedef std::function<void(exit_status_t)> destructor_cb_type;
private:
	const destructor_cb_type destructor_cb;
	const std::string peer_address;

	uint16_t port;
	auto_fd net_socket;
	::ev::io io;
	ioqueue write_queue;
public:
	tcp_client(const destructor_cb_type destructor_cb_, auto_fd&& socket_fd, std::string peer_address_, uint16_t port_);
	tcp_client(const std::string& peer_address_, const uint16_t port_);
	tcp_client(const std::string& peer_address_, const std::string& service_or_port);
	tcp_client(const std::string& peer_address_, const std::string& service_or_port, int ip_addr_family);
	virtual ~tcp_client();
	virtual void write_data(const std::string& data);
	std::string get_peer_address();
	uint16_t get_port();
private:
	void cb_ev(::ev::io &w, int events);
	void push_write_queue(::ev::io &w);
};

}
