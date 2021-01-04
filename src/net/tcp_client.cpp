/*
 * tcp_client.cpp
 *
 *  Created on: 03.01.2021
 *      Author: doralitze
 */

#include "net/tcp_client.hpp"

#include <ev++.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <utility>

#include "net/netio_exception.hpp"

namespace rmrf::net {

tcp_client::tcp_client(const destructor_cb_type destructor_cb_, auto_fd&& socket_fd, std::string peer_address_, uint16_t port_) :
		connection_client{},
		destructor_cb(destructor_cb_),
		socket(std::forward<auto_fd>(socket_fd)),
		peer_address(peer_address_), port(port_),
		io{}, write_queue{} {
	io.set<tcp_client, &tcp_client::cb_ev>(this);
	io.start(this->socket.get(), ::ev::READ);
	// TODO log created client
}

tcp_client::~tcp_client() {
	destructor_cb(EXIT_STATUS_NO_ERROR);
	io.stop();
}

namespace impl {
NICBuffer::NICBuffer(const char* bytes, ssize_t nbytes) : data(nullptr), len(nbytes), pos(0) {
	data = new char[nbytes];
	memcpy(data, bytes, nbytes);
}

NICBuffer::~NICBuffer() {
	delete [] data;
}

char* NICBuffer::dpos() {
	return data + pos;
}

ssize_t NICBuffer::nbytes() {
	return len - pos;
}
}

void tcp_client::write_data(std::string data) {
	// Create NICBuffer from data
	this->write_queue.push_back(std::make_shared<impl::NICBuffer>(data.c_str(), data.size()));
}

inline std::shared_ptr<std::string> buffer_to_string(char* buffer, ssize_t bufflen)
{
	// For some wired reaseon the compiler refuses to find the correct constructor of string
	// without this extra method.
    std::shared_ptr<std::string> ret = std::make_shared<std::string>(buffer, (int) bufflen);
    return ret;
}

void tcp_client::cb_ev(::ev::io &w, int events) {
	if (events & ::ev::ERROR) {
		// Handle errors
		// Log and throw?
		return;
	}

	if (events & ::ev::READ) {
		// notify incomming_data_cb
		char buffer[1024];

		ssize_t n_read_bytes = recv(w.fd, buffer, sizeof(buffer), 0);
		if(n_read_bytes < 0) {
			throw netio_exception("Failed to read from network socket.");
		}

		if(n_read_bytes == 0) {
			// TODO find a way to properly announce the closed connection
			delete this;
		} else {
			this->in_data_cb(buffer_to_string(buffer, n_read_bytes));
		}
	}

	if (events & ::ev::WRITE) {
		// Handle sending data
		push_write_queue(w);
	}

	if (write_queue.empty()) {
		io.set(::ev::READ);
	} else {
		io.set(::ev::READ | ::ev::WRITE);
	}
}

void tcp_client::push_write_queue(::ev::io &w) {
	if (this->is_write_queue_empty()) {
		io.set(::ev::READ);
	    return;
	}

	std::shared_ptr<impl::NICBuffer> buffer = this->write_queue.front();
	ssize_t written = write(w.fd, buffer->dpos(), buffer->nbytes());

	if (written < 0) {
		throw netio_exception("Failed to write latest buffer content.");
	}

	buffer->pos += written;
	if (buffer->nbytes() == 0) {
		this->write_queue.pop_front();
	}
}

inline std::string tcp_client::get_peer_address() {
	return this->peer_address;
}

inline uint16_t tcp_client::get_port() {
	return this->port;
}

inline bool tcp_client::is_write_queue_empty() {
	return this->write_queue.empty();
}

}


