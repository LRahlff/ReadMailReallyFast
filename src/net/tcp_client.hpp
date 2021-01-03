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

#include "net/connection_client.hpp"
#include "net/async_fd.hpp"

#define EXIT_STATUS_NO_ERROR (exit_status) 0
#define EXIT_STATUS_TIMEOUT (exit_status) 1

namespace rmrf::net {

namespace impl {
struct NICBuffer {
	char* data;
	ssize_t len;
	ssize_t pos;

	NICBuffer(const char* bytes, ssize_t nbytes);
	virtual ~NICBuffer();
	char *dpos();
	ssize_t nbytes();

	NICBuffer operator=(const rmrf::net::impl::NICBuffer&) = delete;
	NICBuffer(const NICBuffer &) = delete;
};
}

class tcp_client : public connection_client, std::enable_shared_from_this<tcp_client> {
public:
	typedef unsigned short exit_status;
	typedef std::function<void(exit_status)> destructor_cb_type;
private:
	const destructor_cb_type destructor_cb;
	const auto_fd socket;
	const std::string peer_address;
	const uint16_t port;

	::ev::io io;
	std::list<std::shared_ptr<impl::NICBuffer>> write_queue;
public:
	tcp_client(const destructor_cb_type destructor_cb_, auto_fd&& socket_fd, std::string peer_address_, uint16_t port_);
	virtual ~tcp_client();
	virtual void write_data(std::string data);
	std::string get_peer_address();
	uint16_t get_port();
	bool is_write_queue_empty();
private:
	void cb_ev(::ev::io &w, int events);
	void push_write_queue(::ev::io &w);
};

}
