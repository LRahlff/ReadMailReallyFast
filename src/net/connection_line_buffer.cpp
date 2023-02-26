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

}
