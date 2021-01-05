/*
 * loopback_connection_client.hpp
 *
 *  Created on: 05.01.2021
 *      Author: doralitze
 */

#pragma once

#include <vector>

#include "net/connection_client.hpp"

namespace rmrf::test {

/**
 * Use this class to mock a connection client.
 */
class loopback_connection_client : public rmrf::net::connection_client {
private:
	const rmrf::net::connection_client::incomming_data_cb mut_send_data_cb;
	std::vector<std::string> send_data_archive;
public:
	/**
	 * This constructor uses the given callback to notify the test suite that the module under test
	 * send data.
	 */
	loopback_connection_client(rmrf::net::connection_client::incomming_data_cb mut_send_data_cb_);

	/**
	 * Just the mandatory virtual destructor.
	 */
	virtual ~loopback_connection_client();

	/**
	 * This method gets called by the module under test as it simulates the behavior of a normal connection client.
	 * @param data The data the module wants to send.
	 */
	virtual void write_data(const std::string& data);

	/**
	 * This method sends data to the connections incoming data callback.
	 * Use it to mimic a remote client sending data.
	 *
	 * @param data The data to send.
	 */
	void send_data_to_incomming_data_cb(const std::string& data);

	/**
	 * Use this method in order to get all data the module under test has send.
	 */
	std::vector<std::string> get_send_data();
};

}
