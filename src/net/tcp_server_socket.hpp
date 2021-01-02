/*
 * tcp_server_socket.hpp
 *
 *  Created on: 02.01.2021
 *      Author: doralitze
 */

#pragma once

#include <cstdio>
#include <memory>

#include "async_server.hpp"
#include "netio_exception.hpp"

namespace rmrf::net {


class tcp_server_socket {
private:
	std::shared_ptr<async_server_socket> ss;
public:
	tcp_server_socket(uint16_t port);
};

}
