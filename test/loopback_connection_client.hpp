#pragma once

#include <vector>

#include "net/connection_client.hpp"
#include "net/ioqueue.hpp"


namespace rmrf::test {

/**
 * Use this class to mock a connection client.
 */
class loopback_connection_client : public rmrf::net::connection_client {
private:
    const incomming_data_cb send_data_cb;
    std::vector<std::string> data_archive;
    const bool echo_data_transfer;

public:
    /**
     * This constructor uses the given callback to notify the test suite that the module under test
     * send data.
     */
    loopback_connection_client(
        const incomming_data_cb& send_data_cb_,
        bool echo_data
    ) :
        rmrf::net::connection_client(rmrf::net::null_fd{}, rmrf::net::socketaddr{}, nullptr),
        send_data_cb(send_data_cb_),
        data_archive{},
        echo_data_transfer(echo_data)
    {
        // Does nothing special
    }

    /**
     * Just the mandatory virtual destructor.
     */
    virtual ~loopback_connection_client() {
        // Also doesn't do anything fancy.
    }

    /**
     * This method gets called by the module under test as it simulates the behavior of a normal connection client.
     * @param data The data the module wants to send.
     */
    virtual void write_data(const rmrf::net::iorecord& data) {
        // TODO fixme
        if (this->echo_data_transfer) {
            std::cout << "<-- " << data << std::endl;
        }

        this->data_archive.push_back(data.str());

        if (this->send_data_cb) {
            this->send_data_cb(data);
        }
    }

    /**
     * This method sends data to the connections incoming data callback.
     * Use it to mimic a remote client sending data.
     *
     * @param data The data to send.
     */
    void send_data_to_incomming_data_cb(const rmrf::net::iorecord& data) {
        if (this->echo_data_transfer) {
            std::cout << "--> " << data << std::endl;
        }

        if (this->in_data_cb) {
            this->in_data_cb(data);
        }
    }

    /**
     * Use this method in order to get all data the module under test has send.
     */
    std::vector<std::string> get_send_data() {
        return this->data_archive;
    }

    void clear_sent_data() {
        this->data_archive.clear();
    }

    virtual void read_from_socket(::ev::io& w) {
        // Does nothing as libev is disabled here
        MARK_UNUSED(w);
    }
    
    virtual ssize_t push_write_queue(::ev::io& w, rmrf::net::iorecord& buffer) {
        MARK_UNUSED(w);
        MARK_UNUSED(buffer);
        throw rmrf::net::netio_exception("Error: push_write_queue was called");
        return 0;
    }
};

}
