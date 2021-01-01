#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>

#include "lib/ev/ev.hpp"
#include "lib/nl/nl.hpp"
#include "lib/openssl/openssl.hpp"

#include "mumta/evloop.hpp"

#include "service/daemonctl.hpp"

int main() {
    dctl_status_msg("Checking environment");

    if (!check_version_libev()) {
        return 1;
    }

    if (!check_version_libnl()) {
        return 1;
    }

    if (!check_version_openssl()) {
        return 1;
    }

    dctl_status_msg("Initializing");
    dctl_status_msg("Reading configuration");
    dctl_status_msg("Initializing network");
    dctl_status_msg("Loading caches");
    dctl_status_msg("Refreshing caches");
    dctl_status_msg("Reading state");
    dctl_status_msg("Initializing");
    dctl_status_msg("Binding sockets");

    dctl_status_msg("Activating");
    dctl_status_ready();
    dctl_status_msg("Active");

    dctl_watchdog_refresh();
    rmrf::ev::loop();

    dctl_status_msg("Preparing for shutdown");
    dctl_status_shutdown();
    dctl_status_msg("Finalizing pending transactions");
    dctl_status_msg("Storing active state");
    dctl_status_msg("Storing active caches");
    dctl_status_msg("Closing active sockets");
    dctl_status_msg("Inactive");

    return 0;
}
