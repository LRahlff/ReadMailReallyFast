/*
 * connection_client.cpp
 *
 *  Created on: 03.01.2021
 *      Author: doralitze
 */

#include "net/connection_client.hpp"

namespace rmrf::net {

connection_client::connection_client() : in_data_cb{} {

}

inline void connection_client::set_incomming_data_callback(const incomming_data_cb &cb) {
	this->in_data_cb = cb;
}

}
