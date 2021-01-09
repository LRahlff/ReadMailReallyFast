/*
 * connection_line_buffer.cpp
 *
 *  Created on: 05.01.2021
 *      Author: doralitze
 */

#include "net/connection_line_buffer.hpp"

namespace rmrf::net {

std::string::size_type default_eol_search(
    const std::string &data,
    std::string::size_type start_position
) {
    const std::string::size_type s = data.size();

    for (std::string::size_type i = start_position; i < s; i++) {
        switch (data[i]) {
        case '\r':
            if (i < s - 1) {
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

connection_line_buffer::connection_line_buffer(
    std::shared_ptr<connection_client> c,
    found_next_line_cb_t found_next_line_cb_,
    std::string::size_type max_line_size,
    eol_search_t search_lb
) :
    search(search_lb),
    client(c),
    found_next_line_cb(found_next_line_cb_),
    max(max_line_size),
    data("")
{
    this->client->set_incomming_data_callback(
        [this](const std::string& data_in) {
            conn_data_in_cb(data_in);
        });
}

connection_line_buffer::connection_line_buffer(
    std::shared_ptr<connection_client> c,
    found_next_line_cb_t found_next_line_cb_,
    std::string::size_type max_line_size
) :
    connection_line_buffer{c, found_next_line_cb_, max_line_size, &default_eol_search}
{}

void connection_line_buffer::conn_data_in_cb(const std::string &data_in) {

    // Iterate throug the incomming data as long as we find line breaks
    for (std::string::size_type strpos = 0, nextpos = -1; strpos != std::string::npos; strpos = nextpos++) {
        // Search for the next line break
        nextpos = this->search(data_in, strpos);

        // Advance, if the line would be empty
        if (nextpos == strpos) {
            nextpos = this->search(data_in, ++strpos);
        }

        if (nextpos == std::string::npos) {
            // If we didn't find one we store the remaining data in the buffer
            this->data += data_in.substr(strpos, data_in.length() - strpos);
            break;
        } else {
            // If we find one we send the buffer plus the incomming data up to the line break to the callback
            const std::string data_to_send = this->data + data_in.substr(strpos, nextpos - strpos);
            this->found_next_line_cb(data_to_send, true);
            // and clear the buffer
            this->data = std::string("");
        }
    }

    if (this->data.length() > this->max) {
        this->found_next_line_cb(this->data, false);
        this->data = std::string("");
    }
}

}
