#pragma once

#include <stdint.h>

#include <functional>
#include <iostream>
#include <string>
#include <variant>


namespace rmrf::utils {

enum class hexdump_data_t : uint8_t {
    non_existent,
    unknown
};

typedef std::function<std::variant<uint8_t, hexdump_data_t>(off_t)> hexdump_getdata_t;

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
);

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
);

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
);

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
);

hexdump_getdata_t make_getdata_t(
    uint8_t* buffer,
    size_t buflen
);

off_t hexdump_align(
    off_t voffset_at_zero,
    size_t bytes_per_line
);

}
