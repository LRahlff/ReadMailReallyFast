/*
 * connection_line_buffer.cpp
 *
 *  Created on: 05.01.2021
 *      Author: doralitze
 */

#include "net/connection_line_buffer.hpp"

namespace rmrf::net {

static std::string::size_type default_eol_search(const std::string& data, std::string::size_type start_position) {
	const std::string::size_type s = data.size();
	for (std::string::size_type i = start_position; i < s; i++) {
		switch (data[i]) {
		case '\r':
			if(i < s - 1) {
				if (data[i + 1] == '\n') {
					i++;
				}
			}
			[[fallthrough]];
		case '\n':
			return i;
			break;
		default:
			break;
		}
	}
	return std::string::npos;
}

connection_line_buffer::connection_line_buffer(std::shared_ptr<connection_client> c, found_next_line_cb_t found_next_line_cb_,
		std::string::size_type max_line_size, eol_search_t search_lb) :
		search(search_lb),
		client(c),
		found_next_line_cb(found_next_line_cb_),
		max(max_line_size),
		data("") {
	this->client->set_incomming_data_callback(std::bind(&connection_line_buffer::conn_data_in_cb, this, std::placeholders::_1));
}

connection_line_buffer::connection_line_buffer(std::shared_ptr<connection_client> c, found_next_line_cb_t found_next_line_cb_,
		std::string::size_type max_line_size) : connection_line_buffer{c, found_next_line_cb_, max_line_size, &default_eol_search} { }

void connection_line_buffer::conn_data_in_cb(const std::string& data_in) {

	/**
	 * Known limitation: If the last received data ends with '\r' and the next incoming data would start with '\n' there is no way
	 * to detect this as the received message might indeed be a complete one ending with '\r' and one cannot wait for a potential
	 * continuation of said message to arrive.
	 */

	std::string::size_type strpos = 0;

	while(strpos != std::string::npos) {
		std::string::size_type nextpos = this->search(data_in, strpos);
		if (nextpos == std::string::npos) {
			this->data += data_in.substr(strpos, data_in.length() - strpos);
			break;
		} else {
			this->found_next_line_cb(this->data + data_in.substr(strpos, nextpos - strpos), true);
			this->data = std::string("");
		}
		strpos = nextpos;
	}

	if (this->data.length() > this->max) {
		this->found_next_line_cb(this->data, false);
		this->data = std::string("");
	}
}

}
