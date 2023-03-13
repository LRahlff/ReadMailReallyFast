/*
 * connection_line_buffer.hpp
 *
 *  Created on: 05.01.2021
 *      Author: doralitze
 */

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <sstream>

#include "net/connection_client.hpp"

namespace rmrf::net {

/**
 * This type defines the signature of the search function required to look up the next end
 * of a data segment (for example a CR LF). It needs to return the first position where it
 * found the desired char sequence. Its arguments are a reference to the string that should be
 * searched and an index where to start the search.
 *
 * If the requested sequence wasn't found such a function must return std::string::npos.
 */
typedef std::function<std::string::size_type(const std::string&, std::string::size_type)> eol_search_t;

/**
 * This is the default line ending search method. It looks for CR, LF and CR LF style line endings.
 * @brief A default line ending search callback looking for default line endings.
 * @param data The string data to perform the search in.
 * @param start_position The start position of the search.
 * @return The first occurence index of a new line sequence of std::string::npos if none where found.
 */
std::string::size_type default_eol_search(const std::string& data, std::string::size_type start_position);

/**
 * This class is a middleware between a (TCP) Client and a specific protocol handling client that takes
 * care of the fact that incomming data might not be splitted at the new line positions. In order to do so
 * this class accepts a search method to look for the end of a message (rmrf::net::default_eol_search
 * might be a good candidate here) and calls the specified found_next_line_cb_ callback if a complete
 * message was reconstructed.
 *
 * This class contains an internal buffer that might get filled by a single, yet very large, line. As a consequence
 * it is wise to check the line_complete argument of the provided callback.
 *
 * Known limitation: If the last received data ends with '\r' and the next incoming data would start with '\n' there is no way
 * to detect this as the received message might indeed be a complete one ending with '\r' and one cannot wait for a potential
 * continuation of said message to arrive as they might never arrive. Thus it is recommended to either only transmit line endings
 * that solely rely on LF or, if the data source is known to send out CR LF style endings (and only those) to implement a custom
 * line end search algorithm. A third possibility to handle this issue is to ignore empty incomming lines if the last message ended
 * with CR.
 * @class connection_line_buffer
 * @author doralitze
 * @date 05/01/21
 * @file connection_line_buffer.hpp
 * @brief Filter a client to only serve messages that reassemble a complete line
 * @see rmrf::net::default_eol_search
 * @see rmrf::net::eol_search_t
 */
class connection_line_buffer {
public:
    /**
      * This type represents the callback layout that an accepting client is supposed to implement.
      * The first argument represents the message data. The second argument indicates whether
      * or not the message was completed prior to calling this callback. If the message size exceeds
      * the buffer size of this instance it will call the specified callback with the data it got and will
      * indicate the incompleteness by passing false to the second argument. If the message was
      * passed regularly it will pass true.
      */
    typedef std::function<void(const std::string&, bool)> found_next_line_cb_t;

private:
    const eol_search_t search;
    std::shared_ptr<connection_client> client;
    found_next_line_cb_t found_next_line_cb;
    std::string::size_type max;
    std::ostringstream data_buffer;
    size_t buffer_length;

public:
    /**
     * This constructor creates a new buffer based on a given client, an input callback and a search algorithm callback.
     * @brief Create a new line buffer with a custom search behaviour
     * @param c The client to perform the input buffering for
     * @param found_next_line_cb_ The callback to be called when a complete new line arrived
     * @param max_line_size The maximum line length to aquire prior to collection abort and transmission of the incomplete line
     * @param search_lb The callback to use for line break detection
     */
    connection_line_buffer(std::shared_ptr<connection_client> c, found_next_line_cb_t found_next_line_cb_, std::string::size_type max_line_size, eol_search_t search_lb);

    /**
     * This constructor creates a new buffer based on a given client, an input callback and the maximum line size.
     * This constructor uses the default line break search algorithm (rmrf::net::default_eol_search)
     * @brief Create a new line buffer with a custom search behaviour
     * @param c The client to perform the input buffering for
     * @param found_next_line_cb_ The callback to be called when a complete new line arrived
     * @param max_line_size The maximum line length to aquire prior to collection abort and transmission of the incomplete line
     */
    connection_line_buffer(std::shared_ptr<connection_client> c, found_next_line_cb_t found_next_line_cb_, std::string::size_type max_line_size);

private:
    void conn_data_in_cb(const iorecord& data_in);
    void clear();
};

}
