#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_MODULE RMRF_TESTS
#include <boost/test/included/unit_test.hpp>

#include <chrono>
#include <iostream>
#include <sstream>
#include <memory>
#include <thread>
#include <unistd.h>

#include "mumta/evloop.hpp"

#include "net/client_factory.hpp"
#include "net/socketaddress.hpp"
#include "net/sock_address_factory.hpp"
#include "net/tcp_server_socket.hpp"

#include "lib/ev/ev.hpp"
#include "mumta/evloop.hpp"

#include "net/udp_client.hpp"

using namespace rmrf::net;

const std::string udp_test_string = "TEST UDP PACKET";

volatile bool tcp_called = false;
volatile bool udp_called = false;

int udp_source_family;

static void timeout_cb (::ev::timer& w, int revents) {
    MARK_UNUSED(revents);

    // Terminate the timer
    w.stop();

    if (!tcp_called && !udp_called) {
        // Report failure
        BOOST_CHECK_MESSAGE(false, "Timeout");
    }

    // this causes the innermost ev_run to stop iterating
    ::ev_break (::ev::get_default_loop(), EVBREAK_ONE);
};

void ev_thread_callable() {
    BOOST_TEST(check_version_libev());
    //rmrf::ev::init_libev();
    std::cout << "Configuring timeout." << std::endl;

    ::ev::timer timeout{};
    timeout.set<timeout_cb>(0);
    timeout.start(2);

    std::cout << "Starting ev loop." << std::endl;
    rmrf::ev::loop();
    std::cout << "Stopped ev loop." << std::endl;
}

void udp_test_cb(const iorecord& data) {
    BOOST_CHECK_EQUAL((char*) data.ptr(), udp_test_string.c_str());
    std::stringstream msg_ss;
    msg_ss << "Received UDP packet from: " << data.get_address().str();
    BOOST_TEST_MESSAGE(msg_ss.str());
    BOOST_CHECK_EQUAL(data.get_address().family(), udp_source_family);
    udp_called = true;
}

void run_udp_test() {
    using namespace std::chrono_literals;

    const socketaddr source_address = get_first_general_socketaddr("127.0.0.1", 9862, socket_t::UDP);
    const socketaddr destination_address = get_first_general_socketaddr("127.0.0.1", 9863, socket_t::UDP);

    auto sender = client_factory_construct_udp_client(source_address, udp_test_cb);
    auto receiver = connect(destination_address, socket_t::UDP);
    udp_source_family = destination_address.family();

    udp_packet<1024> data;
    data << udp_test_string;
    sender->send_packet(destination_address, data);

    std::this_thread::yield();
    std::this_thread::sleep_for(100ms);

    sender.reset();
    receiver.reset();
}

void run_tcp_test(const socketaddr& interface_addr) {
    using namespace std::chrono_literals;

    auto server = std::make_shared<tcp_server_socket>(interface_addr,
        [](std::shared_ptr<async_server_socket> s, std::shared_ptr<connection_client> c) {
            BOOST_REQUIRE(s);
            BOOST_REQUIRE(c);
            c->set_incomming_data_callback(
                [c](const iorecord& data) {
                    c->write_data(data);
                });
        });

    auto client = connect("127.0.0.1", "9861");
    client->set_incomming_data_callback(
        [](const iorecord& data) {
            BOOST_CHECK_EQUAL(data.str(), "Moin");
            tcp_called = true;
        });
    const std::string moin_string("Moin");
    client->write_data(iorecord(moin_string.c_str(), moin_string.length()));

    std::this_thread::yield();
    std::this_thread::sleep_for(100ms);

    client.reset();
    server.reset();
}

BOOST_AUTO_TEST_CASE(Netio_Socket_TCP) {
    using namespace std::chrono_literals;

    std::thread ev_thread(ev_thread_callable);
    const auto interface_addr = get_first_general_socketaddr("127.0.0.1", 9861);
    (void)interface_addr;

    BOOST_CHECK_NO_THROW(run_tcp_test(interface_addr));

    // Sleep one second without using ev timer
    std::this_thread::sleep_for(500ms);

    ev_thread.join();

    BOOST_CHECK(tcp_called);
}

BOOST_AUTO_TEST_CASE(Netio_Socket_UDP) {
    using namespace std::chrono_literals;

    std::thread ev_thread(ev_thread_callable);

    BOOST_CHECK_NO_THROW(run_udp_test()); // TODO put in own test while keeping same ev loop setup

    // Sleep one second without using ev timer
    std::this_thread::sleep_for(500ms);

    ev_thread.join();

    BOOST_CHECK(udp_called);
}
