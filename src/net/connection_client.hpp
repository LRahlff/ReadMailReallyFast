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

#if EAGAIN != EWOULDBLOCK
    #define EAGAIN_WRAPPER ((errno != EAGAIN) && (errno != EWOULDBLOCK))
#else
    #define EAGAIN_WRAPPER (errno != EAGAIN)
#endif

namespace rmrf::net {

template<class client>
class connection_client : public std::enable_shared_from_this<client> {
public:
    /**
      * This function type accepts a reference to the incomming data string which it may not alter
      * and may not return any data.
      */
    typedef std::function<void(const std::string&)> incomming_data_cb;

protected:
    incomming_data_cb in_data_cb;

public:
    connection_client() : in_data_cb{} {};

    /**
     * Use this method to send data to the other endpoint.
     * @param data The data to send
     */
    virtual void write_data(const std::string& data) = 0;

    /**
     * Use this method in order to register your callback function that should be
     * called when the client got data to process.
     * @param cb The callback function to register [void(std::string data)]
     */
    inline void set_incomming_data_callback(const incomming_data_cb& cb) {
        this->in_data_cb = cb;
    };
};

}
