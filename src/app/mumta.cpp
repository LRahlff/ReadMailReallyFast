#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>

#include <ev++.h>

#include "service/daemonctl.hpp"

bool check_version_libev() {
    auto ev_major{ev::version_major()};
    auto ev_minor{ev::version_minor()};

    constexpr auto exp_major{EV_VERSION_MAJOR};
    constexpr auto exp_minor{EV_VERSION_MINOR};

    std::cout << "Checking dependency: libev: detected " << ev_major << "." << ev_minor << ", compiled " << exp_major << "." << exp_minor << "\n" << std::flush;

    if(ev_major != exp_major) {
        std::cerr << "Checking dependency: libev: failed version check: Major API version mismatch.\n" << std::flush;
        return false;
    }

    if(ev_minor < exp_minor) {
        std::cerr << "Checking dependency: libev: failed version check: Minor API version too old.\n" << std::flush;
        return false;
    }

    return true;
}

int main() {
    dctl_status_msg("Checking environment");
    if(!check_version_libev()) {
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

    for(size_t x = 10; x; x--) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        dctl_watchdog_refresh();
    }

    dctl_status_msg("Preparing for shutdown");
    dctl_status_shutdown();
    dctl_status_msg("Finalizing pending transactions");
    dctl_status_msg("Storing active state");
    dctl_status_msg("Storing active caches");
    dctl_status_msg("Closing active sockets");
    dctl_status_msg("Inactive");

    return 0;
}
