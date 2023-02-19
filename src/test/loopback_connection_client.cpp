/*
 * loopback_connection_client.cpp
 *
 *  Created on: 05.01.2021
 *      Author: doralitze
 */

#include "test/loopback_connection_client.hpp"

#include <string.h>

namespace rmrf::test {

loopback_connection_client::loopback_connection_client(
    rmrf::net::connection_client::incomming_data_cb mut_send_data_cb_
) :
    rmrf::net::connection_client{},
    mut_send_data_cb(mut_send_data_cb_),
    send_data_archive{}
{
    // Does nothing special
}

loopback_connection_client::~loopback_connection_client() {
    // Also doesn't do anything fancy.
}

void loopback_connection_client::write_data(const std::string &data) {
    // TODO fixme
    this->send_data_archive.push_back(data);

    if (this->mut_send_data_cb != nullptr) {
        this->mut_send_data_cb(data);
    }
}

void loopback_connection_client::send_data_to_incomming_data_cb(const std::string &data) {
    if (this->in_data_cb != nullptr) {
        this->in_data_cb(data);
    }
}

std::vector<std::string> loopback_connection_client::get_send_data() {
    return this->send_data_archive;
}

}
