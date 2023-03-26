/*
 * connection_client.cpp
 *
 *  Created on: 03.01.2021
 *      Author: doralitze
 */
#include <utility>

#include "net/connection_client.hpp"

namespace rmrf::net {
    
    connection_client::~connection_client() {
        async.stop();

        if (this->server_active) {
            io.stop();
        }

        if (destructor_cb) {
            destructor_cb(exit_status_t::NO_ERROR);
        }
    }

    void connection_client::cb_ev(::ev::io& w, int events) {
        if (events & ::ev::ERROR) {
            // Handle errors
            // Log and throw?
            this->stop_server();
            throw netio_exception("Libev client error. libev: state=" + std::to_string(events));
        }

        if (events & ::ev::READ) {
            if(this->in_data_cb != nullptr)
                this->read_from_socket(w);
        }

        if ((events & ::ev::WRITE) && (this->current_burst_count < this->rate_limit || this->rate_limit == this->no_rate_limit)) {
            // Handle sending data
            // TODO register a timer to decrease rate limit if enabled.
            if (!this->write_queue.empty()) {
                this->data_write_active = true;
                iorecord buffer = this->write_queue.pop_front();

                const auto written = push_write_queue(w, buffer);
                if (written >= 0) {
                    buffer.advance((size_t)written);
                    this->current_burst_count++;
                } else if (EAGAIN_WRAPPER) {
                    throw netio_exception("Failed to write latest buffer content.");
                }

                if (partial_write_allowed)
                    this->write_queue.push_front(buffer);
                this->data_write_active = false;
            }
        }

        set_new_flags();
    }
    
    void connection_client::stop_server() {
        this->server_active = false;
        io.stop();
    }

    void connection_client::set_incomming_data_callback(const incomming_data_cb &cb) {
        this->in_data_cb = cb;
        this->async.send();
    }

    void connection_client::set_rate_limit(unsigned int new_limit) {
        this->rate_limit = new_limit;
        // TODO start timer if new limit is not no_rate_limit
        // TODO stop timer if timer exists and new limit is no_rate_limit
    }

    void connection_client::cb_async(::ev::async &w, int events) {
        MARK_UNUSED(w);
        MARK_UNUSED(events);
        set_new_flags();
    }

}
