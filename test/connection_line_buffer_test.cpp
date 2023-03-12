#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_MODULE RMRF_TESTS
#include <boost/test/included/unit_test.hpp>

#include <algorithm>
#include <array>
#include <iostream>
#include <memory>
#include <string>

#include "loopback_connection_client.hpp"

#include "net/connection_line_buffer.hpp"
#include "net/iorecord.hpp"


using namespace rmrf::net;
using namespace rmrf::test;

//BOOST_AUTO_TEST_SUITE( LINE_BUFFER )

int mut_send_stage = 0;
bool extra_failed = false;
const bool display_dbg_msg = false;

iorecord string_to_iorecord(const std::string& s) {
    return iorecord(s.c_str(), s.length());
}

void mut_send_data_cb(const iorecord& data) {
    std::cout << "This method should never have been called yet we've got:" << std::endl;
    std::cout << data.str() << std::endl;
    BOOST_CHECK(false);
}

void next_line_cb(const std::string& data, bool complete) {
    if constexpr (display_dbg_msg) std::cout << mut_send_stage << ": " << data << std::endl;

    switch (mut_send_stage++) {
    case 0:
        BOOST_CHECK_EQUAL(data, "The first line");
        BOOST_CHECK(complete);
        break;
    case 1:
        BOOST_CHECK_EQUAL(data, "The second line");
        BOOST_CHECK(complete);
        break;
    case 2:
        BOOST_CHECK_EQUAL(data, "The third line");
        BOOST_CHECK(complete);
        break;
    case 3:
        BOOST_CHECK_EQUAL(data.length(), 151);
        BOOST_CHECK(!complete);
        BOOST_CHECK(std::find_if(data.cbegin(), data.cend(), [](char c) {
            return c != 'a';
        }) == data.cend());
        break;
    default:
        extra_failed = true;
    }
}

BOOST_AUTO_TEST_CASE(Default_EoL_Search_Test) {
    BOOST_CHECK_EQUAL(default_eol_search("This line contains no line break", 0), std::string::npos);
    std::string data = "This\r line contains line\r\n breaks";
    BOOST_CHECK_EQUAL(default_eol_search(data, 0), 4);
    BOOST_CHECK_EQUAL(data.substr(0, 4), "This");
    BOOST_CHECK_EQUAL(default_eol_search(data, 5), 25);
    BOOST_CHECK_EQUAL(data.substr(5, 25 - 5), " line contains line\r");
}

BOOST_AUTO_TEST_CASE(Connection_Line_Buffer_Test) {
    mut_send_stage = 0;
    auto ll_client = std::make_shared<loopback_connection_client>(mut_send_data_cb, false);
    connection_line_buffer clb(ll_client, next_line_cb, 150);

    if constexpr (display_dbg_msg) std::cout << "Testing legit lines" << std::endl;

    ll_client->send_data_to_incomming_data_cb(string_to_iorecord("The first"));
    ll_client->send_data_to_incomming_data_cb(string_to_iorecord(" line\r"));
    ll_client->send_data_to_incomming_data_cb(string_to_iorecord("The second line\r"));
    ll_client->send_data_to_incomming_data_cb(string_to_iorecord("\nThe third line\n"));

    if constexpr (display_dbg_msg) std::cout << "Testing line overflow" << std::endl;

    for (int i = 0; i < 151; i++) {
        ll_client->send_data_to_incomming_data_cb(string_to_iorecord("a"));
    }

    BOOST_CHECK_EQUAL(mut_send_stage, 4);
    BOOST_CHECK(!extra_failed);
}

BOOST_AUTO_TEST_CASE(Iorecord_String_Collection_Test) {
    std::array<uint8_t, 16> arr = {(uint8_t) 'a', (uint8_t) 'b', (uint8_t) 'c', 0,
                                                              (uint8_t) 'a', 0, (uint8_t) 'b', 0,
                                                              (uint8_t) 'a', (uint8_t) 'b', (uint8_t) 'c', (uint8_t) 'd',
                                                              (uint8_t) 'e', (uint8_t) 'f', (uint8_t) 'g', (uint8_t) 'h'};
    iorecord r(arr.data(), arr.size());
    auto v = r.get_strings_in_record();
    BOOST_CHECK_EQUAL(v.size(), 4);
    BOOST_CHECK_EQUAL(r.potential_strings_in_record(), 4);
    BOOST_CHECK_EQUAL(v[0], "abc");
    BOOST_CHECK_EQUAL(v[1], "a");
    BOOST_CHECK_EQUAL(v[2], "b");
    BOOST_CHECK_EQUAL(v[3], "abcdefgh");
    if constexpr (display_dbg_msg) {
        for(size_t i = 0; i < v.size(); i++) {
            std::cout << i << ':' << v[i] << std::endl;
        }
    }
}

//BOOST_AUTO_TEST_SUITE_END()
