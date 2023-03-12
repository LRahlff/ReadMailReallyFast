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
    // TODO byte stuffing support implementieren bzw. Escaping von newline char; gerne auch als separate Suchfunktion
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
    data_buffer(std::ostringstream::ate),
    buffer_length{0}
{
    this->client->set_incomming_data_callback(
        [this](const iorecord& data_in) {
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

void connection_line_buffer::clear() {
    this->data_buffer.str(std::string());
    this->data_buffer.clear();
    this->buffer_length = 0;
}

void connection_line_buffer::conn_data_in_cb(const iorecord& record_in) {
    // Compute all string occurrences within the record
    const auto ds = record_in.get_strings_in_record();

    for(auto& data_in : ds) {
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
                const auto length = data_in.length() - strpos;
                this->data_buffer << data_in.substr(strpos, length);
                this->buffer_length += length;
                break;
            } else {
                // If we find one we send the buffer plus the incomming data up to the line break to the callback
                const std::string data_to_send = this->data_buffer.str() + data_in.substr(strpos, nextpos - strpos);
                this->found_next_line_cb(data_to_send, true);
                // and clear the buffer
                this->clear();
            }
        }
    }

    if (this->buffer_length > this->max) {
        this->found_next_line_cb(this->data_buffer.str(), false);
        this->clear();
    }
}

}
