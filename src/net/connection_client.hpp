/*
 * connection_client.hpp
 *
 *  Created on: 03.01.2021
 *      Author: doralitze
 */

#pragma once

#include <functional>
#include <memory>
#include <string>

namespace rmrf::net {

class connection_client : public std::enable_shared_from_this<connection_client> {
public:
	typedef std::function<void(std::shared_ptr<std::string>)> incomming_data_cb;
protected:
	incomming_data_cb in_data_cb;
public:
	connection_client();

	/**
	 * Use this method to send data to the other endpoint.
	 */
	virtual void write_data(std::string data) = 0;

	/**
	 * Use this method in order to register your callback function that should be
	 * called when the client got data to process.
	 * @param cb The callback function to register [void(std::string data)]
	 */
	void set_incomming_data_callback(const incomming_data_cb &cb);
};

}
