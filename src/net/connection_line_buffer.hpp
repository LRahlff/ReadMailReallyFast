/*
 * connection_line_buffer.hpp
 *
 *  Created on: 05.01.2021
 *      Author: doralitze
 */

#pragma once

#include <functional>
#include <memory>
#include <string>

#include "net/connection_client.hpp"

namespace rmrf::net {

typedef std::function<std::string::size_type(const std::string&, std::string::size_type)> eol_search_t;

static std::string::size_type default_eol_search(const std::string& data, std::string::size_type start_position);

class connection_line_buffer {
public:
	typedef std::function<void(const std::string&, bool)> found_next_line_cb_t;
private:
	const eol_search_t search;
	std::shared_ptr<connection_client> client;
	found_next_line_cb_t found_next_line_cb;
	std::string::size_type max;
	std::string data;
public:
	connection_line_buffer(std::shared_ptr<connection_client> c, found_next_line_cb_t found_next_line_cb_, std::string::size_type max_line_size, eol_search_t search_lb = default_eol_search);
	connection_line_buffer(std::shared_ptr<connection_client> c, found_next_line_cb_t found_next_line_cb_, std::string::size_type max_line_size);
private:
	void conn_data_in_cb(const std::string& data_in);
};

}
