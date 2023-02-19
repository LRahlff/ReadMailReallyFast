#include "net/udp_client.hpp"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

namespace rmrf::net {

    bool udp_test_successful = false;
    const char* udp_test_string = "TEST UDP PACKET";

    void udp_test_cb(const udp_packet<pkg_size>& data, socketaddr& source) {
        if (strcmp(data.raw, udp_test_string) != 0)
        udp_test_successful = true;
    }

    void run_udp_test() {
        udp_client sender{get_first_general_socketaddr("localhost", 9862)};
        const socketaddr destination_address = get_first_general_socketaddr("localhost", 9863);
        udp_client receiver{destination_address, udp_test_cb};
        udp_packet<1024> data;
        strncpy(data.raw, udp_test_string, sizeof(udp_test_string) + 1);
        sender.send_packet(destination, data);
    }

}
