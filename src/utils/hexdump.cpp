#include "utils/hexdump.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>


namespace rmrf::utils {

void hexdump_to_stream(
    std::ostream &os,
    size_t bytes_per_line,
    size_t bytes_per_group,
    size_t space_per_byte,
    size_t space_per_group,
    off_t offset_start,
    off_t offset_end,
    off_t voffset_at_zero,
    bool display_textual,
    bool display_uppercase,
    hexdump_getdata_t get_data
) {
    off_t offset_line = voffset_at_zero + offset_start;

    for (
        off_t offset = offset_start;
        offset <= offset_end;
        offset += (off_t)bytes_per_line, offset_line += (off_t)bytes_per_line
    ) {
        if (display_uppercase) {
            os << std::uppercase;
        }

        os << std::setw(8) << std::setfill('0') << std::hex << (size_t)offset_line << " : ";

        std::string display_textual_data(bytes_per_line, ' ');
        size_t display_textual_last = 0;

        std::stringstream display_hex_data;

        for (size_t idx = 0; idx < bytes_per_line; idx++) {
            off_t offset_data = offset + (off_t)idx;
            auto data = get_data(offset_data);

            if (space_per_group && idx && !(idx % bytes_per_group)) {
                display_hex_data << std::string(space_per_group, ' ');
            }

            if (space_per_byte && idx) {
                display_hex_data << std::string(space_per_byte, ' ');
            }

            if (std::holds_alternative<hexdump_data_t>(data)) {
                hexdump_data_t hd_data = std::get<hexdump_data_t>(data);

                if (hexdump_data_t::unknown == hd_data) {
                    display_hex_data << "??";
                    display_textual_last = idx + 1;
                    display_textual_data[idx] = '?';
                } else {
                    display_hex_data << "  ";
                    display_textual_data[idx] = ' ';
                }
            } else if (std::holds_alternative<uint8_t>(data)) {
                uint8_t hd_data = std::get<uint8_t>(data);

                if (display_uppercase) {
                    display_hex_data << std::uppercase;
                }

                display_hex_data << std::setw(2) << std::setfill('0') << std::hex << (uint32_t)hd_data;
                display_textual_last = idx + 1;
                display_textual_data[idx] = (hd_data >= 32 && hd_data < 127) ? (char)hd_data : '.';
            } else {
                display_hex_data << "!!";
                display_textual_last = idx + 1;
                display_textual_data[idx] = '!';
            }
        }

        if (display_textual) {
            os << display_hex_data.str();
            os << " : ";
            os << display_textual_data.substr(0, display_textual_last);
        } else {
            os << display_hex_data.str().substr(0, std::max(size_t{}, display_textual_last * 2 + ((off_t)display_textual_last - 1) * space_per_byte + (((off_t)display_textual_last - 1) / bytes_per_group) * space_per_group));
        }

        os << "\n";
    }
}

void hexdump_to_stream(
    std::ostream &os,
    size_t bytes_per_line,
    size_t bytes_per_group,
    size_t space_per_byte,
    size_t space_per_group,
    off_t offset_start,
    off_t offset_end,
    off_t voffset_at_zero,
    bool display_textual,
    bool display_uppercase,
    uint8_t* buffer,
    size_t buffer_length
) {
    hexdump_to_stream(
        os,
        bytes_per_line,
        bytes_per_group,
        space_per_byte,
        space_per_group,
        offset_start,
        offset_end,
        voffset_at_zero,
        display_textual,
        display_uppercase,
        make_getdata_t(buffer, buffer_length)
    );
}

std::string hexdump_to_string(
    size_t bytes_per_line,
    size_t bytes_per_group,
    size_t space_per_byte,
    size_t space_per_group,
    off_t offset_start,
    off_t offset_end,
    off_t voffset_at_zero,
    bool display_textual,
    bool display_uppercase,
    hexdump_getdata_t get_data
) {
    std::stringstream ss;
    hexdump_to_stream(
        ss,
        bytes_per_line,
        bytes_per_group,
        space_per_byte,
        space_per_group,
        offset_start,
        offset_end,
        voffset_at_zero,
        display_textual,
        display_uppercase,
        get_data
    );
    return ss.str();
}

std::string hexdump_to_string(
    size_t bytes_per_line,
    size_t bytes_per_group,
    size_t space_per_byte,
    size_t space_per_group,
    off_t offset_start,
    off_t offset_end,
    off_t voffset_at_zero,
    bool display_textual,
    bool display_uppercase,
    uint8_t* buffer,
    size_t buffer_length
) {
    return hexdump_to_string(
        bytes_per_line,
        bytes_per_group,
        space_per_byte,
        space_per_group,
        offset_start,
        offset_end,
        voffset_at_zero,
        display_textual,
        display_uppercase,
        make_getdata_t(buffer, buffer_length)
    );
}

hexdump_getdata_t make_getdata_t(
    uint8_t* buffer,
    size_t buflen
) {
    return [buffer, buflen](off_t offset) -> std::variant<uint8_t, hexdump_data_t> {
        if (offset < 0) {
            return hexdump_data_t::non_existent;
        } else if ((size_t)offset >= buflen) {
            return hexdump_data_t::non_existent;
        } else {
            return buffer[offset];
        }
    };
}

off_t hexdump_align(
    off_t voffset_at_zero,
    size_t bytes_per_line
) {
    off_t offset = voffset_at_zero % (off_t)bytes_per_line;
    return voffset_at_zero - offset;
}

}
